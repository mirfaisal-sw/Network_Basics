# TODO: pass 2 stuff (was pass 1)...
# - OSCOMP ...

override SHELL := $(QNX_HOST)/usr/bin/sh.exe
export   SHELL

#
#  Extract the Perforce label containing our $(QNX_BASE) from the
#  os_version.label file.  If there is .label file in the version
#  variant directory (one level up), verify that it contains a line
#  matching the original label.  Finally, check that the OS version
#  label is not older than the propject vesrion label.
#

_xtmp:=$(shell $(QNX_HOST)/usr/bin/sed 'p; s@^.*_@ @' < $(QNX_BASE)/os_version.label)
override QNX_P4LABEL        := $(word 1, $(_xtmp))
override QNX_LABEL_DATECODE := $(word 2, $(_xtmp))

ifeq ($(QNX_P4LABEL),)
    $(warning No/invalid os_version.label in $(QNX_BASE))
    $(error   During building in $(_xd_CWD))
endif

_xtmp:=$(shell $(QNX_HOST)/usr/bin/sed 'p; s@^.*_@ @' < $(_xd_QVERSION)/os_version.label)
override PROJECT_P4LABEL    := $(word 1, $(_xtmp))
override PRJ_LABEL_DATECODE := $(word 2, $(_xtmp))

ifeq ($(PROJECT_P4LABEL),)
    $(warning No/invalid os_version.label in $(_xd_QVERSION)/os_version.label)
    $(error   During building in $(_xd_CWD))
endif

#-# $(warning  - QNX_P4LABEL:  $(QNX_LABEL_DATECODE) <= $(QNX_P4LABEL))
#-# $(warning  - PRJ_P4LABEL:  $(PRJ_LABEL_DATECODE) <= $(PROJECT_P4LABEL))
#-# $(warning  ==========================================)

ifneq ($(PRJ_LABEL_DATECODE), $(word 1, $(sort $(QNX_LABEL_DATECODE) $(PRJ_LABEL_DATECODE))))
    $(warning OS-Label datacode $(QNX_LABEL_DATECODE) is less than project requirement $(PRJ_LABEL_DATECODE))
    $(warning Please update operating system version)
    $(error   During building in $(_xd_CWD))
endif

#
#  Determine the top of the build tree (i.e. where the "publish" directory
#  is located).  This is either "Trunk/", "Development/*/" or "Release/*/";
#  a shortcut that's working most of the time is $(PRODUCT_ROOT) (i.e. the
#  directory above the one that contains common.mk.
#

MAIN_ROOT :=
_xp       := $(_xd_COMMON_MK)

_xc := $(patsubst %/,%,$(dir $(_xp)))
_xv := $(notdir $(_xc))
MAIN_ROOT := $(if $(filter Trunk               trunk,               $(_xv)),$(_xc),$(MAIN_ROOT))
MAIN_ROOT := $(if $(filter Development Release development release, $(_xv)),$(_xp),$(MAIN_ROOT))

ifeq ($(MAIN_ROOT),)
    _xp := $(_xc)
    _xc := $(patsubst %/,%,$(dir $(_xp)))
    _xv := $(notdir $(_xc))
    MAIN_ROOT := $(if $(filter Trunk               trunk,               $(_xv)),$(_xc),$(MAIN_ROOT))
    MAIN_ROOT := $(if $(filter Development Release development release, $(_xv)),$(_xp),$(MAIN_ROOT))

    ifeq ($(MAIN_ROOT),)
        _xp := $(_xc)
        _xc := $(patsubst %/,%,$(dir $(_xp)))
        _xv := $(notdir $(_xc))
        MAIN_ROOT := $(if $(filter Trunk               trunk,               $(_xv)),$(_xc),$(MAIN_ROOT))
        MAIN_ROOT := $(if $(filter Development Release development release, $(_xv)),$(_xp),$(MAIN_ROOT))

       ifeq ($(MAIN_ROOT),)
           _xp := $(_xc)
           _xc := $(patsubst %/,%,$(dir $(_xp)))
           _xv := $(notdir $(_xc))
           MAIN_ROOT := $(if $(filter Trunk               trunk,               $(_xv)),$(_xc),$(MAIN_ROOT))
           MAIN_ROOT := $(if $(filter Development Release development release, $(_xv)),$(_xp),$(MAIN_ROOT))

          ifeq ($(MAIN_ROOT),)
              _xp := $(_xc)
              _xc := $(patsubst %/,%,$(dir $(_xp)))
              _xv := $(notdir $(_xc))
              MAIN_ROOT := $(if $(filter Trunk               trunk,               $(_xv)),$(_xc),$(MAIN_ROOT))
              MAIN_ROOT := $(if $(filter Development Release development release, $(_xv)),$(_xp),$(MAIN_ROOT))

             ifeq ($(MAIN_ROOT),)
                 _xp := $(_xc)
                 _xc := $(patsubst %/,%,$(dir $(_xp)))
                 _xv := $(notdir $(_xc))
                 MAIN_ROOT := $(if $(filter Trunk               trunk,               $(_xv)),$(_xc),$(MAIN_ROOT))
                 MAIN_ROOT := $(if $(filter Development Release development release, $(_xv)),$(_xp),$(MAIN_ROOT))
             endif
          endif
       endif
    endif
endif

ifeq ($(MAIN_ROOT),)
    $(warning No Development, Trunk or Release)
    MAIN_ROOT := $(_xd_COMMON_MK)/..    # I'd prefer $(PRODUCT_ROOT), but that that's not set yet...
endif
MAIN_ROOT := $(strip $(MAIN_ROOT))

override INSTALL_ROOT_nto = $(strip $(MAIN_ROOT))/publish/$(_xv_QNX)-$(COMPILER_TYPE)$(COMP_VERSION)

#
#  Provide (sensible) defaults for host tools and their cmd line options;
#  the let individual OS/compiler version override them (if needed).
#
ECHO_HOST	= echo
MKDIR_HOST	= $(QNX_HOST)/usr/bin/mkdir -p
OBJCOPY_HOST	= $(QNX_HOST)/usr/bin/nto$(CPU)-objcopy.exe
STRIP_HOST	= $(QNX_HOST)/usr/bin/nto$(CPU)-strip.exe
USEMSG_HOST	= $(QNX_HOST)/usr/bin/usemsg.exe -t
USE_HOST	= $(QNX_HOST)/usr/bin/use.exe -i

ifeq ($(QNX_VERSION),621)
    STRIP_HOST	= $(QNX_HOST)/usr/bin/strip.exe
    USEMSG_HOST	= $(QNX_HOST)/usr/bin/usemsg.exe
endif


EXTRA_SILENT_VARIANTS += $(_xv_QNX) $(_xv_xCC)
EXTRA_CLEAN           += *.ci2 *.ci3 *.Si2 *.Si3 *.log *.pinfo

include $(_xd_COMMON_MK)/common.mk


USEMSG_TAGS	= USER=$(USERNAME)		\
		  HOST=$(COMPUTERNAME)		\
		  DATE=$(CUR_DATETIME)		\
		  PLABEL=$(DATECODE)		\
		  QNX_LABEL=$(QNX_P4LABEL)	\
		  QNX_VERSION=$(QNX_VERSION)	\
		  GCC_VERSION=$(GCC_VERSION)

#
#  Define actions to be performed during building and installing:
#
#  - DO_STRIP:          how to strip $(INSTALLNAME)
#  - DO_USEMSG:         how to update "use -i" info in the installed file (ELF section)
#  - GET_BUILD_QINFO:   how to get the "use -i" info to be patched into $(BUILDNAME)
#  - GET_INSTALL_QINFO: how to get the "use -i" info to be patched into $(INSTALLNAME)
#  - PATCH_QINFO:       how to patch the "use -i" info in the installed file (.rodata)
#  - PATCH_QINFO_SAR:   how to patch the "use -i" info in the libxxxS.a library
#                       associated and installed along with the shared library libxxx.so
#  - QNXINFO_SYM:       the ELF symbol where the "use -i" should be patched to"
#  - DO_FIX_LINKS:	how to repair/update the ("symbolic") links created by the
#                       INSTALL target for shared objects.
#
#  Note that these are target-specific; we first provide defaults (for a plain executable) ...
#

DO_STRIP	  = $(CP_HOST) $(INSTALLNAME) $(INSTALLNAME).sym; $(STRIP_HOST) $(STRIP_FLAGS) $(INSTALLNAME)
STRIP_FLAGS	  = --strip-all
DO_USEMSG	  = $(USEMSG_HOST) $(addprefix -i,$(USEMSG_TAGS)) $(INSTALLNAME)
GET_BUILD_QINFO   = $(USE_HOST) ./$(BUILDNAME)
GET_INSTALL_QINFO = $(USE_HOST) $(INSTALLNAME)
PATCH_QINFO	  = $(P4_QNX_BUILDPROCESS_ROOT)/patch_qinfo -s $(QNXINFO_SYM)
PATCH_QINFO_SAR	  =
QNXINFO_SYM	  = _QNX_info
DO_FIX_LINKS	  =

#
#  ... and then let the individual target types override them:
#

IS_STARTUP	= $(strip $(filter startup, $(subst -, ,$(BUILDNAME))))
IS_SHARED_LIB	= $(strip $(filter shared so, $(VARIANT_LIST)))
IS_STATIC_LIB	= $(strip $(filter a, $(VARIANT_LIST)))
IS_DLL	= $(strip $(filter dll, $(VARIANT_LIST)))
IS_DEBUG	= $(strip $(filter g, $(VARIANT_LIST)))

ifneq ($(IS_STARTUP),)

    DO_USEMSG		=
    GET_BUILD_QINFO 	= $(QNX_HOST)/usr/bin/cat $(BUILDNAME).pinfo
    GET_INSTALL_QINFO	= $(ECHO_HOST) $(USEMSG_TAGS) | tr ' ' '\n'
    STRIP_FLAGS		= --strip-unneeded
endif

ifneq ($(IS_SHARED_LIB),)

    QNXINFO_SYM		= _QNX_info_$(subst -,_,$(NAME))
    STRIP_FLAGS		= --strip-debug
    DO_FIX_LINKS	= $(CP_HOST) $(INSTALLNAME) $(INSTALL_DIRECTORY)/$(BUILDNAME)
    PATCH_QINFO_SAR	= $(ECHO_HOST) $(USEMSG_TAGS) | tr ' ' '\n' | $(PATCH_QINFO) $(INSTALLNAME_SAR)
endif

ifneq ($(IS_DLL),)

    QNXINFO_SYM		= _QNX_info_$(subst -,_,$(NAME))
    STRIP_FLAGS		= --strip-debug
ifneq ($(SO_VERSION),)
    DO_FIX_LINKS	= $(CP_HOST) $(INSTALLNAME) $(INSTALL_DIRECTORY)/$(BUILDNAME).$(SO_VERSION)
endif
endif


ifneq ($(IS_STATIC_LIB),)

    DO_USEMSG		=
    GET_BUILD_QINFO 	= - $(QNX_HOST)/usr/bin/cat $(BUILDNAME).pinfo
    GET_INSTALL_QINFO	= - $(ECHO_HOST) $(USEMSG_TAGS) | tr ' ' '\n'
    QNXINFO_SYM		= _QNX_info_$(subst -,_,$(NAME))
    DO_STRIP		= 
    STRIP_FLAGS		= --strip-unneeded
endif

ifneq ($(IS_DEBUG),)
    STRIP_FLAGS		= --strip-unneeded
endif

ifeq ($(DISTRIBUTE_TARGET),YES)
LOGFILES  = -g gcc.log
LOGFILES += -l lint.log
LOGFILES += -d $(MAIN_ROOT)/tmp/doxy_$(NAME).log
LOGFILES += -m metrics.log

DO_CREATE_REPORT = \
	$(P4_QNX_BUILDPROCESS_ROOT)/cncc $(SRCS) > metrics.log; \
	$(P4_QNX_BUILDPROCESS_ROOT)/log2html $(NAME) $(DATECODE) $(QNX_P4LABEL) $(P4_QNX_OS_ROOT) $(LOGFILES) -db -o $(INSTALLNAME)_report.html
endif

# XXX: Move to the rules that creates them ...
EXTRA_CLEAN += $(MAIN_ROOT)/tmp/.docbookexec_$(NAME)
EXTRA_CLEAN += $(MAIN_ROOT)/tmp/.doxyexec_$(NAME)
EXTRA_CLEAN += $(MAIN_ROOT)/tmp/.doxyuserexec_$(NAME)
EXTRA_CLEAN += $(MAIN_ROOT)/tmp/doxy_$(NAME).log

# patch XML file into binary if needed
ifneq ($(XML_CONFIGNAME),)
	CMD_XML_CFG = $(QNX_HOST)/usr/bin/cat $(PROJECT_ROOT)/$(XML_CONFIGNAME) | $(P4_QNX_BUILDPROCESS_ROOT)/patch_qinfo -s _XML $(BUILDNAME)
endif

# TODO: verify that "objcopy --remove-section=.note ..." is harmless (for non-executable targets).
define POST_BUILD
    @echo === POST_BUILD start ====================
    $(GET_BUILD_QINFO) | $(PATCH_QINFO) $(BUILDNAME)
    -$(OBJCOPY_HOST) --remove-section=.note $(BUILDNAME)
    $(CMD_XML_CFG) 
    @echo === POST_BUILD done  ====================
endef

define PRE_INSTALL
    @$(ECHO_HOST) Moving results to publish directory
    @$(ECHO_HOST) ===================================
    @$(ECHO_HOST) "Buildname:   $(BUILDNAME)"
    @$(ECHO_HOST) "Installname: $(INSTALLNAME)"

    -$(RM_HOST) $(INSTALLNAME) $(INSTALLNAME).map $(INSTALLNAME).sym $(INSTALLNAME)_report.html
    @echo === PRE_INSTALL done  ====================
endef

# double negation is necessary to work around a bug in the qnx-ksh # [ -f $(BUILDNAME) ] && ... WON'T WORK
define POST_INSTALL
    @echo === POST_INSTALL start  ===================
    -[ ! -f $(BUILDNAME).map ] || $(CP_HOST) $(BUILDNAME).map $(INSTALLNAME).map

    $(DO_USEMSG)
    $(GET_INSTALL_QINFO) | $(PATCH_QINFO) $(INSTALLNAME)
    $(DO_STRIP)
    $(DO_FIX_LINKS)
    $(PATCH_QINFO_SAR)

    $(DO_CREATE_REPORT)

    $(RM_HOST) $(MAIN_ROOT)/$(NAME)_p4.label; $(ECHO_HOST) $(DATECODE) >$(MAIN_ROOT)/$(NAME)_p4.label
    @echo === POST_INSTALL done  ====================
endef

################################################################################################################
#Add compiler specific or architecture specific options

ifeq ($(COMPILER_TYPE),icc)
   # Intel Compiler
   #
   # -xL       Specifies that the compiler is to generate code optimized for the Intel® Atom™ Processor
   #               and future low power Intel processors.  (doesn't work)
   # -Wall     enable all warnings
   # -Wbrief   print brief one-line diagnostics
   # -Wcheck   enable compile-time code checking for certain code

   CCFLAGS     += -Wall
else
   #
   # GNU compiler

# set/correct compiler flags:
ifeq ($(GCC_VERSION),2.95.3)
   CCFLAGS+= -g
endif
ifeq ($(GCC_VERSION),3.3.1)
   CCFLAGS+=-g -fmessage-length=0
endif
ifeq ($(GCC_VERSION),3.3.5)
   CCFLAGS+=-g -fmessage-length=0
endif
ifeq ($(GCC_VERSION),3.4.4)
   CCFLAGS+=-g -fmessage-length=0
   ifeq ($(CPU),sh)
      #SH4A optimization flags
      #XXX: should -Wc,-m4a -Wa,--isa=sh4a better be moved into common.mk????
      CCFLAGS+=-Wc,-m4a -Wa,--isa=sh4a -Wc,-ffast-math
      #stack frame pointer bug in address calculation
      CCFLAGS+=-fno-regmove
      #stack pointer bug when crossjumping
      CCFLAGS+=-fno-crossjumping
   endif
   ifeq ($(CPU),x86)
      #X86 optimization flags
      CCFLAGS+=-Wc,-ffast-math
   endif
endif
ifeq ($(GCC_VERSION),3.4.6)
   CCFLAGS+=-g -fmessage-length=0
endif
ifeq ($(GCC_VERSION),4.2.4)
   CCFLAGS+=-g -fmessage-length=0
   ifeq ($(CPU),sh)
      #SH4A optimization flags
      #XXX: should -Wc,-m4a -Wa,--isa=sh4a better be moved into common.mk????
      CCFLAGS+=-Wc,-m4a -Wa,--isa=sh4a -Wc,-ffast-math
      #stack frame pointer bug in address calculation
      CCFLAGS+=-fno-regmove
      #stack pointer bug when crossjumping
      CCFLAGS+=-fno-crossjumping
   endif
   ifeq ($(CPU),x86)
      #X86 optimization flags
      CCFLAGS+=-Wc,-ffast-math
   endif
endif
endif

#  XXX:
#  - only needed for POST_INSTALL: USEMSG_TAGS (+doxygen, doxyuser)
#  - must be after common.mk (since DVER, NAME, PERF_LABEL_NAME ist defined there).

CUR_DATETIME	:= $(shell $(QNX_HOST)/usr/bin/date -u +%Y/%m/%d_%H-%M-%S_UTC)
DATECODE	:= $(shell $(P4_QNX_BUILDPROCESS_ROOT)/qnxlabel -n$(if $(PERF_LABEL_NAME),$(PERF_LABEL_NAME),$(NAME)) -d$(if $(DVER),$(DVER),A) -o)

################################################################################################################
# Automatic dependency handling

#  Add preprocessor flags that additionally generate/update a file with dependencies.
#  Include all these dependency files, ignoring errors.
#  When cleaning up, remove stale dependency files.

ifeq ($(COMPILER_TYPE),icc)
# -xL       Specifies that the compiler is to generate code optimized for the Intel® Atom™ Processor
#               and future low power Intel processors.  (doesn't work)
# -Wall  enable all warnings
# -Wbrief   print brief one-line diagnostics
# -Wcheck   enable compile-time code checking for certain code

   CCFLAGS     += -MD -MF$(addsuffix .d,$(basename $@))
else
   CCFLAGS     += -Wp,-MD,$(addsuffix .d,$(basename $@))
endif

EXTRA_CLEAN += *.d

-include *.d

################################################################################################################
# Lint - Start
#

# format include-paths
LntInc = $(shell $(ECHO_HOST) $(INCVPATH) | $(QNX_HOST)/usr/bin/sed 's/ / -I/g' | $(QNX_HOST)/usr/bin/sed 's/\. / -I. /g')
LntInc += -i$(P4_QNX_BUILDPROCESS_ROOT)/lint

INCLUDE =

# set qnx-variants
LntVar = $(addprefix -dVARIANT_,$(VARIANT_LIST)) -D__QNXNTO__ -D_QNX_SOURCE

# set lint executable, disable it for startup temporarily
ifeq ($(IS_STARTUP),startup)
LntExe = $(ECHO_HOST)
else
LntExe = $(P4_QNX_BUILDPROCESS_ROOT)/lint/lint-nt
endif

# set lint-settings-file
LntIni = -u $(P4_QNX_BUILDPROCESS_ROOT)/lint/common.lnt
LntIni += $(P4_QNX_BUILDPROCESS_ROOT)/lint/Target_QNXSH4.lnt
LntIni += $(P4_QNX_BUILDPROCESS_ROOT)/lint/Format_MOMENTICS_6_2_1.lnt
LntIni += $(P4_QNX_BUILDPROCESS_ROOT)/lint/Exeptions_for_QNX-Driver.lnt

# set lint executable
LntSuf = +cpp[cpp,cxx,cc]

#set cmpiler
ifeq ($(COMPILER_TYPE),gcc)
	LntCmp	= -D__GNUC__
endif

# set cpu-dependent flags
ifeq ($(CPU),x86)
	LntCpu 	= -D__X86__ -D__i386__
endif
ifeq ($(CPU),sh)
	LntCpu 	= -D__SH__ -D__SH4__
endif
ifeq ($(CPU),ppc)
	LntCpu 	= -D__PPC__
endif
ifeq ($(CPU),arm)
	LntCpu 	= -D__ARM__
endif
ifeq ($(CPU),mips)
	LntCpu 	= -D__MIPS__
endif
ifeq ($(CPU),sparc)
	LntCpu 	= -D__SPARC__
endif

# set endianess
ifeq ($(strip $(filter be,$(VARIANT_LIST))),be)
    LntEndian = -D__BIGENDIAN__
else
    LntEndian = -D__LITTLEENDIAN__
endif

.PHONY:	lint
lint:
	$(LntExe) $(LntInc) $(LntIni) $(LntVar) $(LntCmp) $(LntCpu) $(LntEndian) $(filter -D%, $(CCFLAGS)) $(LntSuf) $(SRCS) 2>&1 | $(TEE_HOST) lint.log

#
# Lint - End
################################################################################################################

DOXY_VARIANT=$(CPU)-$(_xv_QNX)-$(COMPILER_TYPE)$(COMP_VERSION)-$(filter g, $(VARIANT_LIST))
do_DOXY_here=$(if $(DOXY_VERSION),$(DOXY_VERSION)-,sh-qnx630-gcc2_95_3-)

ifeq ($(DOXY_VARIANT),$(do_DOXY_here))

    DOXYGEN_FLAG	= YES
    DOCBOOK_FLAG	= YES

    ifneq ($(filter so a, $(VARIANT_LIST)),)
        DOXYUSER_FLAG	= YES
    endif

endif

################################################################################################################
# Doxygen - Start
#

# set doxygen executable
DoxyExe = $(P4_QNX_BUILDPROCESS_ROOT)/doxygen/doxygen.exe
# set doxygen config
DoxyConf = $(P4_QNX_BUILDPROCESS_ROOT)/doxygen/Templates/DoxyfileTemplate
# set doxygen output folder
DoxyGenFolder = $(MAIN_ROOT)/publish/doc
DoxyGenUserFolder = $(MAIN_ROOT)/publish/doc/user

.PHONY:	doxygen
doxygen: $(if $(DOXYGEN_FLAG),$(MAIN_ROOT)/tmp/.doxyexec_$(NAME))

_dx_SOURCES_DIR		= "$(defsrcvpath) $(EXTRA_SRCVPATH)"
_dx_INCLUDE_DIR		= "$(EXTRA_INCVPATH)"
_dx_PROJECT_NUMBER	= $(DATECODE)
_dx_PROJECT_NAME	= $(NAME)
_dx_TEMPLATE_DIR	= $(P4_QNX_BUILDPROCESS_ROOT)/doxygen/Templates
_dx_PRODUCTS_DIR	= $(DoxyGenFolder)/..
_dx_DOC_DIR		= "doc"

$(MAIN_ROOT)/tmp/.doxyexec_$(NAME):
	-$(MKDIR_HOST) -p $(MAIN_ROOT)/publish
	-$(MKDIR_HOST) -p $(MAIN_ROOT)/tmp
	-$(MKDIR_HOST) -p $(DoxyGenFolder)
	-$(MKDIR_HOST) -p $(DoxyGenFolder)/images
	-$(MKDIR_HOST) -p $(DoxyGenFolder)/Doxygen_$(NAME)
	-$(RM_HOST)    -f $(DoxyGenFolder)/Doxygen_$(NAME)/html/*.*
	-$(RM_HOST) -f $@
	$(ECHO_HOST) PROJECT_NAME           = "$(_dx_PROJECT_NAME)"				>> $@
	$(ECHO_HOST) PROJECT_NUMBER         = "$(_dx_PROJECT_NUMBER)"				>> $@
	$(ECHO_HOST) OUTPUT_DIRECTORY       = "$(_dx_PRODUCTS_DIR)/$(_dx_DOC_DIR)/Doxygen_$(_dx_PROJECT_NAME)" >> $@
	$(ECHO_HOST) INPUT                  = $(_dx_SOURCES_DIR)				>> $@
	$(ECHO_HOST) IMAGE_PATH             = $(_dx_PRODUCTS_DIR)/doc/images			>> $@
	$(ECHO_HOST) HTML_HEADER            = "$(_dx_TEMPLATE_DIR)/Becker_header.html"		>> $@
	$(ECHO_HOST) HTML_FOOTER            = "$(_dx_TEMPLATE_DIR)/Becker_footer.html"		>> $@
	$(ECHO_HOST) INCLUDE_PATH           = $(_dx_INCLUDE_DIR)				>> $@
	$(QNX_HOST)/usr/bin/cat $(DoxyConf) $@ | $(DoxyExe) - 2>&1 | $(TEE_HOST) $(MAIN_ROOT)/tmp/doxy_$(NAME).log


.PHONY:	doxyuser
doxyuser: $(if $(DOXYUSER_FLAG),$(MAIN_ROOT)/tmp/.doxyuserexec_$(NAME))

_du_SOURCES_DIR		= "$(MAIN_ROOT)/src/user_doxy $(MAIN_ROOT)/publish/include/$(IMAGE_PREF_$(BUILD_TYPE))$(NAME)$(VARIANT_TAG)"
_du_PROJECT_NUMBER	= $(DATECODE)
_du_PROJECT_NAME	= $(NAME)
_du_TEMPLATE_DIR	= $(P4_QNX_BUILDPROCESS_ROOT)/doxygen/Templates
_du_PRODUCTS_DIR	= $(DoxyGenUserFolder)/../..
_du_DOC_DIR		= "doc/user"

$(MAIN_ROOT)/tmp/.doxyuserexec_$(NAME):
	-$(MKDIR_HOST) -p $(MAIN_ROOT)/publish
	-$(MKDIR_HOST) -p $(MAIN_ROOT)/tmp
	-$(MKDIR_HOST) -p $(DoxyGenUserFolder)
	-$(MKDIR_HOST) -p $(DoxyGenUserFolder)/../images
	-$(MKDIR_HOST) -p $(DoxyGenUserFolder)/Doxygen_$(NAME)
	-$(RM_HOST)    -f $(DoxyGenUserFolder)/Doxygen_$(NAME)/html/*.*
	-$(RM_HOST) -f $@
	$(ECHO_HOST) PROJECT_NAME           = "$(_du_PROJECT_NAME)"				>> $@
	$(ECHO_HOST) PROJECT_NUMBER         = "$(_du_PROJECT_NUMBER)"				>> $@
	$(ECHO_HOST) OUTPUT_DIRECTORY       = "$(_du_PRODUCTS_DIR)/$(_du_DOC_DIR)/Doxygen_$(_du_PROJECT_NAME)" >> $@
	$(ECHO_HOST) INPUT                  = $(_du_SOURCES_DIR)				>> $@
	$(ECHO_HOST) IMAGE_PATH             = $(_du_PRODUCTS_DIR)/doc/images			>> $@
	$(ECHO_HOST) HTML_HEADER            = "$(_du_TEMPLATE_DIR)/Becker_header.html"		>> $@
	$(ECHO_HOST) HTML_FOOTER            = "$(_du_TEMPLATE_DIR)/Becker_footer.html"		>> $@
	$(ECHO_HOST) INCLUDE_PATH           = $(_du_INCLUDE_DIR)				>> $@
	$(QNX_HOST)/usr/bin/cat $(DoxyConf) $@ | $(DoxyExe) - 2>&1 | $(TEE_HOST) $(MAIN_ROOT)/tmp/doxy_$(NAME).log

# Doxygen - End
################################################################################################################

# XXX: This is needed by the docbook target; maybe replace by $(PROJECT_ROOT)
BASEPRJDIR := $(_xd_COMMON_MK)

################################################################################################################
# DocBook - Start
#
# set docbook destination folder
DocbookFolder = $(MAIN_ROOT)/publish/doc/Docbook
# set docbook executables
XSLTConverter = $(P4_QNX_BUILDPROCESS_ROOT)/docbook/xsltproc/xsltproc
XSLPath = $(P4_QNX_BUILDPROCESS_ROOT)/docbook/docbook-xsl-1.66.1
DocBookOutFile = $(DocbookFolder)/$(NAME).html
DocBookInFile = $(BASEPRJDIR)/$(NAME).xml
XLSTConfig = --nonet --param shade.verbatim 1 --param section.autolabel 1  --output $(DocBookOutFile) $(XSLPath)/html/docbook.xsl

.PHONY:	docbook
docbook:
	$(if $(DOCBOOK_FLAG), \
	[ -f $(MAIN_ROOT)/tmp/.docbookexec_$(NAME) ] || (  \
		if [ ! -d $(DocbookFolder) ]; then $(MKDIR_HOST) $(DocbookFolder); fi; \
		if [ -f $(DocBookOutFile) ]; then $(RM_HOST) $(DocBookOutFile); fi; \
        $(ECHO_HOST) "<?xml version='1.0'?>" >$(BASEPRJDIR)/catalog.xml; \
		$(ECHO_HOST) '<catalog xmlns="urn:oasis:names:tc:entity:xmlns:xml:catalog">' >>$(BASEPRJDIR)/catalog.xml; \
		$(ECHO_HOST) '<group id="DocbookDTD" prefer="public">' >>$(BASEPRJDIR)/catalog.xml; \
		$(ECHO_HOST) '<system systemId="http://www.oasis-open.org/docbook/xml/4.2/docbookx.dtd" uri="file:///$(P4_QNX_BUILDPROCESS_ROOT)/docbook/xml/4.2/docbookx.dtd"/>' >>$(BASEPRJDIR)/catalog.xml \
		$(ECHO_HOST) '<system systemId="http://www.oasis-open.org/docbook/xml/4.3b2/docbookx.dtd" uri="file:///$(P4_QNX_BUILDPROCESS_ROOT)/docbook/xml/4.3/docbookx.dtd"/>' >>$(BASEPRJDIR)/catalog.xml; \
		$(ECHO_HOST) '<system systemId="http://www.oasis-open.org/docbook/xml/4.4/docbookx.dtd" uri="file:///$(P4_QNX_BUILDPROCESS_ROOT)/docbook/xml/4.4/docbookx.dtd"/>' >>$(BASEPRJDIR)/catalog.xml; \
		$(ECHO_HOST) '<system systemId="http://www.oasis-open.org/docbook/xml/4.5b1/docbookx.dtd" uri="file:///$(P4_QNX_BUILDPROCESS_ROOT)/docbook/xml/4.5b1/docbookx.dtd"/>' >>$(BASEPRJDIR)/catalog.xml; \
		$(ECHO_HOST) '<system systemId="http://www.oasis-open.org/docbook/xml/5.0a1/docbookx.dtd" uri="file:///$(P4_QNX_BUILDPROCESS_ROOT)/docbook/xml/5.0a1/docbookx.dtd"/>' >>$(BASEPRJDIR)/catalog.xml; \
		$(ECHO_HOST) '</group>' >>$(BASEPRJDIR)/catalog.xml; \
		$(ECHO_HOST) '</catalog>' >>$(BASEPRJDIR)/catalog.xml; \
	  export XML_CATALOG_FILES=$(BASEPRJDIR)/catalog.xml; \
        if [ -f $(DocBookInFile) ]; then $(XSLTConverter) $(XLSTConfig) $(DocBookInFile); fi; \
		$(RM_HOST) $(BASEPRJDIR)/catalog.xml; \
		if [ ! -d $(MAIN_ROOT)/tmp ]; then $(MKDIR_HOST) $(MAIN_ROOT)/tmp; fi; \
		$(ECHO_HOST) done >$(MAIN_ROOT)/tmp/.docbookexec_$(NAME)))

# DocBook - End
################################################################################################################
