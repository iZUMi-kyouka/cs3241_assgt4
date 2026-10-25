"""Run clang-format without exposing a fake compiler command to IDE importers."""

import argparse
import subprocess
from pathlib import Path


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--formatter", default="clang-format")
parser.add_argument("--check", action="store_true")
args = parser.parse_args()

root = Path(__file__).resolve().parent.parent
options = ["--dry-run", "--Werror"] if args.check else ["-i"]
try:
    result = subprocess.run([args.formatter, *options, "main.cpp"], cwd=root)
except FileNotFoundError:
    parser.exit(1, f"Formatter not found: {args.formatter}. Install requirements-dev.txt.\n")
raise SystemExit(result.returncode)
