# UNIX Domain Sockets Example
#
# Copyright (c) 2014  Joachim Nilsson <troglobit@gmail.com>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

ROOTDIR    := $(shell pwd)
RM         := @rm -f
EXEC       := c d
OBJS       := c.o d.o api.o
DEPS        = $(OBJS:.o=.d)

# Pretty printing and GCC -M for auto dep files
%.o: %.c
	@printf "  CC      $(subst $(ROOTDIR)/,,$(shell pwd)/$@)\n"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c -MMD -MP -o $@ $<

# Pretty printing and create .map files
%: %.o
	@printf "  LINK    $(subst $(ROOTDIR)/,,$(shell pwd)/$@)\n"
	@$(CC) $(CFLAGS) $(LDFLAGS) -Wl,-Map,$@.map -o $@ $^ $(LDLIBS$(LDLIBS-$(@)))

all: $(EXEC)

c: c.o api.o

d: d.o api.o

clean:
	$(RM) $(EXEC) *.o

distclean: clean
	$(RM) *.d *.map

-include $(DEPS)
