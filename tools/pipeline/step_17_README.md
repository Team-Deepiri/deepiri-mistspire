# Pipeline Step 17 — Packaging Configuration

## Windows (primary target)
1. Project Settings → Packaging:
   - Build configuration: Shipping
   - Maps to cook: `/Game/Maps/Main_WP`
   - List of desired cultures: en
2. OpenXR runtime: SteamVR (default on Windows)
3. Package:
   ```powershell
   <UE>/Engine/Build/BatchFiles/RunUAT.bat BuildCookRun ^
     -project=<path>/Mistspire.uproject ^
     -platform=Win64 -configuration=Shipping ^
     -map=/Game/Maps/Main_WP
   ```

## Linux (experimental)
1. Install: `./scripts/setup-linux-deps.sh`
2. Requires: Mesa RADV (AMD) or NVIDIA proprietary driver
3. OpenXR runtime: `scripts/verify-openxr-runtime.sh`
4. Package:
   ```bash
   <UE>/Engine/Build/BatchFiles/Linux/Build.sh \
     -project=<path>/Mistspire.uproject \
     -platform=Linux -configuration=Shipping
   ```

## Shipping checklist
- [ ] No debug/cheat commands exposed
- [ ] Windows executable signed
- [ ] SteamVR overlays configured
- [ ] `git lfs` tracked for `.uasset` / `.umap`
- [ ] Oculus Post-Process present for Link/VD
