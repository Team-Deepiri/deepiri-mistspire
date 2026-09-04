# Demo map — Valley of the Ancient (local `demo` branch)

Local-only demo setup: Mistspire non-VR gameplay on Epic’s **AncientWorld** terrain. Do **not** push branch `demo` or Valley assets to the remote.

## Prerequisites

- UE **5.8**
- Valley project on disk:  
  `C:\Users\System Owner\Documents\Unreal Projects\ValleyoftheAncient\AncientGame.uproject`
- Mistspire on branch `demo` (based on `quang_nguyen/feat/nonvr`)

## What was linked (map only)

No AncientGame C++ / HoverDrone / AncientBattle plugins are used. Content is **junctioned** (not copied) so disk is not duplicated:

| Mistspire path | Points at |
|----------------|-----------|
| `game/Content/AncientContent` | Valley `Content/AncientContent` |
| `game/Content/__ExternalActors__/AncientContent` | Valley `__ExternalActors__/AncientContent` |

Default map in [`game/Config/DefaultEngine.ini`](../../game/Config/DefaultEngine.ini):

`/Game/AncientContent/Maps/AncientWorld.AncientWorld`

Game mode remains **`MistspireGameMode`** (global default).

## First open in Mistspire

1. Close Valley editor if it has the map locked.
2. Open `game/Mistspire.uproject`.
3. Confirm `/Game/AncientContent/Maps/AncientWorld` loads (World Partition streaming).
4. **World Settings → GameMode Override** → `MistspireGameMode` (clear any Echo / Ancient sample override).
5. Place or move **PlayerStart** onto solid landscape (valley floor). Save the map.
6. **Play** (not VR Preview), or `.\run-nonvr.ps1`.

Expect log: `Mistspire non-VR: using existing map geometry under spawn.`  
If you see the gray cube playground, spawn is not over collision — fix PlayerStart.

Missing Echo / sample Blueprint actors are expected (those depend on Valley game code we did not bring over). Terrain, Megascans, and WP cells should still stream.

`AMistspireGameMode::SanitizeDemoViewportOverlays` suppresses the editor **Outdated HLODs** / **BLUEPRINT COMPILE ERROR** banners and clears Valley dirt-mask / film-grain post-process borders.

## Packaging blocker — duplicate Level Script Actor

Cook/package fails with:

`Detected the creation of more than one LevelScriptActor (...AncientWorld_C_1, ...AncientWorld_C_0)`

PIE can still work. Both instances live in `AncientContent/Maps/AncientWorld.umap` (Valley file via the junction), not in ExternalActors. Mistspire does not need Valley’s level Blueprint logic.

### Official engine repair (try this first)

On map load the editor can show a bottom-right toast: **“Map Corruption: Multiple Level Script Actors”** → click **Repair Map** within ~10 seconds (easy to miss / dismiss). Then **Ctrl+S**, clear Message Log, reopen the map. The Ensure must not return on load.

### Console repair (Mistspire)

After Live Coding / compiling the game module with AncientWorld open in the **editor** (not PIE):

```
mistspire.RepairLevelScriptActors
```

This is the same destroy path as **Repair Map**. Then reopen the map and confirm the Ensure is gone. Close the editor before packaging.

### Clear the Level Blueprint (optional cleanup)

Valley’s Level Blueprint calls missing systems (GameplayAbilities, Soundscape, Ancient Game Mode). Emptying the graph avoids compile spam but does **not** by itself remove duplicate `AncientWorld_C_0` / `_1` instances.

1. **Blueprints → Open Level Blueprint** → Ctrl+A → Delete (all graphs) → Compile (0 fatal) → Save map.

Terrain + MistspireGameMode keep working without Valley’s level script.

### Python (optional)

`PythonScriptPlugin` is not enabled in Mistspire by default — [`tools/fix_duplicate_level_script_actors.py`](../../tools/fix_duplicate_level_script_actors.py) only works after enabling that plugin. Prefer **Repair Map** or `mistspire.RepairLevelScriptActors`.

Edits apply to the Valley project’s umap through the junction.

## Recreate junctions (if deleted)

```powershell
$valley = "C:\Users\System Owner\Documents\Unreal Projects\ValleyoftheAncient\Content"
$mist = "e:\repo\deepiri-mistspire\game\Content"
cmd /c "mklink /J `"$mist\AncientContent`" `"$valley\AncientContent`""
cmd /c "mklink /J `"$mist\__ExternalActors__\AncientContent`" `"$valley\__ExternalActors__\AncientContent`""
```

## Git

- Branch: **`demo`** — local only (`git push` not intended).
- Ignored: `game/Content/AncientContent/`, `__ExternalActors__/AncientContent/`.
- Commit only wiring (ini / gitignore / this doc), never the Valley binaries.
