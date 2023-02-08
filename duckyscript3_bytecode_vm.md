# UNDER CONSTRUCTION!!!!

## CPU Instructions

|   Name  | Dec | Hex |                                                                                        Comment                                                                                       |   Byte 1  |   Byte 2  |
|:-------:|:---:|:----:|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|:---------:|:---------:|
|   NOP   |  0  |  0x0 |                                                                                      Do nothing                                                                                      |           |           |
|  PUSHC  |  1  |  0x1 |                                                                               Push a constant on stack                                                                               | CONST_LSB | CONST_LSB |
|  PUSHV  |  2  |  0x2 |                                                                              Push value at ADDR on stack                                                                             |  ADDR_LSB |  ADDR_MSB |
|   POP   |  3  |  0x3 |                                                                     Pop one item off top of stack<br>Write it to ADDR                                                                    |  ADDR_LSB |  ADDR_MSB |
|   BRZ   |  4  |  0x4 |                                                              Pop one item off top of stack<br>If value is zero, jump to ADDR                                                             |  ADDR_LSB |  ADDR_MSB |
|   JMP   |  5  |  0x5 |                                                                                  Unconditional Jump                                                                                  |  ADDR_LSB |  ADDR_MSB |
|   CALL  |  6  |  0x6 |                                                              Jump to Subroutine<br>Push PC+1 to **call stack**<br>Jump to ADDR                                                             |  ADDR_LSB |  ADDR_MSB |
|   RET   |  7  |  0x7 |                                                        Return from Subroutine<br>Pop one item off **call stack**<br>Set PC to its value                                                       |           |           |
|   HALT  |  8  |  0x8 |                                                                                    Stop execution                                                                                    |           |           |

## Binary Operator Instructions

Binary as in **involving two operands**.

All instructions here:

* Pop TWO items off arithmetic stack

* First item is left-hand-side, second item is right-hand-side.

* Perform operation

* Push result back on stack.

|   Name  | Dec | Hex |                                                                                        Comment                                                                                       |
|:-------:|:---:|:----:|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
|    EQ   |  9  |  0x9 |                                                                                         Equal                                                                                        |
|  NOTEQ  |  10 |  0xa |                                                                                       Not equal                                                                                      |
|    LT   |  11 |  0xb |                                                                                       Less than                                                                                      |
|   LTE   |  12 |  0xc |                                                                                  Less than or equal                                                                                  |
|    GT   |  13 |  0xd |                                                                                     Greater than                                                                                     |
|   GTE   |  14 |  0xe |                                                                                 Greater than or equal                                                                                |
|   ADD   |  15 |  0xf |                                                                                          Add                                                                                         |
|   SUB   |  16 | 0x10 |                                                                                       Subtract                                                                                       |
|   MULT  |  17 | 0x11 |                                                                                       Multiply                                                                                       |
|   DIV   |  18 | 0x12 |                                                                                   Integer division                                                                                   |
|   MOD   |  19 | 0x13 |                                                                                       Modulous                                                                                       |
|   POW   |  20 | 0x14 |                                                                                       Power of                                                                                       |
|  LSHIFT |  21 | 0x15 |                                                                                  Logical left shift                                                                                  |
|  RSHIFT |  22 | 0x16 |                                                                                  Logical right shift                                                                                 |
|  BITOR  |  23 | 0x17 |                                                                                      Bitwise OR                                                                                      |
|  BITAND |  24 | 0x18 |                                                                                      Bitwise AND                                                                                     |
| LOGIAND |  25 | 0x19 |                                                                                      Logical AND                                                                                     |
|  LOGIOR |  26 | 0x1a |                                                                                      Logical OR                                                                                      |

## duckyScript Command Instructions

|   Name  | Dec | Hex |                                                                                        Comment                                                                                       |   Byte 1  |   Byte 2  |
|:-------:|:---:|:----:|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|:---------:|:---------:|
|  DELAY  |  27 | 0x1b |                                                             Pop one item off top of stack<br>Delay the amount in milliseconds                                                            |           |           |
|   KUP   |  29 | 0x1d |                                                                                      Release Key                                                                                     |  KEYCODE  |  KEYTYPE  |
|  KDOWN  |  31 | 0x1f |                                                                                       Press Key                                                                                      |  KEYCODE  |  KEYTYPE  |
|   SWCF  |  32 | 0x20 |                                         Switch Color Fill<br>Pop THREE items off top of stack:<br>Red, Green, Blue<br>Set ALL LED color to the RGB value                                         |           |           |
|   MSCL  |  33 | 0x21 |                                                                                     Mouse Scroll                                                                                     |   Amount  |           |
|   MMOV  |  34 | 0x22 |                                                                                      Mouse Move                                                                                      |     X     |     Y     |
|   SWCC  |  35 | 0x23 |                         Switch Color Change<br>Pop FOUR items off top of stack:<br>N, Red, Green, Blue<br>Set N-th switch to the RGB value<br>If N is 0, set current switch.                         |           |           |
|   SWCR  |  36 | 0x24 | Switch Color Reset<br>Pop one item off top of stack<br>If value is 0, reset color of current key<br>If value is between 1 and 15, reset color of that key<br>If value is 99, reset color of all keys |           |           |
|   STR   |  38 | 0x26 |                                                                          Print zero-teminated string at ADDR                                                                         |  ADDR_LSB |  ADDR_MSB |
|  STRLN  |  39 | 0x27 |                                                                          Same as above, presses ENTER at end                                                                         |           |           |
|   EMUK  |  40 | 0x28 |                                                                                     EMUK command                                                                                     |  KEYCODE  |  KEYTYPE  |
|   OLC   |  42 | 0x2a |                                                      OLED_CURSOR<br>Pop TWO items off top of stack:<br>X and Y<br>Change OLED cursor to that                                                     |           |           |
|   OLP   |  43 | 0x2b |                                                                      Print zero-teminated string at ADDR to OLED                                                                     |  ADDR_LSB |  ADDR_MSB |
|   OLU   |  44 | 0x2c |                                                                                      OLED_UPDATE                                                                                     |           |           |
|   OLB   |  45 | 0x2d |                                                                                      OLED_CLEAR                                                                                      |           |           |
|   OLR   |  46 | 0x2e |                                                                                     OLED_RESTORE                                                                                     |           |           |
|   BCLR  |  47 | 0x2f |                                                                              Clears button status buffer                                                                             |           |           |
|  PREVP  |  48 | 0x30 |                                                                                   Previous profile                                                                                   |           |           |
|  NEXTP  |  49 | 0x31 |                                                                                     Next profile                                                                                     |           |           |
|  GOTOP  |  50 | 0x32 |                                                               Pop one item off top of stack<br>Go to profile of that value                                                               |           |           |
|  SLEEP  |  51 | 0x33 |                                                                       Put duckyPad to sleep<br>Terminates execution                                                                      |           |           |