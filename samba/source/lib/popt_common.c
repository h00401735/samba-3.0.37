/*
   Unix SMB/CIFS implementation.
   Common popt routines

   Copyright (C) Tim Potter 2001,2002
   Copyright (C) Jelmer Vernooij 2002,2003
   Copyright (C) James Peach 2006

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

#include "includes.h"

/* Handle command line options:
 *		-d,--debuglevel
 *		-s,--configfile
 *		-O,--socket-options
 *		-V,--version
 *		-l,--log-base
 *		-n,--netbios-name
 *		-W,--workgroup
 *		-i,--scope
 */
#if 0
extern pstring user_socket_options;
extern BOOL AllowDebugChange;
extern BOOL override_logfile;
#endif

struct user_auth_info cmdline_auth_info;

#if 0
static void set_logfile(poptContext con, const char * arg)
{

	pstring logfile;
	const char *pname;

	/* Find out basename of current program */
	pname = strrchr_m(poptGetInvocationName(con),'/');

	if (!pname)
		pname = poptGetInvocationName(con);
	else
		pname++;

	pstr_sprintf(logfile, "%s/log.%s", arg, pname);
	lp_set_logfile(logfile);
}

static void popt_common_callback(poptContext con,
			   enum poptCallbackReason reason,
			   const struct poptOption *opt,
			   const char *arg, const void *data)
{

	if (reason == POPT_CALLBACK_REASON_PRE) {
		set_logfile(con, LOGFILEBASE);
		return;
	}

	switch(opt->val) {
	case 'd':
		if (arg) {
			debug_parse_levels(arg);
			AllowDebugChange = False;
		}
		break;

	case 'V':
		printf( "Version %s\n", SAMBA_VERSION_STRING);
		exit(0);
		break;

	case 's':
		if (arg) {
			pstrcpy(CONFIGFILE, arg);
		}
		break;

	case 'n':
		if (arg) {
			// set_global_myname(arg);
		}
		break;

	case 'l':
		if (arg) {
			set_logfile(con, arg);
			override_logfile = True;
			pstr_sprintf(LOGFILEBASE, "%s", arg);
		}
		break;

	case 'i':
		if (arg) {
			  set_global_scope(arg);
		}
		break;

	case 'W':
		if (arg) {
			set_global_myworkgroup(arg);
		}
		break;
	}
}

/****************************************************************************
 * get a password from a a file or file descriptor
 * exit on failure
 * ****************************************************************************/
static void get_password_file(struct user_auth_info *a)
{
	int fd = -1;
	char *p;
	BOOL close_it = False;
	pstring spec;
	char pass[128];

	if ((p = getenv("PASSWD_FD")) != NULL) {
		pstrcpy(spec, "descriptor ");
		pstrcat(spec, p);
		sscanf(p, "%d", &fd);
		close_it = False;
	} else if ((p = getenv("PASSWD_FILE")) != NULL) {
		fd = sys_open(p, O_RDONLY, 0);
		pstrcpy(spec, p);
		if (fd < 0) {
			fprintf(stderr, "Error opening PASSWD_FILE %s: %s\n",
					spec, strerror(errno));
			exit(1);
		}
		close_it = True;
	}

	for(p = pass, *p = '\0'; /* ensure that pass is null-terminated */
		p && p - pass < sizeof(pass);) {
		switch (read(fd, p, 1)) {
		case 1:
			if (*p != '\n' && *p != '\0') {
				*++p = '\0'; /* advance p, and null-terminate pass */
				break;
			}
		case 0:
			if (p - pass) {
				*p = '\0'; /* null-terminate it, just in case... */
				p = NULL; /* then force the loop condition to become false */
				break;
			} else {
				fprintf(stderr, "Error reading password from file %s: %s\n",
						spec, "empty password\n");
				exit(1);
			}

		default:
			fprintf(stderr, "Error reading password from file %s: %s\n",
					spec, strerror(errno));
			exit(1);
		}
	}
	pstrcpy(a->password, pass);
	if (close_it)
		close(fd);
}

static void get_credentials_file(const char *file, struct user_auth_info *info)
{
	XFILE *auth;
	fstring buf;
	uint16 len = 0;
	char *ptr, *val, *param;

	if ((auth=x_fopen(file, O_RDONLY, 0)) == NULL)
	{
		/* fail if we can't open the credentials file */
		d_printf("ERROR: Unable to open credentials file!\n");
		exit(-1);
	}

	while (!x_feof(auth))
	{
		/* get a line from the file */
		if (!x_fgets(buf, sizeof(buf), auth))
			continue;
		len = strlen(buf);

		if ((len) && (buf[len-1]=='\n'))
		{
			buf[len-1] = '\0';
			len--;
		}
		if (len == 0)
			continue;

		/* break up the line into parameter & value.
		 * will need to eat a little whitespace possibly */
		param = buf;
		if (!(ptr = strchr_m (buf, '=')))
			continue;

		val = ptr+1;
		*ptr = '\0';

		/* eat leading white space */
		while ((*val!='\0') && ((*val==' ') || (*val=='\t')))
			val++;

		if (strwicmp("password", param) == 0)
		{
			pstrcpy(info->password, val);
			info->got_pass = True;
		}
		else if (strwicmp("username", param) == 0)
			pstrcpy(info->username, val);
		else if (strwicmp("domain", param) == 0)
			set_global_myworkgroup(val);
		memset(buf, 0, sizeof(buf));
	}
	x_fclose(auth);
}



struct poptOption popt_common_credentials[] = {
	{ NULL, 0, POPT_ARG_CALLBACK|POPT_CBFLAG_PRE, (void *)popt_common_credentials_callback },
	{ "user", 'U', POPT_ARG_STRING, NULL, 'U', "Set the network username", "USERNAME" },
	{ "no-pass", 'N', POPT_ARG_NONE, &cmdline_auth_info.got_pass, 0, "Don't ask for a password" },
	{ "kerberos", 'k', POPT_ARG_NONE, &cmdline_auth_info.use_kerberos, 'k', "Use kerberos (active directory) authentication" },
	{ "authentication-file", 'A', POPT_ARG_STRING, NULL, 'A', "Get the credentials from a file", "FILE" },
	{ "signing", 'S', POPT_ARG_STRING, NULL, 'S', "Set the client signing state", "on|off|required" },
	{"machine-pass", 'P', POPT_ARG_NONE, NULL, 'P', "Use stored machine account password" },
	POPT_TABLEEND
};
#endif
