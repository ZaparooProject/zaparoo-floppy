#pragma once

//Move the track to the indicated after reading a disc (helps make more noise). Should be set less than 70
#define TARGET_TRACK 10

//Some drives are invereted and allways show write protected until a disk is inserted. Change to 0 if that is the case.
#define WRITE_FLAG 1

//The format the the drive will be initalized to. When inserting disks of different format, there will be a delay while the code detects the disk format
#define DEFAULT_DISK_FORMAT IBMPC1440K