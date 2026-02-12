COPT = -I $(HOME)/include

timetracker : timetracker.o
	cc -o $@ $< `pkg-config --libs gtk+-3.0`

test : test.o
	cc -o $@ $< `pkg-config --libs gtk+-3.0`

.c.o : 
	cc $(COPT) `pkg-config --cflags gtk+-3.0` -c -o $@ $<

