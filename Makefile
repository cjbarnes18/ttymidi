all:
	gcc src/ttymidi.c -o ttymidi -lasound -lpthread
debug:
	gcc src/ttymidi.c -o ttymidi -g -lasound -lpthread
clean:
	rm ttymidi
install:
	mkdir -p $(DESTDIR)/bin
	cp ttymidi $(DESTDIR)/bin
uninstall:
	rm $(DESTDIR)/bin/ttymidi
