# Pipeline Step 18 — QA Test Pass

## Functional tests (in editor PIE + VR Preview)

| Test | Expected |
|------|----------|
| Player spawns at origin | Yes, at 0 altitude |
| Move + turn with stick | Smooth locomotion |
| Grip climb on tagged surfaces | Player moves along surface |
| Grapple line trace + reel | Line shows, player moves |
| Glider toggle | Deploy/retract with fall speed reduction |
| O₂ drain above 5 km | Vignette + heartbeat at < 20% |
| Rest shelter overlap | O₂ + stamina refill |
| Summit marker reach | Fanfare + HUD banner + save |
| Building enter/exit | Teleport to interior pocket, return |
| Beacon HUD | Bearing + distance updates |

## VR comfort tests
- [ ] No motion sickness from smooth turn
- [ ] Teleport arc visible and accurate
- [ ] Mantle assist doesn't cause camera snap
- [ ] HUD wrist attachment stable

## Edge cases
- Fall below 0 altitude → respawn at last checkpoint
- O₂ depleted at high altitude → death respawn
- Building interior while stamina low → safe zone refill
- All 10 biomes transition smoothly (no pop-in)
- Summit reach at all 5 authored summit IDs
