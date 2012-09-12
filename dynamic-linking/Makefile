# Test of dlopen() for a plugin based services architecture for finit -*-Makefile-*-
PLUGIN_DIR  = /tmp
ROOTDIR    := $(shell pwd)
EXEC        = daemon
OBJS        = daemon.o plugin.o
DEPS        = $(addprefix .,$(OBJS:.o=.d))
CFLAGS      = -g -O0
CPPFLAGS    = -W -Wall
CPPFLAGS   += -D_GNU_SOURCE -DPLUGIN_PATH=\"$(PLUGIN_DIR)\"
LDFLAGS     = -rdynamic
LDLIBS      = -ldl

include common.mk
export PLUGIN_DIR ROOTDIR

all: $(EXEC)
	$(MAKE) -C plugins $@

$(EXEC): $(OBJS)

clean:
	$(RM) $(EXEC) *.o
	$(MAKE) -C plugins $@

distclean: clean
	$(RM) $(JUNK) $(DEPS)
	$(MAKE) -C plugins $@

install:
	$(MAKE) -C plugins $@

uninstall:
	$(MAKE) -C plugins $@

ifneq ($(MAKECMDGOALS),clean)
 -include $(DEPS)
endif
