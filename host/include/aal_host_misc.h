#ifndef AAL_HOST_MISC_H
#define AAL_HOST_MISC_H

#include <aal/aal_host_driver.h>

/* mem_alloc.c */
void *aal_pagealloc_init(unsigned long start, unsigned long size,
                         unsigned long unit);
void aal_pagealloc_destroy(void *__desc);
unsigned long aal_pagealloc_alloc(void *__desc, int npages);
void aal_pagealloc_free(void *__desc, unsigned long address, int npages);

/* mm.c */
void *aal_host_map_generic(aal_device_t dev, unsigned long phys,
                           void *virt, unsigned long size, int flags);
int aal_host_unmap_generic(aal_device_t dev, void *virt, unsigned long size);

#endif
