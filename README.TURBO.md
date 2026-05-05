# sokol-turbo

This file is for fork-local notes and workflow details. Keep upstream-facing documentation in `README.md` unchanged when possible so syncing from `floooh/sokol` has fewer conflicts.

## Upstream Remote

Git remotes are local checkout configuration and are not committed to the repository. Set up the upstream remote on each machine that needs to fetch from upstream:

```bash
git remote add upstream https://github.com/floooh/sokol.git
git remote set-url --push upstream DISABLED
git fetch upstream
```

The dummy push URL helps avoid accidentally pushing to upstream while still allowing normal fetches.

## Sync Workflow

Fetch upstream changes:

```bash
git fetch upstream
```

Then merge or rebase the desired upstream branch into the fork branch you are working on. Prefer keeping fork-specific documentation in this file and fork-specific implementation notes under `docs/`.

## CMake Build

This fork adds a root CMake build alongside the existing upstream project files.

Library-only static build:

```bash
cmake -S . -B build -DSOKOL_BUILD_SAMPLES=OFF -DSOKOL_BUILD_MODE=static
cmake --build build --parallel
```

Library-only shared build:

```bash
cmake -S . -B build-shared -DSOKOL_BUILD_SAMPLES=OFF -DSOKOL_BUILD_MODE=shared
cmake --build build-shared --parallel
```

Sample smoke build:

```bash
cmake -S . -B build-samples -DSOKOL_BUILD_MODE=static
cmake --build build-samples --target sample_clear_sapp sample_triangle_sapp sample_cube_sapp --parallel
```

Shader samples require `sokol-shdc`. Set `SOKOL_SHDC` to a local binary, or let CMake fetch `floooh/sokol-tools-bin` when network access is available.

## Fork-Local Files

- `README.TURBO.md`: fork-local workflow notes.
- `docs/sokol-cmake-migration-plan.md`: CMake migration status and follow-up.
- `samples/`: copied `sapp` samples and helper libraries, excluding upstream `samples/libs/sokol` so builds use this repository's headers.
