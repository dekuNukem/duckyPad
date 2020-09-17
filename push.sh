find . -type f -name "*.b#*" -exec rm -f {} \;
find . -type f -name "*.s#*" -exec rm -f {} \;
find . -type f -name "*.l#*" -exec rm -f {} \;
find . -type f -name "*.csv#*" -exec rm -f {} \;
find . -type f -name "*.DS_Store*" -exec rm -f {} \;
find . -name "__pycache__" -exec rm -rf {} \;
find ./pc_software -name "duck_app" -exec rm -rf {} \;
find ./pc_software -name "build" -exec rm -rf {} \;
find ./pc_software -name "dist" -exec rm -rf {} \;
find ./pc_software -name "duckypad_config.spec" -exec rm -rf {} \;
python update_firmware_ref.py
zip -rv sample_profiles.zip sample_profiles
git add --all
git commit -m "$@"
git push origin master
