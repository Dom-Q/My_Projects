# %% [markdown]
# Look at a downloaded Databento MBO file.
#
# Run this one cell at a time (VS Code or Jupyter), from the project root, or run
# it as a plain script. Add your own cells below the questions at the end.

# %%
from pathlib import Path

import databento as db

# Picks the newest file in data/.
files = sorted(Path("data").glob("*.dbn.zst"), key=lambda p: p.stat().st_mtime)
if not files:
    raise SystemExit(
        "No .dbn.zst file in data/. Run this from the project root, and run "
        "research/download_databento_sample.py with --download first."
    )
path = files[-1]
print(path)

# %%
store = db.DBNStore.from_file(path)

# Prices come back as floats in dollars. Pass price_type="fixed" to see the raw
# integers instead, where one unit is 1e-9 dollars.
df = store.to_df()
print(len(df), "records")
print(df.head(50))

# %% Which actions appear, and how often?
print(df["action"].value_counts())

# %% [markdown]
# Questions to answer from the data. Write your own cells for each one.
#
# 1. Do Cancel rows have a price and a side filled in?
# 2. Pick one order_id and print every row for it, in order. Does its life look
#    like Add ... Cancel? Does it ever have more than two rows?
# 3. For one order, how does a Cancel's size compare with the Add's size?
# 4. Do you ever see a Modify row? What is the order_id on Trade rows?
# 5. How many Cancel rows have an order_id with no earlier Add in this file?
#    Why? (Hint: the file starts partway through the trading day.)
# 6. Look at the flags column. Which rows mark the end of a packet of events?
#    (See the Databento documentation for the flag bits.)
