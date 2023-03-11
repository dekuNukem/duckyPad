# Writing duckyScript

[Get duckyPad](https://www.tindie.com/products/21984/) | [Official Discord](https://discord.gg/4sJCBx5) | [Getting Started](getting_started.md) | [Table of Contents](#table-of-contents)

------

## Overview

duckyScript is a simple scripting language for automating keyboard inputs.

It was originally developed for security research with [USB Rubber Ducky](https://shop.hak5.org/products/usb-rubber-ducky-deluxe).

However, [duckyPad uses it too](https://www.tindie.com/products/dekunukem/duckypad-do-it-all-mechanical-macropad/) to provide powerful macro scripting instead.

![Alt text](resources/pics/topdown.jpg)


**duckyScript 3** with much improved capability is currently under [public beta testing](duckyscript3_beta_test.md).

## Examples

duckyScript is very straightforward to write, you basically just tell it what key to press!

Some quick examples:

### Open Task Manager

```
CONTROL SHIFT ESC
```

### Open a webpage on windows

```
WINDOWS r
DELAY 500
STRING https://www.youtube.com/watch?v=dQw4w9WgXcQ
ENTER
```

### Save a webpage then close it

```
CONTROL s
DELAY 750
ENTER
DELAY 750
CONTROL w
```

## List of Commands

[REM](#REM)

[DEFAULTDELAY](#DEFAULTDELAY)

[DEFAULTCHARDELAY](#DEFAULTCHARDELAY)

[DELAY](#DELAY)

[STRING](#STRING)

[STRINGLN](#STRINGLN)

[REPEAT](#REPEAT)

[Special Keys](#Special-Keys)

[Mouse Buttons](#Mouse-Buttons)

[MOUSE_MOVE](#MOUSE_MOVE-X-Y)

[MOUSE_WHEEL](#MOUSE_WHEEL-X)

[KEYDOWN / KEYUP](#KEYDOWN--KEYUP)

[SWCOLOR](#SWCOLOR)

[DP_SLEEP](#DP_SLEEP)

[PREV_PROFILE / NEXT_PROFILE](#PREV_PROFILE--NEXT_PROFILE)

[GOTO_PROFILE](#GOTO_PROFILE)

[EMUK](#emuk)

[LOOP](#LOOP)

[duckyScript 3 Beta Test](#duckyscript-3)

-----

### REM

`REM` is comment, any line starting with `REM` is ignored.

### DEFAULTDELAY

`DEFAULTDELAY` specifies how long (in milliseconds) to wait between **`each line of command`**.

If unspecified, `DEFAULTDELAY` is 18ms.

```
DEFAULTDELAY 100

REM duckyPad will wait 100ms between each subsequent command
```

### DEFAULTCHARDELAY

`DEFAULTCHARDELAY` specifies how long (in milliseconds) to wait between each **`key stroke`**.

If unspecified, `DEFAULTCHARDELAY` is 18ms.

```
DEFAULTCHARDELAY 50

REM duckyPad will wait 50ms between each key stroke
```

### DEFAULTCHARDELAYFUZZ X

Adds an **additional** random delay from 0 to X milliseconds after `each key stroke`, can be used to make typing more human-like.

Set to 0 to disable.

### DELAY

`DELAY` creates a pause in script execution. Useful for waiting for UI to catch up.

```
DELAY 1000

REM waits 1000 milliseconds, or 1 second
```

### STRING

`STRING` types out whatever after it **`as-is`**.

```
STRING Hello world!!!

REM types out "Hello world!!!"
```

**Max `256` characters per line!** Split into multiple lines if longer.

### STRINGLN

Same as above, but presses **enter key** at the end.

Available after [firmware 0.20.2](https://github.com/dekuNukem/duckyPad/blob/master/firmware_updates_and_version_history.md).

### REPEAT

Repeats the last command **`n`** times.

```
STRING Hello world
REPEAT 10

REM types out "Hello world" 11 times (1 original + 10 repeats)
```

### Special Keys

duckyScript also supports many special keys:

```
CTRL / RCTRL
SHIFT / RSHIFT
ALT / RALT
WINDOWS / RWINDOWS
COMMAND / RCOMMAND (mac)
OPTION / ROPTION (mac)
ESC
ENTER
UP
DOWN
LEFT
RIGHT
SPACE
BACKSPACE
TAB
CAPSLOCK
PRINTSCREEN
SCROLLLOCK
PAUSE
BREAK
INSERT
HOME
PAGEUP
PAGEDOWN
DELETE
END
MENU
POWER

F1 to F24

(media keys)
MK_VOLUP
MK_VOLDOWN
MK_MUTE
MK_PREV
MK_NEXT
MK_PP (play/pause)
MK_STOP

(numpad keys)
NUMLOCK
KP_SLASH
KP_ASTERISK
KP_MINUS
KP_PLUS
KP_ENTER
KP_0 to KP_9
KP_DOT
KP_EQUAL

(Japanese input method keys)
(Available after firmware 0.20.4)
ZENKAKUHANKAKU
HENKAN
MUHENKAN
KATAKANAHIRAGANA
```

Those special keys can be used on their own:

`WINDOWS`

...or can be combined with a character to form shortcuts:

`WINDOWS s`

...or can be combined with other special keys:

`WINDOWS SHIFT s`

------

* Type the key names as-is in **`ALL CAPS`**.

* **`UP TO 6 KEYS`** can be pressed simultaneously.

### Mouse Buttons

* `LMOUSE`: Click LEFT mouse button

* `RMOUSE`: Click RIGHT mouse button

* `MMOUSE`: Click MIDDLE mouse button

### MOUSE_MOVE X Y

Move mouse cursor `X` pixels horizontally, and `Y` pixels vertically.

* `X` and `Y` must be between `-127` and `127`

* For `X`, a positive number moves RIGHT, negative number moves LEFT.

* For `Y`, a positive number moves UP, negative number moves DOWN.

* Set `X` or `Y` to 0 if no movement needed.

* Use `REPEAT` command to move further.

### MOUSE_WHEEL X

Scroll mouse wheel `X` lines.

* `X` must be between `-127` and `127`

* A positive number scrolls UP, negative number scrolls DOWN.

### KEYDOWN / KEYUP

Holds / Releases a key.

Can be character or special key.

Example:

```
KEYDOWN SHIFT
STRING hello
KEYUP SHIFT
```

### SWCOLOR

You can change the color of a key on-the-fly with `SWCOLOR` command.

To change the color of the **current key**, use `SWCOLOR R G B`:

`SWCOLOR 128 0 0`

To change the color of a **particular key**, use `SWCOLOR_n R G B`:

`SWCOLOR_1 0 128 0`

* `n` is between 1 and 15 inclusive.

* `R G B` is between `0` and `255` inclusive. 0 off, 255 brightest.

### DP_SLEEP

You can use `DP_SLEEP` command to make duckyPad go to sleep.

Backlight and screen are turned off.

Press any key to wake it up.

### PREV_PROFILE / NEXT_PROFILE

You can use `PREV_PROFILE` or `NEXT_PROFILE` command to switch to the previous / next profile.

### GOTO_PROFILE

Use this command to jump to a particular profile.

`GOTO_PROFILE 3` // jump to profile #3

### EMUK

(Known as `HOLD` **before firmware 0.20.3**)

Emulates a regular key.

It holds a key when you press it, and release only when you release it. Makes it behave more like a traditional keyboard.

Possible uses include push-to-talk voice chat, or WASD gamepad.

You should only use `EMUK` command **on its own**, i.e. the script should only have a single line of `EMUK` command and **nothing else**.

`EMUK` command can be followed by up to 2 keys, they can be character or special keys.

```
EMUK w
```
```
EMUK SHIFT
```

### LOOP

This command allows you to **assign different actions to the same key**.

You can use it to toggle / cycle through several actions like this:

```
LOOP0:
STRING first action
ENTER

LOOP1:
STRING second action
ENTER

LOOP2:
STRING third action
ENTER
```

* Up to 10 actions are supported (LOOP0 - LOOP9).

* When you press a key, a counter increments, and the script at the corresponding loop is executed.

* Loop and color state should persist through profile switches and reboots [**AFTER FIRMWARE 0.20.0**](firmware_updates_and_version_history.md).

* This command is not supported in test-run on PC app.

### LCR (Loop Counter Reset)

* This command resets the counter for `LOOP` commands.

* `LCR` on its own resets **ALL** loop counters in the **CURRENT PROFILE** to **ZERO**. Effectively making everything start from `LOOP0` again.

* `LCR x` resets key `x` loop counter to ZERO, making that key start from `LOOP0` again. `x` is between 1 to 15.

* `LCR x y` resets key `x` loop counter to `y`, making that key start from `LOOP`y again. `x` is between 1 to 15. `y` is between 0 to 9.

* Try not to mix `LOOP` and `LCR` commands in the same script, probably won't end well.

## duckyScript 3!

duckyScript 3 was released by [Hak5](https://docs.hak5.org/hak5-usb-rubber-ducky/) in late 2022 with **vastly improved capabilities**.

It is now much closer to a **general-purpose language**. New features include:

* Variables, IF statements, WHILE loops, functions, and more.

* Print to OLED screen

* Read button status

It is currently under **public beta test**. [Click me for instructions!](duckyscript3_beta_test.md)

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

## Questions or Comments?

Please feel free to [open an issue](https://github.com/dekuNukem/duckypad/issues), ask in the [official duckyPad discord](https://discord.gg/4sJCBx5), DM me on discord `dekuNukem#6998`, or email `dekuNukem`@`gmail`.`com` for inquires.