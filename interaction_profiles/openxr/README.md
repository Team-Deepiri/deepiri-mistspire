# OpenXR profiles for Mistspire

Action set: `mistspire_gameplay` — matches `UMistspireXRActionSubsystem` in C++.

## Binding JSON files (device mapping)

- `bindings_meta_quest.json` — Meta Quest (Touch / Link); OpenXR profile `/interaction_profiles/oculus/touch_controller`
- `bindings_valve_index.json` — Valve Index
- `bindings_htc_vive.json` — HTC Vive
- `bindings_khr_simple.json` — fallback/simple controller

## Common actions

Bindings include: `move`, `strafe`, `turn`, `grip`, `jump`, `climb`, `grapple`, `glider`, `menu`, `teleport`.
