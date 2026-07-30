# Mistspire immersion features

## Traversal (VR)

| Input (Quest / Index) | Action |
|----------------------|--------|
| Grip | Grab / climb |
| Left stick | Move + strafe |
| Right stick X | Turn |
| Right trigger | **Grapple** (line trace); hold to reel faster |
| B button | **Glider** toggle |
| Menu | Teleport blink forward |
| Hands high + forward | **Mantle** ledge assist |

## Survival

- **Stamina** drains while climbing/gliding; **electric storms** drain passively.
- **Oxygen** drains above ~5 km; hypoxia = shake + vignette + heartbeat.
- **Rest shelters** — overlap refill + haptics.
- **Oxygen canisters** — one-shot O₂ pickup (consumable).
- **Wind crystals** — glider boost + auto-deploy glider.

## World systems

| Subsystem / Actor | Role |
|-------------------|------|
| `UMistspireZoneSubsystem` | Valley → Mist → Alpine → Thin Air → Zenith |
| `UMistspireNarrativeSubsystem` | Zone lines, km milestones, summit fanfare |
| `UMistspireBeaconSubsystem` | Wrist bearing; pulse haptics when close |
| `UMistspireCompanionSubsystem` | Spawns **guide spirit** orb toward next summit |
| `UMistspireGhostClimberSubsystem` | **Ghost pillars** for other players on leaderboard |
| `UMistspireAmbienceSubsystem` | Tension/mystery levels for wind audio |
| `UMistspireEnvironmentSubsystem` | Weather, wind, mist, day/night, aurora |
| `UMistspireProgressSubsystem` | Save/load PB altitude + summits |
| `AMistspireSummitMarker` | Pulsing beacon light + label |
| `AMistspirePhysicalButton` | VR push button; optional CycleWeather / Refill / TeleportUp |

## Wrist HUD (left hand)

Altitude, stamina, oxygen, beacon (▲ km + bearing), boost timer when active.

## Progress

Auto-saves on **new personal best** and **summit reached**. Slot: `MistspireProfile`.

## Debug console

```
mistspire.ShowAltitudeHUD 1
mistspire.SetWeather 2          # 0–3
mistspire.RefillSurvival
mistspire.SaveProgress
mistspire.LoadProgress
mistspire.TeleportUp 5000
mistspire.AltitudeStats
```

## Huge world & buildings

- **12 districts** (Valley Haven → Zenith Dock) with flavor text on entry
- **16 enterable buildings** — walk through door volumes; pocket interiors at X ≈ 50 km+
- **POI discovery** — viewpoints, lore plaques, landmarks
- Auto-spawned doors/POIs on play; see [BUILDINGS_AND_INTERIORS.md](BUILDINGS_AND_INTERIORS.md)

```
mistspire.TeleportDistrict 0
mistspire.ExitInterior
mistspire.RespawnWorldMarkers
```

## Level authoring

Place in `Main_WP`:

- `AMistspireSummitMarker` — set `SummitId`, `ReachRadiusCm`
- `AMistspireRestShelter` — camps
- `AMistspireOxygenCanister` / `AMistspireWindCrystal` — pickups
- `AMistspirePhysicalButton` — set `BuiltInAction` for weather shrine, etc.
- `AMistspireBuildingEntrance` / `AMistspireInteriorExit` — or use auto-spawn from atlas
- `AMistspirePOIMarker`, `AMistspireLoreShard` — world detail

Assign wind/heartbeat/summit **audio** on `AMistspireVRPawn` in Blueprint.
