# Manual test — non-VR mode

Prerequisites: compile C++ module; open `Main_WP`; click **Play** (not VR Preview).

## Start screen

- [ ] Title **Mistspire** is centered over the live world; **Press any key to start** at the bottom
- [ ] WASD / look do nothing until a key or mouse button is pressed
- [ ] After start, title disappears, center + crosshair appears, and movement works

## Movement and camera

- [ ] WASD moves the pawn on the ground or fallback floor (accelerates in, coasts to a stop)
- [ ] Mouse look rotates the camera
- [ ] Standing still does not sink or bounce
- [ ] Space jumps only when grounded; the fall reaches the floor (no mid-air snap); landing does not sink
- [ ] Gravity returns pawn to ground after a jump

## Traversal

- [ ] Left Control (hold) near a vertical surface starts climbing
- [ ] Left Shift (hold) increases move speed (sprint) on flat ground
- [ ] F or right mouse fires grapple toward the center crosshair (+)
- [ ] Grapple line extends to the hit, then pulls; the far end stays on the hit when looking away
- [ ] Second F / right mouse while extending or pulling cancels the grapple
- [ ] Grapple reel does not pass through the floor; reeling upward still works
- [ ] G toggles glider
- [ ] Gliding into the floor lands, cancels the glider, and does not fall through
- [ ] T teleports forward (blink)

## Systems

- [ ] On-screen HUD shows altitude, stamina, O₂, weather, biome
- [ ] `mistspire.ShowControls 1` shows control hints without `mistspire.ShowAltitudeHUD 1`
- [ ] `mistspire.TeleportUp 5000` raises altitude; HUD updates
- [ ] `mistspire.AltitudeStats` logs to output log
- [ ] Walking into rest shelter / oxygen canister overlap refills (if placed)

## Interaction

- [ ] E on `AMistspirePhysicalButton` triggers built-in action
- [ ] E on `AMistspireLoreShard` shows lore line

## Packaged (optional)

- [ ] `Mistspire.exe -nonvr` launches without OpenXR runtime
- [ ] Keyboard/mouse controls work in packaged build
- [ ] With a headset connected, `-nonvr` still forces keyboard/mouse mode
- [ ] Without a headset, `-forcvr` / `-forcevr` attempts VR (may fail gracefully)

## VR regression

- [ ] VR Preview still uses controller input
- [ ] Packaged `-forcvr` forces VR when a runtime is available
- [ ] VR jump (controller) still works; desktop keys do not interrupt VR climb
