#ifndef IDSENUMS_H
#define IDSENUMS_H

enum AppType: quint8{
    Root = 0x00,
    Messenger = 0x01,
    VoIP = 0x02,
    VideoStreamer = 0x03,
    DataTransfer = 0x04
};

enum LinkType: quint8{
    TCP = 0x00,
    UDP = 0x01,
    RTP = 0x02
};

#endif // IDSENUMS_H
