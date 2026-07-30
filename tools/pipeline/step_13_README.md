# Pipeline Step 13 — Narrative Triggers + Zones

1. Place `AMistspireNarrativeTrigger` volumes at key altitudes:
   - 0 m — "The mist swallows the valley below..."
   - 1000 m — "The air thins. Your journey has begun."
   - 5000 m — "Cloud gardens bloom where none should live."
   - 10000 m — "The void stares back."
   - 15000 m — "Beyond the sanctuary, the stars await."
   - 20000 m — "The pinnacle. All of Mistspire beneath you."

2. Configure `UMistspireNarrativeSubsystem` rules:
   - Zone line crossings fire one-shot dialogue/UI text
   - Kilometer milestones show HUD banner + chime
   - Summit fanfare on first reach (confetti particles + audio swell)

3. **Lore Shard chains:**
   - Collect 3 shards per district to unlock a lore entry
   - 12 districts = 36 lore entries total

**See:** `docs/gameplay/IMMERSION.md` § Narrative
