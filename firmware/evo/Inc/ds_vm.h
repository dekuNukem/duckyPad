
#ifndef ds_vm_h
#define ds_vm_h

#include <stdint.h>
#include <string.h>

#include <input_task.h>

#define OP_NOP 0
#define OP_PUSHC 1
#define OP_PUSHV 2
#define OP_POP 3
#define OP_BRZ 4
#define OP_JMP 5
#define OP_CALL 6
#define OP_RET 7
#define OP_HALT 8
#define OP_EQ 9
#define OP_NOTEQ 10
#define OP_LT 11
#define OP_LTE 12
#define OP_GT 13
#define OP_GTE 14
#define OP_ADD 15
#define OP_SUB 16
#define OP_MULT 17
#define OP_DIV 18
#define OP_MOD 19
#define OP_POW 20
#define OP_LSHIFT 21
#define OP_RSHIFT 22
#define OP_BITOR 23
#define OP_BITAND 24
#define OP_LOGIAND 25
#define OP_LOGIOR 26
#define OP_DELAY 27
#define OP_KUP 28
#define OP_KDOWN 29
#define OP_MSCL 30
#define OP_MMOV 31
#define OP_SWCF 32
#define OP_SWCC 33
#define OP_SWCR 34
#define OP_STR 35
#define OP_STRLN 36
// #define OP_EMUK 37
#define OP_OLC 38
#define OP_OLP 39
#define OP_OLU 40
#define OP_OLB 41
#define OP_OLR 42
#define OP_BCLR 43
#define OP_PREVP 44
#define OP_NEXTP 45
#define OP_GOTOP 46
#define OP_SLEEP 47

#define OP_OLED_LINE 48
#define OP_OLED_RECT 49
#define OP_OLED_CIRCLE 50

#define OP_VMINFO 255

#define INSTRUCTION_SIZE_BYTES 3
#define MY_STACK_SIZE 16

#define EXE_OK 0

#define EXE_ACTION_SLEEP 1
#define EXE_ACTION_PREV_PROFILE 2
#define EXE_ACTION_NEXT_PROFILE 3
#define EXE_ACTION_GOTO_PROFILE 4

#define EXE_HALT 10
#define EXE_ABORTED 11

#define EXE_ERROR_CODE_START 20
#define EXE_UNKNOWN_OPCODE EXE_ERROR_CODE_START
#define EXE_DSB_INCOMPATIBLE_VERSION (EXE_ERROR_CODE_START + 1)
#define EXE_DSB_FOPEN_FAIL (EXE_ERROR_CODE_START + 2)
#define EXE_DSB_FREAD_ERROR (EXE_ERROR_CODE_START + 3)
#define EXE_STACK_OVERFLOW (EXE_ERROR_CODE_START + 4)
#define EXE_STACK_UNDERFLOW (EXE_ERROR_CODE_START + 5)

#define DEFAULTDELAY_ADDR (0xffff - 0)
#define DEFAULTCHARDELAY_ADDR (0xffff - 1)
#define CHARJITTER_ADDR (0xffff - 2)
#define _RANDOM_MIN (0xffff - 3)
#define _RANDOM_MAX (0xffff - 4)
#define _RANDOM_INT (0xffff - 5)
#define _TIME_MS (0xffff - 6)
#define _READKEY (0xffff - 7)
#define _LOOP_SIZE (0xffff - 8)
#define _KEYPRESS_COUNT (0xffff - 9)
#define _NEEDS_EPILOGUE  (0xffff - 10)
#define _TIME_S  (0xffff - 11)
#define _ALLOW_ABORT (0xffff - 12)
#define _BLOCKING_READKEY (0xffff - 13)
#define _IS_NUMLOCK_ON (0xffff - 14)
#define _IS_CAPSLOCK_ON (0xffff - 15)
#define _IS_SCROLLLOCK_ON (0xffff - 16)
#define _DONT_REPEAT (0xffff - 17)
#define _THIS_KEYID (0xffff - 18)
#define _DP_MODEL (0xffff - 19)

typedef struct
{
  uint8_t result;
  uint16_t next_pc;
  uint8_t data;
  uint8_t epilogue_actions;
} ds3_exe_result;

#define DEFAULT_CMD_DELAY_MS 20
#define DEFAULT_CHAR_DELAY_MS 20

#define EPILOGUE_SAVE_LOOP_STATE 0x1
#define EPILOGUE_SAVE_COLOR_STATE 0x2
#define EPILOGUE_NEED_OLED_RESTORE 0x4
#define EPILOGUE_DONT_AUTO_REPEAT 0x8
#define EPILOGUE_SAVE_GV 0x10

#define BIN_BUF_SIZE 2048
#define VAR_BUF_SIZE 128

#define GLOBAL_VARIABLE_COUNT 16
#define GLOBAL_VARIABLE_OFFSET 64
#define GLOBAL_VARIABLE_START (BIN_BUF_SIZE - 1 - GLOBAL_VARIABLE_OFFSET)
#define GLOBAL_VARIABLE_END_INCLUSIVE (GLOBAL_VARIABLE_START - GLOBAL_VARIABLE_COUNT + 1)

#define DSB_OK 0
#define DSB_FOPEN_FAILED 10
#define DSB_FREAD_ERROR 20
#define DSB_CHUNK_LOAD_ERROR 30
#define DSB_READ_OVERFLOW 40

extern uint8_t bin_buf[BIN_BUF_SIZE];
extern uint8_t allow_abort;
extern uint8_t kb_led_status;
extern uint16_t gv_buf[GLOBAL_VARIABLE_COUNT];
extern uint8_t current_bank;

#define READ_BUF_SIZE 256 * 2
extern char read_buffer[READ_BUF_SIZE];

void run_dsb(ds3_exe_result* er, uint8_t this_key_id, const char* dsb_path);
uint8_t read_byte_with_error(const char* dsb_path, uint16_t addr, uint8_t* result);

#endif

