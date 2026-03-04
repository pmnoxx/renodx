# renodx2
This is a fork of ReShade with custom mods. RenoDX, short for "Renovation Engine for DirectX Games", is a toolset to mod games. Currently it can replace shaders, inject buffers, add overlays, upgrade swapchains, upgrade texture resources, and write user settings to disk. Because RenoDX uses Reshade's add-on system, compatibility is expected to be pretty wide. Using Reshade simplifies all the hooks necessary to tap into DirectX without worrying about patching version-specific exe files.

# Supported Games

* Against the Storm
* Artisan TD
* Blue Archive
* Darkest Dungeon 2
* Phantom Brigade
* Shipbreaker
* The Swapper
* This War of Mine
* Total War: Warhammer 3

# Utilities

## HDR Swapchain Upgrade (for Display Commander)

**renodx-_hdr_upgrade** is a swapchain SDR-to-HDR upgrade addon. Pre-built binaries are published in the **[HDR Swapchain Upgrade for Display Commander](/releases)** release. [Display Commander](https://github.com/pmnoxx/reshade-display-commander/tree/main) can recommend, download, and configure this mod for SDR-to-HDR swapchain upgrade.

The release contains two addon files:

| File | Use when |
|------|----------|
| **renodx-_hdr_upgrade.addon64** | The game or application is 64-bit (most modern games). |
| **renodx-_hdr_upgrade.addon32** | The game or application is 32-bit. |

Place the matching file (and [renodx-devkit](https://clshortfuse.github.io/renodx/renodx-devkit.addon64) for your architecture) into the game directory where ReShade is installed.

* [renodx-devkit.addon64](https://clshortfuse.github.io/renodx/renodx-devkit.addon64) &ndash; Developer kit to help build addons
* [Display Commander](https://github.com/pmnoxx/reshade-display-commander/tree/main) &ndash; Advanced ReShade addon for professional window management, display control, and NVIDIA Reflex integration



