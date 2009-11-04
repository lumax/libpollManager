PROJECT_NAME=libpollManager
MAJOR=1
MINOR=0
VERSION=$(MAJOR).$(MINOR)

DEFS+=-D_GNU_SOURCE=1 -D_REENTRANT
INCLUDES+=-I../../include/libruputils
LIBS+=-L../../lib
#    gcc -Wall -fPIC -c *.c
#    gcc -shared -Wl,-soname,libctest.so.1 -o libctest.so.1.0   *.o
#    mv libctest.so.1.0 /opt/lib
#    ln -sf /opt/lib/libctest.so.1.0 /opt/lib/libctest.so
#    ln -sf /opt/lib/libctest.so.1.0 /opt/lib/libctest.so.1
CFLAGS+=-g -c -Wall -fPIC
LDFLAGS+= -lruputils -shared -Wl #,-soname,$(PROJECT_NAME).so.$(MAJOR)

OBJS = pollManager.o

#include $(MAKE_DIR)/global.mak
rm=/bin/rm -f

CXX=${CROSS_COMPILE}gcc
LINKER=${CROSS_COMPILE}gcc
LOADER=${CROSS_COMPILE}ldd
FILE=file
GREAT_CODE=echo -----

DEFS+=  
EXE= $(PROJECT_NAME).so.$(VERSION)
INCLUDES+=  -I.
LIBS+=

DEFINES+= $(INCLUDES) $(DEFS) 
CFLAGS+= $(DEFINES)
LDFLAGS+= 


OBJS+= 
#$(PROJECT_NAME).so.1 

.c.o:
	$(rm) $@
	$(CXX) $(CFLAGS) -c $*.c
	@$(GREAT_CODE) -file-$*.c

all: clean $(EXE) devpub

$(EXE) : $(OBJS)
	$(LINKER) $(LDFLAGS) -o $(EXE) $(OBJS) $(LIBS)

clean:
	$(rm) $(OBJS) $(EXE) core.* core *~ gmon.out *.gcov *.gcda *.gcno lib*

libs_used:
	$(LOADER) $(EXE)

file_type:
	$(FILE) $(EXE)

devpub:
	ln -sf $(PROJECT_NAME).so.$(VERSION) $(PROJECT_NAME).so
	mv -f $(PROJECT_NAME).so.$(VERSION) $(LIB_DIR)/$(PROJECT_NAME).so.$(VERSION)
	mv -f $(PROJECT_NAME).so $(LIB_DIR)/$(PROJECT_NAME).so
	rm -fR $(INCLUDE_DIR)/$(PROJECT_NAME)
	mkdir -p $(INCLUDE_DIR)/$(PROJECT_NAME)
	cp *.h $(INCLUDE_DIR)/$(PROJECT_NAME)/

#.PHONY : $(PROJECT_NAME).so.$(MAJOR)
