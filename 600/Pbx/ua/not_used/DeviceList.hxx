#ifndef DEVICE_LIST_HXX_
#define DEVICE_LIST_HXX_

/*
 * $Id: DeviceList.hxx,v 1.2 2007/03/03 18:39:28 lizhijie Exp $
 */

typedef enum {
    NULL_HARDWARE = 0,      // Keyboard only
    QUICKNET,               // Quicknet ISA card
    VOICEMAIL,              // VoiceMail device
    PHONECARD,              // Quicknet PC card
    SOUNDCARD,              // SoundCard
    SIMPLE_EXT              // simple external device (e.g. program)
} UseDeviceType;

#endif

