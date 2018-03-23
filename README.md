piccolo c2000 devices can upgrade firmware from SPI slace devices.
the boorom in the f280x device is a master device

Here we want to upgrade the firmware of a piccolo device from linux.
Linux does not have SPI slace device
For that , i built a char device driver that accept the firmware (flash
kernel and app and transfer it to device driver in the linux kernel)

The char device will change the behviour of the SPI pins temporary to SPI.
The driver will than communicate with the bootrom SPI master in the c2000 to
upgrade the firmware.
At the end , user can send ioctl command to transfer back the pins to SPI
mode.

The entire document can be found here at MyDocuments my github repo.
