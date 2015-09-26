#!/bin/sh

srcdir="$(dirname "$0")"
[ "$(echo "$srcdir" | cut -c1)" = '/' ] || srcdir="$PWD/$srcdir"

die() { echo "$*"; exit 1; }

# Import the makerelease.lib
# http://bues.ch/gitweb?p=misc.git;a=blob_plain;f=makerelease.lib;hb=HEAD
for path in $(echo "$PATH" | tr ':' ' '); do
	[ -f "$MAKERELEASE_LIB" ] && break
	MAKERELEASE_LIB="$path/makerelease.lib"
done
[ -f "$MAKERELEASE_LIB" ] && . "$MAKERELEASE_LIB" || die "makerelease.lib not found."


hook_get_version()
{
	local file="$1/firmware/version.h"
	local regex='#define[[:space:]]VERSION[[:space:]]'
	version="$(grep -e "$regex" "$file" | head -n1 | cut -d'"' -f2)"
}

hook_testbuild()
{
	default_hook_testbuild "$1/firmware"
}

hook_regression_tests()
{
	make -C "$1/firmware" tests
}

hook_pre_archives()
{
	# Build the hex files.
	make -C "$2/firmware" all
	make -C "$2/firmware" clean
	mv "$2"/firmware/*.hex "$2"

	# Move the documentation.
	mv "$2"/schematics-lf1600/lf1600.pdf "$2"/schematics-lf1600.pdf

	# Update the README
	local makefile="$2/firmware/Makefile"
	local lfuse="$(grep -e 'LFUSE[[:space:]]*:=' "$makefile" | head -n1 | cut -d'=' -f2 | tr -d '[[:space:]]')"
	local hfuse="$(grep -e 'HFUSE[[:space:]]*:=' "$makefile" | head -n1 | cut -d'=' -f2 | tr -d '[[:space:]]')"
	local efuse="$(grep -e 'EFUSE[[:space:]]*:=' "$makefile" | head -n1 | cut -d'=' -f2 | tr -d '[[:space:]]')"
	sed -i -e 's/%%LFUSE%%/'"$lfuse"'/g' \
	       -e 's/%%HFUSE%%/'"$hfuse"'/g' \
	       -e 's/%%EFUSE%%/'"$efuse"'/g' \
	       "$2/README"
}

project=xytronic-lf
default_archives="tar.xz"
makerelease "$@"
