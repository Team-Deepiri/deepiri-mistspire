# Steam Frame (upcoming)

**Pre-release** notes for Valve's **Steam Frame** standalone headset (streaming-first, expected 2026). Details may change before retail launch.

Binding: likely Index-like (`bindings_valve_index.json`) — confirm when hardware ships.

## What you need (expected)

- Steam Frame headset + bundled **6 GHz wireless dongle** (USB to gaming PC).
- Gaming PC with Steam + **SteamVR**.
- Same room-scale or seated space as other SteamVR titles.

Standalone SteamOS play on the device itself is **out of scope** for Mistspire **editor VR Preview** workflow initially — this guide covers **PC streaming** only.

## Choose your connection path

| Path | OpenXR runtime |
|------|----------------|
| **Wireless dongle → PC SteamVR** | **SteamVR OpenXR** |

Valve positions Frame as streaming from a PC Steam library over a dedicated point-to-point 6 GHz link (not shared home Wi‑Fi).

---

## Windows — SteamVR streaming

### Expected setup (per Valve partner docs)

1. Install Steam + SteamVR on the PC.
2. Plug in the **Steam Frame wireless adapter**; complete pairing per first-run wizard.
3. **Set SteamVR as OpenXR runtime**.
4. Confirm SteamVR sees the Frame; run room setup if prompted.
5. UE → **Play → VR Preview** on `Main_WP` per [OPENXR_DEV_COMMON.md](../OPENXR_DEV_COMMON.md).

### Mistspire dev test

Same as Index: SteamVR OpenXR active → VR Preview → `mistspire.AltitudeStats`.

---

## Linux

When Valve ships Frame streaming support on Linux hosts, use **SteamVR OpenXR** on Linux the same way as tethered Index ([steamvr_lighthouse.md](steamvr_lighthouse.md#linux--steamvr-primary)).

Monitor [Steam Frame partner documentation](https://partner.steamgames.com/doc/steamhardware/steamframe) for Linux host updates.

---

## Relation to Valve Index

Steam Frame is intended to **replace Index** in Valve's lineup. Until hardware is available, develop on **Index + SteamVR** using [steamvr_lighthouse.md](steamvr_lighthouse.md).

---

## Troubleshooting

| Issue | Fix |
|-------|-----|
| Doc vs retail mismatch | Re-check Valve partner docs after launch |
| Dongle not paired | Repeat SteamVR / Frame pairing wizard |

## Official links

- [Steam Frame (Steamworks)](https://partner.steamgames.com/doc/steamhardware/steamframe)
- [OpenXR engine integrations (Steam Frame)](https://partner.steamgames.com/doc/steamhardware/steamframe/engines)
- [SteamVR](https://store.steampowered.com/app/250820/SteamVR/)
