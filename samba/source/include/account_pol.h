#ifndef _ACCOUNT_POL_H_
#define _ACCOUNT_POL_H_

/*
  header for account policy
*/
#ifndef ADVANCED_ACCOUNT
#define init_account_policy() ({True;})
#define account_policy_get(...) account_policy_get_default(__VA_ARGS__)
#define account_policy_set(...) ({False;})
#define get_account_pol_tdb() ({NULL;})
BOOL account_policy_get_default(int account_policy, uint32 *val);
#else
BOOL init_account_policy(void);
BOOL account_policy_get(int field, uint32 *value);
BOOL account_policy_get_default(int account_policy, uint32 *val);
BOOL account_policy_set(int field, uint32 value);
TDB_CONTEXT *get_account_pol_tdb( void );
#endif

#endif	/* _ACCOUNT_POL_H_ */
