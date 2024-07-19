/*
 *  Unix SMB/CIFS implementation.
 *  RPC Pipe client / server routines
 *  Copyright (C) Andrew Tridgell              1992-2000,
 *  Copyright (C) Jean Fran�ois Micouleau      1998-2001.
 *  Copyright (C) Volker Lendecke              2006.
 *  Copyright (C) Gerald Carter                2006.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef ADVANCED_ACCOUNT

#include "includes.h"
#include "groupdb/mapping.h"

/****************************************************************************
initialise first time the mapping list
****************************************************************************/
NTSTATUS add_initial_entry(gid_t gid, const char *sid, enum lsa_SidType sid_name_use, const char *nt_name, const char *comment)
{
	GROUP_MAP map;

	if(!init_group_mapping()) {
		DEBUG(0,("failed to initialize group mapping\n"));
		return NT_STATUS_UNSUCCESSFUL;
	}

	map.gid=gid;
	if (!string_to_sid(&map.sid, sid)) {
		DEBUG(0, ("string_to_sid failed: %s", sid));
		return NT_STATUS_UNSUCCESSFUL;
	}

	map.sid_name_use=sid_name_use;
	fstrcpy(map.nt_name, nt_name);
	fstrcpy(map.comment, comment);

	return pdb_add_group_mapping_entry(&map);
}

/****************************************************************************
 Map a unix group to a newly created mapping
****************************************************************************/
NTSTATUS map_unix_group(const struct group *grp, GROUP_MAP *pmap)
{
	NTSTATUS status;
	GROUP_MAP map;
	const char *grpname, *dom, *name;
	uint32 rid;

	if (pdb_getgrgid(&map, grp->gr_gid)) {
		return NT_STATUS_GROUP_EXISTS;
	}

	map.gid = grp->gr_gid;
	grpname = grp->gr_name;

	if (lookup_name(tmp_talloc_ctx(), grpname, LOOKUP_NAME_ISOLATED,
			&dom, &name, NULL, NULL)) {

		const char *tmp = talloc_asprintf(
			tmp_talloc_ctx(), "Unix Group %s", grp->gr_name);

		DEBUG(5, ("%s exists as %s\\%s, retrying as \"%s\"\n",
			  grpname, dom, name, tmp));
		grpname = tmp;
	}

	if (lookup_name(tmp_talloc_ctx(), grpname, LOOKUP_NAME_ISOLATED,
			NULL, NULL, NULL, NULL)) {
		DEBUG(3, ("\"%s\" exists, can't map it\n", grp->gr_name));
		return NT_STATUS_GROUP_EXISTS;
	}

	fstrcpy(map.nt_name, grpname);

	if (pdb_rid_algorithm()) {
		rid = algorithmic_pdb_gid_to_group_rid( grp->gr_gid );
	} else {
		if (!pdb_new_rid(&rid)) {
			DEBUG(3, ("Could not get a new RID for %s\n",
				  grp->gr_name));
			return NT_STATUS_ACCESS_DENIED;
		}
	}

	sid_compose(&map.sid, get_global_sam_sid(), rid);
	map.sid_name_use = SID_NAME_DOM_GRP;
	fstrcpy(map.comment, talloc_asprintf(tmp_talloc_ctx(), "Unix Group %s",
					     grp->gr_name));

	status = pdb_add_group_mapping_entry(&map);
	if (NT_STATUS_IS_OK(status)) {
		*pmap = map;
	}
	return status;
}

static NTSTATUS alias_memberships(const DOM_SID *members, size_t num_members,
				  DOM_SID **sids, size_t *num)
{
	size_t i;

	*num = 0;
	*sids = NULL;

	for (i=0; i<num_members; i++) {
		NTSTATUS status = one_alias_membership(&members[i], sids, num);
		if (!NT_STATUS_IS_OK(status))
			return status;
	}
	return NT_STATUS_OK;
}

struct aliasmem_closure {
	const DOM_SID *alias;
	DOM_SID **sids;
	size_t *num;
};



/*
 *
 * High level functions
 * better to use them than the lower ones.
 *
 * we are checking if the group is in the mapping file
 * and if the group is an existing unix group
 *
 */

/* get a domain group from it's SID */
BOOL get_domain_group_from_sid(DOM_SID sid, GROUP_MAP *map)
{
	struct group *grp;
	BOOL ret;

	if(!init_group_mapping()) {
		DEBUG(0,("failed to initialize group mapping\n"));
		return(False);
	}

	DEBUG(10, ("get_domain_group_from_sid\n"));

	/* if the group is NOT in the database, it CAN NOT be a domain group */

	become_root();
	ret = pdb_getgrsid(map, sid);
	unbecome_root();

	/* special case check for rid 513 */

	if ( !ret ) {
		uint32 rid;

		sid_peek_rid( &sid, &rid );

		if ( rid == DOMAIN_GROUP_RID_USERS ) {
			fstrcpy( map->nt_name, "None" );
			fstrcpy( map->comment, "Ordinary Users" );
			sid_copy( &map->sid, &sid );
			map->sid_name_use = SID_NAME_DOM_GRP;
			map->gid = (gid_t)-1;

			return True;
		}

		return False;
	}

	DEBUG(10, ("get_domain_group_from_sid: SID found in the TDB\n"));

	/* if it's not a domain group, continue */
	if (map->sid_name_use!=SID_NAME_DOM_GRP) {
		return False;
	}

	DEBUG(10, ("get_domain_group_from_sid: SID is a domain group\n"));

	if (map->gid==-1) {
		return False;
	}

	DEBUG(10, ("get_domain_group_from_sid: SID is mapped to gid:%lu\n",(unsigned long)map->gid));

	grp = getgrgid(map->gid);
	if ( !grp ) {
		DEBUG(10, ("get_domain_group_from_sid: gid DOESN'T exist in UNIX security\n"));
		return False;
	}

	DEBUG(10, ("get_domain_group_from_sid: gid exists in UNIX security\n"));

	return True;
}

/****************************************************************************
 Create a UNIX group on demand.
****************************************************************************/

int smb_create_group(const char *unix_group, gid_t *new_gid)
{
	pstring add_script;
	int 	ret = -1;
	int 	fd = 0;

	*new_gid = 0;

	/* defer to scripts */

	if ( *lp_addgroup_script() ) {
		pstrcpy(add_script, lp_addgroup_script());
		pstring_sub(add_script, "%g", unix_group);
		ret = smbrun(add_script, &fd);
		DEBUG(ret ? 0 : 3,("smb_create_group: Running the command `%s' gave %d\n",add_script,ret));
		if (ret == 0) {
			smb_nscd_flush_group_cache();
		}
		if (ret != 0)
			return ret;

		if (fd != 0) {
			fstring output;

			*new_gid = 0;
			if (read(fd, output, sizeof(output)) > 0) {
				*new_gid = (gid_t)strtoul(output, NULL, 10);
			}

			close(fd);
		}

	}

	if (*new_gid == 0) {
		struct group *grp = getgrnam(unix_group);

		if (grp != NULL)
			*new_gid = grp->gr_gid;
	}

	return ret;
}

/****************************************************************************
 Delete a UNIX group on demand.
****************************************************************************/

int smb_delete_group(const char *unix_group)
{
	pstring del_script;
	int ret;

	/* defer to scripts */

	if ( *lp_delgroup_script() ) {
		pstrcpy(del_script, lp_delgroup_script());
		pstring_sub(del_script, "%g", unix_group);
		ret = smbrun(del_script,NULL);
		DEBUG(ret ? 0 : 3,("smb_delete_group: Running the command `%s' gave %d\n",del_script,ret));
		if (ret == 0) {
			smb_nscd_flush_group_cache();
		}
		return ret;
	}

	return -1;
}

/****************************************************************************
 Set a user's primary UNIX group.
****************************************************************************/
int smb_set_primary_group(const char *unix_group, const char* unix_user)
{
	pstring add_script;
	int ret;

	/* defer to scripts */

	if ( *lp_setprimarygroup_script() ) {
		pstrcpy(add_script, lp_setprimarygroup_script());
		all_string_sub(add_script, "%g", unix_group, sizeof(add_script));
		all_string_sub(add_script, "%u", unix_user, sizeof(add_script));
		ret = smbrun(add_script,NULL);
		flush_pwnam_cache();
		DEBUG(ret ? 0 : 3,("smb_set_primary_group: "
			 "Running the command `%s' gave %d\n",add_script,ret));
		if (ret == 0) {
			smb_nscd_flush_group_cache();
		}
		return ret;
	}

	return -1;
}

/****************************************************************************
 Add a user to a UNIX group.
****************************************************************************/

int smb_add_user_group(const char *unix_group, const char *unix_user)
{
	pstring add_script;
	int ret;

	/* defer to scripts */

	if ( *lp_addusertogroup_script() ) {
		pstrcpy(add_script, lp_addusertogroup_script());
		pstring_sub(add_script, "%g", unix_group);
		pstring_sub(add_script, "%u", unix_user);
		ret = smbrun(add_script,NULL);
		DEBUG(ret ? 0 : 3,("smb_add_user_group: Running the command `%s' gave %d\n",add_script,ret));
		if (ret == 0) {
			smb_nscd_flush_group_cache();
		}
		return ret;
	}

	return -1;
}

/****************************************************************************
 Delete a user from a UNIX group
****************************************************************************/

int smb_delete_user_group(const char *unix_group, const char *unix_user)
{
	pstring del_script;
	int ret;

	/* defer to scripts */

	if ( *lp_deluserfromgroup_script() ) {
		pstrcpy(del_script, lp_deluserfromgroup_script());
		pstring_sub(del_script, "%g", unix_group);
		pstring_sub(del_script, "%u", unix_user);
		ret = smbrun(del_script,NULL);
		DEBUG(ret ? 0 : 3,("smb_delete_user_group: Running the command `%s' gave %d\n",del_script,ret));
		if (ret == 0) {
			smb_nscd_flush_group_cache();
		}
		return ret;
	}

	return -1;
}

NTSTATUS pdb_default_getgrsid(struct pdb_methods *methods, GROUP_MAP *map,
				 DOM_SID sid)
{
	return get_group_map_from_sid(sid, map) ?
		NT_STATUS_OK : NT_STATUS_UNSUCCESSFUL;
}

NTSTATUS pdb_default_getgrgid(struct pdb_methods *methods, GROUP_MAP *map,
				 gid_t gid)
{
	return get_group_map_from_gid(gid, map) ?
		NT_STATUS_OK : NT_STATUS_UNSUCCESSFUL;
}

NTSTATUS pdb_default_getgrnam(struct pdb_methods *methods, GROUP_MAP *map,
				 const char *name)
{
	return get_group_map_from_ntname(name, map) ?
		NT_STATUS_OK : NT_STATUS_UNSUCCESSFUL;
}

NTSTATUS pdb_default_add_group_mapping_entry(struct pdb_methods *methods,
						GROUP_MAP *map)
{
	return add_mapping_entry(map, TDB_INSERT) ?
		NT_STATUS_OK : NT_STATUS_UNSUCCESSFUL;
}

NTSTATUS pdb_default_update_group_mapping_entry(struct pdb_methods *methods,
						   GROUP_MAP *map)
{
	return add_mapping_entry(map, TDB_REPLACE) ?
		NT_STATUS_OK : NT_STATUS_UNSUCCESSFUL;
}

NTSTATUS pdb_default_delete_group_mapping_entry(struct pdb_methods *methods,
						   DOM_SID sid)
{
	return group_map_remove(&sid) ?
		NT_STATUS_OK : NT_STATUS_UNSUCCESSFUL;
}

NTSTATUS pdb_default_enum_group_mapping(struct pdb_methods *methods,
					   const DOM_SID *sid, enum lsa_SidType sid_name_use,
					   GROUP_MAP **pp_rmap, size_t *p_num_entries,
					   BOOL unix_only)
{
	return enum_group_mapping(sid, sid_name_use, pp_rmap, p_num_entries, unix_only) ?
		NT_STATUS_OK : NT_STATUS_UNSUCCESSFUL;
}

NTSTATUS pdb_default_find_alias(struct pdb_methods *methods,
				const char *name, DOM_SID *sid)
{
	GROUP_MAP map;

	if (!pdb_getgrnam(&map, name))
		return NT_STATUS_NO_SUCH_ALIAS;

	if ((map.sid_name_use != SID_NAME_WKN_GRP) &&
	    (map.sid_name_use != SID_NAME_ALIAS))
		return NT_STATUS_OBJECT_TYPE_MISMATCH;

	sid_copy(sid, &map.sid);
	return NT_STATUS_OK;
}

NTSTATUS pdb_default_create_alias(struct pdb_methods *methods,
				  const char *name, uint32 *rid)
{
	DOM_SID sid;
	enum lsa_SidType type;
	uint32 new_rid;
	gid_t gid;
	BOOL exists;
	GROUP_MAP map;
	TALLOC_CTX *mem_ctx;
	NTSTATUS status;

	DEBUG(10, ("Trying to create alias %s\n", name));

	mem_ctx = talloc_new(NULL);
	if (mem_ctx == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	exists = lookup_name(mem_ctx, name, LOOKUP_NAME_LOCAL,
			     NULL, NULL, &sid, &type);
	TALLOC_FREE(mem_ctx);

	if (exists) {
		return NT_STATUS_ALIAS_EXISTS;
	}

	if (!winbind_allocate_gid(&gid)) {
		DEBUG(3, ("Could not get a gid out of winbind\n"));
		return NT_STATUS_ACCESS_DENIED;
	}

	if (!pdb_new_rid(&new_rid)) {
		DEBUG(0, ("Could not allocate a RID -- wasted a gid :-(\n"));
		return NT_STATUS_ACCESS_DENIED;
	}

	DEBUG(10, ("Creating alias %s with gid %d and rid %d\n",
		   name, gid, new_rid));

	sid_copy(&sid, get_global_sam_sid());
	sid_append_rid(&sid, new_rid);

	map.gid = gid;
	sid_copy(&map.sid, &sid);
	map.sid_name_use = SID_NAME_ALIAS;
	fstrcpy(map.nt_name, name);
	fstrcpy(map.comment, "");

	status = pdb_add_group_mapping_entry(&map);

	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(0, ("Could not add group mapping entry for alias %s "
			  "(%s)\n", name, nt_errstr(status)));
		return status;
	}

	*rid = new_rid;

	return NT_STATUS_OK;
}

NTSTATUS pdb_default_delete_alias(struct pdb_methods *methods,
				  const DOM_SID *sid)
{
	return pdb_delete_group_mapping_entry(*sid);
}

NTSTATUS pdb_default_get_aliasinfo(struct pdb_methods *methods,
				   const DOM_SID *sid,
				   struct acct_info *info)
{
	GROUP_MAP map;

	if (!pdb_getgrsid(&map, *sid))
		return NT_STATUS_NO_SUCH_ALIAS;

	if ((map.sid_name_use != SID_NAME_ALIAS) &&
	    (map.sid_name_use != SID_NAME_WKN_GRP)) {
		DEBUG(2, ("%s is a %s, expected an alias\n",
			  sid_string_static(sid),
			  sid_type_lookup(map.sid_name_use)));
		return NT_STATUS_NO_SUCH_ALIAS;
	}

	fstrcpy(info->acct_name, map.nt_name);
	fstrcpy(info->acct_desc, map.comment);
	sid_peek_rid(&map.sid, &info->rid);
	return NT_STATUS_OK;
}

NTSTATUS pdb_default_set_aliasinfo(struct pdb_methods *methods,
				   const DOM_SID *sid,
				   struct acct_info *info)
{
	GROUP_MAP map;

	if (!pdb_getgrsid(&map, *sid))
		return NT_STATUS_NO_SUCH_ALIAS;

	fstrcpy(map.nt_name, info->acct_name);
	fstrcpy(map.comment, info->acct_desc);

	return pdb_update_group_mapping_entry(&map);
}

NTSTATUS pdb_default_add_aliasmem(struct pdb_methods *methods,
				  const DOM_SID *alias, const DOM_SID *member)
{
	return add_aliasmem(alias, member);
}

NTSTATUS pdb_default_del_aliasmem(struct pdb_methods *methods,
				  const DOM_SID *alias, const DOM_SID *member)
{
	return del_aliasmem(alias, member);
}

NTSTATUS pdb_default_enum_aliasmem(struct pdb_methods *methods,
				   const DOM_SID *alias, DOM_SID **pp_members,
				   size_t *p_num_members)
{
	return enum_aliasmem(alias, pp_members, p_num_members);
}

NTSTATUS pdb_default_alias_memberships(struct pdb_methods *methods,
				       TALLOC_CTX *mem_ctx,
				       const DOM_SID *domain_sid,
				       const DOM_SID *members,
				       size_t num_members,
				       uint32 **pp_alias_rids,
				       size_t *p_num_alias_rids)
{
	DOM_SID *alias_sids;
	size_t i, num_alias_sids;
	NTSTATUS result;

	alias_sids = NULL;
	num_alias_sids = 0;

	result = alias_memberships(members, num_members,
				   &alias_sids, &num_alias_sids);

	if (!NT_STATUS_IS_OK(result))
		return result;

	*p_num_alias_rids = 0;

	if (num_alias_sids == 0) {
		TALLOC_FREE(alias_sids);
		return NT_STATUS_OK;
	}

	*pp_alias_rids = TALLOC_ARRAY(mem_ctx, uint32, num_alias_sids);
	if (*pp_alias_rids == NULL)
		return NT_STATUS_NO_MEMORY;

	for (i=0; i<num_alias_sids; i++) {
		if (!sid_peek_check_rid(domain_sid, &alias_sids[i],
					&(*pp_alias_rids)[*p_num_alias_rids]))
			continue;
		*p_num_alias_rids += 1;
	}

	TALLOC_FREE(alias_sids);

	return NT_STATUS_OK;
}

/**********************************************************************
 no ops for passdb backends that don't implement group mapping
 *********************************************************************/

NTSTATUS pdb_nop_getgrsid(struct pdb_methods *methods, GROUP_MAP *map,
				 DOM_SID sid)
{
	return NT_STATUS_UNSUCCESSFUL;
}

NTSTATUS pdb_nop_getgrgid(struct pdb_methods *methods, GROUP_MAP *map,
				 gid_t gid)
{
	return NT_STATUS_UNSUCCESSFUL;
}

NTSTATUS pdb_nop_getgrnam(struct pdb_methods *methods, GROUP_MAP *map,
				 const char *name)
{
	return NT_STATUS_UNSUCCESSFUL;
}

NTSTATUS pdb_nop_add_group_mapping_entry(struct pdb_methods *methods,
						GROUP_MAP *map)
{
	return NT_STATUS_UNSUCCESSFUL;
}

NTSTATUS pdb_nop_update_group_mapping_entry(struct pdb_methods *methods,
						   GROUP_MAP *map)
{
	return NT_STATUS_UNSUCCESSFUL;
}

NTSTATUS pdb_nop_delete_group_mapping_entry(struct pdb_methods *methods,
						   DOM_SID sid)
{
	return NT_STATUS_UNSUCCESSFUL;
}

NTSTATUS pdb_nop_enum_group_mapping(struct pdb_methods *methods,
					   enum lsa_SidType sid_name_use,
					   GROUP_MAP **rmap, size_t *num_entries,
					   BOOL unix_only)
{
	return NT_STATUS_UNSUCCESSFUL;
}

/****************************************************************************
 These need to be redirected through pdb_interface.c
****************************************************************************/
BOOL pdb_get_dom_grp_info(const DOM_SID *sid, struct acct_info *info)
{
	GROUP_MAP map;
	BOOL res;

	become_root();
	res = get_domain_group_from_sid(*sid, &map);
	unbecome_root();

	if (!res)
		return False;

	fstrcpy(info->acct_name, map.nt_name);
	fstrcpy(info->acct_desc, map.comment);
	sid_peek_rid(sid, &info->rid);
	return True;
}

BOOL pdb_set_dom_grp_info(const DOM_SID *sid, const struct acct_info *info)
{
	GROUP_MAP map;

	if (!get_domain_group_from_sid(*sid, &map))
		return False;

	fstrcpy(map.nt_name, info->acct_name);
	fstrcpy(map.comment, info->acct_desc);

	return NT_STATUS_IS_OK(pdb_update_group_mapping_entry(&map));
}

/********************************************************************
 Really just intended to be called by smbd
********************************************************************/
NTSTATUS pdb_create_builtin_alias(uint32 rid)
{
	DOM_SID sid;
	enum lsa_SidType type;
	gid_t gid;
	GROUP_MAP map;
	TALLOC_CTX *mem_ctx;
	NTSTATUS status;
	const char *name = NULL;
	fstring groupname;

	DEBUG(10, ("Trying to create builtin alias %d\n", rid));

	if ( !sid_compose( &sid, &global_sid_Builtin, rid ) ) {
		return NT_STATUS_NO_SUCH_ALIAS;
	}

	if ( (mem_ctx = talloc_new(NULL)) == NULL ) {
		return NT_STATUS_NO_MEMORY;
	}

	if ( !lookup_sid(mem_ctx, &sid, NULL, &name, &type) ) {
		TALLOC_FREE( mem_ctx );
		return NT_STATUS_NO_SUCH_ALIAS;
	}

	/* validate RID so copy the name and move on */

	fstrcpy( groupname, name );
	TALLOC_FREE( mem_ctx );

	if (!winbind_allocate_gid(&gid)) {
		DEBUG(3, ("pdb_create_builtin_alias: Could not get a gid out of winbind\n"));
		return NT_STATUS_ACCESS_DENIED;
	}

	DEBUG(10,("Creating alias %s with gid %d\n", groupname, gid));

	map.gid = gid;
	sid_copy(&map.sid, &sid);
	map.sid_name_use = SID_NAME_ALIAS;
	fstrcpy(map.nt_name, groupname);
	fstrcpy(map.comment, "");

	status = pdb_add_group_mapping_entry(&map);

	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(0, ("pdb_create_builtin_alias: Could not add group mapping entry for alias %d "
			  "(%s)\n", rid, nt_errstr(status)));
	}

	return status;
}
#endif

