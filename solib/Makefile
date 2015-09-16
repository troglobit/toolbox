# Example so lib versioning

LIBS      := lib.so.1 lib.so.2
TARGETS   := app $(LIBS)

all: $(TARGETS)

# See resulting SONAME in app and lib.so.? using objdump -p ...
app: app.c $(LIBS)
	gcc -o $@ $< -L. -l:lib.so.2

lib.so.1: lib1.c
	gcc -fPIC -shared -Wl,-soname,$@ -o $@ $< -ldl

lib.so.2: lib2.c
	gcc -fPIC -shared -Wl,-soname,$@ -o $@ $< -ldl

# Here we "cheat" to test ld.so checking SONAME
install:
	cp lib.so.1 /usr/local/lib/
	ln -s lib.so.1 /usr/local/lib/lib.so.2
	ldconfig

uninstall:
	rm /usr/local/lib/lib.so.1
	rm /usr/local/lib/lib.so.2

test:
	@echo "This shall fail since app requires SONAME lib.so.2"
	./app

clean:
	$(RM) $(TARGETS) *.o

distclean: clean
	$(RM) *~

