#include <stdio.h>    /* printf */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "input_task.h"
#include "ui_task.h"
#include "neopixel.h"
#include "shared.h"
#include "profiles.h"
#include "ssd1306.h"
#include "keypress_task.h"
#include "keyboard.h"
#include "ds_vm.h"
#include "input_task.h"
#include "main.h"
#include "fatfs.h"

uint8_t bin_buf[BIN_BUF_SIZE];
uint8_t var_buf[VAR_BUF_SIZE];
uint16_t defaultdelay_value;
uint16_t defaultchardelay_value;
uint16_t charjitter_value;
uint32_t rand_min, rand_max;
uint16_t loop_size;
uint8_t epilogue_actions;
uint8_t allow_abort;
uint8_t kb_led_status;
uint8_t last_stack_op_result;
uint8_t disable_autorepeat;
uint16_t gv_buf[GLOBAL_VARIABLE_COUNT];
uint8_t current_bank;

typedef struct
{
  uint8_t top;
  uint16_t stack[MY_STACK_SIZE];
} my_stack;

my_stack arithmetic_stack, call_stack;

uint8_t is_global_variable(uint16_t addr)
{
  return (addr <= GLOBAL_VARIABLE_START && addr >= GLOBAL_VARIABLE_END_INCLUSIVE);
}

uint8_t get_gv_index(uint16_t addr)
{
  uint8_t result = GLOBAL_VARIABLE_START - addr;
  if(result >= GLOBAL_VARIABLE_COUNT)
    return 0;
  return result;
}

uint8_t switch_bank(uint16_t addr, const char* dsb_path)
{
  uint8_t target_bank = addr/BIN_BUF_SIZE;
  if(target_bank == current_bank)
    return DSB_OK;

  // printf("\n---cc:%d, tc:%d---\n", current_bank, target_bank);
  uint8_t op_result = DSB_OK;
  UINT bytes_read = 0;
  if(f_open(&sd_file, dsb_path, FA_READ) != 0)
  {
    op_result = DSB_FOPEN_FAILED;
    goto switch_bank_end;
  }

  if(addr >= f_size(&sd_file))
  {
    op_result = DSB_READ_OVERFLOW;
    goto switch_bank_end;
  }

  if(f_lseek(&sd_file, target_bank*BIN_BUF_SIZE) != 0)
  {
    op_result = DSB_CHUNK_LOAD_ERROR;
    goto switch_bank_end;
  }

  memset(bin_buf, 0, BIN_BUF_SIZE);
  f_read(&sd_file, bin_buf, BIN_BUF_SIZE, &bytes_read);
  current_bank = target_bank;
  switch_bank_end:
  f_close(&sd_file);
  return op_result;
}

uint8_t read_byte(uint16_t addr, const char* dsb_path)
{
  if(switch_bank(addr,dsb_path) != DSB_OK)
  {
    keyboard_release_all();
    mouse_release_all();
    HAL_Delay(1000);
    NVIC_SystemReset();
  }
  return bin_buf[addr%BIN_BUF_SIZE];
}

void stack_init(my_stack* ms)
{
  ms->top = 0;
  memset(ms->stack, 0, MY_STACK_SIZE*sizeof(uint16_t));
}

void stack_push(my_stack* ms, uint16_t value)
{
  if(ms->top >= MY_STACK_SIZE)
  {
    last_stack_op_result = EXE_STACK_OVERFLOW;
    return;
  }
  ms->stack[ms->top] = value;
  ms->top++;
  last_stack_op_result = EXE_OK;
}

void stack_pop(my_stack* ms, uint16_t *result)
{
  if(ms->top == 0)
  {
    last_stack_op_result = EXE_STACK_UNDERFLOW;
    return;
  }
  ms->top--;
  *result = ms->stack[ms->top];
  ms->stack[ms->top] = 0;
  last_stack_op_result = EXE_OK;
}

uint16_t make_uint16(uint8_t b0, uint8_t b1)
{
  return b0 | (b1 << 8);
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
void binop(FUNC_PTR bin_func)
{
  uint16_t rhs, lhs;
  stack_pop(&arithmetic_stack, &rhs);
  stack_pop(&arithmetic_stack, &lhs);
  stack_push(&arithmetic_stack, bin_func(lhs, rhs));
}

#define VAR_BOUNDARY (0x1f)

void store_uint16_as_two_bytes_at(uint16_t addr, uint16_t value)
{
  var_buf[addr] = value & 0xff;
  var_buf[addr+1] = value >> 8;
}

void write_var(uint16_t addr, uint16_t value, uint8_t this_key_id)
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
    ; // read only
  else if (addr == _TIME_MS)
    ; // read only
  else if (addr == _TIME_S)
    ; // read only
  else if (addr == _READKEY)
    ; // read only
  else if (addr == _BLOCKING_READKEY)
    ; // read only
  else if (addr == _LOOP_SIZE)
    loop_size = value;
  else if (addr == _KEYPRESS_COUNT)
    curr_pf_info.keypress_count[this_key_id] = value;
  else if (addr == _NEEDS_EPILOGUE)
    epilogue_actions = value;
  else if (addr == _ALLOW_ABORT)
    allow_abort = value;
  else if (addr == _IS_NUMLOCK_ON)
    ; // read only
  else if (addr == _IS_CAPSLOCK_ON)
    ; // read only
  else if (addr == _IS_SCROLLLOCK_ON)
    ; // read only
  else if (addr == _DONT_REPEAT)
    disable_autorepeat = value;
  else if (addr == _THIS_KEYID)
    ; // read only
  else if (addr == _DP_MODEL)
    ; // read only
  else if (is_global_variable(addr))
    gv_buf[get_gv_index(addr)] = value;
  else if(addr < VAR_BUF_SIZE)
    store_uint16_as_two_bytes_at(addr, value);
}

uint8_t readkey_nonblocking_1_indexed(void)
{
  for (uint8_t i = 0; i < MAX_TOTAL_SW_COUNT; i++)
  {
    if(poll_sw_state(i, 0))
      return i+1;
  }
  return 0;
}

uint8_t readkey_blocking(void)
{
  clear_sw_queue();
  while(1)
  {
    delay_ms(10);
    switch_event_t sw_event = {0};
    if(q_pop(&switch_event_queue, &sw_event) && sw_event.type == SW_EVENT_SHORT_PRESS)
      return sw_event.id+1;
  }
}

uint16_t read_var(uint16_t addr, uint8_t this_key_id)
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
  else if (addr == _TIME_MS)
    return (uint16_t)millis();
  else if (addr == _TIME_S)
    return (uint16_t)(millis() / 1000);
  else if (addr == _LOOP_SIZE)
    return loop_size;
  else if (addr == _READKEY)
    return readkey_nonblocking_1_indexed();
  else if (addr == _KEYPRESS_COUNT)
    return curr_pf_info.keypress_count[this_key_id];
  else if (addr == _NEEDS_EPILOGUE)
    return epilogue_actions;
  else if (addr == _ALLOW_ABORT)
    return allow_abort;
  else if (addr == _BLOCKING_READKEY)
    return readkey_blocking();
  else if (addr == _IS_NUMLOCK_ON)
    return (kb_led_status & 0x1) ? 1 : 0;
  else if (addr == _IS_CAPSLOCK_ON)
    return (kb_led_status & 0x2) ? 1 : 0;
  else if (addr == _IS_SCROLLLOCK_ON)
    return (kb_led_status & 0x4) ? 1 : 0;
  else if (addr == _DONT_REPEAT)
    return disable_autorepeat;
  else if (addr == _THIS_KEYID)
    return this_key_id+1;
  else if (addr == _DP_MODEL)
    return 2;
  else if (is_global_variable(addr))
    return gv_buf[get_gv_index(addr)];
  else if(addr < VAR_BUF_SIZE)
    return make_uint16(var_buf[addr], var_buf[addr+1]);
  return 0;
}

#define STR_BUF_SIZE 8
char make_str_buf[STR_BUF_SIZE];
char read_buffer[READ_BUF_SIZE];
char* make_str(uint16_t str_start_addr, uint8_t this_key_id, const char* dsb_path)
{
  uint16_t curr_addr = str_start_addr;
  uint8_t this_char, lsb, msb;
  memset(read_buffer, 0, READ_BUF_SIZE);
  while(1)
  {
    this_char = read_byte(curr_addr, dsb_path);
    if(this_char == 0)
      break;

    if(this_char == VAR_BOUNDARY)
    {
      curr_addr++;
      lsb = read_byte(curr_addr, dsb_path);
      curr_addr++;
      msb = read_byte(curr_addr, dsb_path);
      curr_addr++;
      curr_addr++;
      uint16_t var_addr = make_uint16(lsb, msb);
      uint16_t var_value = read_var(var_addr, this_key_id);
      memset(make_str_buf, 0, STR_BUF_SIZE);
      sprintf(make_str_buf, "%d", var_value);
      strcat(read_buffer, make_str_buf);
      continue;
    }
    memset(make_str_buf, 0, STR_BUF_SIZE);
    sprintf(make_str_buf, "%c", this_char);
    strcat(read_buffer, make_str_buf);
    curr_addr++;
  }
  return read_buffer;
}

uint16_t this_index, red, green, blue;
void parse_swcc(uint8_t opcode, uint8_t key_id_0_indexed)
{
  stack_pop(&arithmetic_stack, &blue);
  stack_pop(&arithmetic_stack, &green);
  stack_pop(&arithmetic_stack, &red);
  stack_pop(&arithmetic_stack, &this_index);

  if(this_index == 0)
    this_index = key_id_0_indexed;
  else
    this_index--;
  if(this_index >= NEOPIXEL_COUNT)
    return;
  halt_all_animations();
  set_pixel_3color(this_index, red, green, blue);
  neopixel_draw_current_buffer();
}

void parse_swcf(void)
{
  stack_pop(&arithmetic_stack, &blue);
  stack_pop(&arithmetic_stack, &green);
  stack_pop(&arithmetic_stack, &red);
  halt_all_animations();
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
    set_pixel_3color(i, red, green, blue);
  neopixel_draw_current_buffer();
}

void parse_swcr(uint8_t key_id_0_indexed)
{
  uint16_t swcr_arg;
  stack_pop(&arithmetic_stack, &swcr_arg);

  if(swcr_arg == 0)
    swcr_arg = key_id_0_indexed;
  else
    swcr_arg--;

  if(swcr_arg >= NEOPIXEL_COUNT)
    redraw_bg();
  else
    reset_key_color(swcr_arg);
}

void parse_olc(void)
{
  uint16_t xxx, yyy;
  stack_pop(&arithmetic_stack, &yyy);
  stack_pop(&arithmetic_stack, &xxx);
  if(xxx >= SSD1306_WIDTH || yyy >= SSD1306_HEIGHT)
    return;
  ssd1306_SetCursor(xxx, yyy);
}

void clamp_value(int16_t* value, int16_t upper_limit)
{
  if(value == NULL)
    return;
  if(*value < 0)
    *value = 0;
  if(*value > upper_limit)
    *value = upper_limit;
}

void parse_oled_draw_line(void)
{
  int16_t x1,y1,x2,y2;
  stack_pop(&arithmetic_stack, (uint16_t *)&y2);
  stack_pop(&arithmetic_stack, (uint16_t *)&x2);
  stack_pop(&arithmetic_stack, (uint16_t *)&y1);
  stack_pop(&arithmetic_stack, (uint16_t *)&x1);
  clamp_value(&x1, SSD1306_WIDTH);
  clamp_value(&x2, SSD1306_WIDTH);
  clamp_value(&y1, SSD1306_HEIGHT);
  clamp_value(&y2, SSD1306_HEIGHT);
  ssd1306_Line(x1, y1, x2, y2, White);
}

void parse_oled_draw_circle(void)
{
  int16_t x,y,radius,fill;
  stack_pop(&arithmetic_stack, (uint16_t *)&fill);
  stack_pop(&arithmetic_stack, (uint16_t *)&radius);
  stack_pop(&arithmetic_stack, (uint16_t *)&y);
  stack_pop(&arithmetic_stack, (uint16_t *)&x);
  clamp_value(&x, SSD1306_WIDTH);
  clamp_value(&y, SSD1306_HEIGHT);
  clamp_value(&radius, SSD1306_HEIGHT/2);
  if(fill)
    ssd1306_FillCircle(x,y,radius,White);
  else
    ssd1306_DrawCircle(x,y,radius,White);
}

void parse_oled_draw_rect(void)
{
  int16_t x1,y1,x2,y2,fill;
  stack_pop(&arithmetic_stack, (uint16_t *)&fill);
  stack_pop(&arithmetic_stack, (uint16_t *)&y2);
  stack_pop(&arithmetic_stack, (uint16_t *)&x2);
  stack_pop(&arithmetic_stack, (uint16_t *)&y1);
  stack_pop(&arithmetic_stack, (uint16_t *)&x1);
  clamp_value(&x1, SSD1306_WIDTH);
  clamp_value(&x2, SSD1306_WIDTH);
  clamp_value(&y1, SSD1306_HEIGHT);
  clamp_value(&y2, SSD1306_HEIGHT);
  if(fill)
    ssd1306_FillRectangle(x1,y1,x2,y2,White);
  else
    ssd1306_DrawRectangle(x1,y1,x2,y2,White);
}

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

void expand_mmov(int16_t xtotal, int16_t ytotal)
{
  int16_t xsign = (xtotal < 0) ? -1 : 1;
  int16_t ysign = (ytotal < 0) ? -1 : 1;

  xtotal = abs(xtotal);
  ytotal = abs(ytotal);

  uint8_t loops_needed_x = abs(xtotal) / 128;
  uint8_t loops_needed_y = abs(ytotal) / 128;

  uint8_t total_loops_needed = MAX(loops_needed_x, loops_needed_y);

  for (int i = 0; i <= total_loops_needed; ++i)
  {
    int8_t this_x_amount;
    if(xtotal > 127)
      this_x_amount = 127;
    else
      this_x_amount = xtotal;
    xtotal -= this_x_amount;

    int8_t this_y_amount;
    if(ytotal > 127)
      this_y_amount = 127;
    else
      this_y_amount = ytotal;
    ytotal -= this_y_amount;

    my_key kk;
    kk.code = (uint8_t)this_x_amount*xsign;
    kk.code2 = (uint8_t)this_y_amount*ysign;
    kk.type = KEY_TYPE_MOUSE_MOVEMENT;
    action_press(&kk, 0);
    delay_ms(defaultdelay_value);
  }
}

void parse_mmov(void)
{
  uint16_t tempx, tempy;
  stack_pop(&arithmetic_stack, &tempy);
  stack_pop(&arithmetic_stack, &tempx);
  expand_mmov(tempx, tempy);
}

void execute_instruction(uint16_t curr_pc, ds3_exe_result* exe, uint8_t this_key_id, const char* dsb_path)
{
  uint8_t this_opcode = read_byte(curr_pc, dsb_path);
  uint8_t byte0 = read_byte(curr_pc+1, dsb_path);
  uint8_t byte1 = read_byte(curr_pc+2, dsb_path);
  uint16_t op_data = make_uint16(byte0, byte1);
  // printf("PC: %04d | Opcode: %02d | 0x%02x 0x%02x | 0x%04x\n", curr_pc, this_opcode, byte0, byte1, op_data);
  
  exe->result = EXE_OK;
  exe->next_pc = curr_pc + INSTRUCTION_SIZE_BYTES;
  exe->data = 0;
  
  if(allow_abort && sw_queue_has_keydown_event())
  {
    exe->result = EXE_ABORTED;
    return;
  }
  else if(this_opcode == OP_NOP || this_opcode == OP_VMINFO)
  {
    return;
  }
  else if(this_opcode == OP_PUSHC)
  {
    stack_push(&arithmetic_stack, op_data);
  }
  else if(this_opcode == OP_PUSHV)
  {
    stack_push(&arithmetic_stack, read_var(op_data, this_key_id));
  }
  else if(this_opcode == OP_POP)
  {
    uint16_t this_item;
    stack_pop(&arithmetic_stack, &this_item);
    write_var(op_data, this_item, this_key_id);
  }
  else if(this_opcode == OP_BRZ)
  {
    uint16_t this_value;
    stack_pop(&arithmetic_stack, &this_value);
    if(this_value == 0)
      exe->next_pc = op_data;
  }
  else if(this_opcode == OP_JMP)
  {
    exe->next_pc = op_data;
  }
  else if(this_opcode == OP_CALL)
  {
    stack_push(&call_stack, exe->next_pc);
    exe->next_pc = op_data;
  }
  else if(this_opcode == OP_RET)
  {
    uint16_t return_pc;
    stack_pop(&call_stack, &return_pc);
    exe->next_pc = return_pc;
  }
  else if(this_opcode == OP_HALT)
  {
    exe->result = EXE_HALT;
  }
  else if(this_opcode == OP_EQ)
  {
    binop(binop_equal);
  }
  else if(this_opcode == OP_NOTEQ)
  {
    binop(binop_not_equal);
  }
  else if(this_opcode == OP_LT)
  {
    binop(binop_lower);
  }
  else if(this_opcode == OP_LTE)
  {
    binop(binop_lower_eq);
  }
  else if(this_opcode == OP_GT)
  {
    binop(binop_greater);
  }
  else if(this_opcode == OP_GTE)
  {
    binop(binop_greater_eq);
  }
  else if(this_opcode == OP_ADD)
  {
    binop(binop_add);
  }
  else if(this_opcode == OP_SUB)
  {
    binop(binop_sub);
  }
  else if(this_opcode == OP_MULT)
  {
    binop(binop_mul);
  }
  else if(this_opcode == OP_DIV)
  {
    binop(binop_divide);
  }
  else if(this_opcode == OP_MOD)
  {
    binop(binop_mod);
  }
  else if(this_opcode == OP_POW)
  {
    binop(binop_power);
  }
  else if(this_opcode == OP_LSHIFT)
  {
    binop(binop_lshift);
  }
  else if(this_opcode == OP_RSHIFT)
  {
    binop(binop_rshift);
  }
  else if(this_opcode == OP_BITOR)
  {
    binop(binop_bitwise_or);
  }
  else if(this_opcode == OP_BITAND)
  {
    binop(binop_bitwise_and);
  }
  else if(this_opcode == OP_LOGIOR)
  {
    binop(binop_logical_or);
  }
  else if(this_opcode == OP_LOGIAND)
  {
    binop(binop_logical_and);
  }
  else if(this_opcode == OP_STR || this_opcode == OP_STRLN)
  {
    char* str_buf = make_str(op_data, this_key_id, dsb_path);
    if(kb_print(str_buf, defaultchardelay_value, charjitter_value))
    {
      exe->result = EXE_ABORTED;
      return;
    }
    if(this_opcode == OP_STRLN)
    {
    	press_key(0x28, 0x03); // ENTER key
    	delay_ms(defaultdelay_value);
    	release_key(0x28, 0x03);
    	delay_ms(defaultdelay_value);
    }
  }
  else if(this_opcode == OP_KDOWN)
  {
    uint16_t combocode;
    stack_pop(&arithmetic_stack, &combocode);
    uint8_t ktype = (combocode >> 8) & 0xff;
    uint8_t kcode = combocode & 0xff;
  	press_key(kcode, ktype);
  	delay_ms(defaultdelay_value);
  }
  else if(this_opcode == OP_KUP)
  {
    uint16_t combocode;
    stack_pop(&arithmetic_stack, &combocode);
    uint8_t ktype = (combocode >> 8) & 0xff;
    uint8_t kcode = combocode & 0xff;
  	release_key(kcode, ktype);
  	delay_ms(defaultdelay_value);
  }
  else if(this_opcode == OP_MMOV)
  {
    parse_mmov();
  }
  else if(this_opcode == OP_DELAY)
  {
    uint16_t delay_amount;
    stack_pop(&arithmetic_stack, &delay_amount);
    delay_ms(delay_amount);
  }
  else if(this_opcode == OP_MSCL)
  {
    uint16_t scroll_amount;
    stack_pop(&arithmetic_stack, &scroll_amount);
    my_key kk;
    kk.code = scroll_amount;
    kk.code2 = 0;
    kk.type = KEY_TYPE_MOUSE_WHEEL;
    action_press(&kk, 0);
    delay_ms(defaultchardelay_value);
    action_release(&kk);
    delay_ms(defaultdelay_value);
  }
  else if(this_opcode == OP_SWCC)
  {
    parse_swcc(this_opcode, this_key_id);
  }
  else if(this_opcode == OP_SWCF)
  {
    parse_swcf();
  }
  else if(this_opcode == OP_SWCR)
  {
    parse_swcr(this_key_id);
  }
  else if(this_opcode == OP_OLC)
  {
    parse_olc();
  }
  else if(this_opcode == OP_OLED_LINE)
  {
    parse_oled_draw_line();
  }
  else if(this_opcode == OP_OLED_CIRCLE)
  {
    parse_oled_draw_circle();
  }
  else if(this_opcode == OP_OLED_RECT)
  {
    parse_oled_draw_rect();
  }
  else if(this_opcode == OP_OLP)
  {
    char* str_buf = make_str(op_data, this_key_id, dsb_path);
    ssd1306_WriteString(str_buf, Font_6x10, White);
  }
  else if(this_opcode == OP_OLU)
  {
    ssd1306_UpdateScreen();
  }
  else if(this_opcode == OP_OLB)
  {
    ssd1306_Fill(Black);
  }
  else if(this_opcode == OP_OLR)
  {
    draw_current_profile();
  }
  else if(this_opcode == OP_BCLR)
  {
    clear_sw_queue();
  }
  else if(this_opcode == OP_NEXTP)
  {
    exe->result = EXE_ACTION_NEXT_PROFILE;
  }
  else if(this_opcode == OP_PREVP)
  {
    exe->result = EXE_ACTION_PREV_PROFILE;
  }
  else if(this_opcode == OP_SLEEP)
  {
    exe->result = EXE_ACTION_SLEEP;
  }
  else if(this_opcode == OP_GOTOP)
  {
    uint16_t target_profile;
    stack_pop(&arithmetic_stack, &target_profile);
    exe->result = EXE_ACTION_GOTO_PROFILE;
    exe->data = (uint8_t)target_profile;
  }
  else
  {
    exe->result = EXE_UNKNOWN_OPCODE;
  }
}

void run_dsb(ds3_exe_result* er, uint8_t this_key_id, const char* dsb_path)
{
  uint16_t current_pc = 0;
  current_bank = 255;
  stack_init(&arithmetic_stack);
  stack_init(&call_stack);
  defaultdelay_value = DEFAULT_CMD_DELAY_MS;
  defaultchardelay_value = DEFAULT_CHAR_DELAY_MS;
  charjitter_value = 0;
  rand_max = 65535;
  rand_min = 0;
  loop_size = 0;
  epilogue_actions = 0;
  allow_abort = 0;
  last_stack_op_result = EXE_OK;
  disable_autorepeat = 0;
  srand(millis());

  while(1)
  {
    execute_instruction(current_pc, er, this_key_id, dsb_path);
    if(er->result != EXE_OK || last_stack_op_result != EXE_OK)
      break;
    current_pc = er->next_pc;
  }
  if(disable_autorepeat)
    epilogue_actions |= EPILOGUE_DONT_AUTO_REPEAT;
  er->epilogue_actions = epilogue_actions;
}
