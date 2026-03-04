# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- GitHub Actions workflow **HDR Swapchain Upgrade for Display Commander**: builds only `_hdr_upgrade` and publishes to release tag `hdr-upgrade-display-commander` (for Display Commander to recommend/download/configure).

### Changed

- README: HDR section now points to the [addons-univ-hdr](https://github.com/pmnoxx/renodx/releases/tag/addons-univ-hdr) release, explains `.addon64` (64-bit) vs `.addon32` (32-bit), and documents Display Commander integration.
- Addon renamed `_autohdr` → `_hdr_upgrade`; `addons-univ-hdr` workflow updated accordingly.

## [0.1.1] - 2026-03-01

### Changed

- Version bump (testing version bumping).

## [0.1.0] - TBD

### Added

- **renodx-_autohdr** addon for swapchain SDR-to-HDR upgrade (documented in README).
- `external/sanitiz_metadata` submodule entry in `.gitmodules` for CI.

### Changed

- README: added Utilities section entry for _autohdr (`.addon64` / `.addon32`).

### Fixed

- GitHub Actions build: submodule `external/sanitiz_metadata` now has URL in `.gitmodules`.
- Removed `private_docs` submodule (had no `.gitmodules` URL), fixing fresh-clone and CI.

### Removed

- `private_docs` submodule.
