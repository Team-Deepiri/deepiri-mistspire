# Audio Blending Strategy: Ambience Layers

## The "Altitude Pressure" System
As the player ascends, the audio environment undergoes a "thinning" process. This is achieved through three primary layers that blend dynamically based on the `MistspireAltitudeSubsystem` data.

## Layer 1: The Pressure Base (Altitude-Driven)
- **Lowlands (0-2000m):** Heavy, bass-rich wind loops. Muffled, "heavy air" filters. Occasional low-frequency rumbles to suggest the weight of the spires.
- **Mid-Ascent (2000-5000m):** The bass drops out, replaced by whistling "airy" textures. Wind becomes sharper, more directional.
- **The Peaks (5000m+):** Near-total silence. The "wind" is now just a high-frequency hiss. The primary audio focus shifts to the player's internal sounds: breathing, heartbeat, and the "clink" of gear.

## Layer 2: The Biome Soul (Localized)
- **Mist:** Constant water drips, muffled "underwater-like" echoes.
- **Arid:** Gritty, sand-paper textures. The "Aeolian Pipes" should be a 3D spatialized sound source near Mesa Crown.
- **Forest:** A dense bed of chittering, rustling, and spore "pops." Use randomized "Mist-Whale" calls in the distance.
- **Ember:** Low-frequency bubbling and high-frequency gas hisses. Occasional "obsidian cracks" (sharp, glass-like snaps).
- **Crystal:** Musical, tonal pads. "Prism-clinks" that trigger when the player brushes against crystal formations.

## Layer 3: The Narrative Pulse (Event-Driven)
- **Summit Proximity:** A rising, low-frequency hum (the "Call of the Spire") that harmonizes with the biome's music as the player approaches a Marker.
- **The Glider:** A rushing wind effect that filters based on speed, with a "fabric flutter" component that increases during high-G turns.
- **Storms:** Violent, chaotic layers of crashing thunder and screaming wind that override the Biome Soul layer.
