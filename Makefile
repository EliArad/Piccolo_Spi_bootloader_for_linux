obj-m := piccolo_spi_bootloader.o
	KERNELDIR := /media/elia/e6931503-f01e-4d6f-8864-4f510792c6ca/imx6ul/buildroot-2018.02/output/build/linux-4f94df7e521040382adcbe5f9dbd835bfcdd3368
all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-  -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions	
