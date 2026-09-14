# Demo recording cheat sheet

Owner-facing sequence for Mistspire demo presentation (Demo Spire + Fab env dress on Main_WP). Systems are driven by altitude, console teleports, Joe’s AI hooks, and hybrid env dress.

## Branch / build

- Branch: `quang_nguyen/feat/demo_world`
- Engine: **UE 5.8** — open `game/Mistspire.uproject`
- Prefer **Play** (non-VR) for flat capture; **VR Preview** for headset footage
- Do **not** open the PR until the acceptance checklist below is green

### Windows launch

```powershell
.\setup.ps1 -NonInteractive   # first time / deps
.\run.ps1                     # editor
.\run-nonvr.ps1               # editor + -nonvr
.\scripts\launch_packaged_win64.ps1   # packaged non-VR (-demoworld)
```

Demo presentation on launch:

```text
.\run-nonvr.ps1
# or add to editor cmdline / packaged args:
-demoworld
```

Or in console after PIE: `mistspire.DemoMode 1` (seeds Mist Inn doors + scaffold if StartPlay ran without `-demoworld`) then `mistspire.ApplyDemoPresentation` if you only need the HUD/ghost beat again.

## Recording sequence

| Beat | Command / action |
|------|------------------|
| Title / start | Press any key (non-VR) |
| HUD | Already on in demo; else `mistspire.ShowAltitudeHUD 1` |
| Welcome + ghosts | Auto with `-demoworld`; else `mistspire.ApplyDemoPresentation` |
| Mist Inn | Walk -Y to porch (~Origin + (400,-1700)); overlap trigger → pocket. Exit through the +X door wall → returns to porch feet (`GetMistInnExitReturnLocation`). |
| Valley dress | Summit shelf at ValleyOrigin `(0,0,3200)` + persistent `DemoEnv_*` + runtime Fab soft-refs |
| Weather | Porch button (plaza edge of the porch, clear of the door trigger) or `mistspire.SetWeather 0` … `3`. Sky / fog / sun tint should visibly shift (grey mist, purple storm, warm zenith) — not HUD-only. |
| Approach / grapple | Walk stairs to 60 m, grapple a few shaft pads |
| Biome tour | `mistspire.DemoTour 0` … `9` (Mist → Pinnacle + forced visuals) |
| Survival | Climb / grapple / glider; `mistspire.RefillSurvival` if needed |
| Joe AI beat | `mistspire.DemoJoeBeat` (or Speak / SpawnGhostSim / AIThink / GOAPPlan manually) |
| RL telemetry | `mistspire.ObservationStart` … `mistspire.ObservationStop` |
| Progress | `mistspire.SaveProgress` / `mistspire.LoadProgress` |
| Altitude | `mistspire.AltitudeStats` · `mistspire.TeleportUp 5000` |
| Scaffold refresh | `mistspire.RebuildDemoScaffold` |

## Summit registry (demo altitudes)

Seeds follow the **Demo Spire helix** ([DEMO_WORLD.md](../gameplay/DEMO_WORLD.md)): Mist 0.5 km … Pinnacle 19 km at radius 2500 cm, 36° steps. `mistspire.DemoTour` lands on those pads.

`mistspire.DemoTour -1` clears forced biome visuals after the tour.

Geometry: auto-spawned by `AMistspireDemoClimbScaffold` under `-demoworld`, or `mistspire.RebuildDemoScaffold`.

## Acceptance checklist (PR gate)

Record or soak both paths before opening a PR:

- [ ] **Non-VR Play** (`-demoworld`): valley spawn, Mist Inn enter/exit, weather button or SetWeather, DemoTour 0–9, DemoJoeBeat
- [ ] **VR Preview**: same beats; approach stairs + a few grapple pads feel readable
- [ ] **Stress**: DemoTour 0→9 quickly; FPS acceptable for capture (no hitch loop)
- [ ] **Packaged Win64**: `scripts/launch_packaged_win64.ps1` boots with `-demoworld`, HUD + tour work

### Known jank guards (verify these still hold)

- Stepping off the `ShelfPad` or a station pad returns you to the Valley Gate — the template
  landscape is collision-disabled, so without the catch a miss is an endless fall.
- The porch weather button is outside the Mist Inn door volume; pressing it must not teleport you.
- `mistspire.DemoTour N` lands on the inner half of each pad, clear of the needle / plinth / pier.
- Running `DemoTour` while inside the Mist Inn exits the interior first, so the door still works after.

## Unreal MCP (optional polish)

With the editor up: streamable HTTP at `http://127.0.0.1:8000/mcp`. Clients: Cursor, `claude`, `agy`. See [AGENTS.md](../../AGENTS.md).

## Out of scope for this sheet

Production PCG biome meshes, Valley of the Ancient.
