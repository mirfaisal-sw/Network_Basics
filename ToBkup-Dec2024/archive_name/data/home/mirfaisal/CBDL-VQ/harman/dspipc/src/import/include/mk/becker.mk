# Ensure that a version-controlled build environment is available.
ifeq ($(P4_QNX_OS_ROOT),)
   $(warning "******************************************")
   $(warning "** P4_QNX_OS_ROOT environment variable  **")
   $(warning "** is not set, please correct to ensure **")
   $(warning "** version controlled build process     **")
   $(warning "******************************************")
   $(warning "** Multi version support not available  **")
   $(warning "******************************************")
   $(warning "**      Build process aborted           **")
   $(error   "******************************************")
endif

# Verify that the auxiliary build tools are actually available.
ifeq ($(wildcard $(P4_QNX_BUILDPROCESS_ROOT)/patch_qinfo.exe),)
   $(warning "******************************************")
   $(warning "** HBAS driver build tools not found in **")
   $(warning "** directory $(P4_QNX_BUILDPROCESS_ROOT) ")
   $(warning "******************************************")
   $(warning "**      Build process aborted           **")
   $(error   "******************************************")
endif

# Determine the (absolute) paths of some directories related to the build process:
# _xd_CWD:       the current working directory
# _xd_QVERSION:  directory defining the OS version we are targeting
# _xd_COMMON_MK: directory containing common.mk
# _xd_BECKER_MK: directory containing becker.mk

# becker.mk: relative path(!) is last word of $(MAKEFILE_LIST) [by construction]
_xd_BECKER_MK	:= $(dir $(lastword $(MAKEFILE_LIST)))		# needs GNU make >= 3.81
_xd_BECKER_MK	:= $(dir $(word $(words $(MAKEFILE_LIST)), $(MAKEFILE_LIST)))

# current working directory: $(CURDIR) [set bey GNU make]
_xd_CWD 	:= $(CURDIR)

# common.mk: Search it (must be somewhere above in the directory tree).
# [Also build list of variants from intermediate directories traversed.]

_xl_VARIANTS	:=
_xd_QVERSION	:= .
_xd_COMMON_MK	:= $(_xd_CWD)

ifeq ($(wildcard $(_xd_COMMON_MK)/common.mk),)
    _xv           := $(notdir $(_xd_COMMON_MK))
    _xl_VARIANTS  := $(_xv) $(_xl_VARIANTS)
    _xd_QVERSION  := $(if $(filter qnx%, $(_xv)), $(_xd_COMMON_MK), $(_xd_QVERSION))
    _xd_COMMON_MK := $(patsubst %/,%,$(dir $(_xd_COMMON_MK)))
    
    ifeq ($(wildcard $(_xd_COMMON_MK)common.mk),)
        _xv           := $(notdir $(_xd_COMMON_MK))
        _xl_VARIANTS  := $(_xv) $(_xl_VARIANTS)
        _xd_QVERSION  := $(if $(filter qnx%, $(_xv)), $(_xd_COMMON_MK), $(_xd_QVERSION))
        _xd_COMMON_MK := $(patsubst %/,%,$(dir $(_xd_COMMON_MK)))
    
        ifeq ($(wildcard $(_xd_COMMON_MK)/common.mk),)
            _xv           := $(notdir $(_xd_COMMON_MK))
            _xl_VARIANTS  := $(_xv) $(_xl_VARIANTS)
            _xd_QVERSION  := $(if $(filter qnx%, $(_xv)), $(_xd_COMMON_MK), $(_xd_QVERSION))
            _xd_COMMON_MK := $(patsubst %/,%,$(dir $(_xd_COMMON_MK)))
        
            ifeq ($(wildcard $(_xd_COMMON_MK)/common.mk),)
                _xv           := $(notdir $(_xd_COMMON_MK))
                _xl_VARIANTS  := $(_xv) $(_xl_VARIANTS)
                _xd_QVERSION  := $(if $(filter qnx%, $(_xv)), $(_xd_COMMON_MK), $(_xd_QVERSION))
                _xd_COMMON_MK := $(patsubst %/,%,$(dir $(_xd_COMMON_MK)))
            
                ifeq ($(wildcard $(_xd_COMMON_MK)/common.mk),)
                    _xv           := $(notdir $(_xd_COMMON_MK))
                    _xl_VARIANTS  := $(_xv) $(_xl_VARIANTS)
                    _xd_QVERSION  := $(if $(filter qnx%, $(_xv)), $(_xd_COMMON_MK), $(_xd_QVERSION))
                    _xd_COMMON_MK := $(patsubst %/,%,$(dir $(_xd_COMMON_MK)))
                
                endif
            endif
        endif
    endif
endif

_xl_VARIANTS := $(subst -, ,$(_xl_VARIANTS))
_xv_QNX      := $(filter qnx%,      $(_xl_VARIANTS))
_xv_xCC      := $(filter gcc% icc%, $(_xl_VARIANTS))

# extract QNX_VERSION  ...  [621, 630, ...]
# extract COMPILER_TYPE ... [gcc, icc ...]
# extract COMP_VERSION ...  [2_95_3, 3_3_1, ...]

override QNX_VERSION   := $(subst qnx,,$(_xv_QNX))
override COMPILER_TYPE := $(strip $(if $(filter gcc%,$(_xv_xCC)),gcc,\
                                  $(if $(filter icc%,$(_xv_xCC)),icc,)))
override COMP_VERSION  := $(subst gcc,,$(subst icc,,$(_xv_xCC)))

# sanity checks (provide defaults if necessary) ...
# TODO: does $(QNX_BASE)/os_version.label) exist?

ifeq (,$(filter $(_xv_xCC), gcc2_95_3 gcc3_3_1 gcc3_3_5 gcc3_4_4 gcc3_4_6 gcc4_2_4 gcc4_3_3 gcc4_4_2 icc11_0 icc11_1))
    $(warning No valid compiler specified in path: $(_xv_xCC))
    override COMPILER_TYPE :=
    override COMP_VERSION  :=
endif

ifeq ($(COMP_VERSION),)
   # overall default is GCC 2.29.3
   override COMPILER_TYPE := gcc
   override COMP_VERSION  := 2_95_3

   # allow OS-specific overrides
   ifeq ($(QNX_VERSION),640)
      override COMP_VERSION:=4_2_4
   endif
   ifeq ($(QNX_VERSION),641)
      override COMP_VERSION:=4_3_3
   endif
   ifeq ($(QNX_VERSION),650)
      override COMP_VERSION:=4_4_2
      # overwrite the QNX_CONFIGURATION Environment variable
      override QNX_CONFIGURATION:=$(P4_ROOT)/OS_Pool/qnx/programs/qssl_misc/qnx650/
   endif
   $(warning Using default compiler $(COMPILER_TYPE) $(COMP_VERSION))
endif

# TODO: make sure _xv_QNX, _xv_xCC are (re-)set -- they are use below!

# set/export GCC_VERSION  ...  [2.95.3, 3.3.1, ...]

override GCC_VERSION := $(subst _,.,$(COMP_VERSION))
export   GCC_VERSION

# set/export QNX_BASE, QNX_HOST, QNX_TARGET ...

override QNX_BASE   := $(P4_QNX_OS_ROOT)/nto/qnx$(QNX_VERSION)/$(if $(NON_DEFAULT_QNX_PATH),$(NON_DEFAULT_QNX_PATH),trunk)
export   QNX_BASE

override QNX_HOST   := $(QNX_BASE)/host/win32/x86
export   QNX_HOST

override QNX_TARGET := $(QNX_BASE)/target/qnx6
export   QNX_TARGET

# set $PATH (for second pass) ...

override PATH := $(QNX_HOST)/usr/bin
export   PATH

# ------------------------------------------------------------------------
# Define TEE_HOST here because it is needed also for the first pass (for gcc.log)
# Note that this will work only if $(SHELL) has not been redefined yet;
# otherwise, the QNX 6.4.11 shell mangles the $(P4_QNX_BUILDPROCESS_ROOT)/tee
# command line (and the build will break with an obscure error message).

TEE_HOST	= $(QNX_HOST)/usr/bin/tee
ifeq ($(QNX_VERSION),632)
    TEE_HOST	= $(P4_QNX_BUILDPROCESS_ROOT)/tee
endif

# ========================================================================
ifeq ($(QNX_MAKEFLAGS),)

override QNX_MAKEFLAGS  := -I$(QNX_TARGET)/usr/include
export   QNX_MAKEFLAGS
unexport MAKEFLAGS

all                                          \
clean iclean spotless	                     \
install	  cinstall   hinstall   qinstall     \
uninstall uncinstall unhinstall	unqinstall   \
showvars pinfo_failure lint doxygen docbook doxyuser:
	$(QNX_HOST)/usr/bin/make $(QNX_MAKEFLAGS) -k $@

# TODO: get rid of DISTRIBUTE_TARGET ...
distribute:
	$(QNX_HOST)/usr/bin/make $(QNX_MAKEFLAGS) DISTRIBUTE_TARGET=YES -k clean
	$(QNX_HOST)/usr/bin/make $(QNX_MAKEFLAGS) DISTRIBUTE_TARGET=YES -k all 2>&1 | $(TEE_HOST) gcc.log
	$(QNX_HOST)/usr/bin/make $(QNX_MAKEFLAGS) DISTRIBUTE_TARGET=YES -k lint doxygen doxyuser docbook install

showdirs:
	@echo "=========================================================="
	@echo "CURDIR: $(CURDIR)"
	@echo
	@echo "- becker.mk: $(_xd_BECKER_MK)"
	@echo "- here:      $(_xd_CWD)"
	@echo "- qversion:  $(_xd_QVERSION)"
	@echo "- common.mk: $(_xd_COMMON_MK)"
	@echo "- variants:  $(_xl_VARIANTS)"
	@echo "- qnx / cc:  $(_xv_QNX) / $(_xv_xCC)"
	@echo
	@echo "- QNX_VERSION:   $(QNX_VERSION)"
	@echo "- COMPILER_TYPE: $(COMPILER_TYPE)"
	@echo "- COMP_VERSION:  $(COMP_VERSION)"
	@echo
	@echo "P4_QNX_OS_ROOT:  $(P4_QNX_OS_ROOT)"
	@echo "   build tools:  $(P4_QNX_BUILDPROCESS_ROOT)"
	@echo "QNX_BASE:        $(QNX_BASE)"
	@echo "=========================================================="
	@exit 1

else # $(QNX_MAKEFLAGS) are defined => pass 2 ============================

include $(_xd_BECKER_MK)/becker2.mk	#xxx:	include $(_xd_COMMON_MK)/common.mk

endif
# ========================================================================
