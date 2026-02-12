COPT = -I $(HOME)/include
OFILES = timetracker.o config.o
CC = cc -g

timetracker : $(OFILES)
	$(CC) -o $@ $(OFILES) `pkg-config --libs gtk+-3.0`

install :
	mkdir -p $(HOME)/.config/TimeTracker
	cp timetracker.css $(HOME)/.config/TimeTracker
	mkdir -p $(HOME)/bin
	cp timetracker $(HOME)/bin

.c.o : 
	$(CC) $(COPT) `pkg-config --cflags gtk+-3.0` -c -o $@ $<

clean :
	\rm $(OFILES)
