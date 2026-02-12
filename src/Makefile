COPT = -I $(HOME)/include
OFILES = timetracker.o config.o
CC = cc -g

timetracker : $(OFILES)
	$(CC) -o $@ $(OFILES) `pkg-config --libs gtk+-3.0`

test : test.o
	$(CC) -o $@ $< `pkg-config --libs gtk+-3.0`

.c.o : 
	$(CC) $(COPT) `pkg-config --cflags gtk+-3.0` -c -o $@ $<

clean :
	\rm $(OFILES)
