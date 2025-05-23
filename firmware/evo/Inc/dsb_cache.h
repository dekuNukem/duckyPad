#ifndef DSBCACHE_H
#define DSBCACHE_H

#include <stdint.h>

#define DSB_CACHE_ENTRIES_SIZE 30
#define DSB_CACHE_BYTE_SIZE 24
#define MY_UINT32_MAX 0xFFFFFFFF

typedef struct
{
  uint8_t profile_idx;
  uint8_t key_idx;
  uint8_t is_press;
  uint32_t timestamp;
  uint8_t data[DSB_CACHE_BYTE_SIZE];
} dsbc_item;

extern dsbc_item dsb_cache[DSB_CACHE_ENTRIES_SIZE];

void dsbc_add(uint8_t pf_idx, uint8_t k_idx, uint8_t is_kdown, uint32_t ts, uint8_t* data, uint8_t data_size);
uint8_t dsbc_search(uint8_t pf_idx, uint8_t k_idx, uint8_t is_kdown, uint8_t* data);
void dsbc_print_item(const dsbc_item* item);

#endif // DSBCACHE_H

