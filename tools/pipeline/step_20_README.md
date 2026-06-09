# Pipeline Step 20 — Shipping Release

## Pre-release checklist
- [ ] All 20 pipeline steps completed
- [ ] C++ modules compiled with no warnings in Shipping
- [ ] Main_WP fully authored with all biomes, buildings, POIs
- [ ] All `AMistspire*` actors placed and configured
- [ ] Audio system fully wired (6 buses, per-biome ambience)
- [ ] OpenXR bindings validated for Meta Quest + Valve Index + HTC Vive
- [ ] 90 FPS sustained on target hardware
- [ ] Save/load round-trip tested

## Build output
```
WindowsNoEditor/
  Mistspire.exe
  Mistspire/
    Content/
      Maps/Main_WP.umap
      ...
```

## Distribution
- Steam build ID registered (App ID TBD)
- Git tag: v1.0.0
- LFS tracked: all `.uasset` + `.umap` + `.png` files
- AGENTS.md + README.md updated for this release tag

## Post-release
- Monitor leaderboard ghost count
- Track summit reach rate telemetry
- Prioritize biome-blending and audio for v1.1
