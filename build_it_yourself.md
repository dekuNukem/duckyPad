# Make a duckyPad Yourself!

[Get duckyPad](https://www.tindie.com/products/21984/) | [Official Discord](https://discord.gg/4sJCBx5) | [Getting Started](getting_started.md) | [Table of Contents](#table-of-contents)

------

## Beware of duckypad Versions!

This guide is for [duckyPad V2](pcb/V2), BOM is [here](pcb/V2/duckypad_v2_bom.xlsx). A video with build process is [here](https://www.youtube.com/watch?v=EGLLCtRuEuM).

A newer version with kailh hot-swap sockets and reverse-mount RGB LED is currently under prototype testing. It features a few different components and proper front and back acrylic plates. More information will be added once testings are done. Files for this new version [is here](pcb), BOM is [here](pcb/V3_rev3/duckypad_V3_rev3_bom.xlsx).

Here are some things to keep in mind if you're making one yourself:

## Circuit board

The board is a simple 2-layer board. Don't forget the cutout for the OLED screen in the milling layer!

![Alt text](resources/pics/pcb_front.jpg)

![Alt text](resources/pics/pcb_back.jpg)

Getting a stencil is highly recommended, because the RGB LED used here WILL melt if you try to hand-solder. It's best to use a stencil and then reflow it in an oven.

Speaking of which...

## RGB LED

The RGB LED is the trusty WS2812 that's thrown into every single RGB project these days, except it isn't. What I used is SK6812, a compatible clone with [allegedly better performance](https://hackaday.com/2017/01/25/ws2812b-led-clones-work-better-than-originals/). It is also the smaller 3535 package, not the regular 5050 kind, so make sure to get the right part.

![Alt text](resources/pics/led.jpg)

I haven't tested WS2812 with the design, it should work on paper, but I suggest just get some 3535 SK6812 and be done with it.

**Careful with the orientation while placing them!** Look at the PCB photo above for details.

## Switches

In theory you can use any cherry MX style switches you like, however, because there are backlights underneath, it's best if you can find one that lets the light shine through.

I used Gateron RGB switches, they are perfectly designed for this kind of usage, translucent case with a large opening for LED. I used greens but there are other colors to choose from as well.

![Alt text](resources/pics/gateron.jpg)

A switch plate is strongly recommended, to improve stability and even out the backlight. I used a laser-cut white 2mm acrylic plate.

![Alt text](resources/pics/plate.jpg)

## OLED

I used a 1.3 inch OLED display from aliexpress. Make sure the connector is `30pin FPC with 0.5mm pin spacing`.

Note that the FPC connector used here is upper-contact variant. Here are some sample listings (illustration purpose only, you don't have to buy from them):

https://www.aliexpress.com/item/32950730252.html

https://www.aliexpress.com/item/4000350633981.html

Datasheet:

![Alt text](resources/pics/oled.jpg)

## Keycaps

I bought some blank R4 keycaps, just make sure they don't block the backlight.

## USB Type-C connector

Search `USB 16p` on aliexpress for the connector. Should look like this:

![Alt text](resources/pics/usbc.png)

Datasheet:

![Alt text](resources/pics/usbc_datasheet.jpg)

## Tactile Buttons

Try searching `3X4X2.5MM White Tactile Button` on aliexpress.

Should look like this:

![Alt text](resources/pics/butt.png)

## Misc

To prevent OLED ribbon cable snagging on things, tape it down to the back of the circuit board.

Remember to make sure the entire board is working before you solder the switches in place, otherwise it would be almost impossible to get them off again.

## Technical notes

The microcontroller used here is a STM32F072C8T6. It costs less than a dollar, has more peripherals, pins, and is faster than any old Aduinos. ST provides a free Keil MDK license for all F0 parts, and there are also completely open source toolchains like arm-gcc.

One interesting design detail is the RGB LED. The WS2812(and its clones) requires a rather high data rate, and Arduino library achieve this by bitbanging in assembly. However the ARM processor in STM32 have some funky pipeline and caches, making asm timing somewhat unreliable. As a result I used SPI for LED control. By selecting the right speed and the right data on the MISO line, you can have the waveform look exactly like what WS2812 requires. In this case sending 0xf8 at 8MHz is bit 1, and sending 0xc0 results in a bit 0. Since SPI is also used by SD card, an AND gate is added to insulate the LED data line when SD card is active.

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