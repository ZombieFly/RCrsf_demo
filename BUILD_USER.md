# User build wrapper (no change to generated Makefile)

## Goal
Use your own sources/includes/defines **without editing** the auto-generated `Makefile`.

## Files
- `Makefile`: auto-generated (leave it untouched)
- `Makefile.user`: stable wrapper build entry
- `user_sources.mk`: your custom resource list

## How to build
```bash
make -f Makefile.user -j4
```

Other common targets:
```bash
make -f Makefile.user clean
make -f Makefile.user user-show
```

## How to add files
Edit `user_sources.mk`:
- Add C sources to `USER_C_SOURCES`
- Add include dirs to `USER_INCLUDE_DIRS` (no `-I`)
- Add macros to `USER_DEFS` (no `-D`)

For `Libs/rcrsf`, `user_sources.mk` now auto-discovers all `.c` files and header directories under the submodule root. Normal file additions and removals inside that library no longer require manual edits.

To add more dependencies later, just append their root directories to `USER_DEP_DIRS` in `user_sources.mk`. The wrapper will recursively collect `.c`, `.s`, `.S`, and header include directories for each dependency automatically.

## Notes
- `Makefile.user` keeps generated project objects unchanged and mirrors only user-added dependency sources under `build/<source-relative-path>.o`, so different dependencies can contain same-named source files without object name collisions.
- If STM32 tool regenerates `Makefile`, this wrapper flow still works.
