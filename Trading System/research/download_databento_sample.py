"""Estimate the cost of a small slice of Databento data, and optionally download it.

Always run without --download first. The dry run only asks Databento for an
estimate and does not download or bill any data.

Example (times are UTC; 13:30 UTC is 9:30 am in New York while daylight saving
time applies, and 14:30 UTC otherwise):

    python research/download_databento_sample.py --symbols GOOGL AVGO \
        --start 2026-10-26T13:30 --end 2026-10-26T13:35

Pick a normal trading day. Weekends and market holidays have no data.
Run this from the project root. Downloads are saved in data/.
"""

import argparse
from pathlib import Path

import databento as db

DATA_DIR = Path(__file__).resolve().parent.parent / "data"


def main() -> None:
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument("--symbols", required=True, nargs="+", help="one or more tickers, for example GOOGL AVGO")
    parser.add_argument("--start", required=True, help="UTC start, for example 2026-10-26T13:30")
    parser.add_argument("--end", required=True, help="UTC end (exclusive), for example 2026-10-26T13:35")
    parser.add_argument("--dataset", default="XNAS.ITCH", help="Databento dataset ID (default: Nasdaq TotalView-ITCH)")
    parser.add_argument("--schema", default="mbo", help="mbo = L3, mbp-10 = L2, mbp-1 = L1")
    parser.add_argument("--download", action="store_true", help="download the data (this is billed)")
    args = parser.parse_args()

    client = db.Historical()  # reads the DATABENTO_API_KEY environment variable

    query = {
        "dataset": args.dataset,
        "schema": args.schema,
        "symbols": args.symbols,
        "start": args.start,
        "end": args.end,
    }

    total_records = 0
    total_bytes = 0
    total_cost = 0.0
    for symbol in args.symbols:
        symbol_query = {**query, "symbols": [symbol]}
        records = client.metadata.get_record_count(**symbol_query)
        size_bytes = client.metadata.get_billable_size(**symbol_query)
        cost = client.metadata.get_cost(**symbol_query)
        print(f"{symbol:<8} {records:>14,} records  {size_bytes / 1e6:>10.1f} MB  ${cost:>10.4f}")
        total_records += records
        total_bytes += size_bytes
        total_cost += cost

    if len(args.symbols) > 1:
        print(f"{'TOTAL':<8} {total_records:>14,} records  {total_bytes / 1e6:>10.1f} MB  ${total_cost:>10.4f}")

    if not args.download:
        print("Dry run only. Add --download to fetch the data.")
        return

    symbols_part = "-".join(args.symbols)
    name = f"{args.dataset}_{args.schema}_{symbols_part}_{args.start}_{args.end}".replace(":", "")
    DATA_DIR.mkdir(exist_ok=True)
    out_path = DATA_DIR / f"{name}.dbn.zst"
    client.timeseries.get_range(**query, path=out_path)
    print(f"Saved {out_path}")


if __name__ == "__main__":
    main()
