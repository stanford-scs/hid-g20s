/*
 * G20S Bluetooth Remote HID Driver
 * Fixes truncated HID descriptor and remaps OK button to Enter
 *
 * This work has been placed in the public domain. No human authorship
 * is claimed for this code, which was generated through automated means.
 *
 * DISCLAIMER: This software is provided "as is" without warranty of any kind,
 * express or implied, including but not limited to the warranties of
 * merchantability, fitness for a particular purpose, and non-infringement.
 * In no event shall the contributors be liable for any damages whatsoever
 * arising from the use of this software. Use at your own risk.
 *
 * The contributors disclaim all liability for any direct, indirect, incidental,
 * special, exemplary, or consequential damages (including, but not limited to,
 * procurement of substitute goods or services; loss of use, data, or profits;
 * or business interruption) however caused and on any theory of liability,
 * whether in contract, strict liability, or tort (including negligence or
 * otherwise) arising in any way out of the use of this software, even if
 * advised of the possibility of such damage.
 *
 * By using this software, you acknowledge that you have read this disclaimer
 * and agree to assume all risks associated with its use.
 */

#include <linux/hid.h>
#include <linux/module.h>
#include <linux/usb.h>

#define USB_VENDOR_ID_G20S    0x1d5a
#define USB_DEVICE_ID_G20S    0xc081

/* Fixed HID descriptor with proper collection endings */
static __u8 g20s_rdesc_fixed[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x06,        // Usage (Keyboard)
    0xa1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   Report ID (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x08,        //   Report Count (8)
    0x05, 0x07,        //   Usage Page (Keyboard)
    0x19, 0xe0,        //   Usage Minimum (224)
    0x29, 0xe7,        //   Usage Maximum (231)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x81, 0x02,        //   Input (Data,Var,Abs)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x08,        //   Report Size (8)
    0x81, 0x01,        //   Input (Const,Array,Abs)
    0x95, 0x05,        //   Report Count (5)
    0x75, 0x01,        //   Report Size (1)
    0x05, 0x08,        //   Usage Page (LEDs)
    0x19, 0x01,        //   Usage Minimum (1)
    0x29, 0x05,        //   Usage Maximum (5)
    0x91, 0x02,        //   Output (Data,Var,Abs)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x03,        //   Report Size (3)
    0x91, 0x01,        //   Output (Const,Array,Abs)
    0x95, 0x06,        //   Report Count (6)
    0x75, 0x08,        //   Report Size (8)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0xff,        //   Logical Maximum (255)
    0x05, 0x07,        //   Usage Page (Keyboard)
    0x19, 0x00,        //   Usage Minimum (0)
    0x29, 0xff,        //   Usage Maximum (255)
    0x81, 0x00,        //   Input (Data,Array,Abs)
    0xc0,              // End Collection

    0x05, 0x0c,        // Usage Page (Consumer)
    0x09, 0x01,        // Usage (Consumer Control)
    0xa1, 0x01,        // Collection (Application)
    0x85, 0x03,        //   Report ID (3)
    0x19, 0x00,        //   Usage Minimum (0)
    0x2a, 0x9c, 0x02,  //   Usage Maximum (668)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0x9c, 0x02,  //   Logical Maximum (668)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x00,        //   Input (Data,Array,Abs)
    0x09, 0x02,        //   Usage (Numeric Key Pad)
    0xa1, 0x02,        //   Collection (Logical)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (1)
    0x29, 0x0a,        //     Usage Maximum (10)
    0x15, 0x01,        //     Logical Minimum (1)
    0x25, 0x0a,        //     Logical Maximum (10)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x08,        //     Report Size (8)
    0x81, 0x40,        //     Input (Data,Array,Abs,Null)
    0xc0,              //   End Collection
    0xc0,              // End Collection

    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x02,        // Usage (Mouse)
    0xa1, 0x01,        // Collection (Application)
    0x85, 0x02,        //   Report ID (2)
    0x09, 0x01,        //   Usage (Pointer)
    0xa1, 0x00,        //   Collection (Physical)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (1)
    0x29, 0x03,        //     Usage Maximum (3)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x95, 0x03,        //     Report Count (3)
    0x75, 0x01,        //     Report Size (1)
    0x81, 0x02,        //     Input (Data,Var,Abs)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x05,        //     Report Size (5)
    0x81, 0x01,        //     Input (Const,Array,Abs)
    0x05, 0x01,        //     Usage Page (Generic Desktop)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7f,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x06,        //     Input (Data,Var,Rel)
    0x09, 0x38,        //     Usage (Wheel)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7f,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x06,        //     Input (Data,Var,Rel)
    0xc0,              //   End Collection
    0xc0,              // End Collection
};

static const __u8 *g20s_report_fixup(struct hid_device *hdev, __u8 *rdesc,
                                      unsigned int *rsize)
{
    if (*rsize == 139 && rdesc[0] == 0x05 && rdesc[1] == 0x01) {
        hid_info(hdev, "fixing G20S report descriptor\n");
        *rsize = sizeof(g20s_rdesc_fixed);
        return g20s_rdesc_fixed;
    }
    return rdesc;
}

static const struct hid_device_id g20s_devices[] = {
    { HID_BLUETOOTH_DEVICE(USB_VENDOR_ID_G20S, USB_DEVICE_ID_G20S) },
    { }
};
MODULE_DEVICE_TABLE(hid, g20s_devices);

static int g20s_input_event(struct hid_device *hdev, struct hid_field *field,
                           struct hid_usage *usage, __s32 value)
{
    struct input_dev *input;

    if (!field || !field->hidinput || !field->hidinput->input)
        return 0;

    input = field->hidinput->input;

    /* Intercept KEY_SELECT events and convert to KEY_ENTER */
    if (usage->code == KEY_SELECT) {
        input_report_key(input, KEY_ENTER, value);
        input_sync(input);
        return 1; /* Event handled, don't pass to default handler */
    }

    return 0; /* Let default handler process other events */
}

static struct hid_driver g20s_driver = {
    .name = "g20s",
    .id_table = g20s_devices,
    .report_fixup = g20s_report_fixup,
    .event = g20s_input_event,
};
module_hid_driver(g20s_driver);

MODULE_AUTHOR("Generated Code - Public Domain");
MODULE_DESCRIPTION("G20S Bluetooth Remote HID driver with fixed descriptor and key remapping");
MODULE_LICENSE("Dual BSD/GPL");  // Closest equivalent for public domain that kernel accepts
MODULE_VERSION("1.0");
