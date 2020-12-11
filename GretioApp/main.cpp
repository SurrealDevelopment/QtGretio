
#include <QApplication>
#include <QLibrary>
#include <QDebug>
#include "J2534.h"
#include <QSettings>
#include <QLibrary>

//void testJ2534() {
//    unsigned long deviceId;
//    char * name = nullptr;
//
//    PassThruOpen(name, &deviceId);
//    qDebug() << "OPEN" << deviceId;
//
//    unsigned long channelId;
//
//
//    PassThruConnect(0L, ISO15765, 0L, 500000L, &channelId);
//
//
//    qDebug() << "J2534 OK";
//}

void testRemote() {
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\PassThruSupport.04.04\\Surreal Dev",QSettings::NativeFormat);
    auto lib = settings.value("FunctionLibrary").toString();

    qDebug() << " lib at " << lib;



    unsigned long test = 0;
    PassThruOpen((void *)"Tester", &test);
    // The DeviceID should be 1


    qDebug() << "Surreal J2534 DLL Resolved PTOPEN DEVID" << test;

    PassThruConnect(test,5,0L,500000L,&test);

    qDebug() << "Surreal J2534 DLL Resolved PTCONNECT CAN ID" << test;

    // enable loopback

    SCONFIG configs[] = {
        SCONFIG {3,1}
    };
    SCONFIG_LIST list = {
        .NumOfParams = 1,
        .ConfigPtr = configs
    };

    PassThruIoctl(test, SET_CONFIG, &list, nullptr);

    // make a msg
    PASSTHRU_MSG msg;

    msg.ProtocolID = 5;
    uint8_t data[] = {0x00, 0x00, 0x07, 0xdf, 0x02, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    mempcpy(msg.Data, data, 12);
    msg.RxStatus = 0;
    msg.Timestamp = 0;
    msg.TxFlags = 0;
    msg.ExtraDataIndex = 12;
    msg.DataSize = 12;
    unsigned long wrote = 1;

    unsigned long filterId = 0;
    PASSTHRU_MSG mask {
        .ProtocolID = 5,
        .DataSize = 4,
        .Data = {0x00, 0x00, 0x07, 0xff},
    };
    PASSTHRU_MSG pattern {
        .ProtocolID = 5,
        .DataSize = 4,
        .Data = {0x00, 0x00, 0x07, 0xff},
    };

    PassThruStartMsgFilter(test, PASS_FILTER, &mask, &pattern, nullptr, &filterId);

    PassThruWriteMsgs(test, &msg, &wrote, 0L);

    unsigned long read = 1;

    PassThruReadMsgs(test, &msg, &read, 0L);

    qDebug() << "Read : " << read << " messages";

    if (msg.DataSize -  (msg.ExtraDataIndex - msg.DataSize) != 12) {
        qErrnoWarning("Message is not as long as it should be");
    }
    if (msg.ProtocolID != 5) {
        qErrnoWarning("Message Protocol ID is not what it should be");
    }

    qDebug() << "RX" << QByteArray((char *)msg.Data, msg.DataSize).toHex() << " DS: " << msg.DataSize << " EDI: " << msg.ExtraDataIndex;
    qDebug() << "TxF: " << msg.TxFlags << " RxF: " << msg.RxStatus << " Timestamp: " << msg.Timestamp;

    PassThruDisconnect(test);


}


int main(int argc, char *argv[])
{

   // testJ2534();
    testRemote();

    return 0;

}


