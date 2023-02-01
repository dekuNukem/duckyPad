# duckyScript 3 Public Beta Test

[Get duckyPad](https://www.tindie.com/products/21984/) | [Official Discord](https://discord.gg/4sJCBx5) | [Getting Started](getting_started.md) | [Table of Contents](#table-of-contents)

----

# UNDER CONSTRUCTION!!!!!!1 information might not be complete or accurate until its done!!!!!1234567890

Welcome to **duckyScript 3** beta test!

DS3 was released by [Hak5](https://docs.hak5.org/hak5-usb-rubber-ducky/) in late 2022, with new features and improved capabilities.

Major work was done to implement it on duckyPad, but I'm glad to announce it's (mostly) finished!

## What's New

duckyScript 3 now behaves a lot more like a **general-purpose language**, with:

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

* More detailed code check error reports

* General cleanup and bug fixes

## Limitations

### Bugs!

This is a huge update, so expect bugs, especially at this stage.

If you run into any, feel free to [open an issue](https://github.com/dekuNukem/duckypad/issues), ask in [Discord](https://discord.gg/4sJCBx5), or email `dekuNukem`@`gmail`.`com` @

### Bytecode Compiler

With DS3 now very much a general-purpose language, it takes a lot more work to process it properly. Examples include:

* Expression evaluation via abstract syntax tree (AST)

* Flow control with jump tables

* Arithmetic and call stacks

This is much too complicated to do on-device, therefore, DS3 is now compiled into **bytecode**, and executed on a **virtual stack machine**.

The configurator takes care of everything for you, so it's business as usual.

However, if you want to manually edit the SD card, it is now a bit more involved, as you'll need to compile the script and copy over the binary too.

See below for instructions, and in the meantime I'll try to keep supporting the old firmware version in the foreseeable future.

## Instructions

Download new firmware and new configurator

suggested to fresh format the SD card

## Variables

## Operators

## Conditional Statements 

## Loops

## LED Colour

## OLED Print

## LOOP info

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
