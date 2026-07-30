# CodeQL Setup for deepiri-mistspire

This folder contains the CodeQL configuration for security scanning in this repository.

## What each file does

- `.github/workflows/codeql.yml`
  - Runs CodeQL for **C++** on PRs/pushes to `main` and `dev`.
  - Builds `native/xr-sandbox` between init and analyze so CodeQL sees compiled C/C++ (the Unreal game project is not built on hosted runners).
- `.github/codeql/codeql-config.yml`
  - Include/ignore paths, with Unreal/VR-specific exclusions.

## CodeQL workflow breakdown (`.github/workflows/codeql.yml`)

### Triggers
```yaml
on:
  pull_request:
    branches: [main, dev]
  push:
    branches: [main, dev]
```

### Permissions
```yaml
permissions:
  actions: read
  contents: read
  security-events: write
```
`security-events: write` is required so CodeQL can upload findings.

### Language setup
```yaml
with:
  languages: cpp
```
C# was dropped: the hosted job never builds Unreal/C# targets, which caused empty-analysis failures. Native OpenXR smoke-test code under `native/xr-sandbox` is the compilable C++ surface for CI.

### Build steps (required for cpp)
Between `codeql-action/init` and `codeql-action/analyze`:

1. Install `libopenxr-dev` + `pkg-config`
2. `cmake` configure `native/xr-sandbox`
3. `cmake --build` that sandbox

### Analyze
```yaml
uses: github/codeql-action/analyze@v3
```
Runs queries and uploads results to GitHub Security.

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

## Related validate workflow

`.github/workflows/validate.yml` (not CodeQL) checks project JSON, shellcheck on `scripts/*.sh`, pinned Ruff (`ruff==0.16.0` + repo `.ruff.toml`) on `scripts/*.py`, and builds `native/xr-sandbox`.

## Best practices

1. Keep trigger scope intentional (`main`, `dev`).
2. Keep language list aligned with what CI can actually compile.
3. Exclude generated/vendor artifacts in `paths-ignore`.
4. Pin to stable major action versions (`@v3`).
5. Triage high/critical CodeQL alerts first.

## Maintenance

Only change `languages` when CI gains another compilable language target. To exclude another generated folder, add a glob under `paths-ignore`.
