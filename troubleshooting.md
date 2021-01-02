# Common Issues / Troubleshooting

[Official Discord](https://discord.gg/4sJCBx5) | [Pre-order on Tindie!](https://www.tindie.com/products/21984/) | [Getting Started](getting_started.md) | [Table of Contents](#table-of-contents)

----

A couple of commonly encountered issues are explained here.

Click a question below to jump to the answer!

* [I press a key and nothing happens!](#I-press-a-key-and-nothing-happens)

* [The letters and symbols come out wrong!](#The-letters-and-symbols-come-out-wrong)

* [My macro script behaves inconsistently!](#My-macro-script-behaves-inconsistently)

* [My duckyPad seems to be getting sluggish!](#My-duckyPad-seems-to-be-getting-sluggish)

* [The RGB backlight is too bright!](#The-RGB-backlight-is-too-bright)

* [Will the OLED suffer burn-in?](#Will-the-OLED-suffer-burn-in)

* [Numpad keys doesn't work!](#Numpad-keys-doesnt-work)

* [In-app test-run doesn't work!](#In-app-test-run-doesnt-work)

* [SD Card doesn't work on mac!](#SD-Card-doesnt-work-on-mac)

----

## I press a key and nothing happens!

* Make sure the key has a valid script mapped to it.

* If in doubt, try one of the sample profiles.

* If it still doesn't work, take off the switch and inspect the pins, chances are one of them is bent:

![Alt text](resources/pics/bent.jpg)

* If so, straighten it up with a pair of pliers, then insert again. Make sure the **pins are straight** and aim for hot-swap socket holes.

## The letters and symbols come out wrong!

You'll need select a different keyboard layout.

* **`HOLD DOWN TOP LEFT KEY`** while **`plugging it in`**.

* Select a keyboard layout, use `+ / -` button to switch pages.

* Read more about [adding keyboard layouts](getting_started.md#keyboard-layouts) and [writing keymaps](keymap_instructions.md).

## My macro script behaves inconsistently!

It might be duckyPad is typing too fast for your computer.

You can slow it down by using `DELAY` commands.

* `DELAY n` adds an arbitrary delay in milliseconds.

* `DEFAULTDELAY n` sets the amount of time to wait between **`each line`** of command.

* `DEFAULTCHARDELAY n` sets the amount of time to wait between **`each keystroke`**.

## My duckyPad seems to be getting sluggish!

If you find duckyPad is getting slower at starting up and switching profiles, it's usually because of the SD card fragmentation.

* Make a backup of the files on SD card.

* Format it in [FAT32 or FAT](resources/pics/format.PNG).

* Copy the files back, it should be much faster now.

You can also use your own faster SD card.

## The RGB backlight is too bright!

* You can adjust the brightness by **holding down `-` button**.

## Will the OLED suffer burn-in?

Measures have been taken to prevent OLED burn-in.

* The pixels shifts around slightly every 2 minutes.

* The screen dims after 5 minutes.

* duckyPad goes to sleep after 30 minutes ([adjustable](getting_started.md#settings--updates)).

## Numpad keys doesn't work!

Make sure your `NUM LOCK` is on.

## In-app test-run doesn't work!

The in-app test-run has some limitations.

* Test-run can't get past [Windows User Account Control](resources/pics/app/uac.png) screens. You'll have to run it on real thing.

* On macOS, if test-run doesn't work, [go to System Preferences -> Security & Privacy -> Accessibility](resources/pics/app/macos.png), unlock, remove then add the app.

* Test-run might not be 100% accurate, some keys and commands like `Capslock`, `Numlock`, `LOOP`, `SWCOLOR` can't be emulated in software. Run your scripts on the real thing if you're not sure.

## SD Card doesn't work on mac!

If macOS doesn't like your SD card, you can reformat it in FAT/FAT32 in macOS itself, then it should work.

Click `Initialize...`:

![Alt text](resources/pics/init.png)

In `Disk Utility`, select `Erase`, then `MS-DOS (FAT)`:

![Alt text](resources/pics/macfat.png)

Now the card should be readable in both macOS and duckyPad.

If you want, you can copy the [sample profiles](https://github.com/dekuNukem/duckyPad/raw/master/sample_profiles.zip) back to the root of the SD card.

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

## Questions or Comments?

Please feel free to [open an issue](https://github.com/dekuNukem/duckypad/issues), ask in the [official duckyPad discord](https://discord.gg/4sJCBx5), DM me on discord `dekuNukem#6998`, or email `dekuNukem`@`gmail`.`com` for inquires.
