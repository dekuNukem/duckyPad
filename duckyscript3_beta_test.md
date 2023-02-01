# duckyScript 3 Public Beta Test

[Get duckyPad](https://www.tindie.com/products/21984/) | [Official Discord](https://discord.gg/4sJCBx5) | [Getting Started](getting_started.md) | [Table of Contents](#table-of-contents)

----

# UNDER CONSTRUCTION!!!!!!1 

# Information might not be complete or accurate until its done!!!!

Welcome to **duckyScript 3** beta test!

DS3 was released by [Hak5](https://docs.hak5.org/hak5-usb-rubber-ducky/) in late 2022, with new features and improved capabilities.

It was quite a journey implementing it on duckyPad, but I'm glad to announce it's (mostly) finished!

## What's New

duckyScript 3 now behaves a lot like a **general-purpose language**, with:

* Variables

* Conditional Statements

* Loops 

* Functions

* Math & Logical Operators

As a result, you can now write much more elaborate scripts for your automation needs!

-----

* **New commands** for **printing to OLED screen** and **reading button status**

* **Backwards compatible** with existing scripts (or at least should!)

* Optimised code for faster performance

* More detailed syntax check error reports

* General cleanup and bug fixes

## Limitations

### Bugs!

This is a huge update, so expect bugs, especially at this stage.

If you run into any, feel free to [open an issue](https://github.com/dekuNukem/duckypad/issues), ask in [Discord](https://discord.gg/4sJCBx5), or email `dekuNukem`@`gmail`.`com`

### Missing Commands

As duckyPad is more about macro scripting than pentesting, I skipped some commands about payload management and data exfiltration.

### Bytecode Compiler

With DS3 now very much a general-purpose language, it takes a lot more work to process it properly.

Examples include:

* Expression evaluation via abstract syntax tree (AST)

* Flow control with jump tables

* Arithmetic and call stacks

This is much too complicated to do on-device, therefore, DS3 is now compiled into **bytecode**, and executed on a **virtual stack machine**.

The configurator takes care of everything for you, so it's business as usual.

However, if you want to manually edit the SD card, you'll need to compile the script and copy over the binary too.

## Instructions

Download new firmware and new configurator

suggested to fresh format the SD card

## New Commands

First of all, [existing duckyScript rules](duckyscript_info.md) still apply, so take a look if you need a refresher.

Also, feel free to check out [official documentation](https://docs.hak5.org/hak5-usb-rubber-ducky/) for more info.

### Constants

You can use `DEFINE` to set a constant.

It can be either **integer** or **string**.

The content is **replaced AS-IS** during pre-processing, very much like `#define` in C.

```
DEFINE MY_EMAIL example@gmail.com
DEFINE MY_AGE 69

STRING My email is MY_EMAIL!
STRING I'm MY_AGE years old! 
```

Internally, `TRUE` is defined as `1`, and `FALSE` is defined as `0`.

### Variables

You can declare a variable using `VAR` command:

```
// Declaration
$spam = 1
$eggs = 10

// Assignment
$spam = 20
```

Variables must start with dollar sign `$`.

Variables are **unsigned 16-bit integers**, and can hold values from **0 to 65535**.

All variables have a **global scope**, and can be referenced anywhere in the script.

They can be printed with `STRING`, `STRINGLN`, and `OLP` commands.

```
STRING The value is: $spam
```

### Operators

You can perform operations on constants and variables.

#### Mathematics

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

#### Comparison

| Operator |        Name        |
|:--------:|:---------------------:|
|    ==    |         Equal         |
|    !=    |       Not equal       |
|     >    |      Greater than     |
|     <    |       Less than       |
|    >=    | Greater than or equal |
|    <=    |   Less than or equal  |

All comparison operators evaluate to **either 0 or 1**.

So if we have:

```
$spam = 5
$eggs = 10
```

Then `$spam == $eggs` evaluates to 0, `$spam <= $eggs` evaluates to 1, and so on.

#### Logical 

| Operator |          Name         | Comment                                                |
|:--------:|:---------------------:|--------------------------------------------------------|
|    &&    |      Logical AND      | Evaluates to 1 if BOTH side are non-zero, otherwise 0. |
|   \|\|   |       Logical OR      | Evaluates to 1 if ANY side is non-zero, otherwise 0.   |

So if we have:

```
$spam = 5
$eggs = 0
```

Then `$spam && $eggs` evaluates to 0, `$spam || $eggs` evaluates to 1, and so on.

### Conditional Statements

#### `IF`

`IF` statement is used to conditionally execute code.

At simplest, it involves `IF expression THEN`, and `END_IF`:

```
VAR $spam = 5

IF $spam > 0 THEN
	STRING spam is non-zero!
END_IF
```

The code inside is executed if the **expression evaluates to non-zero**.

Indent doesn't matter, feel free to add them for a cleaner look.

#### `ELSE IF` and `ELSE`

You can use them for additional checks.

If the first `IF` evaluate to 0, `ELSE IF`s are checked and executed if condition is met.

If none of the conditions are met, then code inside `ELSE` is executed.

```
VAR $spam = 5

IF $spam == 0 THEN
	STRING spam is zero!
ELSE IF $spam == 2 THEN
	STRING spam is two!
ELSE
	STRING spam is none of those!
END_IF
```

### Loops

You can use `WHILE` loops to repeat instructions until a certain condition is met.

#### `WHILE`

Syntax:

```
WHILE expression
	code to repeat
END_WHILE
```

If `expression` evaluates to zero, the code is skipped. Otherwise the code inside is repeated.

Here is a simple example that loops 3 times.

```
VAR $count = 3

WHILE $count > 0
	STRINGLN Counter is $count!
	$count = $count - 1
END_WHILE
```

```
Counter is 3!
Counter is 2!
Counter is 1!
```

#### Infinite Loop

Use `WHILE 1` to run a block of code infinitely.

```
WHILE 1
	PRINTLN Hello!
END_WHILE
```

#### `BREAK`

Use `BREAK` to exit a loop immediately.

```
VAR $count = 0

WHILE 1
	STRINGLN Counter is $count!
	$count = $count + 1

	IF $count == 3 THEN
		BREAK
	END_IF

END_WHILE
```
```
Counter is 0!
Counter is 1!
Counter is 2!
```

#### `CONTINUE`

Use `CONTINUE` to jump to beginning of the loop immediately.

```
VAR $count = 3

WHILE $count > 0
	
	IF $count == 2 THEN
		CONTINUE
	END_IF

	STRINGLN Counter is $count!
	$count = $count - 1
END_WHILE
```

### LED Colour

### OLED Print

### LOOP info

## VM assembly information

## Get in Touch!

Questions, comments, feedbacks? Feel free to ask in the [Official Discord!](https://discord.gg/4sJCBx5)

And of course, there will be regular backer updates on Kickstarter once it's launched.

Media enquiries? Email `dekuNukem` `gmail`.`com`, or DM me on discord `dekuNukem#6998`.

## Table of Contents

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
