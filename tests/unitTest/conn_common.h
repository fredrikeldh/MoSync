/*
Copyright (C) 2011 MoSync AB

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.
*/

#define SINGLE_SOCKET_PORT 5001
#define HTTP_PORT 5002
#define SOCKET_SIZE_PORT 5003
#define HTTP_POST_PORT 5004
#define SOCKET_CANCEL_PORT 5005
#define HTTP_CANCEL_PORT 5006

// this port should be firewalled, so the client has time to cancel.
#define CONNECT_CANCEL_PORT 5007

#define UDP_CANCEL_PORT 5100

#define DATA_SIZE (4*1024)
