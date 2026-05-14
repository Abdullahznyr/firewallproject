#ifndef FIREWALLPROJECT_NFTABLES_ADAPTER_H
#define FIREWALLPROJECT_NFTABLES_ADAPTER_H

int nftables_apply_rules(int dry_run);
int nftables_remove_rules(int dry_run);
int nftables_list_rules(void);
int nftables_backup(const char *backup_path);
int nftables_rollback(const char *backup_path, int dry_run);
int nftables_emergency_reset(int dry_run);

#endif
