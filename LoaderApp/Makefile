CC      = arm-linux-gnueabihf-gcc
CFLAGS  = -g
RM      = rm -f

default: all

all: piccolo_spi_loader

piccolo_spi_loader: piccolo_spi_loader.c
	$(CC) $(CFLAGS) -o piccolo_spi_loader piccolo_spi_loader.c

clean veryclean:
	$(RM) piccolo_spi_loader