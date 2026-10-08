USE_DEBUG = NO
USE_64BIT = NO
USE_UNICODE = NO
USE_CLANG = YES

USE_CYGWIN = NO

# if USE_INNO = YES, build the Inno Setup installer
# if USE_INNO = NO, `setup` and `install` targets will not be used
USE_INNO = YES

# the legacy version of qualify.cpp, does not depend upon c++ string class
USE_LEGACY = NO

include der_libs\tool_select.mak
include der_libs\release.mak

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -O -g
LFLAGS= -mwindows 
else
CFLAGS=-Wall -O2 -c 
LFLAGS=-s -mwindows 
endif
CFLAGS += -Wno-write-strings
CFLAGS += -Wno-format-overflow
CFLAGS += -Weffc++ 

#LiFLAGS = -Ider_libs
CFLAGS += -Ider_libs
CFLAGS += -Imingw_libs

LFLAGS += -Lmingw_libs

ifeq ($(USE_STATIC),YES)
LFLAGS += -static
endif

CPPSRC=wbigcalc.cpp bigcalc.cpp bigmath.cpp bigmisc.cpp bigprint.cpp \
config.cpp options.cpp about.cpp \
der_libs/hyperlinks.cpp \
der_libs/common_funcs.cpp \
der_libs/common_win.cpp \
der_libs/winmsgs.cpp \
der_libs/tooltips.cpp \
der_libs/statbar.cpp
	
OBJS = $(CPPSRC:.cpp=.o) dlgres.o

BASE=wbigcalc
BIN=$(BASE).exe

LIBS=-lcomctl32 -lgdi32 -lcomdlg32

ifeq ($(USE_64BIT),YES)
LIBS += -lhhctrl64
else
LIBS += -lhhctrl32
endif

# Distribution targets: the same names (setup, dist, install) work in both
# modes; USE_INNO (top of file) decides what they do.
#
# USE_INNO = YES  (Inno Setup installer; the generic rules installer,
#                  installer-zip and install-silent live in der_libs\release.mak)
#   make            build the exe
#   make setup      exe (if out of date) -> iscc -> Output\wbigcalcV<ver>.setup.exe
#   make dist       setup, then Output\wbigcalcV<ver>.setup.zip
#   make install    silent-install the setup exe, for smoke-testing
#   "make release" / "make update" (release.mak) depend on "dist" and upload
#   only the setup zip. A missing or broken .iss makes iscc fail loudly.
#   DIST_ZIP points at the setup zip, so release.mak's plain "sha256" target
#   checksums the installer zip.
#
# USE_INNO = NO   (legacy loose-files distribution)
#   make            build the exe
#   make setup      does nothing (make reports "Nothing to be done")
#   make dist       portable zip $(BASE)V<ver>.zip
#   make install    does nothing
#   "make release" / "make update" upload the portable zip + CHANGELOG.md
#   (release.mak's default RELEASE_ASSETS).

# Force these action-only targets to always run
.PHONY: setup dist install

################   USE_INNO  ################
ifeq ($(USE_INNO),YES)

DIST_ZIP = $(SETUP_ZIP)
RELEASE_ASSETS = ./$(SETUP_ZIP)

# Recipe-less rule: adds a prerequisite to the generic "installer" target in
# release.mak, so the installer always packages a freshly built exe.
installer: $(BIN)

# PrettyReMark-style names for the generic release.mak targets.
setup: installer
dist: installer-zip
install: install-silent

else

DIST_ZIP := $(BASE)V$(VERSION).zip

# No installer in this mode: no prerequisites and no recipe, so make just
# reports "Nothing to be done".
setup:
install:

# Clears old zips from the project folder, then builds the portable zip.
dist:
	rm -f *.zip
	zip $(DIST_ZIP) $(BASE).exe $(BASE).chm bigcalc.txt CHANGELOG.md LICENSE.txt readme.md $(BASE).ini

endif

#************************************************************
%.o: %.cpp
	$(TOOLS)/$(GNAME) $(CFLAGS) $< -o $@

all: $(BIN)

clean:
	rm -vf $(BIN) $(OBJS)
ifeq ($(USE_INNO),YES)
	rm -rf $(SETUP_DIR)
endif

wc:
	wc -l *.cpp *.rc

clint:
	cmd /C "python ..\ClaudeLint.py --exclude der_libs"
	
cppc:
	cmd /C "cppcheck --project=compile_commands.json --std=c++14 --suppressions-list=./.suppress.cppcheck"

check:
	cmd /C "d:\llvm\bin\clang-tidy.exe $(CPPSRC)"

depend:
	makedepend $(CPPSRC)

#************************************************************
$(BIN): $(OBJS)
	$(TOOLS)/$(GNAME) $(OBJS) $(LFLAGS) -o $(BIN) $(LIBS) 

dlgres.o: dlgres.rc
	$(TOOLS)\$(WRNAME) $< -O COFF -o $@

# DO NOT DELETE

wbigcalc.o: version.h keywin32.h resource.h bigcalc.h
bigcalc.o: resource.h bigcalc.h keywin32.h
bigmath.o: bigcalc.h
bigmisc.o: keywin32.h bigcalc.h
bigprint.o: bigcalc.h
config.o: bigcalc.h
options.o: resource.h bigcalc.h
about.o: resource.h version.h
der_libs/hyperlinks.o: der_libs/iface_32_64.h der_libs/hyperlinks.h
der_libs/common_funcs.o: der_libs/common.h
der_libs/common_win.o: der_libs/common.h der_libs/commonw.h
der_libs/tooltips.o: der_libs/iface_32_64.h der_libs/common.h
der_libs/tooltips.o: der_libs/tooltips.h
der_libs/statbar.o: der_libs/common.h der_libs/commonw.h der_libs/statbar.h
