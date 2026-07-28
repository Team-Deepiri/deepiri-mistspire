# Day 1 Maintainer Checklist

Use this to answer one question: **does the project compile, load, and run its core systems?**

This repo ships **C++ gameplay code** but **no map or content assets**. Expect to create `Main_WP` and add minimal geometry before anything feels like a game.

---

## Before you start

| Requirement | Notes |
|-------------|-------|
| Unreal Engine **5.8+** | Must match `game/Mistspire.uproject` (`EngineAssociation: 5.8`) |
| Visual Studio 2022 | C++ workload + Windows SDK (for Win64 compile) |
| Git LFS | `git lfs install` then `git lfs pull` (no assets today, but required for future pulls) |
| VR headset + OpenXR runtime | SteamVR (Index/Vive) or Meta Link (Quest). Optional for compile-only pass |
| GPU | Vulkan-capable; project targets high-end PCVR |

**Repo entry point:** `game/Mistspire.uproject`

---

## Phase 1 — Compile check (no VR)

Goal: confirm both C++ modules build.

- [ ] Clone repo and open `game/Mistspire.uproject`
- [ ] When prompted, allow **Generate Visual Studio project files** and **compile**
- [ ] Confirm modules compile without errors:
  - `Mistspire`
  - `MistspireOpenXRNative`
- [ ] In **Edit → Plugins**, confirm enabled:
  - OpenXR
  - MistspireOpenXRNative
  - OpenXRHandTracking (optional)
- [ ] Open **Output Log** and filter for `Error` — should be clean after compile

**Pass:** Editor opens, modules show as compiled, no red errors in Output Log.

**Fail clues:**
- Missing UE 5.8 → wrong engine association
- OpenXR plugin disabled → VR pawn input won't work
- Compile errors in `MistspireOpenXRNative` → check OpenXR / VS toolchain

---

## Phase 2 — Create a playable map

`DefaultEngine.ini` points at `/Game/Maps/Main_WP`, which **does not exist in git**. The editor will warn or fail to load the default map until you create it.

### Option A — Manual (recommended for day 1)

- [ ] **File → New Level → Empty Open World**
- [ ] **File → Save Current Level As…** → `Content/Maps/Main_WP`
- [ ] **Window → World Settings**
  - Game Mode Override: **MistspireGameMode** (C++ class under Mistspire)
  - Enable **World Partition** if not already on
- [ ] Place a **PlayerStart** near `(0, 0, 200)` (200 cm above origin)
- [ ] Save map

### Option B — Python script (may need fixes)

`tools/pipeline/step_01_create_map.py` can auto-create the map, but it references a Blueprint game mode asset that may not exist:

```
/Game/Mistspire/MistspireGameMode.MistspireGameMode_C
```

If the script errors, fall back to **Option A** and set game mode manually to the C++ `MistspireGameMode`.

- [ ] **Tools → Execute Python Script** → select `tools/pipeline/step_01_create_map.py`
- [ ] Verify `Content/Maps/Main_WP` appears in Content Browser
- [ ] Set game mode to C++ `MistspireGameMode` if script didn't

**Pass:** Double-clicking `Main_WP` opens a level with a PlayerStart and no missing-map errors.

---

## Phase 3 — Minimum geometry (5 minutes)

You need *something* to stand on and climb. Without this, VR Preview spawns in empty space.

- [ ] Add a **Landscape** *or* a few **Cube** static meshes scaled as platforms
- [ ] Minimum viable setup:
  - Ground plane at Z ≈ 0 (e.g. 20×20 m cube, scale Z = 0.2)
  - One vertical wall or stacked cubes for climb testing
  - Optional: ramp or stairs to test locomotion collision
- [ ] Ensure **PlayerStart** is above the ground (not inside geometry)
- [ ] Save map

**Pass:** **Play (non-VR)** spawns a pawn on solid geometry (you may see nothing if camera isn't configured for non-VR — that's OK; check pawn exists in World Outliner).

---

## Phase 4 — VR Preview smoke test

### Windows OpenXR prep

- [ ] Put on headset; confirm it tracks in SteamVR or Meta Link
- [ ] Set active OpenXR runtime:
  - **SteamVR:** Settings → Developer → set SteamVR as OpenXR runtime
  - **Meta Quest Link:** Meta Quest Developer Hub → OpenXR runtime
- [ ] Close other VR apps that might hold the runtime

### Launch

- [ ] Open `Main_WP`
- [ ] **Play → VR Preview** (not plain Play)
- [ ] Headset should show stereo view; look for wrist text or on-screen HUD

**Pass:**
- VR Preview starts without crash
- You see the world (even if untextured gray boxes)
- On-screen HUD shows altitude in meters (cyan text, top-left area)

**Fail clues:**
- Black screen / no HMD → OpenXR runtime not active
- Immediate crash → check Output Log for OpenXR or rendering errors
- Pawn falls forever → no ground collision; add geometry (Phase 3)

---

## Phase 5 — Console command verification

Open the console with **`** (tilde) in editor/PIE, or **` (backtick) depending on keyboard layout.

Run commands in order. Check **Output Log** (`Window → Developer Tools → Output Log`) for `LogTemp` / `LogMistspire` lines.

### Core loop

| Command | Expected result | Proves |
|---------|-----------------|--------|
| `mistspire.AltitudeStats` | Log: `current=…cm personal_best=…cm` | Altitude subsystem alive |
| `mistspire.TeleportUp 5000` | Player moves +50 m on Z | Pawn + world coordinates work |
| `mistspire.ShowAltitudeHUD 1` | On-screen HUD visible (altitude, zone, weather, STA/O₂) | Debug HUD subsystem |
| `mistspire.RefillSurvival` | Stamina/O₂ bars (HUD or wrist) go to full | Survival on pawn |

- [ ] All four commands behave as expected

### Progress / summits

| Command | Expected result | Proves |
|---------|-----------------|--------|
| `mistspire.TeleportUp 20000` | Move near first seeded summit (`summit_valley_gate` at Z=20000) | Summit registry coordinates |
| Wait / move slightly | Possible summit-reached notification if within **500 cm** of summit point | Summit detection |
| `mistspire.SaveProgress` | Log: `Mistspire: progress saved.` | Save subsystem |
| Restart PIE, then `mistspire.LoadProgress` | Log: `Mistspire: progress loaded.`; PB altitude restored | Load on start |

- [ ] Save/load round-trip works

### World / environment

| Command | Expected result | Proves |
|---------|-----------------|--------|
| `mistspire.SetWeather 2` | HUD weather line changes to **Electric Turmoil** | Environment subsystem |
| `mistspire.TeleportDistrict 0` | Teleport to Valley Haven district center (+250 m Z) | World atlas (12 districts) |
| `mistspire.RespawnWorldMarkers` | Building door volumes + POI markers respawn | Runtime marker spawn |

- [ ] Weather changes on HUD
- [ ] District teleport works (may land in empty space — that's OK)
- [ ] After respawn, look near origin for text labels (building signs, POI lights)

### Interiors (optional)

| Step | Expected result | Proves |
|------|-----------------|--------|
| `mistspire.TeleportDistrict 0` then walk to `(12000, -8000, 18000)` or use editor to move pawn there | Overlap with **Mist Inn** door volume | Building entrance spawn |
| Walk into door overlap | Teleport to far pocket interior coords; HUD shows `INTERIOR: building_valley_inn` | Interior subsystem |
| `mistspire.ExitInterior` | Return to previous outdoor location | Exit flow |

- [ ] Interior enter/exit works (geometry at pocket coords won't exist yet — teleport alone is enough)

### Debug / visuals / audio

| Command | Expected result | Proves |
|---------|-----------------|--------|
| `mistspire.VisualDebug` | Log: biome, bloom, fog values | Visual enhancement subsystem |
| `mistspire.ForceBiomeVisuals 4` | Post-process shift (Ember biome preset) | Biome visuals |
| `mistspire.DebugAudioStats` | Log: 6 audio bus states | Audio subsystem (no sound files expected) |

- [ ] Visual debug logs appear
- [ ] Audio debug logs appear (warnings about missing sound cues are **expected** — no Content assets)

---

## Phase 6 — Input check (likely partial)

OpenXR actions are created in C++ (`mistspire_gameplay`), but JSON binding files under `interaction_profiles/openxr/` are **not automatically loaded** into the runtime today.

Test in VR Preview:

| Input | Expected if bindings work | If nothing happens |
|-------|-------------------------|---------------------|
| Left stick | Move / strafe | Bindings not wired — use console teleport instead |
| Right stick X | Turn | Same |
| Grip | Start climbing | Same |
| Menu tap | Blink teleport forward | Same |
| Menu hold (~1 s) | Toggle glider | Same |

- [ ] Note whether controller input works (pass = bonus; fail = known gap, not a compile failure)

Use console locomotion for all other tests if sticks don't respond.

---

## Quick pass/fail summary

| Tier | Criteria |
|------|----------|
| **Minimum pass** | Project compiles; `Main_WP` created; VR Preview launches; `mistspire.AltitudeStats` and `TeleportUp` work |
| **Good pass** | Above + on-screen HUD + save/load + weather + district teleport + runtime markers visible |
| **Full pass** | Above + controller locomotion + summit reached + interior teleport |

---

## Known gaps (don't file as bugs on day 1)

| Gap | Impact |
|-----|--------|
| No `.umap` / `.uasset` in repo | Must create map and geometry yourself |
| OpenXR JSON bindings not loaded at runtime | Controller input may be dead; C++ action set exists |
| No sound cues in Content | Audio subsystem runs but stays silent |
| No hand/body meshes assigned | Invisible or default VR hands |
| 10 biome C++ subsystems unused | `EnvironmentSubsystem::BiomeFromAltitude()` drives biomes instead |
| `UMistspireGameInstance` not in config | Multiplayer host/join code inactive |
| Summit reach radius = **500 cm** | Must be very close to seeded coords to trigger |
| Interior pockets are empty coords | Teleport works; no room geometry |

---

## If something fails — where to look

| Symptom | First check |
|---------|-------------|
| Won't open project | UE version 5.8; regenerate VS files |
| Compile error | Output Log; build `Mistspire` and `MistspireOpenXRNative` in VS |
| Missing map on open | Create `Main_WP` (Phase 2) |
| VR Preview black screen | OpenXR runtime active; try non-VR Play first |
| HUD missing | `mistspire.ShowAltitudeHUD 1` |
| Commands unknown | Module didn't load — recompile, restart editor |
| Pawn falls through floor | Collision on geometry; PlayerStart height |
| No controller input | Expected until bindings wired; use console |

---

## Related docs

- [PCVR_DEV_SETUP.md](PCVR_DEV_SETUP.md) — platform setup
- [ARCHITECTURE.md](ARCHITECTURE.md) — module overview
- [game/Content/Maps/README.md](../game/Content/Maps/README.md) — full map authoring
- [IMMERSION.md](IMMERSION.md) — feature list vs console commands
