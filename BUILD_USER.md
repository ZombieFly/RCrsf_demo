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

## Notes
- Keep source basenames unique (because object names use `notdir`).
- If STM32 tool regenerates `Makefile`, this wrapper flow still works.
