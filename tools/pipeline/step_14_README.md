# Pipeline Step 14 — Save/Checkpoint Integration

`UMistspireProgressSubsystem` manages persistence:

1. Place `AMistspireCheckpointVolume` at each rest shelter:
   - Overlap → auto-save on exit
   - Saves: altitude, stamina/oxygen state, collected shards, summits reached
2. Checkpoints are stored in `MistspireProfile` save slot
3. On death (oxygen depleted, fall below 0 altitude):
   - Player respawns at last checkpoint
   - 10% stamina penalty on respawn

**Triggers that save:**
- Reaching a new summit (tracked by SummitMarker reach event)
- Entering a building (save interior state)
- Manual save via `mistspire.SaveProgress`

**Load flow:**
1. Game starts → `LoadProgress` reads slot
2. Player spawns at last checkpoint altitude
3. All previously collected pickups remain collected

**Console:**
```
mistspire.SaveProgress
mistspire.LoadProgress
mistspire.CheckpointList
```
