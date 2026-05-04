#pragma once

//Move the track to the indicated after reading a disc (helps make more noise). Should be set less than 70
#ifndef TARGET_TRACK
#define TARGET_TRACK 5
#endif

//Some drives are invereted and allways show write protected until a disk is inserted. Change to 0 if that is the case.
#ifndef WRITE_FLAG
#define WRITE_FLAG 1
#endif

//The format the the drive will be initalized to. When inserting disks of different format, there will be a delay while the code detects the disk format
#ifndef DEFAULT_DISK_FORMAT
#define DEFAULT_DISK_FORMAT IBMPC1440K
#endif


//Poll the index pulse combined with the disc ready pin. May more responsive in some drives but could cause motor wear.
#ifndef USE_INDEX_POLLING
#define USE_INDEX_POLLING 1
#endif

//Defect the floppy disc format between each read. Will be faster when switching between formats, but slower is using all the same (detecting a switch dymically is about 2.5 extra seconds verse 400ms each time)
#ifndef RE_DETECT
#define RE_DETECT 0
#endif