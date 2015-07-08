# yata
Yet Another Timer API

This timer API was created as an activity with the purpose of learning the open-source project creational process. It was part of an "Open Source Software" class at UFRGS.

It's just a simple API that creates POSIX per-process timers. The main purpose is schedule an activity for a process after a timer expiration. Timers can be periodic or not.

# Compiling

The makefile simply generates a dynamic library.

After cloning the repo you should execute:

$ make

After that, run the below command for library instalation on the /usr/local/lib path.

$ sudo make install

For removing the library from your system you should run:

$ sudo make uninstall

# Usage

Just link it with your application using -lyata. Don't forget it MUST also be linked with -lpthread and -lrt. The code below can be used as reference. Save it named as test.c.

```
#include <stdio.h>
#include <stdlib.h>
#include <yata.h>

void
foo()
{
	printf("I'm on %s\n", __func__);
}

int
main()
{
	yata_timer t;

	yata_timerstart(1000, 1, foo, NULL, &t);

	while(1);
}
```
For compiling this code you can run:

$ gcc test.c -lyata -lpthread -lrt

Running it is as simple as:

$ ./a.out

