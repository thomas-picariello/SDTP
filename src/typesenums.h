#ifndef IDSENUMS_H
#define IDSENUMS_H

enum AppType{
    Root = 0,
    Messenger = 1,
    VoIP = 2,
    VideoStreamer = 3,
    DataTransfer = 4
};

enum LinkType{
    TCP = 0,
    UDP = 1,
    RTP = 2
};

#endif // IDSENUMS_H
