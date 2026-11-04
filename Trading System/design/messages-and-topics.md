# Messages and topics

This is a draft design. Nothing here has been compiled with protoc (the protobuf compiler).

## Kafka in short

- A **topic** is a named log of messages. Producers append to it, and consumers read from it.
- A topic is split into **partitions**. Messages are ordered only within one partition.
- Every message has a **key**. Kafka hashes the key to pick the partition, so messages with the same key always go to the same partition and stay in order.
- Here the key is always the symbol (the ticker, such as AAPL). Each stock's events stay in order.
- A **consumer group** is a set of copies of one service. Kafka divides the partitions among the copies, so they share the work.

## Learning plan for Kafka

Do not create the whole topic set up front.

1. When stage 4 arrives, install Kafka (the Apache download or Docker, KRaft mode, which needs no ZooKeeper, and Java 17 or newer).
2. Create one topic with one partition by hand, using the `kafka-topics.sh` tool that comes with Kafka.
3. Write a tiny producer and a tiny consumer that print messages. Watch what happens.
4. Add each next topic when the service that uses it is built.

## Protobuf conventions

Protobuf (Protocol Buffers) is a format where messages are described in a `.proto` file, and code to read and write them is generated for C++ and Python.

- The `.proto` files live in `proto/` at the project root. C++ and Python both generate their code from that one directory, so the two languages always agree on the message format. Generated code is not committed to git.
- Prices are 64-bit integers, one unit is 1e-9 dollars. Decimals (doubles) only for derived statistics, never for prices or profit and loss.
- Timestamps are 64-bit integers: nanoseconds since 1970 (the Unix epoch), in UTC.
- In every enum (a list of named choices), value 0 is named `*_UNSPECIFIED`. A missing field reads as 0, so this makes "forgot to set it" visible.
- Never reuse or renumber a field. To retire one, mark it `reserved`. Adding new fields is always safe for old readers.
- Book side (bid or ask) and order direction (buy or sell) are separate enums, to avoid sign mistakes.
- Signed quantities (positions) use `sint64`, which stores small negative numbers compactly.

## Messages

Names are working names. The first one may be renamed (see `overview.md`).

| Message | Fields |
|---|---|
| MarketEvent (one L3 event, any action) | symbol; sequence (per symbol, assigned when ingested, and a gap means data was lost); vendor sequence; exchange timestamp; ingest timestamp (computer clock, for debugging only); action; side; price; size; order id; flags; venue |
| TopOfBook | symbol; exchange timestamp; sequence; bid price (0 = no bid); bid size; ask price (0 = no ask); ask size; source (L3-derived, Alpaca IEX, Alpaca combined) |
| Trade | symbol; exchange timestamp; sequence; price; size; aggressor side (bid means the buyer started the trade, unspecified when unknown, since L1 feeds often omit it); source |
| FeatureVector | symbol; exchange timestamp; as-of sequence (the last event applied); best bid; best ask; mid price; spread in basis points (1 basis point is 0.01%); L1 imbalance, which is (bid size − ask size) ÷ (bid size + ask size); signed volume over the last second (buys positive, sells negative); mid-price return over the last second |
| Signal | signal id (a unique id, used for auditing and to avoid duplicates); symbol; exchange timestamp; as-of sequence; model version; direction (long means own shares expecting a rise, short means sold borrowed shares expecting a fall, flat means no position); probability the price goes up; reference price (mid price at decision time) |
| RiskReject | signal id; symbol; rule name; human-readable detail; timestamp |
| OrderRequest | client order id (our own id, and the gateway must ignore a repeat so a redelivered message never sends an order twice); signal id; symbol; side (buy or sell); type (market or limit); time in force (day, or immediate-or-cancel, which fills what it can now and cancels the rest); quantity; limit price; created timestamp |
| ExecutionReport | client order id; broker order id; symbol; side; status (new, partially filled, filled, canceled, rejected); size of the latest fill; price of the latest fill; total filled so far; shares still open; average fill price; timestamp; reject reason |
| PositionSnapshot | symbol; quantity (signed, positive long and negative short); average price; realized profit and loss; timestamp |

FeatureVector has no depth features, because the live feed has no depth. It had a five-level imbalance in the first draft. That field number is `reserved`, as a worked example of retiring a field.

## Topics

Replay mode and live mode each get a complete, separate set of topics. A **topic prefix** is a short string placed at the front of every topic name: `replay.` or `live.`. Each service reads the prefix from its configuration and never writes a topic name in its code. So `replay.md.l1` and `live.md.l1` are two different topics.

Why separate sets, and not one topic with a type field: a replay run and a live run must never mix their messages, and with separate topics one can be deleted or re-run without touching the other.

All topics use the symbol as the key.

While the system handles one stock, every topic needs only one partition. All of its messages then stay in one order. Partitions only start to matter when there are several stocks.

| Topic (after the prefix) | Message | Producer, then consumer |
|---|---|---|
| md.events | MarketEvent | replay_producer, then book_builder (replay only, optional) |
| md.l1 | TopOfBook | book_builder (replay) or quote_adapter (live), then feature_builder |
| md.trades | Trade | the same producers, then feature_builder |
| features | FeatureVector | feature_builder, then model_service |
| signals | Signal | model_service, then risk_manager |
| risk.rejects | RiskReject | risk_manager, then logs |
| orders | OrderRequest | risk_manager, then execution_gateway |
| executions | ExecutionReport | execution_gateway, then position_tracker and risk_manager |
| positions | PositionSnapshot | position_tracker, then risk_manager |

- `md` stands for market data, and `l1` for level 1.
- `positions` is log-compacted: Kafka keeps only the latest message per key, so the topic works like a table of current positions. A restarting risk_manager reads it to recover.
- `risk.rejects` uses one partition. It is a log for reading, so ordering across symbols is convenient there.
- Why the order book does not have to read from Kafka at first: in replay mode the book can run inside one process that reads the Databento file directly, because nothing else needs the raw L3 events. Putting `md.events` in the middle is a later exercise (stage 6).
