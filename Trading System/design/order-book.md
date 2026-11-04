# Order book

This is a design for the C++ order book library. It is a starting point, not a decision. Names are open (see "Open decisions" in `overview.md`).

## Purpose

Take a stream of L3 events for one stock, keep the current book, and report the top of book. An L3 event is any single record from the feed, whatever its action: Add, Cancel, Modify, Clear, Trade, Fill, or None. The book reads all of them. Some change the book and some do not (see "What each action does"). Later, answer deeper questions such as how many shares are queued ahead of a given order.

## Rules for the library

- It is a pure library: no Kafka, no protobuf, no file or network access. A service converts messages into plain C++ events at its edge and converts the results back.
- There is one book per stock. The first version handles one stock, so it needs just one book. Later, for several stocks, a book manager receives events and passes each to the right book, creating books as new stocks appear.
- Each book is used by one thread only. Kafka already keeps each stock's events in order, so no locks are needed.
- Prices are 64-bit integers (one unit is 1e-9 dollars). Never use floating point for prices.
- Processing an event must never throw an exception on bad input. Real feeds are messy, and starting mid-day produces cancels for orders the book never saw. Return a status, count the problem, and keep going.

## Input: one L3 event

The fields are listed in `data.md`. Every action uses the same fields. The book needs: exchange timestamp, sequence, order id, price, size, action, side, flags.

## What each action does

| Action | Effect on the book |
|---|---|
| Add | Insert a new order at the back of the queue at its price. |
| Cancel | Remove `size` shares from the order. If the order reaches 0 shares, remove it, and remove its price level if the level is now empty. If the order id is unknown, count it and do nothing else. |
| Modify | Venue-specific. A common rule: a price change or a size increase moves the order to the back of its queue, and a size decrease at the same price keeps its place. Check the venue's documentation. Nasdaq data may contain no Modify records. Implement this only after real data shows it is needed. |
| Clear | Remove every resting order. |
| Trade, Fill, None | No change. Return "ignored". |

A modify never changes the side of an order.

Why a Cancel carries a size: a cancel can remove only part of an order. The price and side repeat what the book already stored for that order id. If they disagree with the stored copy, the data or the book has a problem. Which to trust is an open decision. Leaning toward the stored copy plus a mismatch counter.

## Publishing the top of book

A packet of events from the exchange can change the book several times. The book is only consistent after the last event in the packet, which the "last event in packet" flag marks (F_LAST in Databento). So publish a top-of-book message only when that flag is set and the top actually changed.

## Data structures (prose)

- A hash map from order id to order. It gives constant-time lookup, meaning the time does not grow as the book grows.
- For each side, a sorted map from price to price level. For bids, the highest price comes first. For asks, the lowest price comes first. The first entry is the best price.
- Each price level holds the total size at that price, and a queue of order ids in arrival order. A linked list works as the queue.
- Each order stores its side, price, size, and a handle to its own place in its queue's linked list (in C++, a list iterator). With this handle, cancelling an order from the middle of a queue takes constant time.

## Operations the library should offer

Names are open.

- Process one event. Returns a status (ok, ignored, unknown order, invalid) and whether the top of book changed.
- Read the top of book: best bid price and size, best ask price and size. A price of 0 means that side is empty.
- Mid price (the average of best bid and best ask) and spread (best ask minus best bid). Both are missing if either side is empty.
- Depth: up to N aggregated price levels per side, best first. This is the L2 view of the L3 book.
- Shares ahead of an order: the shares queued in front of it at its price. The replay fill simulator uses this to decide when a simulated order would have been filled.
- Order count, count of unknown-order cancels, and clear.

## Test checklist

Write these tests before the implementation.

- An empty book has no best bid and no best ask.
- Add one bid: the top of book shows it.
- Add two orders at the same price: the queue keeps arrival order, and the level's total size is the sum.
- Add a better-priced order: the top of book changes.
- Add a worse-priced order: the top of book does not change.
- Cancel part of an order: the size shrinks, and the order keeps its place.
- Cancel a whole order: the order disappears, and an empty level disappears.
- Cancel the best order: the top of book moves to the next level.
- Cancel an unknown order id: the status is "unknown order", the counter goes up, and the book does not change.
- An Add with size 0: the status is "invalid".
- Trade, Fill, and None leave the book unchanged.
- Clear empties the book.
- A replace (Cancel then Add with a new order id) puts the new order at the back of the queue.
- Golden test (stage 3): replay a real Databento day and compare the top of book with Databento's MBP-1 data for the same period.
