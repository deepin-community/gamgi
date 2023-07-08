/******************************************
 *
 * $GAMGI/src/io/gamgi_io_socket.h
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

int gamgi_io_socket_name (char *protocol, char *hostname);

int gamgi_io_socket_number (char *protocol,
unsigned char a1, unsigned char a2, unsigned char a3,
unsigned char a4, unsigned char p1, unsigned char p2);
