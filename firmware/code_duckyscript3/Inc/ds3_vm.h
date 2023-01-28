
#include "stm32f0xx_hal.h"

#ifndef ds3_vm_h
#define ds3_vm_h

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
#define OP_KUPD 29
#define OP_KDOWN 30
#define OP_KDOWND 31
#define OP_MSCL 33
#define OP_MMOV 34
#define OP_SWCB 32
#define OP_SWCC 35
#define OP_SWCR 36
#define OP_STR 38
#define OP_STRLN 39
#define OP_EMUK 40
#define OP_LOOP 41

#define OP_OLC 42
#define OP_OLP 43
#define OP_OLU 44
#define OP_OLB 45
#define OP_OLR 46
#define OP_BCLR 47

#define OP_PREVP 48
#define OP_NEXTP 49
#define OP_GOTOP 50
#define OP_SLEEP 51


#define INSTRUCTION_SIZE_BYTES 3
#define STACK_SIZE 16

#define EXE_OK 0

#define EXE_ACTION_SLEEP 1
#define EXE_ACTION_PREV_PROFILE 2
#define EXE_ACTION_NEXT_PROFILE 3
#define EXE_ACTION_GOTO_PROFILE 4
#define EXE_ACTION_EMUK 5

#define EXE_HALT 20
#define EXE_EMPTY_FILE 21
#define EXE_ERROR 30

#define DSB_OK 0
#define DSB_FOPEN_FAILED 50
#define DSB_FREAD_ERROR 60
#define DSB_EMPTY_FILE 70
#define DSB_FILE_TOO_LARGE 80

#define STACK_OP_OK 0
#define STACK_OP_OVERFLOW 100
#define STACK_OP_UNDERFLOW 110

#define DEFAULTDELAY_ADDR (0xffff - 0)
#define DEFAULTCHARDELAY_ADDR (0xffff - 1)
#define CHARJITTER_ADDR (0xffff - 2)
#define _RANDOM_MIN (0xffff - 3)
#define _RANDOM_MAX (0xffff - 4)
#define _RANDOM_INT (0xffff - 5)
#define _TIME (0xffff - 6)
#define _READKEY (0xffff - 7)

typedef struct
{
  uint8_t result;
  uint16_t next_pc;
  uint8_t data;
  uint8_t data2;
} ds3_exe_result;

uint8_t load_dsb(char* filename);
void run_dsb(ds3_exe_result* er, uint8_t keynum);

#endif

