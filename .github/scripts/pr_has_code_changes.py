#!/usr/bin/env python3
"""Decide whether a PR has non-ignored (code) path changes.

Always run code jobs for workflow_dispatch, schedule, and push.
For pull_request, list changed files via the GitHub API and treat a PR as
docs-only when every path matches the ignore set (markdown, docs/, .gitignore,
root LICENSE*).

Writes run_code=true|false to $GITHUB_OUTPUT.
"""

from __future__ import annotations

import json
import os
import sys
import urllib.error
import urllib.request


def is_ignored(path: str) -> bool:
    path = path.replace("\\", "/")
    if path.endswith(".md"):
        return True
    if path == "docs" or path.startswith("docs/"):
        return True
    if path == ".gitignore":
        return True
    if "/" not in path and path.startswith("LICENSE"):
        return True
    return False


def write_output(run_code: bool) -> None:
    value = "true" if run_code else "false"
    out_path = os.environ.get("GITHUB_OUTPUT")
    if out_path:
        with open(out_path, "a", encoding="utf-8") as handle:
            handle.write(f"run_code={value}\n")
    print(f"run_code={value}")


def list_pr_files(repo: str, number: str, token: str) -> list[str]:
    files: list[str] = []
    page = 1
    while True:
        url = (
            f"https://api.github.com/repos/{repo}/pulls/{number}/files"
            f"?per_page=100&page={page}"
        )
        request = urllib.request.Request(
            url,
            headers={
                "Authorization": f"Bearer {token}",
                "Accept": "application/vnd.github+json",
                "X-GitHub-Api-Version": "2022-11-28",
                "User-Agent": "mistspire-ci-gate",
            },
        )
        with urllib.request.urlopen(request) as response:
            batch = json.load(response)
        if not isinstance(batch, list):
            raise RuntimeError(f"Unexpected PR files payload: {batch!r}")
        files.extend(str(item["filename"]) for item in batch)
        if len(batch) < 100:
            break
        page += 1
    return files


def main() -> int:
    event_name = os.environ.get("GITHUB_EVENT_NAME", "")
    if event_name in ("workflow_dispatch", "schedule", "push"):
        write_output(True)
        return 0

    token = os.environ.get("GITHUB_TOKEN", "")
    repo = os.environ.get("GITHUB_REPOSITORY", "")
    number = os.environ.get("PR_NUMBER", "")
    if not token or not repo or not number:
        print("Missing GITHUB_TOKEN, GITHUB_REPOSITORY, or PR_NUMBER; running code jobs.", file=sys.stderr)
        write_output(True)
        return 0

    try:
        changed = list_pr_files(repo, number, token)
    except urllib.error.URLError as exc:
        print(f"Failed to list PR files ({exc}); running code jobs.", file=sys.stderr)
        write_output(True)
        return 0

    run_code = any(not is_ignored(path) for path in changed)
    write_output(run_code)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
