CFLAGS=-Wall -O2
CFLAGS += -Wno-write-strings
CFLAGS += -Wno-stringop-truncation
CFLAGS += -Wno-format-overflow

#LiFLAGS = -Ider_libs
CFLAGS += -Ider_libs

#  clang-tidy options
CHFLAGS = -header-filter=.*
CHTAIL = -- -Ider_libs 

CPPSRC=wbigcalc.cpp bigcalc.cpp bigmath.cpp bigmisc.cpp bigprint.cpp \
config.cpp options.cpp about.cpp hyperlinks.cpp \
der_libs/common_funcs.cpp \
der_libs/common_win.cpp \
der_libs/winmsgs.cpp \
der_libs/tooltips.cpp \
der_libs/statbar.cpp
	
LINTFILES=lintdefs.cpp lintdefs.ref.h 

OBJS = $(CPPSRC:.cpp=.o) dlgres.o

BIN=wbigcalc.exe

#************************************************************
%.o: %.cpp
	g++ $(CFLAGS) -Weffc++ -c $< -o $@

all: $(BIN)

clean:
	rm -vf $(BIN) $(OBJS)

wc:
	wc -l *.cpp *.rc

check:
	cmd /C "d:\clang\bin\clang-tidy.exe $(CHFLAGS) $(CPPSRC) $(CHTAIL)"

lint:
	cmd /C "c:\lint9\lint-nt +v -width(160,4) -Ider_libs +fcp -ic:\lint9 mingw.lnt -os(_lint.tmp) $(LINTFILES) dlgres.rc $(CPPSRC)"

dist:
	rm -f wbigcalc.zip
	zip wbigcalc.zip wbigcalc.exe wbigcalc.chm bigcalc.txt LICENSE readme.md

$(BIN): $(OBJS)
	g++ $(CFLAGS) -mwindows -s $(OBJS) -o $@ -lcomctl32 -lgdi32 -lcomdlg32 -lhtmlhelp

dlgres.o: dlgres.rc
	windres -O COFF $< -o $@

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
about.o: resource.h version.h hyperlinks.h
hyperlinks.o: hyperlinks.h
der_libs/common_funcs.o: der_libs/common.h
der_libs/common_win.o: der_libs/common.h der_libs/commonw.h
der_libs/tooltips.o: der_libs/iface_32_64.h der_libs/common.h
der_libs/tooltips.o: der_libs/tooltips.h
der_libs/statbar.o: der_libs/common.h der_libs/commonw.h der_libs/statbar.h
