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
- `r.VRSmooting=1` (motion smoothing)
- `vr.EnableFixedFoveated=1` (if headset supports)
- `r.ScreenPercentage=80` (FSR upscale)

## Checklist
- [ ] HLOD generated for all biomes
- [ ] No lightmap leaks
- [ ] Draw calls < 3000 per frame
- [ ] Triangle count < 3M per frame
- [ ] No CPU hitch > 10 ms
