#!/bin/bash
# Run this once a day with cron to "logrotate"
# your Mozilla Firefox bookmarks.html files
#
# Licensed under the GNU General Public License v2
# or, at your option, any later version.
#
# Copyright (C) 2004 Joachim Nilsson <jocke@vmlinux.org>
#

# Find all profile directories
PDIRS=`grep Path $HOME/.mozilla/firefox/profiles.ini |sed 's/^Path=\(.*\)$/\1/g'`

# Maximum number of backup files
let MAX=5

for DIR in $PDIRS; do
	BM=$DIR/bookmarks.html 
	let i=MAX-1
	while [ $i -ge 1 ]; do
		if [ -f $BM.$i ]; then
			mv -f $BM.$i $BM.$((i+1))
		fi
		let i--
	done
	cp $BM $BM.1
done

