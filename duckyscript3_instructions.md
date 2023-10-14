# duckyScript 3 Advanced Features

[Get duckyPad](https://www.tindie.com/products/21984/) | [Official Discord](https://discord.gg/4sJCBx5) | [Getting Started](getting_started.md) | [Table of Contents](#table-of-contents)

----

duckyScript 3 support was added on [firmware version 1.2.0](firmware_updates_and_version_history.md)!

It gives duckyPad a big boost in capability. Highlights include:

* Variables, `IF` statements, `WHILE` loops, functions, and more!

* Print to OLED screen

* Read button status

* Many optimisations and bug fixes

![Alt text](resources/pics/ds3.png)

This guide is a bit long, but should cover everything you need. So please take some time to read through!

## What's New

* Variables, Math, & Logical Operators

* `IF` Statements and `WHILE` Loops 

* Functions

* **Printing to OLED screen** and **reading button status**

* Improved performance

* More detailed syntax check error reports

* Profile bulk import

* General cleanup and bug fixes

## Instructions

* [Follow this guide](firmware_updates_and_version_history.md#usb-firmware-updates) to update duckyPad to latest firmware (at least V1.2.0)

* [Download the latest](https://github.com/dekuNukem/duckyPad/releases/latest) configurator software.

* Launch and use it as usual!

* [Try this script](https://raw.githubusercontent.com/dekuNukem/duckyPad/master/resources/duckyscirpt_3_test_script.txt) for a demo of new features. Copy everything as-is into the configurator.

* Read the **rest of this guide** and try out the new commands yourself!

* Loading and saving might take a bit longer at first, but should be faster afterwards.

* Any issues or questions? Feel free to [open an issue](https://github.com/dekuNukem/duckypad/issues), ask in [Discord](https://discord.gg/4sJCBx5), or email `dekuNukem`@`gmail`.`com`

## Table of Contents

- [C-style comments](#c-style-comments)

- [Constants](#constants)

- [Variables](#variables)

- [Operators](#operators)

- [Variables as Arguments](#variables-as-arguments)

- [Conditional Statements](#conditional-statements)

- [Loops](#loops)

- [Functions](#functions)

- [OLED Commands](#oled-commands)

- [Reserved Variables](#reserved-variables)

- [Reading Buttons](#reading-buttons)

- [Randomisation](#randomisation)

- [Instruction Set Details](#instruction-set-details)

- [Questions or Comments?](#questions-or-comments)

## New Commands

First of all, [existing duckyScript rules](duckyscript_info.md) still apply, so take a look if you need a refresher.

## C-style comments

In addition to `REM`, You can now also use `//` at the beginning of a line for comments.

## `HALT`

You can use `HALT` command to stop execution at anytime

## Constants

You can use `DEFINE` to, well, define a constant.

It can be either **integer** or **string**.

The content is **replaced AS-IS** during pre-processing, very much like `#define` in C.

```
DEFINE MY_EMAIL example@gmail.com
DEFINE MY_AGE 69

STRING My email is MY_EMAIL!
STRING I'm MY_AGE years old! 
```

Internally, `TRUE` is `1`, and `FALSE` is `0`.

## Variables

You can declare a variable using `VAR` command:

```
// Declaration
VAR $spam = 0
VAR $eggs = 10

// Assignment
$spam = 20
```

Variables must start with dollar sign `$`.

Variables are **unsigned 16-bit integers**, and can hold values from **0 to 65535**.

All variables have **global scope**, and can be referenced anywhere in the script.

They can be printed with `STRING`, `STRINGLN`, and (spoiler alert!)`OLED_PRINT` commands.

```
STRING The value is: $spam
```

## Operators

You can perform operations on constants and variables.

### Mathematics

| Operator |   Name  |
|:--------:|:----------:|
|     =    | Assignment |
|     +    |     Add    |
|     -    |  Subtract  |
|     *    |  Multiply  |
|     /    |   Divide   |
|     %    |   Modulus  |
|     ^    |  Exponent  |

Example:

```
$spam = 2+3
$spam = $eggs * 10
```

### Comparison

| Operator |        Name        |
|:--------:|:---------------------:|
|    ==    |         Equal         |
|    !=    |       Not equal       |
|     >    |      Greater than     |
|     <    |       Less than       |
|    >=    | Greater than or equal |
|    <=    |   Less than or equal  |

All comparisons evaluate to **either 0 or 1**.

### Logical 

| Operator |          Name         | Comment                                                |
|:--------:|:---------------------:|--------------------------------------------------------|
|    &&    |      Logical AND      | Evaluates to 1 if BOTH side are non-zero, otherwise 0. |
|   \|\|   |       Logical OR      | Evaluates to 1 if ANY side is non-zero, otherwise 0.   |

### Bitwise

| Operator |          Name         |
|:--------:|:---------------------:|
|     &    |      Bitwise AND      |
|    \|    |       Bitwise OR      |
|    <<    |       Left Shift      |
|    >>    |      Right Shift      |

## Variables as Arguments

You can now also use an **expression or variable** in `DELAY` and `GOTO_PROFILE` command:

```
VAR $amount = 100

DELAY $amount
DELAY $amount * 2 + 5
```

## Conditional Statements

`IF` statement is used to conditionally execute code.

At simplest, it involves `IF`, `THEN`, and `END_IF`:

```
IF expression THEN
	code to execute
END_IF
```

The code inside is executed if the **expression evaluates to non-zero**.

Indent doesn't matter, feel free to add them for a cleaner look.

----

You can use `ELSE IF` and `ELSE` for additional checks.

If the first `IF` evaluate to 0, `ELSE IF`s are checked and executed if condition is met.

If none of the conditions are met, then code inside `ELSE` is executed.

```
VAR $spam = 5

IF $spam == 0 THEN
	STRING spam is zero!
ELSE IF $spam == 1 THEN
	STRING spam is one!
ELSE
	STRING spam is none of those!
END_IF
```

## Loops

You can use `WHILE` loops to repeat instructions until a certain condition is met.

Syntax:

```
WHILE expression
	code to repeat
END_WHILE
```

If `expression` evaluates to zero, the code is skipped. Otherwise the code inside is repeated.

This simple example loops 3 times.

```
VAR $i = 0
WHILE $i < 3
	STRINGLN Counter is $i!
	$i = $i + 1
END_WHILE
```

```
Counter is 0!
Counter is 1!
Counter is 2!
```

### `LBREAK`

Use `LBREAK` to **exit a loop** immediately.

```
VAR $i = 0
WHILE TRUE
	STRINGLN Counter is $i!
	$i = $i + 1

	IF $i == 3 THEN
		LBREAK
	END_IF
END_WHILE
```
```
Counter is 0!
Counter is 1!
Counter is 2!
```

### `CONTINUE`

Use `CONTINUE` to **jump to the start of loop** immediately.

```
VAR $i = 0
WHILE $i < 5
	$i = $i + 1

	IF $i == 3 THEN
		CONTINUE
	END_IF

	STRINGLN Counter is $i!
END_WHILE
```

Here when `$count` is 3, it skips printing and start from beginning instead.

```
Counter is 1!
Counter is 2!
Counter is 4!
Counter is 5!
```

To exit an infinite loop, you can [check button status](#reading-buttons), or just press the RESET button:

![Alt text](resources/pics/dfu_buttons_new.jpg)

## Functions

Functions let you run a block of code efficiently instead of copy pasting.

Syntax:

```
FUNCTION func_name()
	code
END_FUNCTION
```

You can use **`RETURN`** to exit a function early.

Arguments and return values are NOT supported, use global variables instead.

Simple example:

```
FUNCTION print_info()
	STRING My email is example@gmail.com
	STRING I'm 69 years old!
END_FUNCTION

// call it
print_info()
```

No recursion! Don't even think about it!

## OLED Commands

### `OLED_CURSOR x y`

Set where to print on screen.

`x` and `y` are coordinates in pixels. The characters prints from **top-left** corner.

`x` can be constants or variables between `0` and `127`

`y` can be constants or variables between `0` and `63`

### `OLED_PRINT`

`OLED_PRINT hello world!` 

Prints the message into display buffer at current cursor location.

### `OLED_CLEAR`

Clears the display buffer.

### `OLED_UPDATE`

Actually update the OLED.

You should use `OLED_CLEAR`, `OLED_CURSOR`, and `OLED_PRINT` to set up the display, then use this to print it.

This is much faster than updating the whole screen for every change.

### `OLED_RESTORE`

Restore the default profile/key name display. `OLED_UPDATE` **NOT NEEDED**.

## Reserved Variables

There are a few **reserved variables** that are always available.

You can read or write (RW) to them to adjust settings. Some are read-only (RO).

#### `$_DEFAULTDELAY` (RW)

#### `$_DEFAULTCHARDELAY` (RW)

#### `$_CHARJITTER` (RW)

Write to those variables to change the settings.

#### `$_RANDOM_MIN` (RW)

Lower bound of random number generator (RNG).

#### `$_RANDOM_MAX` (RW)

Upper bound of RNG.

#### `$_RANDOM_INT` (RW)

Get a random number between the upper and lower bound.

#### `$_TIME_S` (RO)

Get current time in **seconds**.

#### `$_TIME_MS` (RO)

Get current time in **milliseconds**.

#### `$_READKEY` (RO)

Get the key that is being pressed.

Returns 0 if no key is pressed. 1 to 17 otherwise.

#### `$_KEYPRESS_COUNT` (RO)

Get how many times the current key has been pressed.

## Reading Buttons

Reading the reserved variable `$_READKEY` returns the currently pressed key.

The value is 0 if no key is pressed. 1 to 17 otherwise.

Normally you would read this variable in a loop until it is non-zero:

```
VAR $k = 0

WHILE $k == 0
    $k = $_READKEY
    DELAY 50
END_WHILE

STRINGLN I pressed key $k!
```

If reading button multiple times, use `BCLR` command to clear the button status buffer.

This way it won't trigger again right away.

## Randomisation

Read from `$_RANDOM_INT` to get a random number.

By default, it is between 0 and 65535.

You can change the upper and lower bounds (inclusive) by writing to `_RANDOM_MAX` and `_RANDOM_MIN`.

```
VAR $i = 0
$_RANDOM_MIN = 0
$_RANDOM_MAX = 100

WHILE $i < 5
    STRINGLN Random number: $_RANDOM_INT
    $i = $i + 1
END_WHILE
```

## Instruction Set Details

[Check this out](duckyscript3_bytecode_vm.md) if you want to learn how the new bytecode VM works!

## Questions or Comments?

Please feel free to [open an issue](https://github.com/dekuNukem/duckypad/issues), ask in [official discord](https://discord.gg/4sJCBx5), DM me on discord `dekuNukem#6998`, or email `dekuNukem`@`gmail`.`com` for inquires.

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
