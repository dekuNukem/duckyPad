# Making Your Own Keymap

[Support on Kickstarter!](https://www.kickstarter.com/projects/dekunukem/duckypad-do-it-all-mechanical-macropad) | [Official Discord](https://discord.gg/4sJCBx5) | [Getting Started](getting_started.md) | [Table of Contents](#table-of-contents)

----

duckyPad now supports custom keymaps!

You (or other people) can create a keymap file and load it on the SD card for duckyPad to use.

---

This guide describes how keymap works, and how to write your own.

Note that I'm no expert on keyboard technicalities, I barely got this thing to work! So [feel free to correct me](#questions-or-comments) if you spot anything wrong.

## (Short Story of) HID Usage ID

When you press a key, you keyboard sends a **`USB HID Usage ID`** to the computer, so it knows which key is pressed.

Each key has a unique HID Usage ID, you can [find a list here](https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2).

duckyPad works by acting as a regular USB keyboard, and sending HID Usage IDs to simulate keypresses.

## Keyboard Layouts

The kicker is, HID Usage ID corresponds to the `PHYSICAL LOCATION` of a key on the keyboard, NOT what it says on the key itself!

For example, here is a `ANSI QWERTY keyboard (US)`, very popular in US and other parts of the world.

* The red number is the HID Usage ID (in hexadecimal) that will be sent when pressed.

![Alt text](resources/pics/keymaps/usa.png)

Now let's see what happens on a `Dvorak (US layout)` keyboard:

![Alt text](resources/pics/keymaps/dvorak.png)

Notice how the **HID Usage ID stays the same** while **labels on the key changes!**

Therefore, it is important to understand that the keyboard only tells `WHERE` a key is pressed, and it's **`up to the operating system`** to determine `WHAT` character the key should produce. In modern OSs, you can set multiple keyboard layouts:

![Alt text](resources/pics/keymaps/inputs.png)

## Keymap Madness

This creates an issue when it comes input automation on duckyPad. Consider this command:

`STRING fox`

duckyPad will simply types out `fox`, but what code should it send? 

Remember that HID Usage ID (red numbers) corresponds to the `PHYSICAL LOCATION` of a key. So if we assume `English (US)` layout, it should send out `0x09, 0x12, 0x1b`:

![Alt text](resources/pics/keymaps/foxus.png)

However, if the OS is set to `Dvorak`, it will interpret `0x09, 0x12, 0x1b` as the keys at the `SAME LOCATION`, which comes out as `URQ`. Not good!

![Alt text](resources/pics/keymaps/foxdvo.png)

To type `fox` on `Dvorak`, duckyPad needs to send `0x1c, 0x16, 0x05` instead:

![Alt text](resources/pics/keymaps/foxright.png)

See the problem here? We now need to send **different codes** for the **same characters**! And there are *dozens* of keyboard layouts, each with their own minute quirks and differences.

To make sure everything works, we need a way to **`map a character to its HID Usage ID`** (therefore the physical location on the keyboard) so duckyPad knows what code to send for different keyboard layouts.

And thus, the keymap is born.

## duckyPad Keymaps

Keymaps for duckyPad is a simple text file mapping **`256 ASCII characters to HID Usage Codes`**.

### Example

I have written a few myself, so [check it out here](sample_profiles/keymaps).

Here is a snippet from [`Dvorak (US)`](sample_profiles/keymaps/dpkm_Dvorak.txt) keymap:

```
.........
0x61 0x0004 // a
0x62 0x0011 // b
0x63 0x000c // c
0x64 0x000b // d
0x65 0x0007 // e
.........
```

### Basic Rules

* The file name must start with `dpkm_` and end with the extension of `.txt`.

* Inside, it should contain lines of **two hexadecimal numbers** (plus optional comment) like above

* The first **2-digit hexadecimal number** is the [ASCII code of a character](https://www.ascii-code.com/).

* The second **4-digit hexadecimal number** is called `duckcode`, which will be explained below.

### duckcode

duckcode is a `unsigned 16-bit number` containing the HID Usage Code and some other information:

![Alt text](resources/pics/keymaps/duckcode.png)

* The `lower 8 bits` is the HID Usage Code, refer to the [keyboard diagrams above](#keyboard-layouts).

* `Bit 8` is `SHIFT`, set it to 1 to press it along with HID Usage Code. This is used for upper-case and symbols.

* `Bit 9` is `Alt Gr`, which produces additional characters on European keyboards, works the same way.

* `Upper 4 bits` is for `Dead Keys`. They don't produce a character on their own, but modify the appearance of the next character.

* In this case, setting the appropriate bit adds accents on the base keys. It works like this:

![Alt text](resources/pics/keymaps/dead.png)

## Making Your Own Keymap: Step-by-Step

### Download the templates

A good idea would be starting from templates.

[Click me to download the svg template](resources/ANSI_template.svg) for ANSI-style keyboard:

![Alt text](resources/pics/keymaps/ansi.png)

[Click me to download the svg template](resources/ISO_template.svg) for ISO-style keyboard:

![Alt text](resources/pics/keymaps/iso.png)

* There are multiple layers in the svg file, you can show or hide them

### Overlay template

You can probably find a diagram of your desired keyboard layout [on Wikipedia](https://en.wikipedia.org/wiki/Keyboard_layout).

When you do, overlay the template over the layout to visualise the HID Usage Codes.

Let's say you want to make a keymap for `AZERTY Belgian Comma` layout:

![Alt text](resources/pics/keymaps/be.png)

### Modify keymap entries

[Download the default `English (US)` keymap here](sample_profiles/keymaps/eng_us_template.txt) and open in a text editor.

Start from the top left key:

![Alt text](resources/pics/keymaps/be1.png)

In `AZERTY Belgian Comma` layout, when we press this key, it will print out `²` character.

Now we need to find it in the keymap file, and map it to the corresponding HID code.

Simply search `²` in the file, we find it at line 185. Its ASCII code is `0xb2`, and right now it looks like this:

`0xb2 0x0000 // ²`

The duckcode here is 0x0000, which means unmapped, since we don't have this key on US keyboards.

To map `²` to this key, make a duckcode as [described above](#duckcode). In this case, the HID code is `0x35` (shown on the key), and we don't need anything else, so the duckcode is `0x0035`. Change the line into:

`0xb2 0x0035 // ²`

That's it for this character!

---

However, we're not done yet! When you press this key with `SHIFT`, `³` is printed out.

As before, find `³` in the file, which is at line 186:

`0xb3 0x0000 // ³`

It is also currently unmapped. Since we need to press `SHIFT` to get this character, the `SHIFT` bit needs to be set in the duckcode, and it would be `0x0135`. Note how the HID Usage ID part stays the same.

So we change the line into:

`0xb3 0x0135 // ³`

Now this key is done! We mapped two characters to HID Usage IDs. Now duckyPad will know what code to send when it encounters those characters.

---

As another example, let's move on to the next key:

![Alt text](resources/pics/keymaps/be2.png)

When you press this key, `&` will be print out. Yes you heard that right, it is inverted in this layout!

It should be clear that duckcode for this key is `0x001e`.

As usual, go find `&` in the file, and set the duckcode:

`0x26 0x001e // &`

When you press this key with `SHIFT`, `1` is printed out. Therefore its duckcode should have the `SHIFT` bit set, which is `0x011e`. The line therefore should be:

`0x31 0x011e // 1`

However, we have a *third* character on this key! `|` is printed by pressing the `AltGr` key. So we need to set the `AltGr` bit in duckcode for this character, which would be `0x021e`.

Again, find the line and update the duckcode:

`0x7c 0x021e // |`

---

That's pretty much it! Now you just have to go through each key and check and modify the keymap lines!

Don't forget to set `SHIFT` bit for letter keys too, to distinguish between upper and lower cases.

### Dead Keys

To further complicate things, a lot of european keyboards include `Dead Keys`. They don't generate a character by themselves, but modify the appearance of the character typed immediately after.

In our Belgian keyboard example, there are 4 dead keys, identified with `red marking`.

![Alt text](resources/pics/keymaps/deadbe.png)

If you press the top-left key marked with `^`, nothing will happen, but if you then press letter `a`, `â` will be generated instead.

If you want duckyPad to type characters with dead keys, you can specify them in the beginning of the keymap file like this:

```
dk_grave_accent 0x0232
dk_acute_accent 0x0234
dk_circumflex 0x002f
dk_tilde 0x0238
dk_diaeresis 0x012f
```

The hexadecimal number is the duckcode for the dead key, and some of them will need `SHIFT` or `AltGr`.

If a dead key doesn't exist, set it to 0x0000.

Afterwards, [set the dead key bits in duckcode](#duckcode) with HID code of regular letters to get those letters with accents: 

```
0xd2 0x1112 // Ò
0xd3 0x2112 // Ó
0xd4 0x3112 // Ô
0xd5 0x4112 // Õ
0xd6 0x5112 // Ö
.....
0xf2 0x1012 // ò
0xf3 0x2012 // ó
0xf4 0x3012 // ô
0xf5 0x4012 // õ
0xf6 0x5012 // ö
```

## Good luck!

If you made a keymap for a new keyboard layout, feel free to sumbit a pull request or [contact me](questions-or-comments), and I'll add it for everyone to use!

## Table of Contents

[Main page](README.md)

[Getting Started Guide](getting_started.md)

[Kit Assembly Guide](kit_assembly_guide.md)

[Using duckyScript](duckyscript_info.md)

[Firmware Updates and Version History](firmware_updates_and_version_history.md)

[Make Your Own Keymap](./keymap_instructions.md)

[Manual Setup](./manual_setup.md)

[Build a duckyPad Yourself](build_it_yourself.md)

## Questions or Comments?

Please feel free to [open an issue](https://github.com/dekuNukem/duckypad/issues), ask in the [official duckyPad discord](https://discord.gg/4sJCBx5), DM me on discord `dekuNukem#6998`, or email `dekuNukem`@`gmail`.`com` for inquires.
