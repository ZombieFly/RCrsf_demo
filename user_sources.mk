# User extension resources (safe to edit)
# This file is loaded by Makefile.user and won't be overwritten by STM32 tool generation.

# List dependency roots here. Each dependency is scanned recursively for sources
# and header directories, so normal library changes do not require updating this file.
USER_DEP_DIRS += \
Libs/rcrsf \
Libs/W25Q64


rwildcard = $(strip $(foreach entry,$(wildcard $1*),$(call rwildcard,$(entry)/,$2) $(filter $(subst *,%,$2),$(entry))))

USER_DEP_C_SOURCES := $(strip $(foreach dep,$(USER_DEP_DIRS),$(call rwildcard,$(dep)/,*.c)))
USER_DEP_ASM_SOURCES := $(strip $(foreach dep,$(USER_DEP_DIRS),$(call rwildcard,$(dep)/,*.s)))
USER_DEP_ASMM_SOURCES := $(strip $(foreach dep,$(USER_DEP_DIRS),$(call rwildcard,$(dep)/,*.S)))
USER_DEP_HEADER_FILES := $(strip $(foreach dep,$(USER_DEP_DIRS),$(call rwildcard,$(dep)/,*.h)))
USER_DEP_INCLUDE_DIRS := $(sort $(dir $(USER_DEP_HEADER_FILES)))

# Add your extra C source files here
USER_C_SOURCES += $(USER_DEP_C_SOURCES)

# Add your extra ASM sources if needed
USER_ASM_SOURCES += $(USER_DEP_ASM_SOURCES)
USER_ASMM_SOURCES += $(USER_DEP_ASMM_SOURCES)

# Add include directories (without -I)
USER_INCLUDE_DIRS += $(USER_DEP_INCLUDE_DIRS)

# Add preprocessor defines (without -D)
# USER_DEFS += MY_FEATURE=1
