#!/usr/bin/env python3
"""Cross-platform end-to-end regression tests for Walrus and Oscar."""

import argparse
import re
import subprocess
import tempfile
from pathlib import Path
from typing import Optional


FIXTURES = Path(__file__).parent / "windows" / "configs"


def run_walrus(walrus: Path, config: Path, result: Optional[Path] = None, timeout: int = 10):
    command = [str(walrus), "-cfgname", str(config), "-exitondone"]
    if result is not None:
        command.extend(["-logresult", str(result)])
    completed = subprocess.run(command, capture_output=True, text=True, timeout=timeout)
    print(completed.stdout)
    if completed.stderr:
        print("Walrus stderr:")
        print(completed.stderr)
    if completed.returncode != 0:
        raise AssertionError(f"Expected legacy Walrus exit code 0, got {completed.returncode}")
    return completed.stdout


def require(text: str, pattern: str, message: str):
    if re.search(pattern, text, re.MULTILINE) is None:
        raise AssertionError(message)


def missing_config(walrus: Path, temporary: Path):
    output = run_walrus(walrus, temporary / "config-does-not-exist.txt")
    require(output, r"config-does-not-exist\.txt' not found", "Missing expected not-found diagnostic")
    require(output, r"Failed to init configuration", "Missing configuration-failure diagnostic")


def invalid_config(walrus: Path, _temporary: Path):
    cases = (
        ("no-selected-task", "Error: No task in selected in the config file"),
        ("unknown-task", "Error: Task 'REGRESSION_UNKNOWN_TASK' not found in the config file"),
        ("malformed-hand", "Error: pls put fixed hand on NORTH, N. Your line is: [INVALID]"),
        ("invalid-scorer", "Failed to parse prima scorer"),
        ("invalid-filter", "Unrecognized filter name DefinitelyNotAFilter"),
        ("missing-task-type", "Error: 'TASK_TYPE:' line is missing."),
    )
    for fixture, expected in cases:
        output = run_walrus(walrus, FIXTURES / fixture / "start_from.txt")
        require(output, re.escape(expected), f"Missing expected diagnostic for {fixture}: {expected}")
        require(output, r"Failed to init configuration", f"Missing configuration-failure diagnostic for {fixture}")


def successful_calculation(walrus: Path, temporary: Path):
    result_path = temporary / "oscar-result.txt"
    run_walrus(
        walrus,
        FIXTURES / "deterministic-grand-slam" / "start_from.txt",
        result_path,
        timeout=60,
    )
    if not result_path.is_file():
        raise AssertionError("Oscar did not create the deterministic calculation result file")
    result = result_path.read_text(errors="replace")
    print("Oscar result:")
    print(result)
    require(result, r"Primary scorer \(spades, 13 tr\):", "Missing expected 7S primary scorer diagnostic")
    require(result, r"Processed: [1-9][0-9]* total\. East is on lead\. Goal is 13 tricks in spades\.",
            "Missing expected completed-calculation summary")
    require(result, r"Averages:\s+ideal = 1510, 7S = 1510, 6S = 1010\.\s+Chance to make = 100\.0%\.",
            "Unexpected deterministic grand-slam result")


def statistical_calculation(walrus: Path, temporary: Path):
    result_path = temporary / "oscar-result.txt"
    run_walrus(
        walrus,
        FIXTURES / "seven-d-or-seven-nt" / "start_from.txt",
        result_path,
        timeout=60,
    )
    if not result_path.is_file():
        raise AssertionError("Oscar did not create the 7D/7NT calculation result file")
    result = result_path.read_text(errors="replace")
    print("Oscar result:")
    print(result)
    require(result, r"Primary scorer \(diamonds, 13 tr\):", "Missing expected 7D primary scorer diagnostic")
    require(result, r"Contract-B scorer \(notrump, 13 tr\):", "Missing expected 7NT scorer diagnostic")
    require(result, r"Processed: [1-9][0-9]* total\. West is on lead\. Goal is 13 tricks in diamonds\.",
            "Missing expected completed 7D/7NT calculation summary")

    primary = re.search(r"Averages:\s+7D = (?P<average>-?[0-9]+)\.\s+Chance to make = (?P<make>[0-9]+\.[0-9])%\.", result)
    secondary = re.search(r"7N: avg = (?P<average>-?[0-9]+); makes in\s+(?P<make>[0-9]+\.[0-9])% cases", result)
    if primary is None or secondary is None:
        raise AssertionError("Missing expected 7D/7NT result fields")
    if int(primary["average"]) <= int(secondary["average"]):
        raise AssertionError("Expected 7D average to exceed 7NT average")
    if float(primary["make"]) <= float(secondary["make"]):
        raise AssertionError("Expected 7D make percentage to exceed 7NT make percentage")


CASES = {
    "missing-config": missing_config,
    "invalid-config": invalid_config,
    "successful-calculation": successful_calculation,
    "statistical-calculation": statistical_calculation,
}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--walrus", required=True, type=Path)
    parser.add_argument("--case", required=True, choices=CASES)
    arguments = parser.parse_args()
    walrus = arguments.walrus.resolve(strict=True)
    with tempfile.TemporaryDirectory(prefix="bodealer-regression-") as directory:
        CASES[arguments.case](walrus, Path(directory))
    print(f"PASS: {arguments.case}")


if __name__ == "__main__":
    main()
