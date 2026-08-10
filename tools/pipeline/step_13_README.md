# Pipeline Step 13 — Narrative Triggers + Zones

1. Place `AMistspireNarrativeTrigger` volumes where you want narrative lines and milestone UI to fire (altitudes depend on your authored world).
2. Configure `UMistspireNarrativeSubsystem` rules:
   - Zone line crossings fire one-shot dialogue/UI text
   - Kilometer milestones show HUD banner + chime
   - Summit fanfare on first reach
3. For narrative/system context and the operator/debug surface, see `docs/gameplay/IMMERSION.md`.

**See:** `docs/gameplay/IMMERSION.md`
