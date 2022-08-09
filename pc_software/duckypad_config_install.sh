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
echo "Installing .desktop file to /usr/share/applications/duckyconfig.desktop"
tee /usr/share/applications/duckyconfig.desktop <<EOF &> /dev/null
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
echo "Distro detected: " $NAME

#Check if pip3 is installed
command -v pip3 >/dev/null 2>&1 || { echo >&2 "pip3 cannot be detected on your system."; pip=false; }

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
    sudo apt-get -qq install python3-tk -y &> /dev/null || echo "Unable to install python3-tk"
    echo "Installing python3-appdirs.."
    sudo apt-get -qq install python3-appdirs -y &> /dev/null || echo "Unable to install python3-appdirs"
    echo "Installing python3-hid.."
    sudo apt-get -qq install python3-hid -y &> /dev/null || echo "Unable to install python3-hid"
    echo "Installing pyautogui"
    pip3 install pyautogui &> /dev/null || echo "Unable to install pyautogui"

elif [ "$NAME" == 'Fedora Linux' ]; then

    if [ "$pip" = false ] ;then
        while true; do
            read -p "Do you wish to install pip3? " yn
            case $yn in
                [Yy]* ) sudo dnf install python3-pip -y; break;;
                [Nn]* ) break;;
                * ) echo "Please answer yes or no.";;
            esac
        done
    fi

    echo "Installing python3-tkinter.."
    sudo dnf install python3-tkinter -y &> /dev/null || echo "Unable to install python3-tkinter"
    echo "Installing python3-appdirs.."
    sudo dnf install python3-appdirs -y &> /dev/null || echo "Unable to install python3-appdirs"
    echo "Installing python3-hidapi.."
    sudo dnf install python3-hidapi -y &> /dev/null || echo "Unable to install python3-hidapi"
    echo "Installing pyautogui"
    pip3 install pyautogui &> /dev/null || echo "Unable to install pyautogui"

elif [ "$NAME" == 'Manjaro Linux' ]; then

    if [ "$pip" = false ] ;then
        while true; do
            read -p "Do you wish to install pip3? " yn
            case $yn in
                [Yy]* ) sudo pacman -Sy --noconfirm python-pip; break;;
                [Nn]* ) break;;
                * ) echo "Please answer yes or no.";;
            esac
        done
    fi

    echo "Installing tkinter.."
    sudo pacman -Sy --noconfirm tk &> /dev/null || echo "Unable to install tkinter"
    echo "Installing python-appdirs.."
    sudo pacman -Sy --noconfirm python-appdirs &> /dev/null || echo "Unable to install python-appdirs"
    echo "Installing python-hidapi.."
    sudo pacman -Sy --noconfirm python-hidapi &> /dev/null || echo "Unable to install python-hidapi"
    echo "Installing pyautogui"
    pip3 install pyautogui &> /dev/null || echo "Unable to install pyautogui"

else

    pip3 install appdirs hidapi || echo "Unable to install appdirs or hidapi"

fi

#create directories in /opt for python files
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
