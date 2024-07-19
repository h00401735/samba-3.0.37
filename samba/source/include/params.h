#ifndef __PARAMS_H__
#define __PARAMS_H__

#ifdef SMALL_BUILD


/*************Global**************/
#define lp_dos_charset() DEFAULT_DOS_CHARSET
#define lp_logfile() "/var/samba/log/samba.log"
#define lp_serverstring() "Samba "SAMBA_VERSION_OFFICIAL_STRING
#define lp_mangling_method() "hash2"
#define lp_mangle_prefix() 1
#define lp_utmpdir() ""
#define lp_wtmpdir() ""
#define lp_utmp() False
#define lp_defaultservice() NULL
#define lp_msg_command() ""
#define lp_get_quota_command() NULL
#define lp_set_quota_command() NULL
#define lp_auto_services() NULL
#define lp_passwd_program() ""
#define lp_passwd_chat() DEFAULT_PASSWD_CHAT
#define lp_name_resolve_order() "lmhosts wins host bcast"
#define lp_realm() False
#define lp_log_nt_token_command() False
#define lp_username_map() NULL
#define lp_logon_script() ""
#define lp_logon_path() ""
#define lp_logon_drive() ""
#define lp_logon_home() ""
#define lp_remote_announce() NULL
#define lp_remote_browse_sync() NULL
#define lp_wins_server_list() NULL
#define lp_interfaces() NULL
#define lp_socket_address() "0.0.0.0"
#define lp_nis_home_map_name() "auto.home"
#define lp_announce_version() SAMBA_VERSION_STRING
#define lp_major_announce_version() DEFAULT_MAJOR_VERSION
#define lp_minor_announce_version() DEFAULT_MINOR_VERSION
#define lp_preload_modules() NULL
#define lp_panic_action() ""
#define lp_adduser_script() ""
#define lp_renameuser_script() ""
#define lp_deluser_script() ""
#define lp_guestaccount() GUEST_ACCOUNT
#define lp_addgroup_script() ""
#define lp_delgroup_script() ""
#define lp_addusertogroup_script() ""
#define lp_deluserfromgroup_script() ""
#define lp_setprimarygroup_script() ""
#define lp_addmachine_script() ""
#define lp_shutdown_script() ""
#define lp_abort_shutdown_script() ""
#define lp_username_map_script() ""
#define lp_check_password_script() ""
#define lp_wins_hook() NULL
#define lp_template_homedir() "/home/%D/%U"
#define lp_template_shell() "/bin/false"
#define lp_winbind_separator() "\\"
#define lp_acl_compatibility() 0
#define lp_winbind_enum_users() False
#define lp_winbind_enum_groups() False
#define lp_winbind_use_default_domain() False
#define lp_winbind_trusted_domains_only() False
#define lp_winbind_nested_groups() True
#define lp_winbind_refresh_tickets() False
#define lp_winbind_offline_logon() False
#define lp_winbind_normalize_names() False
#define lp_idmap_domains() ""
#define lp_idmap_backend() ""
#define lp_idmap_alloc_backend() ""
#define lp_idmap_cache_time() 900
#define lp_idmap_negative_cache_time() 120
#define lp_passdb_expand_explicit() False
#define lp_ldap_suffix() ""
#define lp_ldap_admin_dn() ""
#define lp_ldap_ssl() LDAP_SSL_OFF
#define lp_ldap_passwd_sync() LDAP_PASSWD_SYNC_OFF
#define lp_ldap_delete_dn() False
#define lp_ldap_replication_sleep() 100
#define lp_ldap_timeout() LDAP_CONNECT_DEFAULT_TIMEOUT
#define lp_ldap_page_size() LDAP_PAGE_SIZE
#define lp_ldap_debug_level() 0
#define lp_ldap_debug_threshold() 10


#define lp_add_share_cmd() ""
#define lp_change_share_cmd() ""
#define lp_delete_share_cmd() ""
#define lp_usershare_path() LOCKDIR "/usershares"
#define lp_usershare_template() NULL
#define lp_usershare_prefix_allow_list() NULL
#define lp_usershare_prefix_deny_list() NULL
#define lp_eventlog_list() 10
#define lp_usershare_allow_guests() False
#define lp_usershare_owner_only() True
#define lp_disable_netbios() True
#define lp_reset_on_zero_vc() False
#define lp_ms_add_printer_wizard() True
#define lp_wins_support() False
#define lp_we_are_a_wins_server() False
#define lp_wins_proxy() False


#define lp_readbmpx() False
#define lp_readraw() True
#define lp_large_readwrite() True
#define lp_writeraw() True
#define lp_null_passwords() False
#define lp_obey_pam_restrictions() False
#define lp_encrypted_passwords() True
#define lp_update_encrypted() False
#define lp_client_schannel() Auto
#define lp_server_schannel() Auto
#define lp_timestamp_logs() True
#define lp_debug_prefix_timestamp() False
#define lp_debug_hires_timestamp() False
#define lp_debug_pid() True
#define lp_debug_uid() True
#define lp_enable_core_files() True
#define lp_browse_list() True
#define lp_nis_home_map() False
#define lp_time_server() False
#define lp_bind_interfaces_only() False
#define lp_pam_password_change() False
#define lp_unix_password_sync() False
#define lp_passwd_chat_debug() True
#define lp_passwd_chat_timeout() 2
#define lp_nt_pipe_support() False
#define lp_nt_status_support() False
#define lp_stat_cache() True
#define lp_max_stat_cache_size() 1024
#define lp_allow_trusted_domains() True
#define lp_restrict_anonymous() 0
#define lp_lanman_auth() True
#define lp_ntlm_auth() True
#define lp_client_plaintext_auth() True
#define lp_client_lanman_auth() True
#define lp_client_ntlmv2_auth() False
#define lp_host_msdfs() True
#define lp_kernel_oplocks() True
#define lp_enhanced_browsing() True
#define lp_use_mmap() True
#define lp_unix_extensions() True
#define lp_use_spnego() True
#define lp_client_use_spnego() True
#define lp_hostname_lookups() False
#define lp_use_kerberos_keytab() False
#define lp_defer_sharing_violations() True
#define lp_enable_privileges() True
#define lp_enable_asu_support() False
#define lp_os_level() 20
#define lp_max_ttl() 60 * 60 * 24 * 3   /* 3 days default. */
#define lp_max_wins_ttl() 60 * 60 * 24 * 6   /* 6 days default. */
#define lp_min_wins_ttl() 60 * 60 * 6   /* 6 hours default. */
#define lp_max_log_size() 5000
#define lp_max_open_files() MAX_OPEN_FILES
#define lp_open_files_db_hash_size() SMB_OPEN_DATABASE_TDB_HASH_SIZE
#define lp_maxxmit() 0x4104
#define lp_maxmux() 50
#define lp_passwordlevel() 0
#define lp_usernamelevel() 0
#define lp_deadtime() 0
#define lp_maxprotocol() PROTOCOL_NT1
#define lp_minprotocol() PROTOCOL_CORE
#define lp_security() SEC_USER
#define lp_server_role() ROLE_STANDALONE
#define lp_auth_methods() NULL
#define lp_paranoid_server_security() True
#define lp_maxdisksize() 0
#define lp_lpqcachetime() 30
#define lp_max_smbd_processes() 0
#define _lp_disable_spoolss() True
#define lp_syslog() 1
#define lp_announce_as() ANNOUNCE_AS_NT_SERVER
#define lp_lm_announce() 2
#define lp_lm_interval() 60
#define lp_machine_password_timeout() 60 * 60 * 24 * 7   /* 7 days default. */
#define lp_map_to_guest() 0   /* By Default, "Never" */
#define lp_oplock_break_wait_time() 0   /* By Default, 0 msecs. */
#define lp_lock_spin_time() WINDOWS_MINIMUM_LOCK_TIMEOUT_MS   /* msec. */
#define lp_usershare_max_shares() 0
#define lp_winbind_cache_time() 300   /* 5 minutes */
#define lp_winbind_nss_info() NULL
#define lp_algorithmic_rid_base() BASE_RID
#define lp_name_cache_timeout() 660   /* In seconds */
#define lp_client_signing() Auto
#define lp_server_signing() False
#define lp_svcctl_list() NULL

/*************Local**************/
#define lp_dontdescend(...) ""
#define lp_username(...) NULL
#define lp_print_ok(...) False
#define lp_change_notify(...) True
#define lp_kernel_change_notify(...) True

#define lp_magicoutput(...) ""
#define lp_magicscript(...) ""
#define lp_comment(...) ""
#define lp_force_user(...) ""
#define lp_force_group(...) ""
#define lp_printer_admin(...) ""
#define lp_fstype(...) FSTYPE_STRING
#define lp_vfs_objects(...) NULL
#define lp_msdfs_proxy(...) ""
#define lp_volume(...) ""
#define lp_mangled_map(...) ""
#define lp_veto_files(...) ""
#define lp_hide_files(...) ""
#define lp_veto_oplocks(...) ""
#define lp_msdfs_root(...) False
#define lp_aio_write_behind(...) ""
#define lp_dfree_command(...) ""
#define lp_casesensitive(...) Auto
#define lp_preservecase(...) True
#define lp_shortpreservecase(...) True
#define lp_hide_dot_files(...) True
#define lp_hide_special_files(...) False
#define lp_hideunreadable(...) False
#define lp_hideunwriteable_files(...) False
#define lp_browseable(...) True
#define lp_no_set_dir(...) True
#define lp_guest_ok(...) False
#define lp_administrative_share(...) False
#define lp_map_hidden(...) False
#define lp_map_archive(...) True
#define lp_store_dos_attributes(...) False
#define lp_dmapi_support(...) False
#define lp_locking(...) True
#define lp_strict_locking(...) Auto
#define lp_posix_locking(...) True
#define lp_share_modes(...) True
#define lp_oplocks(...) True
#define lp_level2_oplocks(...) True
#define lp_manglednames(...) True
#define lp_widelinks(...) True
#define lp_symlinks(...) True
#define lp_syncalways(...) False
#define lp_strict_allocate(...) False
#define lp_strict_sync(...) False
#define lp_magicchar(...) '~'
#define lp_map_system(...) False
#define lp_delete_readonly(...) False
#define lp_fake_oplocks(...) False
#define lp_recursive_veto_delete(...) False
#define lp_dos_filemode(...) False
#define lp_dos_filetimes(...) True
#define lp_dos_filetime_resolution(...) False
#define lp_fake_dir_create_times(...) False
#define lp_blocking_locks(...) True
#define lp_inherit_perms(...) False
#define lp_inherit_acls(...) False

#define lp_inherit_owner(...) False
#define lp_use_client_driver(...) False
#define lp_default_devmode(...) True
#define lp_force_printername(...) False
#define lp_nt_acl_support(...) True
#define lp_force_unknown_acl_user(...) False
#define lp_ea_support(...) False
#define _lp_use_sendfile(...) False
#define lp_profile_acls(...) False
#define lp_map_acl_inherit(...) False
#define lp_afs_share(...) False
#define lp_acl_check_permissions(...) True
#define lp_acl_group_control(...) False
#define lp_acl_map_full_control(...) False

#define lp_create_mask(...) 0744
#define lp_force_create_mode(...) 0000
#define lp_security_mask(...) 0777
#define lp_force_security_mode(...) 0
#define lp_dir_mask(...) 0755
#define lp_force_dir_mode(...) 0000
#define lp_dir_security_mask(...) 0777
#define lp_force_dir_security_mode(...) 0

#define lp_max_connections(...) 0
#define lp_defaultcase(...) CASE_LOWER
#define lp_minprintspace(...) 0
#define lp_printing(...) DEFAULT_PRINTING
#define lp_max_reported_jobs(...) 0

#define lp_oplock_contention_limit(...) 2
#define lp_csc_policy(...) 0
#define lp_write_cache_size(...) 0
#define lp_block_size(...) 1024
#define lp_dfree_cache_time(...) 0
#define lp_allocation_roundup_size(...) SMB_ROUNDUP_ALLOCATION_SIZE
#define lp_aio_read_size(...) 0
#define lp_aio_write_size(...) 0
#define lp_map_readonly(...) MAP_READONLY_YES
#ifdef BROKEN_DIRECTORY_HANDLING
#define lp_directory_name_cache_size(...) 0
#else
#define lp_directory_name_cache_size(...) 100
#endif



#define add_to_file_list(...)
#define lp_file_list_changed() False
#define lp_idmap_uid(...) False
#define lp_idmap_gid(...) False

#define load_usershare_service(...) ({-1;})
#define load_usershare_shares(...) ({0;})


#define map_username(...) False
#define debug_init()
#define handle_debug_list(...)
#define gfree_debugsyms()

#define IS_DC ({False;})

#endif

#endif
