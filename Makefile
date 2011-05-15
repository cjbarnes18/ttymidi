all:
	gcc src/ttymidi.c -o ttymidi -lasound
clean:
	rm ttymidi
install:
	cp ttymidi /usr/local/bin
uninstall:
	rm /usr/local/bin/ttymidi