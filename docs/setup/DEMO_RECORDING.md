# Demo recording cheat sheet

Owner-facing sequence for a dumbed-down Mistspire presentation **before** the custom map pass. Systems are driven by altitude, console teleports, and Joe’s AI hooks — not authored PCG yet.

## Branch / build

- Branch: `quang_nguyen/feat/demo_world`
- Engine: **UE 5.8** — open `game/Mistspire.uproject`
- Prefer **Play** (non-VR) for flat capture; **VR Preview** for headset footage

### Windows launch

```powershell
.\setup.ps1 -NonInteractive   # first time / deps
.\run.ps1                     # editor
.\run-nonvr.ps1               # editor + -nonvr
.\scripts\launch_packaged_win64.ps1   # packaged non-VR
```

Demo presentation on launch:

```text
.\run-nonvr.ps1
# or add to editor cmdline / packaged args:
-demoworld
```

Or in console after PIE: `mistspire.DemoMode 1` then `mistspire.ApplyDemoPresentation`.

## Recording sequence

| Beat | Command / action |
|------|------------------|
| Title / start | Press any key (non-VR) |
| HUD | Already on in demo; else `mistspire.ShowAltitudeHUD 1` |
| Welcome + ghosts | Auto with `-demoworld`; else `mistspire.ApplyDemoPresentation` |
| Weather | `mistspire.SetWeather 0` … `3` (Clear / MistStorm / Electric / ZenithGlow) |
| Biome tour | `mistspire.DemoTour 0` … `9` (Mist → Pinnacle mid-bands + forced visuals) |
| Survival | Climb / grapple / glider; `mistspire.RefillSurvival` if needed |
| AI (Joe) | `mistspire.Speak companion_greeting` · `mistspire.SpawnGhostSim` · `mistspire.AIThink` · `mistspire.GOAPPlan BeaconReached` · `mistspire.StateMachineDebug` |
| RL telemetry | `mistspire.ObservationStart` … `mistspire.ObservationStop` |
| Progress | `mistspire.SaveProgress` / `mistspire.LoadProgress` |
| Altitude | `mistspire.AltitudeStats` · `mistspire.TeleportUp 5000` |

## Summit registry (demo altitudes)

Seeds sit near biome mid-bands (cm), offset from origin XY for a compact teleport tour: Mist 0.5 km, Arid 2 km, Forest 4 km, Ember 6 km, Crystal 8 km, Void 10.5 km, Tundra 13 km, Aether 15 km, Sanctum 17 km, Pinnacle 19 km.

`mistspire.DemoTour -1` clears forced biome visuals after the tour.

## Unreal MCP (map pass later)

With the editor up: `http://127.0.0.1:8000/mcp` should return **405**. Clients: Cursor, `claude`, `agy`. See [AGENTS.md](../../AGENTS.md).

## Out of scope for this sheet

C++ climb scaffold, PCG biome meshes, Valley of the Ancient.
