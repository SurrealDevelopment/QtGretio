#include "J2534.h"

#include <QtDebug>
#include "gretiowebcontext.h"
#include <QJsonObject>
#include <QJsonArray>
#include "GretioWebConstants.h"
#include <isolateqapp.h>


GretioWebContext * context = nullptr;
IsolateQApp * app = nullptr;

// just guard the app
void guardApp() {
    if (app == nullptr) {
        app = new IsolateQApp();
    }
}

long STDCALL PassThruOpen(void* pName, unsigned long* pDeviceID) {
    // these linker comments are required because its stupid
    if (context != nullptr) {
        *pDeviceID = -1;
        return ERR_DEVICE_IN_USE;
    }
    // ensure app exists
    guardApp();

    context = new GretioWebContext(app);

    // prevent caller from doing stuff too early
    context->waitForOpenOrTimeout();


    // we always just use 0L. Never anything else.
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


    auto result = context->sendReceiveCseq(msg, 5000L, &msg);

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
        {"id", MESSAGE_ID_PT_CONNECT},
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

long STDCALL PassThruReadMsgs(unsigned long ChannelID, PASSTHRU_MSG* pMsg, unsigned long* pNumMsgs, unsigned long Timeout) {

    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };
    unsigned long preCallNumMsgs = *pNumMsgs;

    // wrap messages into json

    QJsonArray msgs;

    for (int i = 0; i < preCallNumMsgs; i++) {
        auto msg = pMsg[i];

        auto header = QByteArray((char *)&(msg.Data[0]), 4);
        auto data = QByteArray((char *)&(msg.Data[4]), msg.DataSize-4);
        QJsonObject ptMsg {
            {"header", QString(header.toHex())},
            {"data", QString(data.toHex())}
        };
        msgs.push_back(ptMsg);

    }

    QJsonObject msg {
        {"msgs", msgs},
        {"timeout", (int)Timeout}
    };

    context->sendReceiveCseq(msg, Timeout * 2L, &msg);


    return 0L;
}

long STDCALL PassThruWriteMsgs(unsigned long ChannelID, PASSTHRU_MSG* pMsg, unsigned long* pNumMsgs, unsigned long Timeout) {
    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };

    unsigned long preCallNumMsgs = *pNumMsgs;
    //auto res = LocalWriteMsgs(ChannelID, pMsg, pNumMsgs, Timeout);

    //logger.outfile << "PTWRTMSG: " << ChannelID << " NUM: " << *pNumMsgs << " TO: " << Timeout << std::endl;

    for (int x = 0; x < pMsg->DataSize; x++) {
        //logger.outfile << pMsg->Data[x];
    }
    //logger.outfile << std::endl;

    return 0L;
}

long STDCALL PassThruStartPeriodicMsg(unsigned long ChannelID, PASSTHRU_MSG* pMsg, unsigned long* pMsgID, unsigned long TimeInterval) {

    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };

    //auto res = LocalStartPeriodicMsg(ChannelID, pMsg, pMsgID, TimeInterval);

    //logger.outfile << "PTWRTMSGPERIODIC: " << ChannelID << " PID: " << *pMsgID << " TI: " << TimeInterval << std::endl;

    for (int x = 0; x < pMsg->DataSize; x++) {
        //logger.outfile << pMsg->Data[x];
    }
    //logger.outfile << std::endl;

    return 0L;
}

long STDCALL PassThruStopPeriodicMsg(unsigned long ChannelID, unsigned long MsgID) {

    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };

    //auto res = LocalStopPeriodicMsg(ChannelID, MsgID);

    //logger.outfile << "PTSTPPERIODIC: " << ChannelID << " PID: " << MsgID << std::endl;


    return 0L;

}

long STDCALL PassThruStartMsgFilter(unsigned long ChannelID, unsigned long FilterType, PASSTHRU_MSG* pMaskMsg,
    PASSTHRU_MSG* pPatternMsg, PASSTHRU_MSG* pFlowControlMsg, unsigned long* pFilterID) {

    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };

    //auto res = LocalStartMsgFilter(ChannelID, FilterType, pMaskMsg, pPatternMsg, pFlowControlMsg, pFilterID);

    //logger.outfile << "FILTER: " << ChannelID << " FID: " << *pFilterID << " TYPE: " << FilterType << std::endl;


    return 0L;
}

long STDCALL PassThruStopMsgFilter(unsigned long ChannelID, unsigned long FilterID) {

    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };

    //auto res = LocalStopMsgFilter(ChannelID, FilterID);

    //logger.outfile << "FILTER: " << ChannelID << " FID: " << FilterID << std::endl;


    return 0L;
}

long STDCALL PassThruSetProgrammingVoltage(unsigned long DeviceID, unsigned long PinNumber, unsigned long Voltage) {

    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };


    //auto res = LocalSetProgrammingVoltage(DeviceID, PinNumber, Voltage);

    //logger.outfile << "PROGVOLTAGE" << std::endl;


    return 0L;
}

long STDCALL PassThruReadVersion(unsigned long DeviceID, char* pFirmwareVersion, char* pDllVersion, char* pApiVersion) {


    auto ver = "TODO: ENTER VER";
    strncpy(pFirmwareVersion, ver, 80);

    auto dllver = "TODO: ENTER DLLVER";
    strncpy(pDllVersion, ver, 80);

    auto apiver = "TODO: ENTER APIVER";
    strncpy(pApiVersion, ver, 80);

    return 0L;
}

long STDCALL PassThruGetLastError(char* pErrorDescription) {

    auto msg = "TODO: ENTER MSG";

    strncpy(pErrorDescription, msg, 80);


    return 0L;
}

long STDCALL PassThruIoctl(unsigned long ChannelID, unsigned long IoctlID, void* pInput, void* pOutput) {

    if (context == nullptr) {
        return ERR_DEVICE_NOT_CONNECTED;
    };


    //auto res = LocalIoctl(ChannelID, IoctlID, pInput, pOutput);


    switch (IoctlID) {
    case GET_CONFIG:								// pInput = SCONFIG_LIST, pOutput = NULL
    case SET_CONFIG:								// pInput = SCONFIG_LIST, pOutput = NULL
    {
        { //pInput
            SCONFIG_LIST& configList = *(SCONFIG_LIST*)pInput;
            if (configList.ConfigPtr == NULL) {
                //logger.outfile << "SET CONFIG NULL" << std::endl;

            }
            else {
                for (int i = 0; i < configList.NumOfParams; i++) {
                    SCONFIG& config = configList.ConfigPtr[i];

                    switch (config.Parameter) {
                    case DATA_RATE:					// 5-500000
                        //logger.outfile << "SET CONFIG RATE " << config.Parameter << std::endl;

                        break;

                    case LOOPBACK:					// 0 (OFF), 1 (ON) [0]
                        //logger.outfile << "SET CONFIG LOOPBACK " << config.Value << std::endl;
                        break;

                    case NODE_ADDRESS:				// J1850PWM: 0x00-0xFF
                    case ISO15765_BS:				// ISO15765: 0x0-0xFF [0]
                    case ISO15765_STMIN:			// ISO15765: 0x0-0xFF [0]
                    case ISO15765_WFT_MAX:			// ISO15765: 0x0-0xFF [0]
                        //logger.outfile << "SET CONFIG WFT MAX " << config.Value << std::endl;
                        break;

                    case BIT_SAMPLE_POINT:			// CAN: 0-100 (1% per bit) [80]
                    case SYNC_JUMP_WIDTH:			// CAN: 0-100 (1% per bit) [15]
                        //logger.outfile << "SET CONFIG JUMP WIDTH " << config.Value << std::endl;
                        break;

                    case NETWORK_LINE:				// J1850PWM: 0 (BUS_NORMAL), 1 (BUS_PLUS), 2 (BUS_MINUS) [0]
                    case PARITY:					// ISO9141 or ISO14230: 0 (NO_PARITY), 1 (ODD_PARITY), 2 (EVEN_PARITY) [0]
                    case DATA_BITS:					// ISO9141 or ISO14230: 0 (8 data bits), 1 (7 data bits) [0]
                    case FIVE_BAUD_MOD:				// ISO9141 or ISO14230: 0 (ISO 9141-2/14230-4), 1 (Inv KB2), 2 (Inv Addr), 3 (ISO 9141) [0]
                        //logger.outfile << "SET CONFIG FIVE BAUD MOD WIDTH " << config.Value << std::endl;
                        break;

                    case P1_MIN:					// ISO9141 or ISO14230: Not used by interface
                    case P2_MIN:					// ISO9141 or ISO14230: Not used by interface
                    case P2_MAX:					// ISO9141 or ISO14230: Not used by interface
                    case P3_MAX:					// ISO9141 or ISO14230: Not used by interface
                    case P4_MAX:					// ISO9141 or ISO14230: Not used by interface
                        break;

                    case P1_MAX:					// ISO9141 or ISO14230: 0x1-0xFFFF (.5 ms per bit) [40 (20ms)]
                    case P3_MIN:					// ISO9141 or ISO14230: 0x0-0xFFFF (.5 ms per bit) [110 (55ms)]
                    case P4_MIN:					// ISO9141 or ISO14230: 0x0-0xFFFF (.5 ms per bit) [10 (5ms)]
                    case W0:						// ISO9141: 0x0-0xFFFF (1 ms per bit) [300]
                    case W1:						// ISO9141 or ISO14230: 0x0-0xFFFF (1 ms per bit) [300]
                    case W2:						// ISO9141 or ISO14230: 0x0-0xFFFF (1 ms per bit) [20]
                    case W3:						// ISO9141 or ISO14230: 0x0-0xFFFF (1 ms per bit) [20]
                    case W4:						// ISO9141 or ISO14230: 0x0-0xFFFF (1 ms per bit) [50]
                    case W5:						// ISO9141 or ISO14230: 0x0-0xFFFF (1 ms per bit) [300]
                    case TIDLE:						// ISO9141 or ISO14230: 0x0-0xFFFF (1 ms per bit) [300]
                    case TINIL:						// ISO9141 or ISO14230: 0x0-0xFFFF (1 ms per bit) [25]
                    case TWUP:						// ISO9141 or ISO14230: 0x0-0xFFFF (1 ms per bit) [50]
                    case T1_MAX:					// SCI: 0x0-0xFFFF (1 ms per bit) [20]
                    case T2_MAX:					// SCI: 0x0-0xFFFF (1 ms per bit) [100]
                    case T3_MAX:					// SCI: 0x0-0xFFFF (1 ms per bit) [50]
                    case T4_MAX:					// SCI: 0x0-0xFFFF (1 ms per bit) [20]
                    case T5_MAX:					// SCI: 0x0-0xFFFF (1 ms per bit) [100]
                    case ISO15765_BS_TX:			// ISO15765: 0x0-0xFF,0xFFFF [0xFFFF]
                    case ISO15765_STMIN_TX:			// ISO15765: 0x0-0xFF,0xFFFF [0xFFFF]
                        //logger.outfile << "SET CONFIG ST MIN " << config.Value << std::endl;

                        break;

                    default:
                        break;
                    }
                }
            }
        }
        break;
    }

    case FIVE_BAUD_INIT:							// pInput = SBYTE_ARRAY, pOutput = SBYTE_ARRAY
    {

        break;
    }

    case FAST_INIT:									// pInput = PASSTHRU_MSG, pOutput = PASSTHRU_MSG

        break;

    case CLEAR_TX_BUFFER:							// pInput = NULL, pOutput = NULL
    case CLEAR_RX_BUFFER:							// pInput = NULL, pOutput = NULL
    case CLEAR_PERIODIC_MSGS:						// pInput = NULL, pOutput = NULL
    case CLEAR_MSG_FILTERS:							// pInput = NULL, pOutput = NULL
    case CLEAR_FUNCT_MSG_LOOKUP_TABLE:				// pInput = NULL, pOutput = NULL
    case SW_CAN_HS:									// pInput = NULL, pOutput = NULL
    case SW_CAN_NS:									// pInput = NULL, pOutput = NULL

        break;

    case ADD_TO_FUNCT_MSG_LOOKUP_TABLE:				// pInput = SBYTE_ARRAY, pOutput = NULL
    case DELETE_FROM_FUNCT_MSG_LOOKUP_TABLE:		// pInput = SBYTE_ARRAY, pOutput = NULL
    case SET_POLL_RESPONSE:							// pInput = SBYTE_ARRAY, pOutput = NULL
    {

        break;
    }

    case READ_VBATT:								// pInput = NULL, pOutput = unsigned long
    case READ_PROG_VOLTAGE:							// pInput = NULL, pOutput = unsigned long

        break;

    case BECOME_MASTER:								// pInput = unsigned char, pOutput = NULL

        break;

    default:
        break;
    }

    return 0L;
}

