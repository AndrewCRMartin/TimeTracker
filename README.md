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
`~/.config/TimeTracker/timetracker.conf` must be edited to set project
names and task names. These are specified using the syntax:

```
project+=projectname1
project+=projectname2
...
task+=task1
task+=task2
...
```

Note used **must** use the `+=` which indicates to the config file
parser that the parameter can take multiple values.

The current project and task names are displayed at the top of the
TimeTracker window and are saved in the CSV file.  The full set of
project and task names are listed with radio buttons allowing you to
select a different project and task, which will be stored in the CSV
file allowing time to be tracked across different projects. You can
only change these when the timer is in the stopped state.

When you exit the program, the config file is updated with the current
project and task. The `Quit` button is also only active when the timer is
in the stopped state.

Simply run the program (currently from the command line) and it will
create (or append to) a CSV file. Click the toggle button to start or stop.

Eventually I will set it up as a proper application with an icon, etc.

### Compiling

Ensure you have the gtk3-devel package installed, the just enter the
`src` directory, type `make` then `make install`. The `timetracker`
executable will be placed in your `bin` directory.


