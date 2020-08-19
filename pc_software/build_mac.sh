rm -rfv ./duck_app
mkdir ./duck_app
cp ./*.py ./duck_app
cp ./*.spec ./duck_app
cp ./icon.ico ./duck_app
cd ./duck_app

# pyinstaller --onefile --add-binary='/System/Library/Frameworks/Tk.framework/Tk':'tk' --add-binary='/System/Library/Frameworks/Tcl.framework/Tcl':'tcl' --noconsole --icon=icon.ico main.py
# pyinstaller duckypad_config_mac.spec
# pyinstaller --onefile --add-binary='/System/Library/Frameworks/Tk.framework/Tk':'tk' --add-binary='/System/Library/Frameworks/Tcl.framework/Tcl':'tcl' --noconsole --icon=icon.ico main.py

pyinstaller --windowed duckypad_config.py

cd dist/duckypad_config.app/Contents/MacOS/
mkdir tcl tk
cp -R /Library/Frameworks/Python.framework/Versions/3.8/lib/tcl* tcl/
cp -R /Library/Frameworks/Python.framework/Versions/3.8/lib/tk* tk/
cp -R /Library/Frameworks/Python.framework/Versions/3.8/lib/Tk* tk/ 