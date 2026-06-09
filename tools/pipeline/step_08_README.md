# Pipeline Step 8 — Audio Zones + Ambience

Place audio volumes per biome band:

1. Add ambient sound loops per altitude range:
   - Mist: Wind through reeds, distant water
   - Arid: Hot wind, stone creak
   - Forest: Leaves rustle, birds
   - Ember: Crackling fire, lava bubble
   - Crystal: Chime drone, ice stress
   - Void: Low rumble, atmospheric hiss
   - Tundra: Blizzard howl, snow crunch
   - Aether: High-frequency shimmer
   - Sanctum: Cathedral-like reverb tone
   - Pinnacle: Cosmic hiss, star hum

2. Assign `UAudioBus` per layer (6-bus system: Master, Ambience, SFX, Voice, UI, Physics)

3. Configure `UMistspireAudioSubsystem` parameters per zone:
   - `TensionLevel` (0–1) drives wind pitch + intensity
   - `MysteryLevel` (0–1) adds reverb + delay tails

4. **VR spatialization:** Use Oculus Audio or Windows Sonic for HRTF

**See:** Source: `MistspireAudioSubsystem.cpp` — audio buses + tension system
