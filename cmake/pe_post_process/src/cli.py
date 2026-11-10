import argparse
from pathlib import Path

from .processor import process


def main() -> None:
    parser = argparse.ArgumentParser(description="Post-process a PE executable")

    parser.add_argument(
        "map_file",
        type=Path,
        help="Linker map file",
    )

    args = parser.parse_args()

    if not args.map_file.is_file():
        parser.error(f"Map file not found: {args.map_file}")

    process(args.map_file)
