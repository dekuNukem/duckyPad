# Writing duckyScript

[Kickstarter](https://www.kickstarter.com/projects/dekunukem/duckypad-do-it-all-mechanical-macropad) ([Nov 4 launch details](kickstarter_info.md)) | [Official Discord](https://discord.gg/4sJCBx5) | [Table of Contents](#table-of-contents) 

------

## Overview

duckyScript is a simple scripting language for automating keypresses. It was originally developed for [USB Rubber Ducky](https://shop.hak5.org/products/usb-rubber-ducky-deluxe).

This guide gives an concise overview of duckyScript. For more information, [see this page](https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript).

## Examples

duckyScript is very easy and straightforward to write, let's take a look at some examples first. 

### Open Task Manager

```
CONTROL SHIFT ESC
```

### Open a webpage on windows

```
WINDOWS r
DELAY 400
STRING https://www.youtube.com/watch?v=dQw4w9WgXcQ
ENTER
```

### Save a webpage then close it

```
CONTROL s
DELAY 600
ENTER
DELAY 600
CONTROL w
```

------

* Check out the [sample profiles](https://github.com/dekuNukem/duckyPad/tree/master/sample_profiles) for more examples.

## Syntax

* One command per line. **`Each line has a 256 character limit!`**

* 1000 milliseconds = 1 second

### REM

`REM` is comment, any line starting with `REM` is ignored.

### DEFAULTDELAY

`DEFAULTDELAY` specifies how long (in milliseconds) to wait between each **`line of command`**.

If unspecified, `DEFAULTDELAY` is 18ms in duckyPad.

```
DEFAULTDELAY 100

REM duckyPad will wait 100ms between each subsequent command
```

### DEFAULTCHARDELAY

`DEFAULTCHARDELAY` specifies how long (in milliseconds) to wait between each **`key stroke`**.

If unspecified, `DEFAULTCHARDELAY` is 18ms in duckyPad.

```
DEFAULTCHARDELAY 50

REM duckyPad will wait 50ms between pressing each key
```

### DELAY

`DELAY` creates a momentary pause in script execution. Useful for waiting for UI to catch up.

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

### REPEAT

Repeats the last command **`n`** times.

```
STRING Hello world
REPEAT 10

REM types out "Hello world" 11 times (1 original + 10 repeats)
```

### Special Keys

duckyScript also supports a whole bunch of special keys:

```
CTRL
SHIFT
ALT
WINDOWS
GUI
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
F1
F2
F3
F4
F5
F6
F7
F8
F9
F10
F11
F12

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
KP_0
KP_1
KP_2
KP_3
KP_4
KP_5
KP_6
KP_7
KP_8
KP_9
KP_DOT
KP_EQUAL
```

Those special keys can be used on their own:

`WINDOWS`

...or can be combined with a character to form shortcuts:

`WINDOWS s`

...or can be combined with other special keys:

`WINDOWS SHIFT s`

------

* Type the key names as-is in **`ALL CAPS`**.

* **`UP TO 3 KEYS`** can be pressed simultaneously.

* For macOS, use `WINDOWS` for `Command` key, use `ALT` for `Option` key.

### HOLD

Holds a key when you press it, and release only when you release it.

`HOLD` command must be followed by a single key, it can be a character or a special key.

```
HOLD w
```
```
HOLD SHIFT
```

You should only use `HOLD` command on its own, i.e. the script should only have a single line of `HOLD` command and nothing else.

This command makes duckyPad behave more like a traditional keyboard.

You can probably use it as a WASD gamepad this way.

## Table of Contents

[Main page](README.md)

[Getting Started Guide](getting_started.md)

[Kit Assembly Guide](kit_assembly_guide.md)

[Using duckyScript](duckyscript_info.md)

[Firmware Updates and Version History](firmware_updates_and_version_history.md)

[Manual Setup](./manual_setup.md)

[Build a duckyPad Yourself](build_it_yourself.md)

## Questions or Comments?

Please feel free to [open an issue](https://github.com/dekuNukem/duckypad/issues), ask in the [official duckyPad discord](https://discord.gg/4sJCBx5), DM me on discord `dekuNukem#6998`, or email `dekuNukem`@`gmail`.`com` for inquires.