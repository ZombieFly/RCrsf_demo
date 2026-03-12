# User extension resources (safe to edit)
# This file is loaded by Makefile.user and won't be overwritten by STM32 tool generation.

# Add your extra C source files here
USER_C_SOURCES += \
Libs/rcrsf/rcrsf.c \
Libs/rcrsf/rcs_utils.c

# Add your extra ASM sources if needed
# USER_ASM_SOURCES += path/to/file.s
# USER_ASMM_SOURCES += path/to/file.S

# Add include directories (without -I)
USER_INCLUDE_DIRS += \
Libs/rcrsf

# Add preprocessor defines (without -D)
# USER_DEFS += MY_FEATURE=1
