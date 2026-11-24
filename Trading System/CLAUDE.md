# Trading system: context for Claude Code

## How to work with me

This is a learning project. I want to understand and write the code myself.

- Guide, explain, review, and ask questions. Do not write the implementation unless I ask for a specific piece.
- Scaffolding is fine to write: build files, dependency files, configuration, and data-fetching scripts.
- Keep answers short, in plain and grammatical English. Answer the question I asked first.
- Define every term the first time you use it. Do not introduce new jargon, abbreviations, or concepts without explaining them.
- Use descriptive names for files, functions, and variables. No numeric prefixes such as `01_`. No abbreviations a newcomer would not know.
- Check facts against the documentation before stating them, and say when something is unverified.
- Ask before adding anything I did not ask for.
- When I ask for a command, give me the command. Do not change a script to get the same result unless I ask.
- Give direct answers and status updates, not lists of options, unless I ask for options.

## What I am building

A personal trading system. Goals: Kafka streaming; order book design and use (L3 data); a very simple model to learn Python ML tools; trade execution and validation; C++ and Python services talking over Kafka; AWS once it works locally. The first target is a working prototype I understand. Profit is not a goal yet.

The design is in `design/`. Read these before answering design questions:

- `design/overview.md`: goals, the services, the language split, the stages, open decisions, status.
- `design/data.md`: L1, L2, L3, Databento, Alpaca, and which feed to train and trade on.
- `design/order-book.md`: the C++ order book design and test checklist.
- `design/messages-and-topics.md`: Kafka basics, protobuf conventions, messages, and topics.

These are drafts. The decisions listed as open in `design/overview.md` are mine to make.

## Current focus

Stage 1: read real Databento L3 data and learn what the events look like, before writing the order book.

- `research/download_databento_sample.py` takes `--symbols` (one or more), `--start` and `--end` in UTC, prints records, size, and cost per symbol (free metadata calls), and downloads only with `--download`, into `data/`.
- `research/explore_order_events.py` is a cell-by-cell script. It lists questions to answer from the data. I answer them. Do not answer them for me.
- The research scripts run with `python/.venv` (created by `make py-install`), which already has `databento` and `pandas`. Do not create a separate environment for them. Run them from this folder, for example `python/.venv/bin/python research/download_databento_sample.py ...`.
- The Databento API key is in the `DATABENTO_API_KEY` environment variable, set in Ubuntu's `~/.bashrc`. `db.Historical()` reads it automatically.

Scope: build the whole system end to end for one stock first. Add more stocks later.

## Environment

- The machine is Windows 11 on an ARM64 processor (Snapdragon X). All building and running happens inside WSL (Windows Subsystem for Linux) with Ubuntu 24.04.
- Inside Ubuntu, this folder is `/mnt/c/Users/domjq/src/My_Projects/Trading System`. On Windows it is `C:\Users\domjq\src\My_Projects\Trading System`. (Until November 2026 there was a second, nested clone at `src/My_Projects/My_Projects`. It has been removed; do not recreate it.)
- Python, `make`, and the C++ tools exist only in Ubuntu, not on Windows. Claude Code may run on Windows; it runs Ubuntu commands with `wsl bash -c "..."`.
- Cursor must be connected to WSL (bottom-left shows "WSL: Ubuntu") to use `python/.venv/bin/python` as the interpreter and run cells. A Cursor window on Windows cannot run the Linux Python.
- Git in Ubuntu has its own settings: name and email are set, and pushing uses the Windows Git Credential Manager (`credential.helper` points to `/mnt/c/Program Files/Git/clangarm64/bin/git-credential-manager.exe`). The GitHub username is `Dom-Q`.
- Ubuntu blocks `pip install` into its own Python ("externally-managed-environment"), which is why `python/.venv` exists.
- Installed in Ubuntu: g++ 13.3, make 4.3, cmake 3.28, Python 3.12, protoc 3.21.12 (the protobuf compiler), and the C++ protobuf library.
- This folder is part of the `My_Projects` git repository (github.com/Dom-Q/My_Projects). It is not a separate repository. Do not run `git init` here.
- Writing many files from Ubuntu to the Windows drive is slow. A large `pip install` can take 10 minutes or more.

## Build commands

Run these from this folder, inside Ubuntu:

- `make cpp-build` and `make cpp-test`: build the C++ code and run its tests with ctest. The first build downloads GoogleTest (the C++ testing library).
- `make py-install` and `make py-test`: create `python/.venv`, install every dependency group, and run pytest.
- `make proto`: generate C++ and Python code from `proto/*.proto` into `generated/`, which git ignores. It does nothing while there are no `.proto` files.
- `make clean`: delete `cpp/build/` and `generated/`.

`make py-test` and `make cpp-test` pass when there are no tests yet.

## Known issue to handle later

The Python protobuf library installed by pip is version 7.x, and Ubuntu's protoc is 3.21. Python code generated by this protoc may be rejected by that library. Check this when the first `.proto` file is written. The fix is a newer protoc, or pinning the Python library to a matching version.

## Decisions so far

- First stock: GOOGL. Live data later: Alpaca's free plan (IEX only). Databento's live data needs at least the $199/month Standard plan, so it is not used.
- Order book: a standalone C++ library. Python reads the Databento file, feeds each event to the book through a pybind11 binding (the book imported as a Python module), and analyzes and plots the book's output with pandas. Do not iterate pandas rows (`iterrows`) to feed it; loop over the file's records.
- Still open: whether the book's input event is a plain C++ struct or the protobuf `MarketEvent` type. A struct keeps the book free of protobuf; protobuf gives one shared definition. Speed does not matter either way.
- Data windows start at 00:00 UTC so they include Databento's start-of-day snapshot, and end at 05:00 UTC the next day (midnight New York time in winter). Apply every event from the snapshot on; ignore the book's output from before the day being studied.
- Correctness check (stage 3): compare the book's top of book, read only at records with the `F_LAST` flag, against Databento's MBP-1 data for the same day. Hidden orders never appear in the data, and the book is Nasdaq's only, not the national best price.

## Status

- Done: the setup. Every make target has run successfully. The `cpp/`, `python/`, and `proto/` folders hold build and dependency files only.
- Done: Databento account and API key (with $125 starter credit, applied automatically).
- Downloaded: `data/XNAS.ITCH_mbo_GOOGL_2026-11-20T0000_2026-11-21T0500.dbn.zst`, GOOGL MBO from 00:00 UTC November 20 to 05:00 UTC November 21, 2026 (about 10.4 million records, 191 MB compressed). Git ignores it; do not download it again.
- In progress: answering the questions in `research/explore_order_events.py`. The first rows show a Clear (action `R`) then Adds at 00:00 UTC, which looks like the snapshot; confirm with the flags.
- To check: why about 2,300 records fall outside 3:00 am to 8:00 pm New York time; how often `F_MAYBE_BAD_BOOK` appears.
- Not yet: no order book code, no `.proto` files.
- Next: finish the exploration questions, then design the order book's data structures and write it test-first (stage 2).
