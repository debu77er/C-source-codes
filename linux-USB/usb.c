#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/usb/ch9.h>
#include <linux/usbdevice_fs.h>

int main() {
    const char *device_path = "/dev/bus/usb/004/013"; // Replace with your device path
    int fd = open(device_path, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    // Prepare setup packet for control transfer
    unsigned char setup_packet[8] = {
        0x80,        // bmRequestType: Direction=Device-to-host, Type=Standard, Recipient=Device
        0x06,        // bRequest: GET_DESCRIPTOR
        0x00, 0x01,  // wValue: Descriptor Type (Device) and Descriptor Index
        0x00, 0x00,  // wIndex: Language ID (0)
        18,          // wLength: Length of data to transfer
    };

    unsigned char buffer[18];

    // Prepare the setup packet for ioctl
    struct usbdevfs_ctrltransfer ctrl = {
        .bRequestType = setup_packet[0],
        .bRequest = setup_packet[1],
        .wValue = setup_packet[2] | (setup_packet[3] << 8),
        .wIndex = setup_packet[4] | (setup_packet[5] << 8),
        .wLength = 18,
        .data = buffer,
        .timeout = 1000, // in milliseconds
    };

    // Send control transfer
    int ret = ioctl(fd, USBDEVFS_CONTROL, &ctrl);
    if (ret < 0) {
        perror("Control transfer failed");
        close(fd);
        return 1;
    }

    printf("Device Descriptor:\n");
    for (int i = 0; i < ret; i++) {
        printf("%02x ", buffer[i]);
    }
    printf("\n");

    close(fd);
    return 0;
}
/*

    You need to identify your device's device file (e.g., /dev/bus/usb/XXX/YYY).
    You need appropriate permissions, or run the program as root.
    This example is simplified for educational purposes and assumes the device supports control transfers.
Important notes:

    Replace "/dev/bus/usb/001/002" with your actual device file.
    You can find the device file using lsusb and ls /dev/bus/usb/XXX/YYY.
    This code performs a control transfer to get the device descriptor.
    Running as root may be necessary due to permissions.

Summary:

This example directly uses Linux's usbdevfs interface via ioctl() to communicate with a USB device. It avoids external libraries like libusb and demonstrates basic control transfer operations.

Let me know if you'd like more advanced examples or explanations!

*/
