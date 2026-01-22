#include <string.h>
#include <stdio.h>
#include "main.h"
#include "shared.h"
#include <time.h>

char temp_buf[TEMP_BUFSIZE];

uint32_t millis(void)
{
  return htim2.Instance->CNT;
}

void delay_ms(uint32_t amount)
{
  if(amount == 0)
    return;
  uint32_t start = millis();
  while ((millis() - start) < amount)
  {
    ;
  }
}

char* goto_next_arg(char* buf, char* buf_end)
{
  char* curr = buf;  
  if(buf == NULL || curr >= buf_end)
    return NULL;
  while(curr < buf_end && *curr != ' ')
      curr++;
  while(curr < buf_end && *curr == ' ')
      curr++;
  if(curr >= buf_end)
    return NULL;
  return curr;
}

void strip_newline(char* line, uint32_t size)
{
  for(uint32_t i = 0; i < size; ++i)
    if(line[i] == '\n' || line[i] == '\r')
      line[i] = 0;
}

void idle_loop(void)
{
  while(1)
    delay_ms(1000);
}

uint32_t get_uuid(void)
{
  return (*STM32F0_UUID0) ^ (*STM32F0_UUID1) ^ (*STM32F0_UUID2);
}

uint8_t is_rtc_valid(void)
{
  return HAL_RTCEx_BKUPRead(&hrtc, RTC_MAGICNUM_REG) == RTC_MAGIC_NUMBER;
}

void mark_rtc_as_valid(void)
{
  HAL_PWR_EnableBkUpAccess();
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_MAGICNUM_REG, RTC_MAGIC_NUMBER);
  HAL_PWR_DisableBkUpAccess();
}

void set_utc_offset(int16_t minutes)
{
  HAL_PWR_EnableBkUpAccess();
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_UTC_OFFSET_REG, (uint32_t)minutes);
  HAL_PWR_DisableBkUpAccess();
}

int16_t get_utc_offset(void)
{
  return (int16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_UTC_OFFSET_REG);
}

uint8_t RTC_SetFromUnixTimestamp(RTC_HandleTypeDef *rtc_ptr, uint32_t unix_timestamp)
{
  HAL_StatusTypeDef status;
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  time_t raw_time = (time_t)unix_timestamp;
  struct tm *time_info = localtime(&raw_time);
  if (time_info == NULL)
    return 11;
  sTime.Hours = time_info->tm_hour;
  sTime.Minutes = time_info->tm_min;
  sTime.Seconds = time_info->tm_sec;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  status = HAL_RTC_SetTime(rtc_ptr, &sTime, RTC_FORMAT_BIN);
  if (status != HAL_OK)
    return 22;
  if (time_info->tm_wday == 0)
    sDate.WeekDay = RTC_WEEKDAY_SUNDAY;
  else
    sDate.WeekDay = (uint8_t)time_info->tm_wday;
  // struct tm months are 0-11; STM32 HAL expects 1-12
  sDate.Month = (uint8_t)(time_info->tm_mon + 1);
  sDate.Date = (uint8_t)time_info->tm_mday;
  // struct tm year is "years since 1900" (e.g., 2023 = 123)
  // STM32 HAL usually expects 2-digits for years 2000-2099
  sDate.Year = (uint8_t)(time_info->tm_year - 100);
  status = HAL_RTC_SetDate(rtc_ptr, &sDate, RTC_FORMAT_BIN);
  return 0;
}

struct tm* get_local_time(RTC_HandleTypeDef *rtc_ptr, int16_t offset_minutes)
{
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  struct tm temp_tm = {0};
  
  HAL_RTC_GetTime(rtc_ptr, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(rtc_ptr, &sDate, RTC_FORMAT_BIN);

  temp_tm.tm_year = sDate.Year + 100; 
  temp_tm.tm_mon  = sDate.Month - 1;
  temp_tm.tm_mday = sDate.Date;
  temp_tm.tm_hour = sTime.Hours;
  temp_tm.tm_min  = sTime.Minutes;
  temp_tm.tm_sec  = sTime.Seconds;
  temp_tm.tm_isdst = 0; // Explicitly set DST to 0 (we are handling offsets manually)

  time_t utc_epoch = mktime(&temp_tm);
  time_t local_epoch = utc_epoch + (offset_minutes * 60);

  return localtime(&local_epoch);
}
