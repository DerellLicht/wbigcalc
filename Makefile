CFLAGS=-Wall -O3
CFLAGS += -Wno-write-strings

CPPSRC=wbigcalc.cpp bigcalc.cpp bigmath.cpp bigmisc.cpp bigprint.cpp bigio.cpp statbar.cpp wcommon.cpp
	
OBJS = $(CPPSRC:.cpp=.o) dlgres.o

BIN=wbigcalc.exe

%.o: %.cpp
	g++ $(CFLAGS) -c $<

all: $(BIN)

clean:
	del /zqe $(BIN) *.o

wc:
	wc -l *.cpp *.rc

lint:
	cmd /C "c:\lint9\lint-nt +v -width(160,4) +fcp -ic:\lint9 mingw.lnt -os(_lint.tmp) lintdefs.cpp *.rc $(CPPSRC)"

src:
	zip wbigcalc.zip *.cpp *.h *.rc makefile *.txt *.doc *.ico

$(BIN): $(OBJS)
	g++ $(CFLAGS) -s $(OBJS) -o $@ -lcomctl32 -lgdi32

dlgres.o: dlgres.rc
	windres -O COFF $< -o $@

depend:
	makedepend $(CPPSRC)

# DO NOT DELETE

wbigcalc.o: keywin32.h resource.h bigcalc.h statbar.h
bigcalc.o: bigcalc.h keywin32.h
bigmath.o: bigcalc.h biggvar.h
bigmisc.o: bigcalc.h biggvar.h
bigprint.o: bigcalc.h biggvar.h
bigio.o: bigcalc.h
statbar.o: statbar.h
wcommon.o: wcommon.h
