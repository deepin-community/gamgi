#!/bin/sh

BROWSER=${BROWSER:-/usr/bin/sensible-browser}
export BROWSER

export GAMGI_TEXT=/usr/share/fonts/truetype/ttf-dejavu
export GAMGI_HELP=/usr/share/doc/gamgi-doc/doc

exec /usr/lib/gamgi/gamgi.real "$@"
