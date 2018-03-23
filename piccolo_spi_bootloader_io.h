#ifndef PACKT_IOCTL_H
#define PACKT_IOCTL_H
/*
* We need to choose a magic number for our driver, and sequential numbers
* for each command:
*/
#define EEP_MAGIC 				'E'

#define START_UPLOAD_SEQ_ID			0x01
#define ENABLE_DISABLE_LOADER_SEQ_ID 		0x02
/*
* Partition name must be 32 byte max
*/
#define MAX_PART_NAME 32
/*
* Now let's define our ioctl numbers:
*/
#define START_UPLOAD _IO(EEP_MAGIC, START_UPLOAD_SEQ_ID)
#define ENABLE_DISABLE_LOADER _IOW(EEP_MAGIC, ENABLE_DISABLE_LOADER_SEQ_ID, unsigned long)
#endif