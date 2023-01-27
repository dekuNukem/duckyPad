#include <stdio.h>    /* printf */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ds3_vm.h"
#include "parser.h"

#include <stdlib.h>
#include "keyboard.h"
#include "animations.h"

#define BIN_BUF_SIZE 200
uint8_t bin_buf[BIN_BUF_SIZE];

uint16_t defaultdelay_value;
uint16_t defaultchardelay_value;
uint16_t charjitter_value;
uint16_t rand_min, rand_max;

typedef struct
{
  uint8_t top;
  uint32_t stack[STACK_SIZE]; // 32b instead of 16b to leave room for operations
} my_stack;

my_stack arithmetic_stack, call_stack;

void stack_init(my_stack* ms)
{
  ms->top = 0;
  memset(ms->stack, 0, STACK_SIZE);
}

uint8_t stack_push(my_stack* ms, uint16_t value)
{
  if(ms->top >= STACK_SIZE)
    return STACK_OP_OVERFLOW;
  ms->stack[ms->top] = value;
  ms->top++;
  return STACK_OP_OK;
}

uint8_t stack_pop(my_stack* ms, uint16_t *result)
{
  if(ms->top == 0)
    return STACK_OP_UNDERFLOW;
  ms->top--;
  *result = ms->stack[ms->top];
  ms->stack[ms->top] = 0;
  return STACK_OP_OK;
}

uint16_t make_uint16(uint8_t b0, uint8_t b1)
{
  return b0 | (b1 << 8);
}

uint8_t load_dsb(char* filename)
{
  uint8_t op_result = DSB_OK;
  UINT bytes_read = 0;
  if(f_open(&sd_file, filename, FA_READ) != 0)
  {
  	op_result = DSB_FOPEN_FAILED;
  	goto load_dsb_end;
  }
 
 	uint32_t this_file_size = f_size(&sd_file);
  if(this_file_size == 0)
    op_result = DSB_EMPTY_FILE;
  if(this_file_size >= BIN_BUF_SIZE)
    op_result = DSB_FILE_TOO_LARGE;
  memset(bin_buf, 0, BIN_BUF_SIZE);
  f_read(&sd_file, bin_buf, BIN_BUF_SIZE, &bytes_read);
  if(bytes_read != this_file_size)
  	op_result = DSB_FREAD_ERROR;
  load_dsb_end:
  f_close(&sd_file);
  return op_result;
}

void store_uint16_as_two_bytes_at(uint16_t value, uint8_t* buf)
{
  buf[0] = value & 0xff;
  buf[1] = (value >> 8);
}

uint16_t binop_add(uint16_t a, uint16_t b) {return a + b;}
uint16_t binop_sub(uint16_t a, uint16_t b) {return a - b;}
uint16_t binop_mul(uint16_t a, uint16_t b) {return a * b;}
uint16_t binop_divide(uint16_t a, uint16_t b) {return a / b;}
uint16_t binop_mod(uint16_t a, uint16_t b) {return a % b;}
uint16_t binop_greater(uint16_t a, uint16_t b) {return a > b;}
uint16_t binop_greater_eq(uint16_t a, uint16_t b) {return a >= b;}
uint16_t binop_lower(uint16_t a, uint16_t b) {return a < b;}
uint16_t binop_lower_eq(uint16_t a, uint16_t b) {return a <= b;}
uint16_t binop_equal(uint16_t a, uint16_t b) {return a == b;}
uint16_t binop_not_equal(uint16_t a, uint16_t b) {return a != b;}
uint16_t binop_bitwise_and(uint16_t a, uint16_t b) {return a & b;}
uint16_t binop_bitwise_or(uint16_t a, uint16_t b) {return a | b;}
uint16_t binop_lshift(uint16_t a, uint16_t b) {return a << b;}
uint16_t binop_rshift(uint16_t a, uint16_t b) {return a >> b;}
uint16_t binop_logical_and(uint16_t a, uint16_t b) {return a && b;}
uint16_t binop_logical_or(uint16_t a, uint16_t b) {return a || b;}
uint16_t binop_power(uint16_t x, uint16_t exponent)
{
    uint16_t result = 1;
    for (int i = 0; i < exponent; ++i)
        result *= x;
    return result;
}

typedef uint16_t (*FUNC_PTR)(uint16_t, uint16_t);

void binop(ds3_exe_result* exe, FUNC_PTR bin_func)
{
  uint16_t rhs, lhs;
  uint8_t op_result = stack_pop(&arithmetic_stack, &rhs);
  if(op_result != STACK_OP_OK)
  {
    exe->result = op_result;
    return;
  }
  op_result = stack_pop(&arithmetic_stack, &lhs);
  if(op_result != STACK_OP_OK)
  {
    exe->result = op_result;
    return;
  }
  op_result = stack_push(&arithmetic_stack, bin_func(lhs, rhs));
  if(op_result != STACK_OP_OK)
  {
    exe->result = op_result;
    return;
  }
}

void press_key(uint8_t code, uint8_t type)
{
	my_key kk;
	kk.code = code;
  kk.type = type;
  keyboard_press(&kk, 0);
}

void release_key(uint8_t code, uint8_t type)
{
	my_key kk;
	kk.code = code;
  kk.type = type;
  keyboard_release(&kk);
}

#define VAR_BOUNDARY (0x1f)

void write_var(uint8_t* pgm_start, uint16_t addr, uint16_t value)
{
  if(addr == DEFAULTDELAY_ADDR)
    defaultdelay_value = value;
  else if (addr == DEFAULTCHARDELAY_ADDR)
    defaultchardelay_value = value;
  else if (addr == CHARJITTER_ADDR)
    charjitter_value = value;
  else if (addr == _RANDOM_MIN)
    rand_min = value;
  else if (addr == _RANDOM_MAX)
    rand_max = value;
  else if (addr == _RANDOM_INT)
    ; // this is read only, so do nothing
  else
    store_uint16_as_two_bytes_at(value, pgm_start + addr);
}

uint16_t read_var(uint8_t* pgm_start, uint16_t addr)
{
  if(addr == DEFAULTDELAY_ADDR)
    return defaultdelay_value;
  else if (addr == DEFAULTCHARDELAY_ADDR)
    return defaultchardelay_value;
  else if (addr == CHARJITTER_ADDR)
    return charjitter_value;
  else if (addr == _RANDOM_MIN)
    return rand_min;
  else if (addr == _RANDOM_MAX)
    return rand_max;
  else if (addr == _RANDOM_INT)
    return rand() % (rand_max + 1 - rand_min) + rand_min;
  else
    return make_uint16(pgm_start[addr], pgm_start[addr+1]);
}

void print_str(char* start, uint8_t* pgm_start)
{
  char* curr = start;
  uint8_t this_char, lsb, msb;
  my_key kk;
  while(1)
  {
    this_char = curr[0];
    if(this_char == 0)
      break;

    if(this_char == VAR_BOUNDARY)
    {
      curr++;
      lsb = curr[0];
      curr++;
      msb = curr[0];
      curr++;
      curr++;
      uint16_t var_addr = make_uint16(lsb, msb);
      uint16_t var_value = read_var(pgm_start, var_addr);
      memset(temp_buf, 0, PATH_SIZE);
      sprintf(temp_buf, "%d", var_value);
      kb_print(temp_buf, defaultchardelay_value, charjitter_value);
      continue;
    }
    kk.type = KEY_TYPE_CHAR;
    kk.code = utf8ascii(this_char);
    if(kk.code != 0)
      kb_print_char(&kk, defaultchardelay_value, charjitter_value);
    curr++;
  }
}

uint16_t index, red, green, blue;
void parse_color(uint8_t opcode, uint8_t keynum)
{
  stack_pop(&arithmetic_stack, &blue);
  stack_pop(&arithmetic_stack, &green);
  stack_pop(&arithmetic_stack, &red);
  stack_pop(&arithmetic_stack, &index);

  if(index == 0)
    index = keynum;
  else
    index--;
  if(index >= MAPPABLE_KEY_COUNT)
    return;
  set_pixel_3color_update_buffer(index, red, green, blue);
  if(opcode == OP_SWCC)
    neopixel_update();
}

void execute_instruction(uint8_t* pgm_start, uint16_t curr_pc, ds3_exe_result* exe, uint8_t keynum)
{
  uint8_t this_opcode = pgm_start[curr_pc];
  uint8_t byte0 = pgm_start[curr_pc+1];
  uint8_t byte1 = pgm_start[curr_pc+2];
  uint8_t op_result;
  uint16_t op_data = make_uint16(byte0, byte1);
  // printf("PC: %04d | Opcode: %02d | 0x%02x 0x%02x | 0x%04x\n", curr_pc, this_opcode, byte0, byte1, op_data);
  
  exe->result = EXE_OK;
  exe->next_pc = curr_pc + INSTRUCTION_SIZE_BYTES;
  exe->data = 0;

  if(this_opcode == OP_NOP)
  {
    return;
  }
  else if(this_opcode == OP_PUSHC)
  {
    op_result = stack_push(&arithmetic_stack, op_data);
    if(op_result != STACK_OP_OK)
    {
      exe->result = op_result;
      return;
    }
  }
  else if(this_opcode == OP_PUSHV)
  {
    op_result = stack_push(&arithmetic_stack, read_var(pgm_start, op_data));
    if(op_result != STACK_OP_OK)
    {
      exe->result = op_result;
      return;
    }
  }
  else if(this_opcode == OP_POP)
  {
    uint16_t this_item;
    op_result = stack_pop(&arithmetic_stack, &this_item);
    if(op_result != STACK_OP_OK)
    {
      exe->result = op_result;
      return;
    }
    write_var(pgm_start, op_data, this_item);
  }
  else if(this_opcode == OP_BRZ)
  {
    uint16_t this_value;
    op_result = stack_pop(&arithmetic_stack, &this_value);
    if(op_result != STACK_OP_OK)
    {
      exe->result = op_result;
      return;
    }
    if(this_value == 0)
      exe->next_pc = op_data;
  }
  else if(this_opcode == OP_JMP)
  {
    exe->next_pc = op_data;
  }
  else if(this_opcode == OP_CALL)
  {
    op_result = stack_push(&call_stack, exe->next_pc);
    if(op_result != STACK_OP_OK)
    {
      exe->result = op_result;
      return;
    }
    exe->next_pc = op_data;
  }
  else if(this_opcode == OP_RET)
  {
    uint16_t return_pc;
    op_result = stack_pop(&call_stack, &return_pc);
    if(op_result != STACK_OP_OK)
    {
      exe->result = op_result;
      return;
    }
    exe->next_pc = return_pc;
  }
  else if(this_opcode == OP_HALT)
  {
    exe->result = EXE_HALT;
  }
  else if(this_opcode == OP_EQ)
  {
    binop(exe, binop_equal);
  }
  else if(this_opcode == OP_NOTEQ)
  {
    binop(exe, binop_not_equal);
  }
  else if(this_opcode == OP_LT)
  {
    binop(exe, binop_lower);
  }
  else if(this_opcode == OP_LTE)
  {
    binop(exe, binop_lower_eq);
  }
  else if(this_opcode == OP_GT)
  {
    binop(exe, binop_greater);
  }
  else if(this_opcode == OP_GTE)
  {
    binop(exe, binop_greater_eq);
  }
  else if(this_opcode == OP_ADD)
  {
    binop(exe, binop_add);
  }
  else if(this_opcode == OP_SUB)
  {
    binop(exe, binop_sub);
  }
  else if(this_opcode == OP_MULT)
  {
    binop(exe, binop_mul);
  }
  else if(this_opcode == OP_DIV)
  {
    binop(exe, binop_divide);
  }
  else if(this_opcode == OP_MOD)
  {
    binop(exe, binop_mod);
  }
  else if(this_opcode == OP_POW)
  {
    binop(exe, binop_power);
  }
  else if(this_opcode == OP_LSHIFT)
  {
    binop(exe, binop_lshift);
  }
  else if(this_opcode == OP_RSHIFT)
  {
    binop(exe, binop_rshift);
  }
  else if(this_opcode == OP_BITOR)
  {
    binop(exe, binop_bitwise_or);
  }
  else if(this_opcode == OP_BITAND)
  {
    binop(exe, binop_bitwise_and);
  }
  else if(this_opcode == OP_LOGIOR)
  {
    binop(exe, binop_logical_or);
  }
  else if(this_opcode == OP_LOGIAND)
  {
    binop(exe, binop_logical_and);
  }
  else if(this_opcode == OP_STR || this_opcode == OP_STRLN)
  {
    char* str_start = pgm_start + op_data;
    print_str(str_start, pgm_start);
    if(this_opcode == OP_STRLN)
    {
    	press_key(0x28, 0x03); // ENTER key
    	osDelay(defaultdelay_value);
    	release_key(0x28, 0x03);
    	osDelay(defaultdelay_value);
    }
  }
  else if(this_opcode == OP_KDOWN)
  {
  	press_key(byte0, byte1);
  }
  else if(this_opcode == OP_KUP)
  {
  	release_key(byte0, byte1);
  }
  else if(this_opcode == OP_KDOWND)
  {
  	press_key(byte0, byte1);
  	osDelay(defaultdelay_value);
  }
  else if(this_opcode == OP_KUPD)
  {
  	release_key(byte0, byte1);
  	osDelay(defaultdelay_value);
  }
  else if(this_opcode == OP_MMOV)
  {
    my_key kk;
    kk.code = byte1;
    kk.code2 = byte0;
    kk.type = KEY_TYPE_MOUSE_MOVEMENT;
    keyboard_press(&kk, 0);
    osDelay(defaultdelay_value);
  }
  else if(this_opcode == OP_MSCL)
  {
    my_key kk;
    kk.code = byte0;
    kk.code2 = 0;
    kk.type = KEY_TYPE_MOUSE_WHEEL;
    keyboard_press(&kk, 0);
    osDelay(defaultdelay_value);
  }
  else if(this_opcode == OP_DELAY)
  {
    uint16_t delay_amount;
    op_result = stack_pop(&arithmetic_stack, &delay_amount);
    if(op_result != STACK_OP_OK)
    {
      exe->result = op_result;
      return;
    }
    osDelay(delay_amount);
  }
  else if(this_opcode == OP_MSCL)
  {
    my_key kk;
    kk.code = byte0;
    kk.code2 = 0;
    kk.type = KEY_TYPE_MOUSE_WHEEL;
    keyboard_press(&kk, 0);
    osDelay(defaultdelay_value);
  }
  else if(this_opcode == OP_SWCB || this_opcode == OP_SWCC)
  {
    parse_color(this_opcode, keynum);
  }
  else
  {
    // UNKNOWN OP CODE
    exe->result = EXE_ERROR;
  }
}

void run_dsb(ds3_exe_result* er, uint8_t keynum)
{
  uint16_t current_pc = 0;
	
  stack_init(&arithmetic_stack);
  stack_init(&call_stack);
  uint8_t header_size = make_uint16(bin_buf[0], bin_buf[1]);
  uint8_t* pgm_start = bin_buf + header_size;
  defaultdelay_value = DEFAULT_DEFAULTDELAY_MS;
  defaultchardelay_value = DEFAULT_DEFAULTCHARDELAY_MS;
  charjitter_value = 0;
  rand_max = 65535;
  rand_min = 0;
  srand(HAL_GetTick());

  while(1)
  {
    execute_instruction(pgm_start, current_pc, er, keynum);
    if(er->result != EXE_OK)
      break;
    current_pc = er->next_pc;
  }
  // printf("execution halted: %d\n", er->result);
}
