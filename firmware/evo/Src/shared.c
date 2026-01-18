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

HAL_StatusTypeDef RTC_SetFromUnixTimestamp(RTC_HandleTypeDef *hrtc, uint32_t unix_timestamp, int16_t utc_offset_minutes)
{
  HAL_StatusTypeDef status;
  struct tm *time_info;
  time_t local_time;
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  // 1. Adjust timestamp by the UTC offset (converted to seconds)
  local_time = (time_t)(unix_timestamp + (utc_offset_minutes * 60));

  // 2. Convert epoch to broken-down time (struct tm)
  // gmtime is used because we manually applied the offset above
  time_info = gmtime(&local_time);
  if (time_info == NULL)
    return HAL_ERROR;

  // 3. Fill the RTC Time structure
  sTime.Hours   = (uint8_t)time_info->tm_hour;
  sTime.Minutes = (uint8_t)time_info->tm_min;
  sTime.Seconds = (uint8_t)time_info->tm_sec;
  sTime.TimeFormat = RTC_HOURFORMAT12_AM; // Not used if RTC_HOURFORMAT_24 is selected in Init
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;

  // 4. Fill the RTC Date structure
  // tm_year is years since 1900; RTC expects years since 2000 (0-99)
  sDate.Year    = (uint8_t)(time_info->tm_year - 100); 
  // tm_mon is 0-11; RTC expects 1-12
  sDate.Month   = (uint8_t)(time_info->tm_mon + 1);    
  sDate.Date    = (uint8_t)time_info->tm_mday;
  // tm_wday is 0(Sun)-6(Sat); STM32 Weekday is 1(Mon)-7(Sun)
  sDate.WeekDay = (uint8_t)((time_info->tm_wday == 0) ? RTC_WEEKDAY_SUNDAY : time_info->tm_wday);

  // 5. Update the Hardware registers
  // We use RTC_FORMAT_BIN because we are passing standard integers
  status = HAL_RTC_SetDate(hrtc, &sDate, RTC_FORMAT_BIN);
  if (status != HAL_OK)
    return status;

  status = HAL_RTC_SetTime(hrtc, &sTime, RTC_FORMAT_BIN);
  return status;
}
