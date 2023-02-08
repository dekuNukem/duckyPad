# UNDER CONSTRUCTION!!!!

# duckyScript 3 Bytecode Instruction Set

[Get duckyPad](https://www.tindie.com/products/21984/) | [Official Discord](https://discord.gg/4sJCBx5) | [Getting Started](getting_started.md) | [Table of Contents](#table-of-contents)

----

Details of duckyPad duckyScript 3 (**DPDS3**) instruction set, compiler, and binary format.

This is NOT the official implementation, which appears to be closed-source.

I designed everything here myself from scratch.

## Background

In duckyScript 1, it's sufficient to simply parse line-by-line, as each line is independent.

duckyScript 3 introduced variables, `IF` statements, `WHILE` loops, and function calls.

Being much more complex, it makes sense to compile the script into bytecode and execute it on a virtual machine instead.

## Virtual Stack Machine

DPDS3 VM is a simple stack machine with two stacks and a Program Counter (PC).

* PC points to the current instruction being executed.

* Arithmetic Stack: Used to perform calculations and store arguments.

* Call Stack: Keeps track of function calls.

Compiled binary is loaded into memory, and PC starts executing at the beginning of the program.

## Instruction Set

Each DPDS3 instruction has a **fixed-length** of **three bytes**.

* First byte (Byte 0) is **Opcode**, specifying the action this instruction perform.

* Second and third byte (Byte 1 and 2) provides optional data.

* They are treated as one uint16_t or two uint8_t depending on the instruction.

* [Click me to see details](#cpu-instructions)

## Compiler

The DPDS3 compiler takes a text file and outputs a executable binary file.

The binary file should have extension `.dsb` (duckyScript binary).

Normally this is taken care of in the configurator. But you can also try it out on its own:

* [Download the source code](https://github.com/dekuNukem/duckyPad/raw/master/pc_software/ds3/duckypad_config_latest_source.zip)

* Unzip, and run with Python 3: `python3 make_bytecode.py input output`

Let's say you have a simple script in `sample_script.txt`:

```
DEFINE TEN 10
VAR $spam = TEN*7+3
DELAY $spam
```

Compile with `python3 make_bytecode.py sample_script.txt output.dsb`

Compiled binary is written to `output.dsb`, along with some extra info:

```
--------- Program Listing After Preprocessing: ---------
1    VAR $spam = 10*7+3
2    DELAY $spam

--------- Assembly Listing, Unresolved ---------

PUSHC     10    0xa           ;VAR $spam = 10*7+3
PUSHC     7     0x7           ;VAR $spam = 10*7+3
MULT                          ;VAR $spam = 10*7+3
PUSHC     3     0x3           ;VAR $spam = 10*7+3
ADD                           ;VAR $spam = 10*7+3
POP       spam                ;VAR $spam = 10*7+3
PUSHV     spam                ;DELAY $spam
DELAY                         ;DELAY $spam
HALT

--------- Assembly Listing, Resolved ---------

0    PUSHC     10    0xa           ;VAR $spam = 10*7+3
3    PUSHC     7     0x7           ;VAR $spam = 10*7+3
6    MULT                          ;VAR $spam = 10*7+3
9    PUSHC     3     0x3           ;VAR $spam = 10*7+3
12   ADD                           ;VAR $spam = 10*7+3
15   POP       27    0x1b          ;VAR $spam = 10*7+3
18   PUSHV     27    0x1b          ;DELAY $spam
21   DELAY                         ;DELAY $spam
24   HALT

--------- Bytecode header ---------
0x08 0x00 0x1b 0x00 0x1d 0x00 0x00 0x00

--------- Bytecode ---------
0x01 0x0a 0x00 0x01 0x07 0x00 0x11 0x00 0x00
0x01 0x03 0x00 0x0f 0x00 0x00 0x03 0x1b 0x00
0x02 0x1b 0x00 0x1b 0x00 0x00 0x08 0x00 0x00
0x00 0x00

Binary Size: 37 Bytes
```

## Binary Format

Under construction


## CPU Instructions

All reference to **"stack"** refers to **Arithmetic Stack**. Unless noted otherwise.

|   Opcode<br>Name  | Byte 0<br>Decimal | Byte 0<br>Hex |                                                                                        Comment                                                                                       |   Byte 1  |   Byte 2  |
|:-------:|:---:|:----:|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|:---------:|:---------:|
|   NOP   |  0  |  0x0 |                                                                                      Do nothing                                                                                      |           |           |
|  PUSHC  |  1  |  0x1 |                                                                               Push a constant on stack                                                                               | CONST_LSB | CONST_MSB |
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

|   Opcode<br>Name  | Byte 0<br>Decimal | Byte 0<br>Hex |                                                                                        Comment                                                                                       |
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
|   MOD   |  19 | 0x13 |                                                                                       Modulus                                                                                       |
|   POW   |  20 | 0x14 |                                                                                       Power of                                                                                       |
|  LSHIFT |  21 | 0x15 |                                                                                  Logical left shift                                                                                  |
|  RSHIFT |  22 | 0x16 |                                                                                  Logical right shift                                                                                 |
|  BITOR  |  23 | 0x17 |                                                                                      Bitwise OR                                                                                      |
|  BITAND |  24 | 0x18 |                                                                                      Bitwise AND                                                                                     |
| LOGIAND |  25 | 0x19 |                                                                                      Logical AND                                                                                     |
|  LOGIOR |  26 | 0x1a |                                                                                      Logical OR                                                                                      |

## duckyScript Command Instructions

All reference to **"stack"** refers to **Arithmetic Stack**. Unless noted otherwise.

|   Opcode<br>Name  | Byte 0<br>Decimal | Byte 0<br>Hex |                                                                                        Comment                                                                                       |   Byte 1  |   Byte 2  |
|:-------:|:---:|:----:|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|:---------:|:---------:|
|  DELAY  |  27 | 0x1b |                                                             Pop one item off top of stack<br>Delay the amount in milliseconds                                                            |           |           |
|   KUP   |  29 | 0x1d |                                                                                      Release Key                                                                                     |  KEYCODE  |  KEYTYPE  |
|  KDOWN  |  31 | 0x1f |                                                                                       Press Key                                                                                      |  KEYCODE  |  KEYTYPE  |
|   SWCF  |  32 | 0x20 |                                         Switch Color Fill<br>Pop THREE items off top of stack:<br>Red, Green, Blue<br>Set ALL LED color to the RGB value                                         |           |           |
|   MSCL  |  33 | 0x21 |                                                                                     Mouse Scroll                                                                                     |   Amount  |           |
|   MMOV  |  34 | 0x22 |                                                                                      Mouse Move                                                                                      |     X     |     Y     |
|   SWCC  |  35 | 0x23 |                         Switch Color Change<br>Pop FOUR items off top of stack:<br>N, Red, Green, Blue<br>Set N-th switch to the RGB value<br>If N is 0, set current switch.                         |           |           |
|   SWCR  |  36 | 0x24 | Switch Color Reset<br>Pop one item off top of stack<br>If value is 0, reset color of current key<br>If value is between 1 and 15, reset color of that key<br>If value is 99, reset color of all keys |           |           |
|   STR   |  38 | 0x26 |                                                                          Print zero-terminated string at ADDR                                                                         |  ADDR_LSB |  ADDR_MSB |
|  STRLN  |  39 | 0x27 |                                                                          Same as above, presses ENTER at end                                                                         |           |           |
|   EMUK  |  40 | 0x28 |                                                                                     EMUK command                                                                                     |  KEYCODE  |  KEYTYPE  |
|   OLC   |  42 | 0x2a |                                                      OLED_CURSOR<br>Pop TWO items off top of stack:<br>X and Y<br>Change OLED cursor to that                                                     |           |           |
|   OLP   |  43 | 0x2b |                                                                      Print zero-terminated string at ADDR to OLED                                                                     |  ADDR_LSB |  ADDR_MSB |
|   OLU   |  44 | 0x2c |                                                                                      OLED_UPDATE                                                                                     |           |           |
|   OLB   |  45 | 0x2d |                                                                                      OLED_CLEAR                                                                                      |           |           |
|   OLR   |  46 | 0x2e |                                                                                     OLED_RESTORE                                                                                     |           |           |
|   BCLR  |  47 | 0x2f |                                                                              Clears button status buffer                                                                             |           |           |
|  PREVP  |  48 | 0x30 |                                                                                   Previous profile                                                                                   |           |           |
|  NEXTP  |  49 | 0x31 |                                                                                     Next profile                                                                                     |           |           |
|  GOTOP  |  50 | 0x32 |                                                               Pop one item off top of stack<br>Go to profile of that value                                                               |           |           |
|  SLEEP  |  51 | 0x33 |                                                                       Put duckyPad to sleep<br>Terminates execution                                                                      |           |           |

## Project Pages

[Main page](README.md)

[Getting Started Guide](getting_started.md)

[Kit Assembly Guide](kit_assembly_guide.md)

[Using duckyScript](duckyscript_info.md)

[Common issues / Troubleshooting](troubleshooting.md)

[Firmware Updates and Version History](firmware_updates_and_version_history.md)

[Make Your Own Keymap](./keymap_instructions.md)

[Manual Setup](./manual_setup.md)

[Build a duckyPad Yourself](build_it_yourself.md)

[Kickstarter Backer Acknowledgments](kickstarter_backers.md)
