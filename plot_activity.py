import argparse
import csv

import matplotlib.pyplot as plt


def read_activity(path: str):
    x = []
    y = []
    with open(path, "r", newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            x.append(int(row["bucket_start_sec"]))
            y.append(int(row["bytes"]))
    return x, y


def main():
    parser = argparse.ArgumentParser(
        description="Draw network activity graph from CSV produced by pcap_indicator."
    )
    parser.add_argument(
        "-i", "--input", default="activity.csv", help="Path to input CSV file"
    )
    parser.add_argument(
        "-o",
        "--output",
        default="activity.png",
        help="Output image path (PNG)",
    )
    parser.add_argument(
        "--show",
        action="store_true",
        help="Show interactive window in addition to saving PNG",
    )
    args = parser.parse_args()

    x, y = read_activity(args.input)
    if not x:
        raise SystemExit("No data found in input CSV.")

    plt.figure(figsize=(10, 5))
    plt.plot(x, y, marker="o", linewidth=1.5)
    plt.title("Network activity over time")
    plt.xlabel("Time from start, sec")
    plt.ylabel("Captured bytes per interval")
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(args.output, dpi=150)
    print(f"Saved graph to {args.output}")

    if args.show:
        plt.show()


if __name__ == "__main__":
    main()
