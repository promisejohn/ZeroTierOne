# Pick clang or gcc, with preference for clang
CC=$(shell if [ -e /usr/bin/clang ]; then echo clang; else echo gcc; fi)
CXX=$(shell if [ -e /usr/bin/clang++ ]; then echo clang++; else echo g++; fi)

INCLUDES=
DEFS=
LIBS=

include objects.mk
OBJS+=osdep/LinuxEthernetTap.o 

# "make official" is a shortcut for this
ifeq ($(ZT_OFFICIAL_RELEASE),1)
	DEFS+=-DZT_OFFICIAL_RELEASE 
endif

# Build with ZT_ENABLE_NETWORK_CONTROLLER=1 to build with the Sqlite network controller
ifeq ($(ZT_ENABLE_NETWORK_CONTROLLER),1)
        DEFS+=-DZT_ENABLE_NETWORK_CONTROLLER 
        LIBS+=-L/usr/local/lib -lsqlite3
        OBJS+=controller/SqliteNetworkController.o 
endif

# "make debug" is a shortcut for this
ifeq ($(ZT_DEBUG),1)
	DEFS+=-DZT_TRACE 
	CFLAGS=-Wall -g -pthread $(INCLUDES) $(DEFS)
	LDFLAGS=
	STRIP=echo
	# The following line enables optimization for the crypto code, since
	# C25519 in particular is almost UNUSABLE in heavy testing without it.
ext/lz4/lz4.o node/Salsa20.o node/SHA512.o node/C25519.o node/Poly1305.o: CFLAGS = -Wall -O2 -g -pthread $(INCLUDES) $(DEFS)
else
	CFLAGS=-Wall -O3 -fPIE -fvisibility=hidden -fstack-protector -pthread $(INCLUDES) -DNDEBUG $(DEFS)
	LDFLAGS=-pie -Wl,-z,relro,-z,now
	STRIP=strip --strip-all
endif

# Uncomment for gprof profile build
#CFLAGS=-Wall -g -pg -pthread $(INCLUDES) $(DEFS)
#LDFLAGS=
#STRIP=echo

CXXFLAGS=$(CFLAGS) -fno-rtti

all:	one

one:	$(OBJS) one.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o zerotier-one $(OBJS) one.o $(LIBS)
	$(STRIP) zerotier-one
	ln -sf zerotier-one zerotier-idtool
	ln -sf zerotier-one zerotier-cli

selftest:	$(OBJS) selftest.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o zerotier-selftest selftest.o $(OBJS) $(LIBS)
	$(STRIP) zerotier-selftest

installer: one FORCE
	./ext/installfiles/linux/buildinstaller.sh

clean:
	rm -rf *.o node/*.o controller/*.o osdep/*.o service/*.o ext/http-parser/*.o ext/lz4/*.o ext/json-parser/*.o zerotier-one zerotier-idtool zerotier-cli zerotier-selftest build-* ZeroTierOneInstaller-* *.deb *.rpm

debug:	FORCE
	make -j 4 ZT_DEBUG=1

official: FORCE
	make -j 4 ZT_OFFICIAL_RELEASE=1
	make ZT_OFFICIAL_RELEASE=1 installer

FORCE:
