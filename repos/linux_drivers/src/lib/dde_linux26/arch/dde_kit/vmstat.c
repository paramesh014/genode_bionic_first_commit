#include "local.h"

#include <linux/fs.h>

atomic_long_t vm_stat[NR_VM_ZONE_STAT_ITEMS];


void dec_zone_page_state(struct page *page, enum zone_stat_item item)
{
	WARN_UNIMPL;
}


void inc_zone_page_state(struct page *page, enum zone_stat_item item)
{
	WARN_UNIMPL;
}


void __inc_zone_page_state(struct page *page, enum zone_stat_item item)
{
	WARN_UNIMPL;
}

void __get_zone_counts(unsigned long *active, unsigned long *inactive,
                       unsigned long *free, struct pglist_data *pgdat)
{
	WARN_UNIMPL;
}
