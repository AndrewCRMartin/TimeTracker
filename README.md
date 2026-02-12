TimeTracker
===========

Andrew C.R.Martin, abYinformatics (c) 2026
------------------------------------------

I was completely frustrated by the lack of a simple time tracker that
met my needs for consultancy and contracting work. Everything I had
tried was too complicated. All I want to do is have a small window in
which I can click start and stop and keep a record of the dates/times
I'm working together with a running total of the time worked for
billing purposes.

**TimeTracker** is written to meet that need. It is a very simple
stopwatch / time tracker which logs start and stop times to a CSV
file and keeps a total of time while it is running.

The config file, which lives in
`~/.config/TimeTracker/timetracker.conf` can be edited to set a
project name and task name. These are displayed in the TimeTracker
window and saved in the CSV file. In future, I plan to add more
flexibility by allow a set of different projects/tasks to be listed
such that time can be tracked across those projects by clicking on the
required project/task.

Simply run the program (currently from the command line) and it will
create (or append to) a CSV file. Click the toggle button to start or stop.

Eventually I will set it up as a proper application with an icon, etc.

### Compiling

Ensure you have the gtk3-devel package installed, the just enter the
`src` directory and type `make`. Put the `timetracker` executable
somewhere in your path. That's it.
