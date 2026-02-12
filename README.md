TimeTracker
===========

Andrew C.R.Martin (c) 2016
--------------------------

I was completely frustrated by the lack of a simple time tracker that
met my needs for consultancy and contracting work. Everything I had
tried was too complicated. All I want to do is have a small window in
which I can click start and stop and keep a record of the dates/times
I'm working together with a running total of the time worked for
billing purposes.

**TimeTracker** is written to meet that need. It is a very simple
stopwatch / time tracker which logs start and stop times to a CSV
file and keeps a total of time while it is running.

In future, I plan to add a little more flexibility to allow a set of
different projects to be listed such that time can be tracked across
those projects.

Simply run the program (currently from the command line) and it will
create (or append to) a CSV file. Click the toggle button to start or stop.

### Compiling

Ensure you have the gtk3-devel package installed, the just enter the
`src` directory and type `make`. Put the `timetracker` executable
somewhere in your path. That's it.



