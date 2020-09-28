# duckyPad Firmware Updates and Version History

[Main Page](/README.md) | [Buy duckyPad](/purchase_information.md)

-----

## USB Firmware Updates

You can update duckyPad's firmware via USB, for bug fixes and and/or new features. 

To do this, download and install the [STM32 DfuSe tool](resources/en.stsw-stm32080_stm32_DfuSe.zip) from this repo. (You can also download it from [the official website](https://www.st.com/en/development-tools/stsw-stm32080.html), but it requires creating an account.)

First, locate the debug buttons on the board:

![Alt text](resources/pics/buttons.jpg)

To update firmware:

* Make sure the board is unplugged.

* **Press and hold down the DFU button (one on the left)** while plugging it into a computer.

* Once it is plugged in, you can release the button.

If all went well, the backlight and display should stay off.

Now launch the `DfuSeDemo` software:

![Alt text](resources/pics/dfu_start.png)

In the `Available DUF Devices` section at top left corner, you should see a `STM Device in DFU Mode`.

![Alt text](resources/pics/dfu_available.png)

* If nothing shows up, you might need to install a driver. [Click me for details](https://github.com/dekuNukem/daytripper/blob/master/dfu_driver_install.md)

Press the `Choose...` button and select a firmware file in `.dfu` format.

* [Click me](https://github.com/dekuNukem/duckyPad/raw/master/firmware/duckypad_v0.2.1.dfu) to download the latest firmware (v0.2.1)

* ...or [scroll down](#firmware-version-history) for older firmware versions.

![Alt text](resources/pics/dfu_choose.png)

Press `Upgrade` button to start uploading the new firmware into the microcontroller.

![Alt text](resources/pics/dfu_upgrade.png)

And now the firmware is being updated!

![Alt text](resources/pics/dfu_uploading.png)

It could take anywhere between a few seconds to a few minutes. Try using different USB ports if it is particularly slow. 

After uploading is complete, unplug and plug it back in to start using the new firmware. 

------

## Firmware Version History

### 0.2.1

[Download Here](https://github.com/dekuNukem/duckyPad/raw/master/firmware/duckypad_v0.2.1.dfu)

* duckyPad now store its firmware version on SD card, so PC app can check for updates.

### 0.2.0

[Download Here](https://github.com/dekuNukem/duckyPad/raw/master/firmware/duckypad_v0.2.0.dfu)

Major update. New features and optimisations.

* You can now use symbols like `?*<>/\` in key names.

* Significantly increased profile switching speed.

* Added sleep mode. Your duckyPad can now turn off after a configurable delay. Press any key to wake it up.

* Other minor fixes

### 0.1.0

[Download Here](https://github.com/dekuNukem/duckyPad/raw/master/firmware/duckypad_v0.1.0.dfu)

Initial Release

## Questions or Comments?

Please feel free to [open an issue](https://github.com/dekuNukem/duckypad/issues), ask in the [official duckyPad discord](https://discord.gg/4sJCBx5), DM me on discord `dekuNukem#6998`, or email `dekuNukem`@`gmail`.`com` for inquires.





