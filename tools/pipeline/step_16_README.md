# Pipeline Step 16 — Debug + Dev Tools

## Physical debug buttons
Place `AMistspirePhysicalButton` in a dev-only layer:

| Button Label | BuiltInAction | Location |
|-------------|---------------|----------|
| Refill | `RefillSurvival` | Near player start |
| Teleport Up | `TeleportUp` (5000) | Near player start |
| Weather Cycle | `SetWeather` (cycle 0–3) | On test platform |
| Save | `SaveProgress` | Near player start |

## Console cheats (shipping disabled)
```
mistspire.AltitudeStats
mistspire.ShowAltitudeHUD 1/0
mistspire.TeleportUp <cm>
mistspire.SetWeather 0/1/2/3
mistspire.RefillSurvival
mistspire.SaveProgress
mistspire.LoadProgress
mistspire.TeleportDistrict <id>
mistspire.ExitInterior
mistspire.RespawnWorldMarkers
```

All cheats gated behind `WITH_EDITOR` or cheat manager in shipping builds.

## HUD debug overlay
Toggle with `mistspire.ShowAltitudeHUD 1`:
- Altitude (m / ft)
- Stamina / O₂ bars
- Beacon: target name + bearing + distance
- Boost timer
- Current biome name
