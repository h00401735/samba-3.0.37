/*
 *utils for smb.
 *
 * HM3: copy from some other files.
 */

#include "includes.h"


 /****************************************************************************
  Map standard UNIX permissions onto wire representations.
 ****************************************************************************/

uint32 unix_perms_to_wire(mode_t perms)
 {
	 unsigned int ret = 0;

	 ret |= ((perms & S_IXOTH) ?  UNIX_X_OTH : 0);
	 ret |= ((perms & S_IWOTH) ?  UNIX_W_OTH : 0);
	 ret |= ((perms & S_IROTH) ?  UNIX_R_OTH : 0);
	 ret |= ((perms & S_IXGRP) ?  UNIX_X_GRP : 0);
	 ret |= ((perms & S_IWGRP) ?  UNIX_W_GRP : 0);
	 ret |= ((perms & S_IRGRP) ?  UNIX_R_GRP : 0);
	 ret |= ((perms & S_IXUSR) ?  UNIX_X_USR : 0);
	 ret |= ((perms & S_IWUSR) ?  UNIX_W_USR : 0);
	 ret |= ((perms & S_IRUSR) ?  UNIX_R_USR : 0);
#ifdef S_ISVTX
	 ret |= ((perms & S_ISVTX) ?  UNIX_STICKY : 0);
#endif
#ifdef S_ISGID
	 ret |= ((perms & S_ISGID) ?  UNIX_SET_GID : 0);
#endif
#ifdef S_ISUID
	 ret |= ((perms & S_ISUID) ?  UNIX_SET_UID : 0);
#endif
	 return ret;
 }

 /****************************************************************************
  Map wire permissions to standard UNIX.
 ****************************************************************************/

mode_t wire_perms_to_unix(uint32 perms)
 {
	 mode_t ret = (mode_t)0;

	 ret |= ((perms & UNIX_X_OTH) ? S_IXOTH : 0);
	 ret |= ((perms & UNIX_W_OTH) ? S_IWOTH : 0);
	 ret |= ((perms & UNIX_R_OTH) ? S_IROTH : 0);
	 ret |= ((perms & UNIX_X_GRP) ? S_IXGRP : 0);
	 ret |= ((perms & UNIX_W_GRP) ? S_IWGRP : 0);
	 ret |= ((perms & UNIX_R_GRP) ? S_IRGRP : 0);
	 ret |= ((perms & UNIX_X_USR) ? S_IXUSR : 0);
	 ret |= ((perms & UNIX_W_USR) ? S_IWUSR : 0);
	 ret |= ((perms & UNIX_R_USR) ? S_IRUSR : 0);
#ifdef S_ISVTX
	 ret |= ((perms & UNIX_STICKY) ? S_ISVTX : 0);
#endif
#ifdef S_ISGID
	 ret |= ((perms & UNIX_SET_GID) ? S_ISGID : 0);
#endif
#ifdef S_ISUID
	 ret |= ((perms & UNIX_SET_UID) ? S_ISUID : 0);
#endif
	 return ret;
 }

void free_ntquota_list(SMB_NTQUOTA_LIST **qt_list)
 {
	 if (!qt_list)
		 return;

	 if ((*qt_list)->mem_ctx)
		 talloc_destroy((*qt_list)->mem_ctx);

	 (*qt_list) = NULL;
 }

