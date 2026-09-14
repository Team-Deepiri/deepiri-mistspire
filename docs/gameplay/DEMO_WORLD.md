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
| Mist Inn pocket | ~(5 000 000, 0, 20 000) floor; pawn lands `+PlayerStartZCm` above it |
| Approach | Walkable stairs 40 cm rise, 450° helix, StartRadius 2200 |
| Grapple shaft | Floating pads every 45 m, ±400 Y stagger |
| Station dress ring | ≥520 cm from pad center; env dress **NoCollision** |
| Tour landing | 120 cm above pad, inset 140 cm toward the mast |
| Fall catch | Below `ValleyFloorZ - 5000` returns the player to the Valley Gate |

Station altitudes (relative to ValleyOrigin.Z) unchanged: Mist 0.5 km … Pinnacle 19 km.

`mistspire.DemoTour N` lands on the **inner half** of each station pad. The pad centre is not safe:
the Pinnacle needle, Sanctum plinth and Void pier all cross it, and `ApplyTeleport` does not sweep,
so a centred landing left the capsule depenetrating inside solid geometry. Inter-station travel for
recording is **DemoTour** by design.

`GetMistInnInteriorSpawn()` is the pocket **floor**, not a pawn spawn. Anything that places the
player must use `GetMistInnInteriorPawnSpawn()`: putting the capsule centre on the floor plane
buried half the capsule in the slab, which either popped the player or dropped them through the
pocket into a 200 m fall.

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
> `Landscape` with streaming proxies. That is the grey card behind the demo, not a floor mesh.
> `HideTemplateLandscape()` hides the proxies **and disables their collision** (leaving collision
> on caused Mist Inn exit teleports to snap onto / get ejected by landscape). Because that removes
> the only floor outside the demo geometry, `CatchFallenPlayer()` polls the pawn every 0.4 s and
> returns it to the Valley Gate once it drops more than `FallCatchDepthCm` below the valley floor —
> without it, stepping off the `ShelfPad` is an unrecoverable fall that ends a recording take.

**Main_WP** `DL_Landmarks_Authored`: persistent `DemoEnv_*` Iceland/Rock actors are **destroyed**
at runtime by `PurgeLegacyMapDress()` (below `LegacyDressPurgeMaxAltitudeCm`). Runtime Fab dress
lives on the scaffold as components — not as world `StaticMeshActors`. Save Main_WP after manual
strip edits if you want the purge out of PIE logs permanently.

## Immersion props

Unchanged set (summit markers, shelters, crystals, lore, weather button, Mist Inn door/exit).

Weather button sits on the **plaza edge** of the Mist Inn porch, at `InnDoor + (-140, 340, 40)`. It
must stay outside the entrance `DoorVolume` (world X ±180, Y ±100 around the trigger): at the old
`+200 Y` it was inside the box, so walking up to press it teleported the player into the inn instead
of cycling the weather.

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
2. Mist Inn enter/exit without stuck capsule; porch weather button cycles weather without teleporting
3. DemoTour 0/3/6/9 readable vignettes; no colliding dress on pads
4. Walk off the `ShelfPad` edge — the fall catch returns you to the gate instead of falling forever
5. `mistspire.DemoTour 5` from inside the Mist Inn, then walk back into the door: it still enters
