"""
Show all HID devices information
"""
import sys
import pywinusb.hid as hid

if __name__ == '__main__':
    if sys.version_info < (3,):
        import codecs
        output = codecs.getwriter('mbcs')(sys.stdout)
    else:
        # python3, you have to deal with encodings, try redirecting to any file
        output = sys.stdout
    try:
        hid.core.show_hids(output = output)
    except UnicodeEncodeError:
        print("\nError: Can't manage encodings on terminal, try to run the script on PyScripter or IDLE")