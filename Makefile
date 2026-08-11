USE_DEBUG = NO
USE_64BIT = NO
USE_UNICODE = NO
USE_CLANG = NO

# sadly, cygwin mingw does not support gdiplus...
USE_CYGWIN = NO
# use -static for clang and cygwin/mingw
#  clang vs tdm
#  clang gives *much* clearer compiler error messages...
#  However, programs built with clang++ will require libc++.dll and libunwind.dll
#  in order to be used elsewhere 
#  (unless built with -static, which significantly boosts file size)

# the legacy version of qualify.cpp, does not depend upon c++ string class
USE_LEGACY = NO

include ..\tool_select.mak 

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

CPPSRC=wbigcalc.cpp bigcalc.cpp bigmath.cpp bigmisc.cpp bigprint.cpp \
config.cpp options.cpp about.cpp \
der_libs/hyperlinks.cpp \
der_libs/common_funcs.cpp \
der_libs/common_win.cpp \
der_libs/winmsgs.cpp \
der_libs/tooltips.cpp \
der_libs/statbar.cpp
	
LINTFILES=lintdefs.cpp lintdefs.ref.h 

OBJS = $(CPPSRC:.cpp=.o) dlgres.o

BIN=wbigcalc.exe

LIBS=-lcomctl32 -lgdi32 -lcomdlg32 -lhtmlhelp

#************************************************************
%.o: %.cpp
	$(TOOLS)/$(GNAME) $(CFLAGS) $< -o $@

all: $(BIN)

clean:
	rm -vf $(BIN) $(OBJS)

wc:
	wc -l *.cpp *.rc

clint:
	cmd /C "python ..\ClaudeLint.py --exclude der_libs"
	
cppc:
	cmd /C "cppcheck --project=compile_commands.json --std=c++14 --suppressions-list=./.suppress.cppcheck"

check:
	cmd /C "d:\llvm\bin\clang-tidy.exe $(CPPSRC)"

lint:
	cmd /C "c:\lint9\lint-nt +v -width(160,4) -Ider_libs +fcp -ic:\lint9 mingw.lnt -os(_lint.tmp) $(LINTFILES) dlgres.rc $(CPPSRC)"

dist:
	rm -f wbigcalc.zip
	zip wbigcalc.zip wbigcalc.exe wbigcalc.chm bigcalc.txt LICENSE.txt readme.md

$(BIN): $(OBJS)
	$(TOOLS)/$(GNAME) $(OBJS) $(LFLAGS) -o $(BIN) $(LIBS) 

dlgres.o: dlgres.rc
	$(TOOLS)\$(WRNAME) $< -O COFF -o $@

depend:
	makedepend $(CPPSRC)

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
