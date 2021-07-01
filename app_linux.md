# Running On Linux

These instructions can be used to run the software directly from the repo (useful for development) or from a downloaded release. Before following the instructions you should open a terminal and change directory either to the root of this repo or the directory where you have extracted the release you downloaded. The directory you are in should contain the `pc_software` folder.

```bash
cd pc_software
python -m venv venv
. venv/bin/activate
pip install appdirs hidapi
python duckypad_config.py
```

## Udev Rule

If you want to use the functionality to manage your duckyPad profile via USB then you may need to install a Udev rule to allow your user to access the device. You can do that by creating the file `/etc/udev/rules.d/20-duckyPad.rules` with this content:

```
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="d11c", TAG+="uaccess", TAG+="udev-acl"
```

The rule will be applied on the next reboot or you can force a reload of the rules by running:

```bash
sudo udevadm trigger
sudo udevadm control --reload-rules
```

## Debugging

To view all of the HID connection debug logging (warning this is very verbose) you can start the app with `HID_OP_LOG=debug` like this:

```bash
HID_OP_LOG=debug python duckypad_config.py
```
