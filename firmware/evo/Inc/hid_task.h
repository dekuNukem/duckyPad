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
#define HID_COMMAND_ENTER_EXCLUSIVE_MODE 31
#define HID_COMMAND_DUMP_SD 32
#define HID_COMMAND_OPEN_FILE_FOR_READING 33
#define HID_COMMAND_READ_FILE 11
#define HID_COMMAND_OPEN_FILE_FOR_WRITING 14
#define HID_COMMAND_WRITE_FILE 15
#define HID_COMMAND_DELETE_FILE 17
#define HID_COMMAND_CREATE_DIR 18
#define HID_COMMAND_DELETE_DIR 19

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

#define HID_READ_FILE_PATH_SIZE_MAX 55
#define HID_FILE_READ_PAYLOAD_SIZE 61

void handle_hid_command(const uint8_t* hid_rx_buf);
void sd_walk(uint8_t* res_buf);
void md5_test(void);
uint8_t make_file_walk_hid_packet(char* file_name, char* profile_name, uint8_t* tx_buf);

extern volatile uint8_t is_in_file_access_mode;

#ifdef __cplusplus
}
#endif

#endif


