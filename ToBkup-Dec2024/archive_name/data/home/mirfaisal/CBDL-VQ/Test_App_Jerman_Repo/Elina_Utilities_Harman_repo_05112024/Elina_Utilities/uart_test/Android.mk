LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES:= $(LOCAL_PATH)/src
LOCAL_SRC_FILES:= src/baud_setting.c src/parsing.c src/uart_test.c
LOCAL_MODULE := uart_test

#Build profile, possible values: release, debug, profile, coverage
BUILD_PROFILE ?= release

# Uncomment below to enable logs via logcat
#LOGGING := ANDROID_LOGGING
ifneq ($(LOGGING), )
  LOCAL_CFLAGS += -D$(LOGGING)
  ifeq ($(LOGGING), ANDROID_LOGGING)
    LOCAL_LDLIBS := -llog
  else
    ifeq ($(LOGGING), DLT_LOGGING)
      LOCAL_LDLIBS := -ldlt
    endif	#ifeq ($(LOGGING), DLT_LOGGING)
  endif	#ifeq ($(LOGGING), ANDROID_LOGGING)
endif	#ifneq ($(LOGGING), )

#Compiler flags for build profiles
CCFLAGS_release += -O2 -DNDEBUG
CCFLAGS_debug += -g -O0 -fno-builtin
CCFLAGS_coverage += -g -O0 -ftest-coverage -fprofile-arcs
LDFLAGS_coverage += -ftest-coverage -fprofile-arcs
CCFLAGS_profile += -g -O0 -finstrument-functions

#Generic compiler flags (which include build type flags)
LOCAL_CFLAGS += -Wall -Werror -Wextra -fmessage-length=0 -DANDROID_ENABLED
LOCAL_CFLAGS += $(CCFLAGS_$(BUILD_PROFILE))

LOCAL_MODULE_TAGS := optional
LOCAL_HEADER_LIBRARIES += libutils_headers
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

