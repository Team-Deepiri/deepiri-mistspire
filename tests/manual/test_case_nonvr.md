# Manual test — non-VR mode

Prerequisites: compile C++ module; open `Main_WP`; click **Play** (not VR Preview).

## Movement and camera

- [ ] WASD moves the pawn on the ground or fallback floor
- [ ] Mouse look rotates the camera
- [ ] Space jumps; gravity returns pawn to ground

## Traversal

- [ ] Left Control (hold) near a vertical surface starts climbing
- [ ] Left Shift (hold) increases move speed (sprint) on flat ground
- [ ] F or right mouse fires grapple toward a surface in front
- [ ] G toggles glider
- [ ] T teleports forward (blink)

## Systems

- [ ] On-screen HUD shows altitude, stamina, O₂, weather, biome
- [ ] `mistspire.TeleportUp 5000` raises altitude; HUD updates
- [ ] `mistspire.AltitudeStats` logs to output log
- [ ] Walking into rest shelter / oxygen canister overlap refills (if placed)

## Interaction

- [ ] E on `AMistspirePhysicalButton` triggers built-in action
- [ ] E on `AMistspireLoreShard` shows lore line

## Packaged (optional)

- [ ] `Mistspire.exe -nonvr` launches without OpenXR runtime
- [ ] Keyboard/mouse controls work in packaged build

## VR regression

- [ ] VR Preview still uses controller input (`-forcvr` if testing packaged)
