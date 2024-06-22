{nixpkgs ? import <nixpkgs> {}}:

nixpkgs.mkShell {

  nativeBuildInputs = with nixpkgs; [ pkgs.python311Full pkgs.python3Packages.pyautogui pkgs.python3Packages.appdirs pkgs.python3Packages.hidapi pkgs.unzip];
  shellHook = ''
  if [[ ! -d duckypad ]]; then
    mkdir -p duckypad
    if [ -f duckypad_config_1.6.3_source.zip ]; then
        mv duckypad_config_1.6.3_source.zip duckypad/
    else
        wget https://github.com/dekuNukem/duckyPad/releases/download/1.6.3/duckypad_config_1.6.3_source.zip
        mv duckypad_config_1.6.3_source.zip duckypad/
    fi
  cd duckypad
    unzip duckypad_config_1.6.3_source.zip
    mv duckypad_config_1.6.3_source.zip ../
    cd ../
  fi
  python3 duckypad/duckypad_config.py
  '';
}