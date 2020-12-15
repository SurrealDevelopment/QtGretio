#include "J2534.h"

#include <QtDebug>
#include "gretiowebcontext.h"
#include <QJsonObject>
#include <QJsonArray>
#include "GretioWebConstants.h"
#include <isolateqapp.h>


GretioWebContext * context = nullptr;
IsolateQApp * app = nullptr;
QString lastError = "No error yet";

// just guard the app
void guardApp() {
    if (app == nullptr) {
        app = new IsolateQApp();
    }
}

// this will make our context and try connecting to a websocket HTTP Gretio server
long STDCALL PassThruOpen(void* pName, unsigned long* pDeviceID) {
    // these linker comments are required because its stupid
    if (context != nullptr) {
        *pDeviceID = -1;
        return ERR_DEVICE_IN_USE;
    }
    // ensure app exists
    guardApp();

    context = new GretioWebContext(&(app->app));

    // prevent caller from doing stuff too early
    bool open = context->waitForOpen();

    // look at result
    if (!open) {
        PassThruClose(1L);
        return ERR_DEVICE_NOT_CONNECTED;
    }

    // we always just use 1L. Never anything else.
    *pDeviceID = 1L;


    return STATUS_NOERROR;
}

long STDCALL PassThruClose(unsigned long DeviceID) {

    if (context == nullptr) {
        return STATUS_NOERROR;
    }

    if (DeviceID != 1L) {
        return ERR_INVALID_DEVICE_ID;
    }

    delete context;
    context = nullptr;

    // end app
    delete app;
    app = nullptr;

    return STATUS_NOERROR;
}


long STDCALL PassThruConnect(unsigned long DeviceID, unsigned long ProtocolID,
    unsigned long Flags, unsigned long BaudRate, unsigned long* pChannelID) {
    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };

    if (DeviceID != 1L) {
        return ERR_INVALID_DEVICE_ID;
    }

    QJsonObject msg {
        {"id", MESSAGE_ID_PT_CONNECT},
        {"t", MESSAGE_TYPE_REQUEST},
        {"protocolId", (int)ProtocolID},
        {"flags", (int)Flags},
        {"baudRate", (int)BaudRate},
    };


    auto result = context->sendReceiveCseq(msg, WS_REQUEST_TIMEOUT, &msg);

    if (result) {
        *pChannelID = (unsigned long)msg["pChannelId"].toInt();
        return msg["passThruResult"].toInt();

    } else {
        return ERR_FAILED;
    }
}

long STDCALL PassThruDisconnect(unsigned long ChannelID) {

    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };

    QJsonObject msg {
        {"id", MESSAGE_ID_PT_DISCONNECT},
        {"t", MESSAGE_TYPE_REQUEST},
        {"channelId", (int)ChannelID},
    };

    auto result = context->sendReceiveCseq(msg, 5000L, &msg);

    if (result) {
        return msg["passThruResult"].toInt();

    } else {
        return ERR_FAILED;
    }



    return 0L;
}

QJsonObject PtMsgToJson(PASSTHRU_MSG &msg) {

    auto header = QByteArray((char *)&(msg.Data[0]), 4);
    auto data = QByteArray((char *)&(msg.Data[4]), msg.DataSize-4);

    QJsonObject ptMsg {
        {"header", QString(header.toHex())},
        {"data", QString(data.toHex())},
        {"protocolId", QString::number(msg.ProtocolID)},
        {"timeStamp", QString::number(msg.Timestamp)},
        {"rxFlag", QString::number(msg.RxStatus)},
        {"txFlag", QString::number(msg.TxFlags)},
        {"extraDataIndex", QString::number(msg.ExtraDataIndex)}

    };

    return ptMsg;
}

void CopyJsonMsg(QJsonObject msg, PASSTHRU_MSG * dest) {

    auto headerTxt = msg["header"].toString();
    auto combined = headerTxt + msg["data"].toString();
    auto rawData = QByteArray::fromHex(combined.toUtf8());

    // copy
    dest->DataSize = rawData.length();
    dest->ExtraDataIndex = msg["extraDataIndex"].toVariant().toLongLong();
    dest->ProtocolID = msg["protocolId"].toVariant().toLongLong();
    dest->Timestamp = msg["timeStamp"].toVariant().toLongLong();
    dest->RxStatus = msg["rxFlag"].toVariant().toLongLong();
    dest->TxFlags = msg["txFlag"].toVariant().toLongLong();


    if (rawData.length() < 4 || rawData.length() > 4128 ) {
        return;; // incomplete message, skip
    }

    memcpy(dest->Data, rawData.constData(), rawData.length());
}

long STDCALL PassThruReadMsgs(unsigned long ChannelID, PASSTHRU_MSG* pMsg, unsigned long* pNumMsgs, unsigned long Timeout) {

    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };

    QJsonObject msg {
        {"id", MESSAGE_ID_PT_READ_MSGS},
        {"t", MESSAGE_TYPE_REQUEST},
        {"channelId", QString::number(ChannelID)},
        {"numMsgs", QString::number(*pNumMsgs)},
        {"timeout", QString::number(Timeout)}
    };

    bool result = context->sendReceiveCseq(msg,WS_REQUEST_TIMEOUT + Timeout, &msg);

    if (!result) {
        lastError = "Could not send message to server";
        return ERR_FAILED;
    }

    auto ptResult = msg["passThruResult"].toInt();

    // wrap messages into json

    QJsonArray msgs = msg["msgs"].toArray();

    *pNumMsgs = msgs.count();

    for (int i = 0; i < msgs.count(); i++) {
        auto msg = msgs[i].toObject();
        CopyJsonMsg(msg, &pMsg[i]);
    }

    return ptResult;
}

long STDCALL PassThruWriteMsgs(unsigned long ChannelID, PASSTHRU_MSG* pMsg, unsigned long* pNumMsgs, unsigned long Timeout) {
    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };

    unsigned long preCallNumMsgs = *pNumMsgs;
    // wrap messages into json

    QJsonArray msgs;

    for (unsigned long i = 0; i < preCallNumMsgs; i++) {
        msgs.push_back(PtMsgToJson(pMsg[i]));
    }


    QJsonObject msg {
        {"id", MESSAGE_ID_PT_WRITE_MSGS},
        {"t", MESSAGE_TYPE_REQUEST},
        {"channelId", QString::number(ChannelID)},
        {"msgs", msgs},
        {"timeout", QString::number(Timeout)}
    };

    bool result = context->sendReceiveCseq(msg, WS_REQUEST_TIMEOUT + Timeout, &msg);

    if (!result) {
        lastError = "Could not send message to server";
        return ERR_FAILED;
    }


    return msg["passThruResult"].toInt();
}

long STDCALL PassThruStartPeriodicMsg(unsigned long ChannelID, PASSTHRU_MSG* pMsg, unsigned long* pMsgID, unsigned long TimeInterval) {

    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };

    if (pMsg == nullptr) {
        return ERR_NULL_PARAMETER;
    }


    QJsonObject msg {
        {"id", MESSAGE_ID_PT_START_PERIODIC},
        {"t", MESSAGE_TYPE_REQUEST},
        {"channelId", QString::number(ChannelID)},
        {"msg", PtMsgToJson(*pMsg)},
        {"timeInterval", QString::number(TimeInterval)}
    };

    bool result = context->sendReceiveCseq(msg, WS_REQUEST_TIMEOUT, &msg);

    if (!result) {
        lastError = "Could not send message to server";
        return ERR_FAILED;
    }

    *pMsgID = msg["msgId"].toInt();

    return msg["passThruResult"].toInt();


    return 0L;
}

long STDCALL PassThruStopPeriodicMsg(unsigned long ChannelID, unsigned long MsgID) {

    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };

    QJsonObject msg {
        {"id", MESSAGE_ID_PT_STOP_PERIODIC},
        {"t", MESSAGE_TYPE_REQUEST},
        {"channelId", QString::number(ChannelID)},
        {"msgId", QString::number(MsgID)}
    };

    bool result = context->sendReceiveCseq(msg, WS_REQUEST_TIMEOUT, &msg);

    if (!result) {
        lastError = "Could not send message to server";
        return ERR_FAILED;
    }

    return msg["passThruResult"].toInt();



    return 0L;

}

long STDCALL PassThruStartMsgFilter(unsigned long ChannelID, unsigned long FilterType, PASSTHRU_MSG* pMaskMsg,
    PASSTHRU_MSG* pPatternMsg, PASSTHRU_MSG* pFlowControlMsg, unsigned long* pFilterID) {

    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };
    if (FilterType == FLOW_CONTROL_FILTER && pFlowControlMsg == nullptr) {
        return ERR_NULL_PARAMETER;
    }

    if (pPatternMsg == nullptr || pMaskMsg == nullptr || pFilterID == nullptr) {
        return ERR_NULL_PARAMETER;
    }


    QJsonObject msg {
        {"id", MESSAGE_ID_PT_START_MESSAGE_FILTER},
        {"t", MESSAGE_TYPE_REQUEST},
        {"channelId", QString::number(ChannelID)},
        {"filterType", QString::number(FilterType)},
        {"maskMsg", PtMsgToJson(*pMaskMsg)},
        {"patternMsg", PtMsgToJson(*pPatternMsg)},
    };

    // flow control msg is nullable optional
    if (pFlowControlMsg != nullptr) {
        msg["flowControlMessage"] = PtMsgToJson(*pFlowControlMsg);
    }

    bool result = context->sendReceiveCseq(msg, WS_REQUEST_TIMEOUT, &msg);

    if (!result) {
        lastError = "Could not send message to server";
        return ERR_FAILED;
    }

    *pFilterID = msg["channelId"].toInt();


    return msg["passThruResult"].toInt();
}

long STDCALL PassThruStopMsgFilter(unsigned long ChannelID, unsigned long FilterID) {

    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };

    QJsonObject msg {
        {"id", MESSAGE_ID_PT_STOP_MESSAGE_FILTER},
        {"t", MESSAGE_TYPE_REQUEST},
        {"channelId", QString::number(ChannelID)},
        {"filterId", QString::number(FilterID)},

    };

    bool result = context->sendReceiveCseq(msg, WS_REQUEST_TIMEOUT * 2L, &msg);

    if (!result) {
        lastError = "Could not send message to server";
        return ERR_FAILED;
    }


    return msg["passThruResult"].toInt();
}

long STDCALL PassThruSetProgrammingVoltage(unsigned long DeviceID, unsigned long PinNumber, unsigned long Voltage) {

    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };

    if (DeviceID != 1L) {
        return ERR_INVALID_DEVICE_ID;
    }

    QJsonObject msg {
        {"id", MESSAGE_ID_PT_SET_PROG_VOLTAGE},
        {"t", MESSAGE_TYPE_REQUEST},
        {"pinNumber", QString::number(PinNumber)},
        {"voltage", QString::number(Voltage)},
    };

    bool result = context->sendReceiveCseq(msg, WS_REQUEST_TIMEOUT * 2L, &msg);

    if (!result) {
        lastError = "Could not send message to server";
        return ERR_FAILED;
    }


    return msg["passThruResult"].toInt();
}

long STDCALL PassThruReadVersion(unsigned long DeviceID, char* pFirmwareVersion, char* pDllVersion, char* pApiVersion) {

    auto ver = "0.01";
    strncpy(pFirmwareVersion, ver, 5);

    auto dllver = "0.01";
    strncpy(pDllVersion, dllver, 5);

    auto apiver = "04.04";
    strncpy(pApiVersion, apiver, 6);

    return 0L;
}

long STDCALL PassThruGetLastError(char* pErrorDescription) {

    if (context == nullptr) {
        auto msg = "Context is null. (J2534 Wrapper not started?)";

        strncpy(pErrorDescription, msg, 80);

        return 0L;
    }
    strncpy(pErrorDescription, lastError.toStdString().c_str(), 80);

    return 0L;
}

long simpleIotcl(unsigned long ChannelID, unsigned long IoctlID) {
    QJsonObject msg {
        {"id", MESSAGE_ID_PT_IOCTL},
        {"t", MESSAGE_TYPE_REQUEST},
        {"handleId", QString::number(ChannelID)},
        {"ioctlId", QString::number(IoctlID)},
    };

    auto result = context->sendReceiveCseq(msg, WS_REQUEST_TIMEOUT, &msg);

    if (!result) {
        return ERR_FAILED;
    }

    // result in msg
    return msg["passThruResult"].toInt();
}

long STDCALL PassThruIoctl(unsigned long ChannelID, unsigned long IoctlID, void* pInput, void* pOutput) {

    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };

    // message we send will vary according to IoctlID

    switch (IoctlID) {
    case GET_CONFIG: {                              // pInput = SCONFIG_LIST, pOutput = NULL

        // the config list will be filled with current configs
        SCONFIG_LIST& configList = *(SCONFIG_LIST*)pInput;
        if (configList.ConfigPtr == NULL) {
            // this is non null
            return ERR_NULL_PARAMETER;
        }
        QJsonArray jsonList;

        for (int i = 0; i < configList.NumOfParams; i++) {
            SCONFIG& config = configList.ConfigPtr[i];

            auto param = QString::number(config.Parameter);
            auto value = QString::number(config.Value);
            QJsonObject pair {
                {"first", param},
                {"second", value}
            };

            jsonList.push_back(pair);

        }

        QJsonObject msg {
            {"id", MESSAGE_ID_PT_IOCTL},
            {"t", MESSAGE_TYPE_REQUEST},
            {"handleId", QString::number(ChannelID)},
            {"ioctlId", QString::number(IoctlID)},
            {"configList", jsonList},
        };

        auto result = context->sendReceiveCseq(msg, WS_REQUEST_TIMEOUT, &msg);

        if (!result) {
            return ERR_FAILED;
        }

        // result in msg
        return msg["passThruResult"].toInt();

        // @TODO - fill with results

        break;
    }
    case SET_CONFIG:								// pInput = SCONFIG_LIST, pOutput = NULL
    {
        //pInput
        SCONFIG_LIST& configList = *(SCONFIG_LIST*)pInput;
        if (configList.ConfigPtr == NULL) {
            // this is non null
            return ERR_NULL_PARAMETER;
        }
        QJsonArray jsonList;

        for (int i = 0; i < configList.NumOfParams; i++) {
            SCONFIG& config = configList.ConfigPtr[i];

            auto param = QString::number(config.Parameter);
            auto value = QString::number(config.Value);
            QJsonObject pair {
                {"first", param},
                {"second", value}
            };

            jsonList.push_back(pair);

        }

        QJsonObject msg {
            {"id", MESSAGE_ID_PT_IOCTL},
            {"t", MESSAGE_TYPE_REQUEST},
            {"handleId", QString::number(ChannelID)},
            {"ioctlId", QString::number(IoctlID)},
            {"configList", jsonList},
        };

        auto result = context->sendReceiveCseq(msg, WS_REQUEST_TIMEOUT, &msg);

        if (!result) {
            return ERR_FAILED;
        }

        // result in msg
        return msg["passThruResult"].toInt();

        break;

    }

    case FIVE_BAUD_INIT:							// pInput = SBYTE_ARRAY, pOutput = SBYTE_ARRAY
    {
        return simpleIotcl(ChannelID, IoctlID);
        break;
    }

    case FAST_INIT: {                               // pInput = PASSTHRU_MSG, pOutput = PASSTHRU_MSG

        return simpleIotcl(ChannelID, IoctlID);
        break;
    }
    case CLEAR_TX_BUFFER:							// pInput = NULL, pOutput = NULL
    case CLEAR_RX_BUFFER:							// pInput = NULL, pOutput = NULL
    case CLEAR_PERIODIC_MSGS:						// pInput = NULL, pOutput = NULL
    case CLEAR_MSG_FILTERS:							// pInput = NULL, pOutput = NULL
    case CLEAR_FUNCT_MSG_LOOKUP_TABLE:				// pInput = NULL, pOutput = NULL
    case SW_CAN_HS:									// pInput = NULL, pOutput = NULL
    case SW_CAN_NS:									// pInput = NULL, pOutput = NULL
    {
        return simpleIotcl(ChannelID, IoctlID);

        break;
    }

    case ADD_TO_FUNCT_MSG_LOOKUP_TABLE:				// pInput = SBYTE_ARRAY, pOutput = NULL
    case DELETE_FROM_FUNCT_MSG_LOOKUP_TABLE:		// pInput = SBYTE_ARRAY, pOutput = NULL
    case SET_POLL_RESPONSE:							// pInput = SBYTE_ARRAY, pOutput = NULL
    {
        return simpleIotcl(ChannelID, IoctlID);

        break;
    }

    case READ_VBATT:								// pInput = NULL, pOutput = unsigned long
    case READ_PROG_VOLTAGE:							// pInput = NULL, pOutput = unsigned long
        return simpleIotcl(ChannelID, IoctlID);

        break;

    case BECOME_MASTER:								// pInput = unsigned char, pOutput = NULL
        return simpleIotcl(ChannelID, IoctlID);
        break;

    default:
        break;
    }

    return simpleIotcl(ChannelID, IoctlID);
}

