#!/bin/sh

ECLIPSE_DIR=$(readlink -f `dirname "$0"`)

LD_PRELOAD=$ECLIPSE_DIR/libeclipse-hotkeys-fix.so $ECLIPSE_DIR/eclipse $@
