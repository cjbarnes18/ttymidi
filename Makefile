all:
	gcc src/ttymidi.c -o ttymidi -lasound -lpthread
clean:
	rm ttymidi
install:
	mkdir -p $(DESTDIR)/usr/bin
	cp ttymidi $(DESTDIR)/usr/bin
uninstall:
	rm $(DESTDIR)/usr/bin/ttymidi
