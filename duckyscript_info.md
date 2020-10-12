# Writing duckyScript

[Table of Contents](#table-of-contents) | [Buy duckyPad](/purchase_information.md) | [Official Discord](https://discord.gg/4sJCBx5)

------

## Overview

duckyScript is a simple scripting language for automating keypresses. It was originally developed for [USB Rubber Ducky](https://shop.hak5.org/products/usb-rubber-ducky-deluxe).

This guide gives an concise overview of duckyScript. For more information, [see this page](https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript).

## Syntax

* One command per line. **`Each line has a 256 character limit!`**

* 1000 milliseconds = 1 second

### REM

`REM` is comment, any line starting with `REM` is ignored.

### DEFAULTDELAY

`DEFAULTDELAY` specifies how long (in milliseconds) to wait between each **`line of command`**.

If unspecified, `DEFAULTDELAY` is 20ms in duckyPad.

```
DEFAULTDELAY 100
REM duckyPad will wait 100ms between each subsequent command
```

### DEFAULTCHARDELAY

`DEFAULTCHARDELAY` specifies how long (in milliseconds) to wait between each **`key stroke`**.

If unspecified, `DEFAULTCHARDELAY` is 20ms in duckyPad.

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

### Modifier Keys

duckyScript also supports a whole bunch of modifier keys:

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
```

Those modifier keys can be used on their own:

`WINDOWS`

...or can be combined with a character to form shortcuts:

`WINDOWS s`

...or can be combined with other modifier keys:

`WINDOWS SHIFT s`

------

* Type the key names as-is in **`ALL CAPS`**.

* **`UP TO 3 KEYS`** can be pressed simultaneously.

* For macOS, use `WINDOWS` for `Command` key, use `ALT` for `Option` key.

## Examples

As you will see, those are very straightforward and simple.

Check out the [sample profiles](https://github.com/dekuNukem/duckyPad/tree/master/sample_profiles) for more examples.

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

## Table of Contents

[Main page](README.md)

[Buy a duckyPad](purchase_information.md)

[Getting Started Guide](getting_started.md)

[Kit Assembly Guide](kit_assembly_guide.md)

[Using duckyScript](duckyscript_info.md)

[Firmware Updates and Version History](firmware_updates_and_version_history.md)

[Build a duckyPad Yourself](build_it_yourself.md)

## Questions or Comments?

Please feel free to [open an issue](https://github.com/dekuNukem/duckypad/issues), ask in the [official duckyPad discord](https://discord.gg/4sJCBx5), DM me on discord `dekuNukem#6998`, or email `dekuNukem`@`gmail`.`com` for inquires.