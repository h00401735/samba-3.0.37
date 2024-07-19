/* 
   Unix SMB/CIFS implementation.
   Common popt arguments
   Copyright (C) Jelmer Vernooij	2003
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef _POPT_COMMON_H
#define _POPT_COMMON_H

#include <popt.h>

#ifndef POPT_TABLEEND
#define POPT_TABLEEND { NULL, '\0', 0, 0, 0, NULL, NULL }
#endif

struct user_auth_info {
	pstring username;
	pstring password;
	BOOL got_pass;
	BOOL use_kerberos;
	int signing_state;
};

extern struct user_auth_info cmdline_auth_info;

#endif /* _POPT_COMMON_H */
