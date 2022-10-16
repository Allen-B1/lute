CLCT=/home/allen/Documents/Code/tl-1/clct

.PHONY: test
test:
	gcc test/test.c -Isrc -I$(CLCT) src/*.c src/x11/*.c $(CLCT)/*.c `pkg-config --cflags --libs cairo` `pkg-config --cflags --libs xcb` -ggdb