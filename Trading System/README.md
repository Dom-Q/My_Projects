# Trading system

A learning project: an L3 order book in C++, market data and strategy services in Python, and Kafka with protobuf between them. `CLAUDE.md` holds the working rules and the first task for Claude Code. Claude Code reads it automatically when you start it in this folder.

## What is here

```
CLAUDE.md      working rules and the first task for Claude Code
design/        plans and design notes (markdown only, no code)
research/      scripts and notes for looking at real market data
data/          downloaded market data (not tracked by git)
Makefile       build and test commands (run inside WSL/Ubuntu)
cpp/           C++ code; CMakeLists.txt builds it and fetches GoogleTest
python/        Python services; pyproject.toml lists the dependencies
proto/         the .proto message files that C++ and Python share
```

`cpp/`, `python/`, and `proto/` hold build and dependency files only so far. You write the code and the `.proto` files yourself.

Make targets: `cpp-build`, `cpp-test`, `py-install`, `py-test`, `proto` (generates code into `generated/`), and `clean`.

## First steps: look at real data

1. Create a Databento account and an API key. New accounts get starter credits (check the current terms).
2. Set up Python. Run everything inside Ubuntu (WSL), from this folder:
   ```
   cd "/mnt/c/Users/domjq/src/My_Projects/Trading System"
   make py-install
   echo 'export DATABENTO_API_KEY=your_key_here' >> ~/.bashrc
   source ~/.bashrc
   ```
   `make py-install` creates `python/.venv`, which includes `databento` and `pandas`. The research scripts use this same environment. Paths such as `python/.venv/bin/python` are relative, so they only work from this folder.
3. Check the cost first. This downloads nothing:
   ```
   python/.venv/bin/python research/download_databento_sample.py --symbols GOOGL --start 2026-10-26T13:30 --end 2026-10-26T13:35
   ```
   Times are UTC. Pick a normal trading day and use a short window. To type `python` instead of `python/.venv/bin/python`, run `source python/.venv/bin/activate` once per terminal.
4. If the cost is acceptable, run the same command with `--download`.
5. Open `research/explore_order_events.py` and run it one cell at a time. The questions at the bottom are yours to answer.

## Working with Claude Code

Run `claude` from this folder. Its first task is in `CLAUDE.md`.
