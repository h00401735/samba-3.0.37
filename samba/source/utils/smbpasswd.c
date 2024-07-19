/*
 * Unix SMB/CIFS implementation.
 * Copyright (C) Jeremy Allison 1995-1998
 * Copyright (C) Tim Potter     2001
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675
 * Mass Ave, Cambridge, MA 02139, USA.  */

#include "includes.h"

extern BOOL AllowDebugChange;

/*
 * Next two lines needed for SunOS and don't
 * hurt anything else...
 */
extern char *optarg;
extern int optind;

/* forced running in root-mode */
static BOOL stdin_passwd_get = False;
static fstring user_name;
static char *new_passwd = NULL;


/*********************************************************
 Print command usage on stderr and die.
**********************************************************/
static void usage(void)
{
	printf("When run by root:\n");
	printf("    smbpasswd [options] [username]\n");
	printf("otherwise:\n");
	printf("    smbpasswd [options]\n\n");

	printf("options:\n");
	printf("  -h                   print this usage message\n");
	printf("  -s                   use stdin for password prompt\n");
	printf("  -D LEVEL             debug level\n");

	printf("extra options when run by root or in local mode:\n");
	printf("  -a                   add user\n");
	printf("  -d                   disable user\n");
	printf("  -e                   enable user\n");
	printf("  -n                   set no password\n");
	printf("  -x                   delete user\n");
	printf("  -l                   list users\n");

	exit(1);
}

#define set_line_buffering(f) do{ setvbuf(f, NULL, _IOLBF, 0); } while(0)

/*******************************************************************
 Process command line options
 ******************************************************************/

static int process_options(int argc, char **argv)
{
	int ch;
	int local_flags = 0;
 	const char *configfile = CONFIGFILE;

	ZERO_STRUCT(user_name);

	while ((ch = getopt(argc, argv, "axdehnslD:")) != EOF) {
		switch(ch) {
		case 'a':
			local_flags |= LOCAL_ADD_USER;
			local_flags |= LOCAL_SET_PASSWORD;
			break;
		case 'x':
			local_flags |= LOCAL_DELETE_USER;
			break;
		case 'd':
			local_flags |= LOCAL_DISABLE_USER;
			break;
		case 'e':
			local_flags |= LOCAL_ENABLE_USER;
			break;
		case 'n':
			local_flags |= LOCAL_SET_NO_PASSWORD;
			new_passwd = smb_xstrdup(LOCAL_SMB_NO_PASSWORD);
			break;
		case 's':
			local_flags |= LOCAL_SET_PASSWORD;
			set_line_buffering(stdin);
			set_line_buffering(stdout);
			set_line_buffering(stderr);
			stdin_passwd_get = True;
			break;
		case 'l':
			local_flags |= LOCAL_LIST_USER;
			break;
		case 'D':
#ifndef WITHOUT_LOG
			DEBUGLEVEL = atoi(optarg);
#endif
			break;
		case 'U': {
			fstrcpy(user_name, optarg);
			break;
		}
		case 'h':
		default:
			usage();
		}
	}

	if( (local_flags & LOCAL_ADD_USER) & LOCAL_DELETE_USER )
	{
		fprintf(stderr, "ADD and DEL user at the same time?\n");
		exit(1);
	}

	argc -= optind;
	argv += optind;

	switch(argc) {
	case 0:
		break;
	case 1:
		// HM3: one more option to go, treat as user name.
		if( 0 != user_name[0] )
		{
			usage();
		}
		else
		{
			fstrcpy(user_name, argv[0]);
		}
		break;
	default:
		usage();
	}

	if (!lp_load(configfile,True,False,False,True)) {
		fprintf(stderr, "Can't load %s - run testparm to debug it\n", configfile);
		exit(1);
	}

	return local_flags;
}

/*************************************************************
 Utility function to prompt for new password.
*************************************************************/
static char *prompt_for_new_password(BOOL stdin_get)
{
	char *p;
	fstring new_pw;

	ZERO_ARRAY(new_pw);

	p = get_pass("New SMB password:", stdin_get);

	fstrcpy(new_pw, p);
	SAFE_FREE(p);

	p = get_pass("Retype new SMB password:", stdin_get);

	if (strcmp(p, new_pw)) {
		fprintf(stderr, "Mismatch - password unchanged.\n");
		ZERO_ARRAY(new_pw);
		SAFE_FREE(p);
		return NULL;
	}

	return p;
}


/*************************************************************
 Change a password either locally or remotely.
*************************************************************/

static inline NTSTATUS password_change(char *username,
					char *old_passwd, char *new_pw,
					int local_flags)
{
	NTSTATUS ret;
	pstring err_str;
	pstring msg_str;

	ret = local_password_change(username, local_flags, new_pw,
				     err_str, sizeof(err_str), msg_str, sizeof(msg_str));

	if(*msg_str)
		printf("%s", msg_str);
	if(*err_str)
		fprintf(stderr, "%s", err_str);

	return ret;
}

static void list_users()
{
	struct pdb_search *ps = pdb_search_users(ACB_NORMAL);
	struct samr_displayentry *entries = NULL;
	uint32 count = pdb_search_entries(ps, 0, 0xffffffff, &entries);
	uint32 i;

	fprintf(stdout, "total count: %d\n", count);
	for (i = 0; i < count; i++) {
		fprintf(stdout, "\t\taccount[%d]:%s (%s)\n",
			i, entries[i].account_name, entries[i].fullname);
	}
	pdb_search_destroy(ps);
}

/*************************************************************
 Handle password changing for root.
*************************************************************/

static int process_root(int local_flags)
{
	struct passwd *pwd;
	int result = 0;
	char *old_passwd = NULL;

	/* Ensure passdb startup(). */
	if(!initialize_password_db(False)) {
		DEBUG(0, ("Failed to open passdb!\n"));
		exit(1);
	}

	/* Ensure we have a SAM sid. */
	get_global_sam_sid();

	if ((local_flags & LOCAL_LIST_USER) != 0) {
		list_users();
		return 0;
	}

	/* Only load interfaces if we are doing network operations. */

	/* HM3: if no user specified, get the local user name from EUID. */
	if (!user_name[0] && (pwd = getpwuid_alloc(NULL, geteuid()))) {
		fstrcpy(user_name, pwd->pw_name);
		TALLOC_FREE(pwd);
	}

	if (!user_name[0]) {
		fprintf(stderr,"You must specify a username\n");
		exit(1);
	}

	if(True) 	{
		if (!(local_flags & LOCAL_ADD_USER)) {
			old_passwd = get_pass("Old SMB password:", stdin_passwd_get);
		}

		if (!(local_flags & LOCAL_SET_PASSWORD)) {

			/*
			 * If we are trying to enable a user, first we need to find out
			 * if they are using a modern version of the smbpasswd file that
			 * disables a user by just writing a flag into the file. If so
			 * then we can re-enable a user without prompting for a new
			 * password. If not (ie. they have a no stored password in the
			 * smbpasswd file) then we need to prompt for a new password.
			 */

			if(local_flags & LOCAL_ENABLE_USER) {
				struct samu *sampass = samu_new(NULL);
				if (!sampass) {
					fprintf(stderr, "talloc fail for struct samu.\n");
					exit(1);
				}
				if (!pdb_getsampwnam(sampass, user_name)) {
					fprintf(stderr, "Failed to find user %s in passdb backend.\n",
						user_name );
					exit(1);
				}

				if(pdb_get_nt_passwd(sampass) == NULL) {
					local_flags |= LOCAL_SET_PASSWORD;
				}
				TALLOC_FREE(sampass);
			}
		}

		if((local_flags & LOCAL_SET_PASSWORD) && (new_passwd == NULL)) {
			new_passwd = prompt_for_new_password(stdin_passwd_get);

			if(!new_passwd) {
				fprintf(stderr, "Unable to get new password.\n");
				exit(1);
			}
		}
	}

	if (!NT_STATUS_IS_OK(password_change(user_name,
					     old_passwd, new_passwd,
					     local_flags))) {
		fprintf(stderr,"Failed to modify password entry for user %s\n", user_name);
		result = 1;
		goto done;
	}

 done:
	SAFE_FREE(new_passwd);
	return result;
}

/*********************************************************
 Start here.
**********************************************************/
int main(int argc, char **argv)
{
	int local_flags = 0;

	AllowDebugChange = False; // HM3: used by [debug_parse_levels].

	if (getuid() != 0) {
		fprintf(stderr, "smbpasswd must run as root.\n");
		exit(1);
	}

	load_case_tables();

	local_flags = process_options(argc, argv);

	setup_logging("smbpasswd", True);

	/*
	 * Set the machine NETBIOS name if not already
	 * set from the config file.
	 */
	if (!init_names())
		return 1;

	/* Check the effective uid - make sure we are not setuid */
	if (is_setuid_root()) {
		fprintf(stderr, "smbpasswd must *NOT* be setuid root.\n");
		exit(1);
	}

	secrets_init();

	return process_root(local_flags);
}
