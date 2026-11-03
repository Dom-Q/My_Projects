# Data

## L1, L2, and L3

- **L1**: the best bid and best ask with their sizes, plus the trades. Nothing deeper.
- **L2**: the total size at each price level, for several levels. It shows how much is waiting at each price, but not the individual orders.
- **L3**: every individual order event: an order was added, cancelled, or traded. From these events the whole book can be rebuilt, including the order of the queue at each price.

L1 is enough for a basic strategy. L3 is for learning how a book works and for research on simulating fills.

## Databento (historical data)

- Databento's name for L3 is **MBO** (market by order), for L2 it is **MBP-10** (market by price, 10 levels), and for L1 it is **MBP-1**. **TBBO** is each trade together with the best bid and ask at that moment.
- US equities history starts in May 2018. Files use Databento's own format, DBN. There are Python, C++, and Rust libraries.
- Pricing is based on usage, and new accounts get starter credits. Check the current amount and terms on the Databento site.
- Always check the cost before downloading. `research/download_databento_sample.py` does this by default.
- Start with one ticker and a few minutes of data.
- Prices are 64-bit integers where one unit is 1e-9 dollars (one billionth). Example: $187.42 is 187,420,000,000.
- Records identify a stock by a numeric `instrument_id`, not by its ticker.
- A historical MBO file includes a synthetic snapshot of the book at the start of each UTC day. A request that starts later in the day has no snapshot, so expect cancels for orders whose add is not in the file.

## What an L3 event looks like (Databento MBO)

Every record has the same fields, whatever the action:

| Field | Meaning |
|---|---|
| exchange timestamp | the exchange's timestamp for the event, in nanoseconds since 1970 (UTC). Databento calls it `ts_event`. Check its exact meaning in the Databento documentation. |
| order id | the exchange's number for the order |
| action | what happened (see below) |
| side | bid or ask (may be missing for some actions) |
| price | in the integer format above |
| size | number of shares |
| flags | bits that mark things such as the last event in a packet |
| sequence | the exchange's running count of events |

Actions: Add, Modify, Cancel, Clear, Trade, Fill, None.

- Add puts a new order on the book.
- Modify changes the price and/or size of an existing order.
- Cancel removes shares from an order. The size is the amount removed, so a cancel can be partial.
- Clear removes every resting order.
- Trade, Fill, and None do not change the book. A trade is the aggressing order. A fill is a resting order that was matched. The shares leaving the book come as separate Cancel records.

### Nasdaq (dataset XNAS.ITCH)

This is what Databento's normalization page says. Verify it on real data.

- An execution arrives as Trade, then Fill, then Cancel. Trade records have order id 0.
- A partial cancel is a Cancel whose size is the shares removed.
- A delete is a Cancel whose size is the order's whole remaining size.
- A replace is a Cancel for the old order id followed by an Add for the new order id. It always loses its place in the queue.
- The page lists no Modify records for Nasdaq. Modify appears on other venues, such as CME futures.

## Alpaca (live data)

- The free plan gives real-time data from one exchange, IEX, which is about 4% of US stock volume. The IEX top of book is one exchange's quote, not the best price across the country.
- The free plan allows 30 symbols on the live stream (the websocket), history since 2016, and no queries for the most recent 15 minutes.
- Alpaca has no L2 or L3 data for stocks.
- The paid plan ($99 per month) adds data from all exchanges combined. I chose the free plan for now.
- Alpaca's paper trading uses practice money. It tests the plumbing (order states, rejections, partial fills, reconciling positions), not profit.

## Which feed to train on and trade on

- Train and trade on the same feed. Every message records its `source` (L3-derived, Alpaca IEX, or Alpaca combined) so this can be checked.
- The strategy path is expected to be Alpaca-only: train and backtest on Alpaca's historical IEX quotes and trades, then trade on the live IEX stream.
- The Databento L3 book is for learning, for checking the book against Databento's own L1 data, and for researching fill simulation. It is not the strategy's data feed.
- Features that need depth (L2 or L3) are excluded from the model, because the live feed has none.
- Time windows and sampling use the exchange's timestamp, never the computer's clock. Then the replay speed does not change the results.

## Questions to answer from real data

These are the same questions listed at the bottom of `research/explore_order_events.py`.

1. Do Cancel rows have a price and a side filled in?
2. For one order id, what is its whole life, row by row? Does it ever have more than two rows?
3. For one order, how does a Cancel's size compare with the Add's size?
4. Does any Modify row appear? What is the order id on Trade rows?
5. How many Cancel rows have an order id with no earlier Add in the file? Why?
6. Which flag bits mark the end of a packet of events?
