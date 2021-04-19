import pywinusb.hid as hid
from time import sleep

target_vendor_id = 0x0483

target_usage = hid.get_full_usage_id(0x1, 0x3a) # page, usage

all_devices = hid.HidDeviceFilter(vendor_id = target_vendor_id).get_devices()

if all_devices is None:
    print("vender ID not found")
    exit()

def sample_handler(data):
    print("Decimal: {0}".format(data))
    print("HEX:", [hex(x) for x in data])
    # print("ASCII:", "".join([chr(x) for x in data]))
    print('------')

for device in all_devices:
    device.open()
    for report in device.find_output_reports():
        print(report)
        if target_usage in report:
            device.set_raw_data_handler(sample_handler)
            buffff = [0] * 64
            buffff[0] = 5
            buffff[1] = 0
            buffff[2] = 3
            report.set_raw_data(buffff)
            report.send()
            print("SENT!")
            sleep(1)
    device.close()