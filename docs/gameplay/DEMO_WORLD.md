# Demo World — Demo Spire

Owner-recording vertical slice for Mistspire. Runtime geometry is spawned by `AMistspireDemoClimbScaffold` when `-demoworld` / `mistspire.DemoMode 1` is active (or via `mistspire.RebuildDemoScaffold`).

## Fantasy

A compact **helix of stone stations** rising from a **mountain-top shelf** through ten mist bands. The Valley Gate and Mist Inn share that shelf so the template void never reads in the opening shot. Empty kilometers between stations stay fog; each station owns a short climb vignette so VR stays readable.

## Layout (source of truth)

Shared constants: [`MistspireDemoSpireLayout.h`](../../game/Source/Mistspire/Public/MistspireDemoSpireLayout.h)

| Param | Value |
|-------|-------|
| ValleyOrigin | `(0, 0, 50)` — gate village / helix root (near floor; mountain mass hides template) |
| Helix radius | 2500 cm |
| Angle step | 36° (10 stations) |
| Valley spawn | Origin + `(850, 0, 120)` — in front of gate, yaw 180 |
| Valley Gate | Origin + `(400, ±400)` pillars (between spawn and mast) |
| Mist Inn door | Origin + `(400, -1700, 80)`, trigger +120 Y toward plaza; yaw 90 |
| Mist Inn pocket | ~(5 000 000, 0, 20 000), +X exit corridor open |
| Approach | Walkable stairs 40 cm rise, 450° helix, StartRadius 2200 |
| Grapple shaft | Floating pads every 45 m, ±400 Y stagger |
| Station dress ring | ≥520 cm from pad center; env dress **NoCollision** |
| Tour landing clearance | 120 cm above pad |

Station altitudes (relative to ValleyOrigin.Z) unchanged: Mist 0.5 km … Pinnacle 19 km.

`mistspire.DemoTour N` lands on station pad centers. Inter-station travel for recording is **DemoTour** by design.

## Modules (greybox + Fab env dress)

- Small `ShelfPad` + Valley Gate arch + brazier (walkable greybox under Fab shelf)
- Iceland **SummitMass** under the village + gate rocks (soft-ref)
- Closed modular Mist Inn from **mesh bounds**: front is `Wall_Door_4m` (has the opening),
  sides/back `Wall_4m`, roof `Roof_Both_Ends_4m`, porch `Porch_4x4m`. Module width and roof/
  porch Z come from `GetBounds()`. Cabin shell is **NoCollision** — walking the plaza-side
  trigger teleports to the pocket interior (that is the intended enter). Door_01 leaf is not
  placed (its pivot did not match the cutout).
- Approach stairs → **HandoffBeacon** → grapple highway → Mist vignette
- Per station: pad, 3-side rails, jump stairs, one-sentence dress ring props
- Backdrop is **one** landform: `SummitMass`, 3 km across, NoCollision, **placed by tracing its own
  surface** so the deck sits on the mountain rather than above or inside it. Rings of secondary
  massifs were tried and removed — at any spacing they read as separate slabs around the big one.
- `AimSunAtVillage()` re-aims the map's directional light to `(-38, 162, 0)` so light rakes the face
  the village sits on (the shelf was otherwise in the summit's own shadow) and stays behind the
  player, who spawns looking -X. It forces Movable first, since Stationary lights ignore runtime rotation.
- Solid shelf dress is refused within `InnDoorClearanceCm` of the inn door, which is how the doorway
  got walled off before.

> Iceland `SM_Mountain_*` / plateau meshes are **square terrain patches**, not standalone peaks.
> Place them only through `AddTerrainMassif`, which sizes and sinks each one from its own bounds
> so the patch edge stays buried and the summit lands on an explicit world Z. Floating one at
> altitude produces the "slice" artifact; anything airborne must be greybox or a rock mesh.

> **Main_WP ships the `/Engine/Maps/Templates/OpenWorld` landscape** — a flat, untextured
> `Landscape` with streaming proxies. That is the grey card behind the demo, not a floor mesh, so
> no masking geometry can cover it. `HideTemplateLandscape()` hides the proxies at runtime (and on
> a 3 s re-apply, since World Partition streams more in as you climb). Collision stays enabled so
> a fall still lands on something.

**Main_WP** `DL_Landmarks_Authored`: the persistent `DemoEnv_*` rocks/mountains are superseded by runtime dressing. `HideTemplateBackdrop()` hides them and clears their collision at play time (one sat on the inn door), so they need no map edit — but they are still dead weight if you are editing the strip.

## Immersion props

Unchanged set (summit markers, shelters, crystals, lore, weather button, Mist Inn door/exit). Weather button on Mist Inn porch wing.

## VR / traversal notes

- Pawn budget: step 45 cm, jump apex ~88 cm, grapple 80 m
- Approach = walkable; after HandoffBeacon = grapple highway
- Station dress stays outside pad/rail clearance
- Recording: spawn at gate → Mist Inn → approach stairs → grapple → `DemoTour` heroes

## Console

```
mistspire.RebuildDemoScaffold
mistspire.DemoMode 1
mistspire.DemoTour 0..9
mistspire.DemoJoeBeat
```

## Acceptance (polish)

1. First frame: mountain shelf, gate ahead, hut beside gate, no grey void underfoot
2. Mist Inn enter/exit without stuck capsule
3. DemoTour 0/3/6/9 readable vignettes; no colliding dress on pads
