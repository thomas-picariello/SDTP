#ifndef IDSENUMS_H
#define IDSENUMS_H

#include <QtGlobal>

enum AppType: quint8{
    Manager = 0x00,     //0x00 is reseved for AppManager routing
    Messenger = 0x01,
    VoIP = 0x02,
    VideoStreamer = 0x03,
    DataTransfer = 0x04,

    Undefined = 0xFF    //reserved default
};

enum LinkType: quint8{
    TCP = 0x00,
    UDP = 0x01,
    RTP = 0x02
};

#endif // IDSENUMS_H
