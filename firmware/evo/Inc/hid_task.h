#ifndef __HID_TASK_H
#define __HID_TASK_H

#ifdef __cplusplus
 extern "C" {
#endif 

#define HID_COMMAND_GET_INFO 0
#define HID_COMMAND_GOTO_PROFILE_BY_NUMBER 1
#define HID_COMMAND_PREV_PROFILE 2
#define HID_COMMAND_NEXT_PROFILE 3

#define HID_COMMAND_SET_LED_SINGLE 4

#define HID_COMMAND_SW_RESET 20
#define HID_COMMAND_SLEEP 21
#define HID_COMMAND_WAKEUP 22

#define HID_COMMAND_GOTO_PROFILE_BY_NAME 23
#define HID_COMMAND_DUMP_SD 32
#define HID_COMMAND_RESUME 12

#define HID_RESPONSE_OK 0
#define HID_RESPONSE_GENERIC_ERROR 1
#define HID_RESPONSE_BUSY 2

#define HID_RESPONSE_NO_PROFILE 4
#define HID_RESPONSE_INVALID_ARG 5
#define HID_RESPONSE_UNKNOWN_CMD 6

#define HID_USAGE_ID_KEYBOARD 1
#define HID_USAGE_ID_MEDIA_KEY 2
#define HID_USAGE_ID_MOUSE 3
#define HID_USAGE_ID_NAMED_PIPE 4

void handle_hid_command(const uint8_t* hid_rx_buf);
void sd_walk(void);
extern volatile uint8_t is_in_file_access_mode;

#ifdef __cplusplus
}
#endif

#endif


