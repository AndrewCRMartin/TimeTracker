all : test test2


test : test.o
	cc -o $@ $< `pkg-config --libs gtk+-3.0`
test2 : test2.o
	cc -o $@ $< `pkg-config --libs gtk+-3.0`

.c.o : 
	cc `pkg-config --cflags gtk+-3.0` -c -o $@ $<

