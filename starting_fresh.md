# Starting Fresh with duckyPad (2020)

[Get duckyPad](https://dekunukem.github.io/duckyPad-Pro/doc/landing.html) | [Official Discord](https://discord.gg/4sJCBx5) | [Getting Started](getting_started.md) | [Table of Contents](#table-of-contents)

----

Since its launch in 2020, the original duckyPad has seen several major software and firmware updates.

This guide shows how to do a **clean upgrade** to the latest version.

Ideal if you haven’t used it in a while and want to catch up on new features.

## Check Your Duck

This guide is for the **original duckyPad**

* Released in 2020
* Has 15 Keys

## Back up SD Card

* Pop out the SD Card
* Mount on your computer with a card reader
* Copy everything off as-is

## Prepare SD Card

* Format the card in `FAT32`

![Alt text](resources/pics/format.PNG)

* Download the [sample profiles](https://github.com/dekuNukem/duckyPad/raw/master/sample_profiles.zip) 

* Unzip and copy everything to the **root of your SD card**:

![Alt text](resources/pics/sample_pf.png)

## FW / SW Updates

* [Follow this guide](firmware_updates_and_version_history) to update your duckyPad to latest firmware.

* [Also download the latest configurator](https://github.com/duckyPad/duckyPad-Configurator/releases/latest)

## Try It Out!

* Unmount the SD Card from PC

* Pop it back in duckyPad

* Follow [the latest user guide](https://dekunukem.github.io/duckyPad/getting_started.html) to start using duckyPad again!

## Restore Backup

* If it works with the default sample profiles, you can try to restore the backup.
* Pop out SD card, mount on PC.
* Do another format, copy the backups back.
* Eject, put card back in duckyPad.
* **Load then save again** using the latest configurator.
	* This updates the files on the card
* Try it out!

## Deprecated Commands

If syntax errors pops up, it's likely due to the following changes:

* `HOLD` and `EMUK` command are deprecated
	* Use `KEYDOWN` and `KEYUP` in `on_press` and `on_release` instead.

## How'd It Go?

* If it works, congrats!

* Still wonky? Feel free to ...

	* [Open an issue](https://github.com/dekuNukem/duckypad/issues)
	* Ask in the [official duckyPad discord](https://discord.gg/4sJCBx5)
	* Email `dekuNukem`@`gmail`.`com` .

----

## Table of Contents

[Main page](README.md)

[Getting Started Guide](getting_started.md)

[Kit Assembly Guide](kit_assembly_guide.md)

[Using duckyScript](duckyscript_info.md)

[Common issues / Troubleshooting](troubleshooting.md)

[Firmware Updates and Version History](firmware_updates_and_version_history.md)

[Make Your Own Keymap](./keymap_instructions.md)

[Build a duckyPad Yourself](build_it_yourself.md)

[Kickstarter Backer Acknowledgments](kickstarter_backers.md)
