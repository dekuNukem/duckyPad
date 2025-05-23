#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "dsb_cache.h"

const char* key_release_file_string = "release";
uint8_t dsvm_cached_data[DSB_CACHE_BYTE_SIZE];

dsbc_item dsb_cache[DSB_CACHE_ENTRIES_SIZE];

uint8_t get_empty_idx(dsbc_item* dlist)
{
  uint32_t lowest_ts = MY_UINT32_MAX;
  uint8_t purge_idx = 0;
  for (int i = 0; i < DSB_CACHE_ENTRIES_SIZE; ++i)
  {
    dsbc_item* this_item = &dlist[i];
    if(this_item->timestamp == 0)
      return i;
    if(this_item->timestamp <= lowest_ts)
    {
      purge_idx = i;
      lowest_ts = this_item->timestamp;
    }
  }
  return purge_idx;
}

void dsbc_add(uint8_t pf_idx, uint8_t k_idx, uint8_t is_kdown, uint32_t ts, uint8_t* data, uint8_t data_size)
{
  if (data == NULL || data_size == 0 || data_size > DSB_CACHE_BYTE_SIZE)
    return;

  // Get an empty or oldest index
  uint8_t idx = get_empty_idx(dsb_cache);
  dsbc_item* item = &dsb_cache[idx];

  // Zero out the entire struct
  memset(item, 0, sizeof(dsbc_item));

  // Fill in the fields
  item->profile_idx = pf_idx;
  item->key_idx = k_idx;
  item->is_press = is_kdown;
  item->timestamp = ts;

  // Copy data safely
  memcpy(item->data, data, data_size);
}

// void dsbc_print_item(const dsbc_item* item)
// {
//   printf("  profile_idx: %lu\n", item->profile_idx);
//   printf("  key_idx:     %lu\n", item->key_idx);
//   printf("  is_press:    %lu\n", item->is_press);
//   printf("  timestamp:   %lu\n", item->timestamp);
// }

uint8_t dsbc_search(uint8_t pf_idx, uint8_t k_idx, uint8_t is_kdown, uint8_t* data)
{
  if (data == NULL)
    return 0;

  for (int i = 0; i < DSB_CACHE_ENTRIES_SIZE; ++i)
  {
    dsbc_item* item = &dsb_cache[i];

    if (item->profile_idx == pf_idx &&
        item->key_idx == k_idx &&
        item->is_press == is_kdown)
    {
      // Zero out the destination buffer
      memset(data, 0, DSB_CACHE_BYTE_SIZE);

      // Copy the cached data
      memcpy(data, item->data, DSB_CACHE_BYTE_SIZE);

      return 1; // Found
    }
  }

  return 0; // Not found
}
