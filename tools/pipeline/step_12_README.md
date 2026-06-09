# Pipeline Step 12 — Companion Orb + Ghost Climber Setup

## Companion Orb
`UMistspireCompanionSubsystem` manages the guide spirit:
1. Place `AMistspireCompanionOrb` spawn point near start
2. The orb leads toward the next unclaimed summit
3. Configure trail VFX + chime audio
4. Orb disappears if player strays > 200 m off path, reappears when player returns

## Ghost Climbers
`UMistspireGhostClimberSubsystem` renders ghost silhouettes:
1. Place `AMistspireGhostAnchor` along the climb path
2. Ghosts appear as translucent player-shaped pillars at recorded altitudes
3. They pulse with a soft blue glow
4. Distant ghosts are visible through fog as vertical light shafts

**Leaderboard integration:**
- Ghosts correspond to top 10 PB altitudes from save data
- Each ghost has a floating name + altitude label

**See:** `Source/Mistspire/Systems/MistspireCompanionSubsystem.*`, `MistspireGhostClimberSubsystem.*`
