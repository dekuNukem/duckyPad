# Running duckyPad Configurator on Linux

To use duckyPad configurator, clone the repo or download the latest source files from the [release page](https://github.com/dekuNukem/duckyPad/releases/latest). Enter `pc_software` directory in terminal, then use the following commands to install dependencies and launch the app:

```bash
cd pc_software
pip3 install appdirs hidapi

sudo python3 duckypad_config.py

# OR if using Hi-DPI
sudo DUCKYPAD_UI_SCALE=2 python3 duckypad_config.py
# adjust the number for best results
```

## Udev Rule

To enable USB profile editing on Linux, you may need to install a Udev rule to allow your user to access the HID device.

Create a rule file with your favorite text editor:

`sudo nano /etc/udev/rules.d/20-duckyPad.rules`

Put the following text into the file, and save.

```
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="d11c", TAG+="uaccess", TAG+="udev-acl"
```

The rule will be applied on the next reboot.

You can also force a reload of the rules by running:

```bash
sudo udevadm trigger
sudo udevadm control --reload-rules
```

## Debugging

To view HID connection debug log, you can start the app with `HID_OP_LOG=debug` like this:

```bash
HID_OP_LOG=debug python duckypad_config.py
```
