# Automated Releases

This repository includes automated GitHub Actions workflows that build and release ReNoDX2 addons when new ReShade versions are released upstream.

## Workflows

### 1. Auto Release (`auto-release.yml`)

**Triggers:**
- **Scheduled**: Runs every 6 hours to check for new upstream ReShade tags
- **Manual**: Can be triggered manually with an optional specific tag

**What it does:**
1. Checks the upstream ReShade repository for new releases
2. If a new tag is found that doesn't exist in this repository:
   - Updates the ReShade submodule to the new tag
   - Builds both x64 and x86 versions of all addons
   - Creates a GitHub release with the built addon DLLs

**Output:**
- Creates releases with names like `renodx2-v6.5.1`
- Includes both `.addon64` (x64) and `.addon32` (x86) versions
- Packages everything in a zip file for easy download

### 2. Manual Release (`manual-release.yml`)

**Triggers:**
- **Manual only**: Must be triggered manually from the GitHub Actions tab

**What it does:**
1. Allows you to specify any upstream ReShade tag to build
2. Lets you choose which architectures to build (both, x64-only, or x86-only)
3. Builds and releases the specified version

**Usage:**
1. Go to the "Actions" tab in GitHub
2. Select "Manual Release Build"
3. Click "Run workflow"
4. Enter the upstream tag (e.g., `v6.5.1`)
5. Choose architectures to build
6. Click "Run workflow"

### 3. Build All Tags (`build-all-tags.yml`)

**Triggers:**
- **Manual only**: Must be triggered manually from the GitHub Actions tab

**What it does:**
1. Scans all available upstream ReShade tags
2. Optionally skips tags that already have releases
3. Builds releases for multiple tags in parallel
4. Perfect for backfilling missing releases

**Usage:**
1. Go to the "Actions" tab in GitHub
2. Select "Build All Upstream Tags"
3. Click "Run workflow"
4. Configure options:
   - **Max tags**: How many recent tags to build (default: 10)
   - **Start from tag**: Build from a specific tag onwards (optional)
   - **Architectures**: Choose which architectures to build
   - **Skip existing**: Skip tags that already have releases (recommended)
5. Click "Run workflow"

### 4. Build Tag Range (`build-tag-range.yml`)

**Triggers:**
- **Manual only**: Must be triggered manually from the GitHub Actions tab

**What it does:**
1. Builds releases for a specific range of upstream tags
2. Useful for building releases between two specific versions
3. Builds all tags in the range in parallel

**Usage:**
1. Go to the "Actions" tab in GitHub
2. Select "Build Tag Range"
3. Click "Run workflow"
4. Configure options:
   - **Start tag**: First tag to build (e.g., `v6.0.0`)
   - **End tag**: Last tag to build (e.g., `v6.5.1`) - leave empty for latest
   - **Architectures**: Choose which architectures to build
5. Click "Run workflow"

## When to Use Each Workflow

### Auto Release (`auto-release.yml`)
- **Best for**: Continuous integration - automatically builds new releases
- **When**: You want hands-off automation for new upstream releases
- **Frequency**: Runs every 6 hours automatically

### Manual Release (`manual-release.yml`)
- **Best for**: Building a specific version quickly
- **When**: You need to test a particular ReShade version or build on-demand
- **Use case**: "I want to build v6.5.1 right now"

### Build All Tags (`build-all-tags.yml`)
- **Best for**: Backfilling missing releases or building many versions at once
- **When**: You want to build releases for the last 10-20 upstream tags
- **Use case**: "I want to build releases for all recent ReShade versions"

### Build Tag Range (`build-tag-range.yml`)
- **Best for**: Building a specific range of versions
- **When**: You need releases between two specific versions
- **Use case**: "I want to build all releases from v6.0.0 to v6.5.1"

## Built Addons

The workflows build the following addon types:

### Core Addons
- `renodx-devkit.addon64/.addon32` - Development toolkit
- `renodx-fpslimiter.addon64/.addon32` - FPS limiting functionality

### Game-Specific Addons
- `renodx-_univ.addon64/.addon32` - Universal addon for general compatibility
- `renodx-_hbr.addon64/.addon32` - Heaven Burns Red
- `renodx-_ksp.addon64/.addon32` - Kerbal Space Program
- `renodx-_madodora.addon64/.addon32` - Madoka Magica Exedra
- `renodx-_mary_skelter_finale.addon64/.addon32` - Mary Skelter Finale
- `renodx-_p5px.addon64/.addon32` - Persona 5 X
- `renodx-_p5s.addon64/.addon32` - Persona 5 Strikers
- `renodx-_yohane.addon64/.addon32` - Yohane the Parhelion
- `renodx-gatesofhell.addon64/.addon32` - Gates of Hell
- `renodx-unrealengine.addon64/.addon32` - Unreal Engine games

## Installation

1. Download the appropriate addon files for your game from the latest release
2. Place the `.addon64` files in your ReShade addons directory for 64-bit games
3. Place the `.addon32` files in your ReShade addons directory for 32-bit games
4. Configure ReShade to load the addons

## Configuration

### Required Secrets

The workflows use the default `GITHUB_TOKEN` which is automatically provided by GitHub Actions. No additional secrets are required.

### Upstream Repository

The workflows monitor the upstream ReShade repository at `crosire/reshade` for new releases.

## Troubleshooting

### Workflow Fails to Build

1. Check the workflow logs in the Actions tab
2. Common issues:
   - Upstream tag doesn't exist
   - CMake configuration fails
   - Missing dependencies

### No New Releases

1. The auto-release workflow only triggers when a new upstream tag is found
2. If you want to build a specific version, use the manual release workflow
3. Check that the upstream repository has new releases

### Missing Addon Files

1. Ensure the addon source files exist in `src/addons/` or `src/games/`
2. Check that the CMake configuration includes all addon directories
3. Verify the build completed successfully

## Development

To add new addons:

1. Create a new directory in `src/addons/` or `src/games/`
2. Add an `addon.cpp` file
3. The CMake configuration will automatically detect and build it
4. The workflows will include it in future releases

## Monitoring

You can monitor the workflows by:

1. Going to the "Actions" tab in GitHub
2. Viewing the workflow runs and their status
3. Checking the logs for any errors
4. Reviewing the created releases

The auto-release workflow will send notifications if it fails, and successful releases will be visible in the repository's releases section.
