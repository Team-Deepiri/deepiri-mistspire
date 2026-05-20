#!/usr/bin/env python3
"""Hoist legacy deepiri-cloudloom/ nested tree to repo root (one-time migration)."""
from __future__ import annotations

import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
NESTED = ROOT / "deepiri-cloudloom"


def main() -> None:
    if not NESTED.is_dir():
        print("Nothing to hoist — repo layout is already flat.")
        return

    for child in NESTED.iterdir():
        dest = ROOT / child.name
        if dest.exists():
            shutil.rmtree(dest) if dest.is_dir() else dest.unlink()
        shutil.move(str(child), str(dest))
    NESTED.rmdir()
    print(f"Hoisted {NESTED} -> {ROOT}")

    removed = 0
    for path in sorted(ROOT.rglob("*"), key=lambda p: len(p.parts), reverse=True):
        if "Cloudloom" in path.name or "Skyreach" in path.name:
            if path.is_dir():
                shutil.rmtree(path, ignore_errors=True)
            else:
                path.unlink(missing_ok=True)
            removed += 1
    print(f"Removed {removed} legacy paths.")


if __name__ == "__main__":
    main()
