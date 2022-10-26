CLCT=/home/allen/Documents/Code/tl-1/clct

.PHONY: test
test:
	gcc -Wall -Wno-int-to-pointer-cast test/test.c -Isrc -I$(CLCT) src/*.c src/x11/*.c src/widgets/*.c $(CLCT)/*.c `pkg-config --cflags --libs cairo` `pkg-config --cflags --libs xcb` -ggdb

test_button:
	gcc -Wall -Wno-unused -Wno-int-to-pointer-cast -Wno-int-conversion test/button.c -Iinclude -I$(CLCT) src/*.c src/x11/*.c src/widgets/*.c src/themes/*.c $(CLCT)/*.c `pkg-config --cflags --libs cairo` `pkg-config --cflags --libs xcb` -ggdb