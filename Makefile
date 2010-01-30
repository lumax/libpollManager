PROJECT_NAME=libpollManager
MAJOR=1
MINOR=0
VERSION=$(MAJOR).$(MINOR)

DEFS+=-D_GNU_SOURCE=1 -D_REENTRANT
INCLUDES+=-I$(INCLUDE_DIR)/libruputils
LIBS+=

CFLAGS+=-g -c -Wall -fPIC 
LDFLAGS+= -lruputils -shared -Wl

OBJS = pollManager.o


EXE_ANHANG = .so.$(VERSION)

include $(MAKE_DIR)/global.mak

