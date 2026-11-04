# Overview

## Goal

Build a personal trading system as a learning project. The first target is a working prototype that I understand. Profit is not a goal yet.

Scope: build the whole system end to end for one stock first. Add more stocks only after that works. The design keys everything by symbol, so adding stocks later is mostly configuration.

What I want to learn:
- Kafka streaming.
- Order book design and use, with L3 data (defined in `data.md`).
- A very simple machine learning model, to learn the Python tools.
- Trade execution and validation.
- C++ and Python programs talking to each other over Kafka.
- AWS (Amazon's cloud) once everything works on my own machine.

## Terms used in these documents

- **Order book**: the list of all resting buy orders and sell orders for one stock. A resting order is one that is waiting to be matched.
- **Bid and ask**: the bid is the highest price a buyer will pay. The ask is the lowest price a seller will accept.
- **Top of book**: the best bid and best ask, each with the number of shares offered at that price.
- **Replay mode**: the system reads recorded historical data, as if it were arriving live.
- **Live mode**: the system reads real-time data from the market.
- **Service**: one program that does one job. Services pass messages to each other through Kafka.

## The system in one paragraph

Replay mode: historical Databento L3 data goes into a C++ order book, which produces top-of-book messages. Those feed the features, the model, the signals, the risk checks, and simulated fills.

Live mode: Alpaca's free real-time quote stream (one exchange only, called IEX) produces the same top-of-book messages. They go through the same downstream code, and orders go to Alpaca's paper trading account (practice money).

The two modes meet at one point: the TopOfBook and Trade messages. Everything after that point does not know which source produced the data.

## Services

| Service | Job |
|---|---|
| replay_producer | Reads Databento files and publishes L3 events (optional, used in a later stage). |
| order_book_service | Runs the C++ order book on L3 events (every action: Add, Cancel, Modify, Clear, Trade, Fill, None) and publishes top-of-book and trade messages. |
| quote_adapter | Reads Alpaca's live quotes and trades and publishes the same top-of-book and trade messages. |
| feature_builder | Turns top-of-book and trade messages into numbers the model uses (the features). |
| model_service | Runs the model on the features and publishes a signal (long, short, or flat). |
| risk_manager | Checks each signal against limits. Approved signals become orders. Rejected ones are logged. |
| execution_gateway | Sends orders to the broker (or the simulated fill engine in replay mode) and publishes execution reports. |
| portfolio_service | Keeps the current position (shares held) and profit and loss per symbol, from the execution reports. |

`risk_manager` and `portfolio_service` stay as single copies. A limit such as total exposure needs every symbol's position at once, so the work cannot be split by symbol.

## Language split

- The order book is C++.
- Everything else starts in Python, for two separate reasons:
  - The data services (`quote_adapter`, and the replay code that reads Databento files) use the vendors' official Python libraries, which are their main, best-documented ones. Databento also has official C++ and Rust libraries.
  - Research and model training use Python's data and machine learning tools (pandas, scikit-learn). That work happens offline, in scripts and notebooks. `model_service` only loads the trained model and runs it on features it reads from Kafka.
- Services talk through protobuf messages, so any service can be rewritten in another language later without changing the others. The `.proto` files live in one shared `proto/` directory, and both C++ and Python generate their code from it.
- Other ways C++ and Python meet: a pybind11 binding (a way to call C++ from Python) for tests and notebooks, and Parquet files (a column-based file format) for training data.

## Stages

1. Look at real Databento data. This is the current stage.
2. Write the order book library in C++, test-first, with no Kafka.
3. Add the pybind11 binding. Replay a Databento day through the book from Python. Compare the book's top of book with Databento's own top-of-book data (MBP-1) for the same period. This is the golden test: the book is right if the two agree.
4. Add Kafka. A replay process runs the book and publishes top-of-book messages. Build features, model, signals, risk, simulated execution, and positions on top.
5. Add the quote_adapter for Alpaca's live stream. Add Alpaca paper execution.
6. Optional, for more Kafka and C++ practice: replay_producer, then Kafka, then a C++ order_book_service. After that, AWS (MSK is Amazon's managed Kafka, S3 is Amazon's file storage, plus containers).

## Open decisions (mine to make)

- Names. The draft used `apply` for the main method and `Event` for the event type. Other candidates: `process_event`, `OrderEvent`, and `add_order`, `modify_order`, `cancel_order` for the helpers. A rename is cheap until the code exists.
- When a cancel record's price or side disagrees with what the book stored, which one to trust. Leaning toward the stored copy plus a counter of mismatches.
- Whether to implement Modify now, or only after real data shows it is needed.
- Top-of-book and trade messages: two topics (`md.l1` and `md.trades`, the current draft) or one topic whose message holds either one (a protobuf `oneof`). Kafka does not keep order across two topics, so with two, `feature_builder` must buffer both and merge them by exchange timestamp. With one topic, the producer writes them in the order they happened, and every reader gets that order for free. Leaning toward one topic while there is one stock.
  - Side note: Alpaca does not promise that quotes and trades arrive in timestamp order (see `data.md`). With one topic this can be fixed in one place: `quote_adapter` holds incoming messages for a short reorder window (for example 100 milliseconds), sorts them by exchange timestamp, then writes them. Still to decide: how long the window is (longer catches more late messages but adds delay), and what to do with a message that arrives after its window (drop it, or pass it through marked as late). Measure how often Alpaca's messages arrive out of order in stage 5 before choosing. Replay mode does not need this, because the Databento file is already in exchange order.

## Status

- Written: the two research scripts and these documents.
- Not verified: the scripts have not run against real Databento data. The message designs have never been compiled with protoc (the protobuf compiler).
- Nothing has been built on my machine yet.
