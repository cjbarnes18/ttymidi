all:
	gcc src/ttymidi.c src/termios2.c src/term_posix.c \
	       	-Iinc -o ttymidi -lasound -lpthread
clean:
	rm ttymidi
install:
	mkdir -p $(DESTDIR)/bin
	cp ttymidi $(DESTDIR)/bin
uninstall:
	rm $(DESTDIR)/bin/ttymidi
