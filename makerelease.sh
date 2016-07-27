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

do_build()
{
	local fwdir="$1"
	local targetdir="$2"
	shift 2

	echo
	echo "Building configuration:  $*"
	mkdir -p "$targetdir"

	echo "$*" > "$targetdir/build_config.txt"
	make -C "$fwdir" all "$@"

	echo "Fuse configuration:" > "$targetdir/fuses.txt"
	echo >> "$targetdir/fuses.txt"
	make --no-print-directory -C "$fwdir" print_fuses "$@" >> "$targetdir/fuses.txt"

	mv "$fwdir"/*.hex "$targetdir"/
	make -C "$fwdir" distclean "$@"
}

hook_pre_archives()
{
	# Build the hex files.
	for hw in legacy smd; do
		for dev in 88 328p; do
			for dbg in release debug; do
				local fwdir="$2/firmware"
				local targetdir="$2/hex/board_$hw/atmega$dev/$dbg"

				[ $dev = 88 ] && local CONF_KCONF=0
				[ $dev != 88 ] && local CONF_KCONF=1

				[ $dbg = debug ] && local CONF_DEBUG=1
				[ $dbg != debug ] && local CONF_DEBUG=0

				do_build "$fwdir" "$targetdir" \
					HW=$hw DEV=m$dev \
					CONF_CALIB=0 \
					CONF_DEBUG=$CONF_DEBUG \
					CONF_KCONF=$CONF_KCONF
			done
		done
	done

	# Move the documentation.
	mv "$2"/schematics-lf1600/lf1600.pdf "$2"/schematics-lf1600.pdf

	# Update the README
	local makefile="$2/firmware/Makefile"
	for arch in M88 M328P; do
		local lfuse="$(grep -e "$arch"'_LFUSE[[:space:]]*:=' "$makefile" | head -n1 | cut -d'=' -f2 | tr -d '[[:space:]]')"
		local hfuse="$(grep -e "$arch"'_HFUSE[[:space:]]*:=' "$makefile" | head -n1 | cut -d'=' -f2 | tr -d '[[:space:]]')"
		local efuse="$(grep -e "$arch"'_EFUSE[[:space:]]*:=' "$makefile" | head -n1 | cut -d'=' -f2 | tr -d '[[:space:]]')"
		sed -i -e 's/%%'"$arch"'_LFUSE%%/'"$lfuse"'/g' \
		       -e 's/%%'"$arch"'_HFUSE%%/'"$hfuse"'/g' \
		       -e 's/%%'"$arch"'_EFUSE%%/'"$efuse"'/g' \
		       "$2/README.md"
	done
}

project=xytronic-lf
default_archives="tar.xz"
makerelease "$@"
