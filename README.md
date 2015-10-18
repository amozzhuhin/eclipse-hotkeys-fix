eclipse-hotkeys-fix
===================

Eclipse platform has old annoying [bug](https://bugs.eclipse.org/bugs/show_bug.cgi?id=61190) with
hotkeys for users with several keyboard layouts on Linux.

This project provide a shared library that temporary fixes this problem.

Tested on `Eclipse CDT`, `Eclipse JEE`, `Apache Directory Studio`, and might work with any other GTK+ application
with same bug.

Install
===================

* Install build tools (Ubuntu/Debian)
```
# apt-get install git cmake build-essential
```

* Install development files for appropriate version of the GTK+ library:

```
# apt-get install libgtk2.0-dev                  # for GTK2
or
# apt-get install libgtk-3-dev                   # for GTK3
```

* Get sources

```
$ git clone https://github.com/amozzhuhin/eclipse-hotkeys-fix.git eclipse-hotkeys-fix
```

* Create temporary build directory

```
$ mkdir eclipse-hotkeys-fix-build
$ cd eclipse-hotkeys-fix-build
```

* Run `CMake` in build directory

```
$ cmake ../eclipse-hotkeys-fix                   # for GTK2
or
$ cmake -DGTK3=1 ../eclipse-hotkeys-fix          # for GTK3
```

* Build library

```
$ make all
```

Usage
===================

Copy `lib/libeclipse-hotkeys-fix.so` and `eclipse.sh` from build directory into root directory of Eclipse distribution. Now you can run Eclipse with `eclipse.sh`.

Alternatives
===================

* [LESCH](http://sourceforge.net/projects/lesch/)
* [SWT patch](https://bugs.eclipse.org/bugs/attachment.cgi?id=239557)
