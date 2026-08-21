# Pipeline Step 10 — Survival Pickups

Place `AMistspireOxygenCanister` and `AMistspireWindCrystal` along the climb:

**Oxygen Canisters** (spawn every 200–400 m above 5 km):
- One-shot O₂ refill (handled by survival component)
- Visual: Glowing blue canister with beacon
- Respawn on zone reload

**Wind Crystals** (spawn near glider-required gaps):
- Grants glider boost + auto-deploy
- Visual: Spinning crystal with wind VFX

**Stamina restoration:**
- Passive near Rest Shelters (placed in step 6)
- No separate pickup needed — stamina auto-refills at shelters

**Placement density:**
| Altitude | O₂ Canisters | Wind Crystals |
|----------|-------------|---------------|
| 0–5 km | None | 2–3 (tutorial) |
| 5–8 km | 5–8 | 4–5 |
| 8–12 km | 8–12 | 4–6 |
| 12–16 km | 10–15 | 3–4 |
| 16–20 km | 5–8 | 2–3 |

**See:** `UMistspireSurvivalComponent` in `MistspireVRPawn.cpp`
