# Pipeline Step 15 — Performance Optimization

Target: **90 FPS** on high-end PCVR (RTX 3080 / RX 6800 XT)

## Settings
1. **HLOD:** Generate Hierarchical LODs for all static meshes
   - World Settings → HLOD setup → Auto generate
2. **World Partition streaming:**
   - Cell size: 128 m (near), 256 m (far)
   - Streaming distance: 5000 cm near, 15000 cm far
3. **Fog culling:** Volumetric fog off below 10 fps threshold
4. **Scalability:**
   - `sg.ViewDistanceQuality=3` (High)
   - `sg.AntiAliasingQuality=2` (Medium for VR)
   - `sg.PostProcessQuality=3` (High)
   - `sg.ShadowQuality=2` (Medium)
   - `sg.TextureQuality=3` (High)

## VR-specific

Values in `game/Config/DefaultEngine.ini` (VR comfort):

- `vr.FixedFoveation.Level=0` (raise if the headset supports foveation)
- `vr.Swapchain.AllowTearing=0`
- `vr.StereoViewOffset=1`

Pipeline knobs (not all stored in DefaultEngine.ini):

- `r.ScreenPercentage=80` (upscale; tune per GPU)
- Motion reprojection / smoothing is a **SteamVR / runtime** setting, not an engine CVar.

## Checklist
- [ ] HLOD generated for all biomes
- [ ] No lightmap leaks
- [ ] Draw calls < 3000 per frame
- [ ] Triangle count < 3M per frame
- [ ] No CPU hitch > 10 ms
