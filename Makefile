MAKEFLAGS += --no-builtin-rules

V ?= $(VERBOSE)
ifeq ($(V),1)
	Q :=
	VECHO := @true
	MAKEMINS :=
else
	Q := @
	VECHO := @echo
	MAKEMINS := -s
endif

CPP				:=	g++

DBUS_CFLAGS		!=	pkg-config --cflags dbus-1
DBUS_LIBS		!=	pkg-config --libs dbus-1

CPPFLAGS		:= -O3 -fPIC $(DBUS_CFLAGS) $(DBUS_LIBS) -lboost_program_options -I.

SERVER			:= dbus-tiny-server
CLIENT			:= dbus-tiny-client

LIBOBJS			:= exception.o server.o client.o
LIB				:= libdbus-tiny.so
EXECOBJS		:= $(SERVER).o $(CLIENT).o
HDRS			:= dbus-tiny.h
SWIG_DIR		:= DBUS
SWIG_SRC		:= DBUS\:\:Tiny.i
SWIG_PM			:= Tiny.pm
SWIG_PM_2		:= $(SWIG_DIR)/Tiny.pm
SWIG_WRAP_SRC	:= DBUS\:\:Tiny_wrap.cpp
SWIG_WRAP_OBJ	:= DBUS\:\:Tiny_wrap.o
SWIG_SO			:= DBUS\:\:Tiny.so
SWIG_SO_2		:= $(SWIG_DIR)/Tiny.so

.PRECIOUS:		*.cpp *.i
.PHONY:			all swig

all:			$(LIB) $(SERVER) $(CLIENT) swig

swig:			$(SWIG_PM_2) $(SWIG_SO_2)

clean:
				$(VECHO) "CLEAN"
				-$(Q) rm -rf $(LIBOBJS) $(EXECOBJS) $(SERVER) $(CLIENT) $(SWIG_WRAP_SRC) $(SWIG_PM) $(SWIG_PM_2) $(SWIG_WRAP_OBJ) $(SWIG_SO) $(SWIG_SO_2) $(SWIG_DIR) 2> /dev/null

exception.o:	$(HDRS)
server.o:		$(HDRS)
client.o:		$(HDRS)
$(SERVER).o:	$(HDRS)
$(CLIENT).o:	$(HDRS)
$(SWIG_PM):		$(HDRS)
$(SWIG_SRC):	$(HDRS)

%.o:			%.cpp
				$(VECHO) "CPP $< -> $@"
				$(Q) $(CPP) @gcc-warnings $(CPPFLAGS) -c $< -o $@

$(LIB):			$(LIBOBJS)
				$(VECHO) "LD $(LIBOBJS) -> $@"
				$(Q) $(CPP) @gcc-warnings $(CPPFLAGS) $(LIBOBJS) -shared -o $@

$(CLIENT):		$(CLIENT).o $(LIB)
				$(VECHO) "LD $(CLIENT).o -> $@"
				$(Q) $(CPP) @gcc-warnings $(CPPFLAGS) $(CLIENT).o -L. -ldbus-tiny -Wl,-rpath=$(PWD) -o $@

$(SERVER):		$(SERVER).o $(LIB)
				$(VECHO) "LD $(SERVER).o -> $@"
				$(Q) $(CPP) @gcc-warnings $(CPPFLAGS) $(SERVER).o -L. -ldbus-tiny -Wl,-rpath=$(PWD) -o $@

$(SWIG_WRAP_SRC) $(SWIG_PM): $(SWIG_SRC)
				$(VECHO) "SWIG $< -> $@"
				$(Q) swig -c++ -cppext cpp -perl5 $<

$(SWIG_WRAP_OBJ):	$(SWIG_WRAP_SRC)
				$(VECHO) "SWIG CPP $< -> $@"
				$(Q) $(CPP) $(CPPFLAGS) -Wno-unused-parameter \
						`perl -MConfig -e 'print join(" ", @Config{qw(ccflags optimize cccdlflags)}, "-I$$Config{archlib}/CORE")'` -c $< -o $@

$(SWIG_SO):		$(SWIG_WRAP_OBJ) $(LIBOBJS)
				$(VECHO) "SWIG LD $< -> $@"
				$(Q) $(CPP) $(CPPFLAGS) `perl -MConfig -e 'print $$Config{lddlflags}'` $(SWIG_WRAP_OBJ) $(LIBOBJS) -o $@

$(SWIG_PM_2):	$(SWIG_PM)
				$(VECHO) "SWIG FINISH PM $< -> $@"
				mkdir -p DBUS
				cp $(SWIG_PM) $(SWIG_PM_2)

$(SWIG_SO_2):	$(SWIG_SO) $(SWIG_PM)
				$(VECHO) "SWIG FINISH SO $< -> $@"
				mkdir -p DBUS
				cp $(SWIG_SO) $(SWIG_SO_2)
