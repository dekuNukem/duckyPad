#
"""
Simple example on how to send simple reports based on usages
"""
import pywinusb.hid as hid

def click_signal(target_usage, target_vendor_id):
    """This function will find a particular target_usage over output reports on
    target_vendor_id related devices, then it will flip the signal to simulate
    a 'click' event"""
    # usually you'll find and open the target device, here we'll browse for the
    # current connected devices
    all_devices = hid.HidDeviceFilter(vendor_id = target_vendor_id).get_devices()
    print(all_devices)
    if not all_devices:
        print("Can't find target device (vendor_id = 0x%04x)!" % target_vendor_id)
    else:
        # search for our target usage
        # target pageId, usageId

        for device in all_devices:
            try:
                device.open()
                # browse output reports, we could search over feature reports also,
                # changing find_output_reports() to find_feature_reports()
                for report in device.find_output_reports():
                    if target_usage in report:
                        # found out target!
                        report[target_usage] = 1 # yes, changing values is that easy
                        # at this point you could change different usages at a time...
                        # and finally send the prepared output report
                        report.send()
                        # now toggle back the signal
                        report[target_usage] = 0
                        report.send()
                        print("\nUsage clicked!\n")
                        return
            finally:
                device.close()
        print("The target device was found, but the requested usage does not exist!\n")
    #
if __name__ == '__main__':
    target_vendor_id = 0x0483 # just an example, change it to the actual vendor_id
    target_usage = hid.get_full_usage_id(0x1, 0x0) # generic vendor page, usage_id = 2
    # go for it!
    print(hex(target_usage))
    click_signal(target_usage, target_vendor_id)

