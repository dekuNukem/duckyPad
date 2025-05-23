#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define DSB_CACHE_ENTRIES_SIZE 4
#define DSB_CACHE_BYTE_SIZE 32
#define MY_UINT32_MAX 0xFFFFFFFF

typedef struct
{
  uint8_t profile_idx;
  uint8_t key_idx;
  uint8_t is_press;
  uint32_t timestamp;
  uint8_t data[DSB_CACHE_BYTE_SIZE];
} dsbc_item;

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

void dsbc_add(uint8_t pf_idx, uint8_t k_idx, uint8_t is_kdown, uint8_t ts, uint8_t* data, uint8_t data_size)
{
  if (data == NULL || data_size == 0 || data_size >= DSB_CACHE_BYTE_SIZE)
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


void dsbc_print_item(const dsbc_item* item)
{
  if (item == NULL)
  {
    printf("NULL item\n");
    return;
  }

  printf("dsbc_item contents:\n");
  printf("  profile_idx: %u\n", item->profile_idx);
  printf("  key_idx:     %u\n", item->key_idx);
  printf("  is_press:    %u\n", item->is_press);
  printf("  timestamp:   %u\n", item->timestamp);
  printf("  data:        ");

  for (int i = 0; i < DSB_CACHE_BYTE_SIZE; ++i)
  {
    printf("%02X ", item->data[i]);
  }

  printf("\n");
}

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


int main()
{
  // Sample input data
  uint8_t data1[5] = {1, 2, 3, 4, 5};
  uint8_t data2[5] = {6, 7, 8, 9, 10};

  // Add to cache
  dsbc_add(1, 1, 1, 10, data1, sizeof(data1));
  dsbc_add(2, 2, 0, 20, data2, sizeof(data2));

  // Search for existing entry
  uint8_t result_data[DSB_CACHE_BYTE_SIZE];
  uint8_t found = dsbc_search(1, 1, 1, result_data);

  printf("Search result: %s\n", found ? "FOUND" : "NOT FOUND");
  if (found)
  {
    printf("Returned data: ");
    for (int i = 0; i < DSB_CACHE_BYTE_SIZE; ++i)
    {
      printf("%02X ", result_data[i]);
    }
    printf("\n");
  }

  // Search for non-existing entry
  found = dsbc_search(9, 9, 1, result_data);
  printf("Search result (non-existing): %s\n", found ? "FOUND" : "NOT FOUND");

  return 0;
}