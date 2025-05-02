if [[ $# -eq 0 ]] ; then
    echo 'error: please enter a commit comment'
    exit 1
fi

find . -type f -name "*.b#*" -exec rm -f {} \;
# find . -type f -name "*.dsb" -exec rm -f {} \;
find . -type f -name "*.s#*" -exec rm -f {} \;
find . -type f -name "*.l#*" -exec rm -f {} \;
find . -type f -name "*.csv#*" -exec rm -f {} \;
find . -type f -name "*.DS_Store*" -exec rm -f {} \;
find . -type f -name "*.eggs*" -exec rm -f {} \;
find . -name "__pycache__" -exec rm -rf {} \;

rm -rfv ./resources/new_hid_dump/hid_dump

git add --all
git commit -m "$@"
git push origin master
