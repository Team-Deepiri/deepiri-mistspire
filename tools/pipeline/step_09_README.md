# Pipeline Step 9 — Navigation + Climbable Surfaces

1. Add `ANavMeshBoundsVolume` covering main traversal paths
   - Rebuild: Build → Build Paths
2. Mark **climbable surfaces:**
   - Static meshes with `Climbable` tag (handled by `UMistspireClimbingComponent`)
   - Use `AMistspireClimbableSurface` actor for ad-hoc climb volumes
3. **Teleport volumes:**
   - Add `MantleVolume` triggers at ledge edges
   - Assign `AMistspireTeleportAnchor` for long-range blink points
4. **Blocking volumes** around out-of-bounds areas (below terrain, inside geometry)

**VR comfort:**
- Add teleport arc visualization (Blueprint: `BP_TeleportArc`)
- Smooth turn vs snap turn toggle (handled by VRPawn)
