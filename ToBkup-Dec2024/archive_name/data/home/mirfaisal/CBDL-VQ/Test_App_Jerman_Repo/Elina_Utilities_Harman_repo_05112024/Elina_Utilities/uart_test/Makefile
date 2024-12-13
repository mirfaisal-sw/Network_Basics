ARTIFACT = uart_test

#Build architecture/variant string, possible values: x86, armv7le, etc...
PLATFORM ?= aarch64le

#Build profile, possible values: release, debug, profile, coverage
BUILD_PROFILE ?= release

#TARGET_OS, possible values: linux, qnx
TARGET_OS ?= linux

CONFIG_NAME ?= $(PLATFORM)-$(BUILD_PROFILE)
OUTPUT_DIR_BASE = build
OUTPUT_DIR = $(OUTPUT_DIR_BASE)/$(CONFIG_NAME)
TARGET = $(OUTPUT_DIR)/$(ARTIFACT)

#Default target
all: $(TARGET)

#Compiler definitions
ifeq ($(TARGET_OS), qnx)
 CC = qcc -Vgcc_nto$(PLATFORM)
 CXX = qcc -lang-c++ -Vgcc_nto$(PLATFORM)
 LIBS_profile += -lprofilingS
 CCFLAGS_coverage += -nopipe -Wc,-auxbase-strip,$@
 CCFLAGS_all += -DQNX_ENABLED
 ifeq ($(LOGGING), QNX_SLOG2_LOGGING)
  LIBS += -lslog2
 endif	#ifeq ($(LOGGING), QNX_SLOG2_LOGGING)
else
 CC ?= gcc
 CXX ?= gcc++
 CCFLAGS_all += -DLINUX_ENABLED
endif

ifneq ($(LOGGING), )
 CCFLAGS_all += -D$(LOGGING)
 ifeq ($(LOGGING), DLT_LOGGING)
  LIBS += -ldlt
 endif	#ifeq ($(LOGGING), DLT_LOGGING)
endif	#ifneq ($(LOGGING), )

LD = $(CC)

$(info Building for $(TARGET_OS))

#User defined include/preprocessor flags and libraries

#INCLUDES += -I/path/to/my/lib/include
#INCLUDES += -I../mylib/public

#LIBS += -L/path/to/my/lib/$(PLATFORM)/usr/lib -lmylib
#LIBS += -L../mylib/$(OUTPUT_DIR) -lmylib

#Compiler flags for build profiles
CCFLAGS_release += -O2 -DNDEBUG
CCFLAGS_debug += -g -O0 -fno-builtin
CCFLAGS_coverage += -g -O0 -ftest-coverage -fprofile-arcs
LDFLAGS_coverage += -ftest-coverage -fprofile-arcs
CCFLAGS_profile += -g -O0 -finstrument-functions

#Generic compiler flags (which include build type flags)
CCFLAGS_all += -Wall -Werror -Wextra -fmessage-length=0
CCFLAGS_all += $(CCFLAGS_$(BUILD_PROFILE))

#Shared library has to be compiled with -fPIC
#CCFLAGS_all += -fPIC
LDFLAGS_all += $(LDFLAGS_$(BUILD_PROFILE)) -pthread
LIBS_all += $(LIBS_$(BUILD_PROFILE))
DEPS = -Wp,-MMD,$(@:%.o=%.d),-MT,$@

#Macro to expand files recursively: parameters $1 -  directory, $2 - extension, i.e. cpp
rwildcard = $(wildcard $(addprefix $1/*.,$2)) $(foreach d,$(wildcard $1/*),$(call rwildcard,$d,$2))

#Source list
SRCS = $(call rwildcard, src, c)

#Object files list
OBJS = $(addprefix $(OUTPUT_DIR)/,$(addsuffix .o, $(basename $(SRCS))))

#Rules section for default compilation and linking
#Compiling rule
$(OUTPUT_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(DEPS) -o $@ $(INCLUDES) $(CCFLAGS_all) $(CCFLAGS) $<

#Linking rule
$(TARGET):$(OBJS)
	$(LD) -o $(TARGET) $(LDFLAGS_all) $(LDFLAGS) $(OBJS) $(LIBS_all) $(LIBS)
ifeq ($(TARGET_OS), qnx)
	usemsg $@ uart_test.use
endif

#Cleaning rule
clean:
	rm -fr $(OUTPUT_DIR_BASE)

#Rebuilding rule
rebuild: clean
	$(MAKE) all

#Help rule
help:
	@echo 'Valid targets are :'
	@echo '  all     - Build the target application (default)'
	@echo '  clean   - Remove all generated files'
	@echo '  rebuild - Remove all generated files & build the target application'
	@echo 'For further info see the ./README file'

#Inclusion of dependencies (object files to source and includes)
-include $(OBJS:%.o=%.d)

.PHONY : all clean help rebuild
