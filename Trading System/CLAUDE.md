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

- `research/download_databento_sample.py` estimates the cost of a small slice, then downloads it with `--download`.
- `research/explore_order_events.py` is a cell-by-cell script. It lists questions to answer from the data. I answer them. Do not answer them for me.
- `research/requirements.txt` holds what the research scripts need. The research folder runs on its own, separate from `python/`.

## First task for Claude Code

Create the `cpp/`, `python/`, and `proto/` directories with build files and dependency files only. Write no implementation code and no source files with logic.

1. Check what is installed: git, make, cmake, a C++20 compiler (g++ or clang++), and Python 3.11 or newer. Tell me what is missing. Ask before installing anything.
2. Create these files:
   - `Makefile` at the project root with targets `cpp-build`, `cpp-test`, `py-install`, `py-test`, and `clean`. `make cpp-test` and `make py-test` must not fail only because no tests exist yet.
   - `cpp/CMakeLists.txt`: C++20, strict compiler warnings, and GoogleTest (the C++ testing library) fetched with CMake's FetchContent. It needs internet on the first run. It must configure with no source files yet.
   - `python/pyproject.toml` with optional dependency groups (extras): core (`confluent-kafka`, `protobuf`), data (`databento`, `alpaca-py`, `pandas`, `pyarrow`), research (`scikit-learn`, `matplotlib`, `jupyter`), and dev (`pytest`, `ruff`). `make py-install` should create a `.venv` and install them. If an editable install does not work with no package yet, pick the simplest fix and tell me what you chose.
   - `proto/` containing only a `.gitkeep` file. This is the one shared home for the `.proto` message files (protobuf message descriptions). C++ and Python both generate their code from it, so the two languages always agree on the message format. I write the `.proto` files myself, so create none. Add a `make proto` target that generates C++ and Python code from whatever `.proto` files exist into build folders that git ignores, and does nothing when there are none. Check whether `protoc` (the protobuf compiler) and the C++ protobuf library are installed. If not, tell me the options and ask before installing.
   - Update `.gitignore` if the new build folders or the generated code need it.
3. Run `git init`. Do not commit until I ask.
4. Do not install Kafka, download data, create topics, or write any order book or service code.
5. Show me the final directory tree and one line on what each new file does. Then wait.

## Status

- Written: the two research scripts and the design documents.
- Not verified: the scripts have not run against real Databento data. The message designs have never been compiled with protoc (the protobuf compiler). The order book design has no code yet.
- Nothing has been built on my machine yet.
