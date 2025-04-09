#!/bin/sh

srcdir="$(realpath "$0" | xargs dirname)"

# Import the makerelease.lib
# https://bues.ch/cgit/misc.git/tree/makerelease.lib
die() { echo "$*"; exit 1; }
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

	echo "$*" | tr '[:space:]' '\n' > "$targetdir/build_config.txt"
	make -C "$fwdir" all "$@" C=1

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
		for dev in 88 88p 328p; do
			local releases="release"
			[ $dev = 328p ] && local releases="release debug"

			for rel in $releases; do
				local fwdir="$2/firmware"
				local targetdir="$2/hex/board_$hw/atmega$dev/$rel"

				if [ $dev = 88 -o $dev = 88p ]; then
					local CONF_KCONF=0
				else
					local CONF_KCONF=1
				fi

				if [ $rel = debug ]; then
					local CONF_DEBUG=1
				else
					local CONF_DEBUG=0
				fi

				do_build "$fwdir" "$targetdir" \
					HW=$hw DEV=m$dev \
					CONF_PRESETS=1 \
					CONF_BOOST=1 \
					CONF_IDLE=1 \
					CONF_IDLETOGGLE=1 \
					CONF_DEBUG=$CONF_DEBUG \
					CONF_EERING=1 \
					CONF_ADJ=1 \
					CONF_KCONF=$CONF_KCONF
			done
		done
	done

	# Move the documentation.
	mv "$2"/schematics-lf1600/lf1600.pdf "$2"/schematics-lf1600.pdf
}

hook_pre_documentation()
{
	# Update the README
	local makefile="$1/firmware/Makefile"
	for arch in M88 M88P M328P; do
		local lfuse="$(grep -e "$arch"'_LFUSE[[:space:]]*:=' "$makefile" | head -n1 | cut -d'=' -f2 | tr -d '[[:space:]]')"
		local hfuse="$(grep -e "$arch"'_HFUSE[[:space:]]*:=' "$makefile" | head -n1 | cut -d'=' -f2 | tr -d '[[:space:]]')"
		local efuse="$(grep -e "$arch"'_EFUSE[[:space:]]*:=' "$makefile" | head -n1 | cut -d'=' -f2 | tr -d '[[:space:]]')"
		sed -i -e 's/%%'"$arch"'_LFUSE%%/'"$lfuse"'/g' \
		       -e 's/%%'"$arch"'_HFUSE%%/'"$hfuse"'/g' \
		       -e 's/%%'"$arch"'_EFUSE%%/'"$efuse"'/g' \
		       "$1/README.md"
	done
}

project=xytronic-lf
default_archives="tar.xz"
makerelease "$@"
