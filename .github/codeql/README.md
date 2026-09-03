# CodeQL Setup for deepiri-mistspire

This folder contains the CodeQL configuration for security scanning in this repository.

## What each file does

- `.github/workflows/codeql.yml`
  - Runs CodeQL for **C++** on pull requests (any target branch), Monday 06:00 UTC (`schedule`), and `workflow_dispatch`.
  - Does **not** run on `push` (avoids a second post-merge bill). Default-branch alerts come from the weekly schedule.
  - Docs-only PRs still start the workflow: `CI gate` always reports success; `Analyze (cpp)` no-ops so required checks do not hang.
  - Builds `native/xr-sandbox` between init and analyze so CodeQL sees compiled C/C++ (the Unreal game project is not built on hosted runners).
- `.github/scripts/pr_has_code_changes.py`
  - Shared path filter used by CodeQL and validate. Ignores `*.md` (including repo-root files), `docs/**`, `.gitignore`, and root `LICENSE*`.
- `.github/codeql/codeql-config.yml`
  - Include/ignore paths, with Unreal/VR-specific exclusions.

## CodeQL workflow breakdown (`.github/workflows/codeql.yml`)

### Triggers
```yaml
on:
  pull_request:
  schedule:
    - cron: "0 6 * * 1"
  workflow_dispatch:
```

There is no `branches:` filter, so stacked PRs into long-lived feature bases still get analysis. `dev` is not required to exist on `origin`.

Concurrency uses `cancel-in-progress: ${{ github.event_name == 'pull_request' }}` so a weekly or manual default-branch run is not cancelled mid-analyze.

### Permissions
```yaml
permissions:
  actions: read
  contents: read
  pull-requests: read
  security-events: write
```
`security-events: write` is required so CodeQL can upload findings. `pull-requests: read` is for the CI gate file list.

### Language setup
```yaml
with:
  languages: cpp
```
C# was dropped: the hosted job never builds Unreal/C# targets, which caused empty-analysis failures. Native OpenXR smoke-test code under `native/xr-sandbox` is the compilable C++ surface for CI.

### Build steps (required for cpp)
Between `codeql-action/init` and `codeql-action/analyze` (skipped on docs-only PRs):

1. Install `libopenxr-dev` + `pkg-config`
2. `cmake` configure `native/xr-sandbox`
3. `cmake --build` that sandbox

Checkout uses the action default fetch depth (full history is not required for this compile+analyze path).

### Analyze
```yaml
uses: github/codeql-action/analyze@v3
```
Runs queries and uploads results to GitHub Security. Job timeout is 30 minutes.

## Config breakdown (`.github/codeql/codeql-config.yml`)

### `paths-ignore`
Generated/build/runtime artifact paths are excluded to reduce noise and runtime:

```yaml
paths-ignore:
  - '**/node_modules/**'
  - '**/dist/**'
  - '**/build/**'
  - '**/coverage/**'
  - '**/logs/**'
  - '**/*.min.js'
  - 'game/Binaries/**'
  - 'game/Intermediate/**'
  - 'game/Saved/**'
  - 'game/Plugins/**/Binaries/**'
  - 'game/Plugins/**/Intermediate/**'
  - 'native/xr-sandbox/build/**'
  - 'native/xr-sandbox/bin/**'
  - 'scripts/**'
  - 'tests/manual/**'
```

With `languages: cpp`, CodeQL extraction follows the **build**. Most `paths` / `paths-ignore` entries in this config apply to interpreted languages; C++ exclusion has to come from what cmake actually compiles. The list above still documents intent and would apply if another interpreted language is added later.

## Related validate workflow

`.github/workflows/validate.yml` (not CodeQL) checks project JSON, shellcheck on `scripts/*.sh`, and pinned Ruff (`ruff==0.16.0` + repo `.ruff.toml`) on `scripts/*.py` and `.github/scripts/*.py`. It does **not** rebuild `native/xr-sandbox`; that compile is CodeQL's job so PRs do not pay for the same cmake twice.

Validate uses the same CI gate / docs-only no-op pattern, `permissions: contents: read` plus `pull-requests: read`, lint timeouts of 10 minutes, and `cancel-in-progress` only on `pull_request`.

## Best practices

1. Keep default-branch CodeQL on a cheap `schedule` (not a post-merge `push` double-fire).
2. Keep language list aligned with what CI can actually compile.
3. Exclude generated/vendor artifacts in `paths-ignore` (interpreted languages; cmake for C++).
4. Pin to stable major action versions (`@v3`).
5. Triage high/critical CodeQL alerts first.

## Maintenance

Only change `languages` when CI gains another compilable language target. To exclude another generated folder, add a glob under `paths-ignore`. Update this README when workflow triggers, concurrency, or timeouts change.
