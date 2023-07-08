/******************************************
 *
 * $GAMGI/src/io/gamgi_io_socket.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_io.h"

#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>

int gamgi_io_socket_name (char *protocol, char *hostname)
{
struct hostent* host;
struct servent *service;
struct sockaddr_in data;
int socket_fd;

/*********************************
 * get hostname and service data *
 *********************************/

if ((host = gethostbyname (hostname)) == NULL)
  return -1;

if ((service = getservbyname (protocol, "tcp")) == NULL)
  return -1;

/**************************
 * create internet socket *
 **************************/

if ((socket_fd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
  return -1;

/*******************
 * set socket data *
 *******************/

data.sin_family = AF_INET;
data.sin_addr = *((struct in_addr *) host->h_addr_list[0]);
data.sin_port = service->s_port;

/*******************************
 * establish remote connection *
 *******************************/

if (connect (socket_fd, (struct sockaddr *) &data, sizeof (data)) < 0)
  {
  close (socket_fd);
  return -1;
  }

return socket_fd;
}

int gamgi_io_socket_number (char *protocol,
unsigned char a1, unsigned char a2, unsigned char a3, 
unsigned char a4, unsigned char p1, unsigned char p2)
{
struct sockaddr_in data;
int socket_fd;

/**************************
 * create internet socket *
 **************************/

if ((socket_fd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
  return -1;

/**********************************
 * htonl, htons convert host byte *
 * order to network byte order    *
 **********************************/

data.sin_family = AF_INET;
data.sin_addr.s_addr = htonl ((a1 << 24) | (a2 << 16) | (a3 << 8) | a4);
data.sin_port = htons ((p1 << 8) | p2);

/*******************************
 * establish remote connection *
 *******************************/

if (connect (socket_fd, (struct sockaddr *) &data, sizeof (data)) < 0)
  {
  close (socket_fd);
  return -1;
  }

return socket_fd;
}
