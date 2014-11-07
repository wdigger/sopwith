bindir=$(prefix)/bin

SUBDIRS = @SOPWITH_SUBDIRS@
EXTRA_SUBDIRS = sdl gtk
DIST_SUBDIRS = $(EXTRA_SUBDIRS)

bin_PROGRAMS = @SOPWITH_BINS@
EXTRA_PROGRAMS = sopwith gtksopwith

COMMON_SOURCES =       \
video.h       swcollsn.cpp  swgames.h     swmain.h      swutil.h                \
font.h        swcollsn.h    swground.cpp  swtext.cpp    tcpcomm.cpp             \
pcsound.h     swconf.cpp    swground.h    swtext.h      swsound.cpp   tcpcomm.h \
std.h         swconf.h      swgrpha.cpp   swmove.cpp    swsound.h     timer.h   \
sw.h          swdisp.cpp    swgrpha.h     swmove.h      swsymbol.cpp            \
swasynio.cpp  swdisp.h      swinit.cpp    swsymbol.h                            \
swasynio.h    swend.cpp     swinit.h      swtitle.cpp                           \
swauto.cpp    swend.h       swobject.cpp  swtitle.h                             \
swauto.h      swgames.cpp   swmain.cpp    swobject.h    swutil.cpp              \
swsplat.cpp   swsplat.h     swstbar.cpp   swstbar.h                             \
video.cpp

# include dos port sources in distribution

DOS_FILES = dos/Makefile  dos/pcsound.c  dos/README  dos/timer.c

EXTRA_DIST=$(DOS_FILES)

# SDL build

sopwith_SOURCES = $(COMMON_SOURCES) vid_vga.cpp
sopwith_LDADD = sdl/libsdlsopwith.a @SDL2_LIBS@

# Gtk+ build

gtksopwith_SOURCES = $(COMMON_SOURCES) vid_vga.cpp
gtksopwith_LDADD = gtk/libgtksopwith.a @GTK_LIBS@

