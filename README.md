![Xwing][xwing_logo]

WHAT IS XWING?
-----------------

Xwing is a powerful open source video hosting engine which is designed for extremely high loads. It uses event-driven model combined with multithreading. Xwing completely written in C++.
Xwing can be paired with any well-known HTTP-server using FastCGI protocol.

BUILDING
-----------------

To build Xwing, you will need CMake build system. If you are building for Linux, you can install the appropriate package from the repositories. CMake for other operating systems can be found on the [official site](http://www.cmake.org/cmake/resources/software.html).

First, you need to generate Makefile

    $ cmake .

Secondly, you need to compile source files

    $ make

or

    $ mingw32-make

if you are using MinGW, or

    $ nmake

if you are using MS Visual C++.

RUNNING
-----------------

To run Xwing, enter

    $ ./objs/xwing-server

If it does not started, check the log file for errors. To find the location of the log file, view configuration file (server.json). If the log file is empty or does not exist, check syslog.

REPORTING BUGS
-----------------

Send your bug to <me@xpast.me> or <vvladxx@gmail.com> with the subject "Xwing bug". You can also use [this bug tracker](https://github.com/VladX/Xwing-Engine/issues). Thank you.

LICENSE
-----------------

Xwing is free software and distributed under the terms of the GNU General Public License v3. See COPYING file for details.

[xwing_logo]: https://raw.githubusercontent.com/VladX/Xwing-Engine/development/res/logo.png
