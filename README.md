eclipse-hotkeys-fix
===================

Eclipse platform has old annoying [bug](https://bugs.eclipse.org/bugs/show_bug.cgi?id=61190) with
hotkeys for user with multiply keyboard layouts on Linux.

This project provide a shared library that temporary fixes this problem.

Tested on `Eclipse CDT`, `Eclipse JEE`, `Apache Directory Studio`, and might work with any other GTK+ application
with same bug.

Install
===================

* Install dependencies (Ubuntu/Debian)

```
# apt-get install git cmake build-essential libgtk2.0-dev
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
$ cmake ../eclipse-hotkeys-fix
```

* Build library

```
$ make all
```

Usage
===================

Copy `libeclipse-hotkeys-fix.so` and `eclipse.sh` into Eclipse folder. Now you can run Eclipse with `eclipse.sh`.
