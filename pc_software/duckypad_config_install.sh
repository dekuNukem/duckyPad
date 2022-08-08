#!/bin/bash
#Install Script for Ducky Config
#

# Check if running as root
if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

while true; do
    read -p "Do you wish to install with Hi-DPI support? " yn
    case $yn in
        [Yy]* ) HIDPI="env DUCKYPAD_UI_SCALE=2"; break;;
        [Nn]* ) HIDPI=""; break;;
        * ) echo "Please answer yes or no.";;
    esac
done

## create .desktop file
tee /usr/share/applications/duckyconfig.desktop <<EOF
#!/usr/bin/env xdg-open
[Desktop Entry]
Name=DuckyPad Configurator
Version=v0.13.3
Icon=/opt/duckypad_config/icon.ico
Exec=$HIDPI python3 /opt/duckypad_config/duckypad_config.py
Terminal=false
Type=Application
EOF

# Determine Distro
. /etc/os-release

#Check if pip3 is installed
command -v pip3 >/dev/null 2>&1 || { echo >&2 "pip3 cannot be detected on your system. Please install."; pip=false; }

# Install Python libraries based on distro
if [ "$NAME" == 'Linux Mint' ] || [ "$NAME" == 'Pop!_OS' ] || [ "$NAME" == 'Ubuntu' ]; then

    if [ "$pip" = false ] ;then
        while true; do
            read -p "Do you wish to install pip3? " yn
            case $yn in
                [Yy]* ) sudo apt-get install python3-pip -y; break;;
                [Nn]* ) break;;
                * ) echo "Please answer yes or no.";;
            esac
        done
    fi

    echo "Installing python3-tk.."
    sudo apt-get -qq install python3-tk -y || echo "Unable to install python3-tk"
    echo "Installing python3-appdirs.."
    sudo apt-get -qq install python3-appdirs -y || echo "Unable to install python3-appdirs"
    echo "Installing python3-hid.."
    sudo apt-get -qq install python3-hid -y || echo "Unable to install python3-hid"
    echo "Installing pyautogui"
    pip3 install pyautogui || echo "Unable to install pyautogui"

else

    pip3 install appdirs hidapi

fi

mkdir -p /opt/duckypad_config
cp -R ./ /opt/duckypad_config/

# Setup udev rules
echo "Setting up udev rules..."
udev_rule="SUBSYSTEMS==\"usb\", ATTRS{idVendor}==\"0483\", ATTRS{idProduct}==\"d11c\", TAG+=\"uaccess\", TAG+=\"udev-acl\""
echo $udev_rule > /etc/udev/rules.d/20-duckyPad.rules

# Reload udev rules
echo "Reloading udev rules (1/2)..."
sudo udevadm trigger
echo "Reloading udev rules (2/2)..."
sudo udevadm control --reload-rules
