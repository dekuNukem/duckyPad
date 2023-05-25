# Running duckyPad Configurator on Linux

## Script Install Method
To use duckyPad configurator, clone the repo or download the latest source files from the [release page](https://github.com/dekuNukem/duckyPad/releases/latest). Enter `pc_software` directory in terminal, then use `linux_install.sh` file to install the software on your system.

The Install script will move all files to the `/opt/duckypad_config` directory. It will also install the python dependencies and the udev rules. Currently, this has only been tested with Ubuntu based distros.

```bash
sudo +x ./linux_install.sh
./linux_install.sh
```

## Manual Install Method

To use duckyPad configurator, clone the repo or download the latest source files from the [release page](https://github.com/dekuNukem/duckyPad/releases/latest). Enter `pc_software` directory in terminal, then use the following commands to install dependencies and launch the app:

```bash
cd pc_software
pip3 install appdirs hidapi

sudo python3 duckypad_config.py

# OR if using Hi-DPI
sudo DUCKYPAD_UI_SCALE=2 python3 duckypad_config.py
# adjust the number for best results
```

### Arch Linux

Users of Arch Linux can take advantage of `duckypad-git` PKGBUILD in [aur](https://aur.archlinux.org/packages/duckypad-git) which also installs udev rule (below).

```bash
paru -S duckypad-git
# replace paru with yay if required
```

### Ubuntu/Pop!_OS/Linux Mint Users

If the above commands give you errors when attempting to execute `sudo python3 duckypad_config.py` perform the following first:

```bash
sudo apt install python3-tk
sudo apt install python3-appdirs
sudo apt install python3-hid
pip3 install pyautogui
```

Ubuntu/Pop!_OS/Linux Mint Users will also need to execute the duckypad_config.py file as non-sudo.

```bash
python3 duckypad_config.py
```

### Udev Rule

To enable USB profile editing on Linux, you may need to install a Udev rule to allow your user to access the HID device.

Create a rule file with your favorite text editor:

`sudo nano /etc/udev/rules.d/20-duckyPad.rules`

Put the following text into the file, and save.

```
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="d11c", TAG+="uaccess", TAG+="udev-acl"
```

The rule will be applied once duckyPad is reinserted or after a system reboot.

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
