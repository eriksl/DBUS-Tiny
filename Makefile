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

CCWARNINGS		:=	-Wall -Wextra -Werror \
						-Wformat-overflow=2 -Wshift-overflow=2 -Wimplicit-fallthrough=5 -Wformat-signedness \
						-Wformat-truncation=2 -Wstringop-overflow=4 -Wunused-const-variable=2 -Walloca \
						-Warray-bounds=2 -Wswitch-bool -Wsizeof-array-argument -Wduplicated-branches \
						-Wduplicated-cond -Wlto-type-mismatch -Wdangling-else \
						-Wdangling-pointer=2 -Wpacked -Wfloat-equal -Winit-self -Wmissing-include-dirs \
						-Wmissing-noreturn -Wbool-compare -Wsuggest-attribute=noreturn -Wsuggest-attribute=format \
						-Wmissing-format-attribute -Wuninitialized -Wtrampolines -Wframe-larger-than=16384 \
						-Wunsafe-loop-optimizations -Wshadow -Wpointer-arith -Wcast-qual -Wwrite-strings \
						-Wsequence-point -Wlogical-op -Wlogical-not-parentheses -Wredundant-decls -Wvla \
						-Wdisabled-optimization -Wunreachable-code -Wparentheses -Wmissing-declarations \
						-Wcast-align -Winline -Wmultistatement-macros -Warray-bounds=2 \
						\
						-Wno-error=cast-qual \
						-Wno-error=unsafe-loop-optimizations \
						\
						-Wno-packed \
						-Wno-unused-parameter \

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
				$(Q) $(CPP) $(CCWARNINGS) $(CPPFLAGS) -c $< -o $@

$(LIB):			$(LIBOBJS)
				$(VECHO) "LD $(LIBOBJS) -> $@"
				$(Q) $(CPP) $(CCWARNINGS) $(CPPFLAGS) $(LIBOBJS) -shared -o $@

$(CLIENT):		$(CLIENT).o $(LIB)
				$(VECHO) "LD $(CLIENT).o -> $@"
				$(Q) $(CPP) $(CCWARNINGS) $(CPPFLAGS) $(CLIENT).o -L. -ldbus-tiny -o $@

$(SERVER):		$(SERVER).o $(LIB)
				$(VECHO) "LD $(SERVER).o -> $@"
				$(Q) $(CPP) $(CCWARNINGS) $(CPPFLAGS) $(SERVER).o -L. -ldbus-tiny -o $@

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
