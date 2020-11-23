#include "J2534.h"

#include <QtDebug>
#include "gretiowebcontext.h"

bool ready = true;

GretioWebContext * context = nullptr;


long PTAPI PassThruOpen(void* pName, unsigned long* pDeviceID) {
    if (!ready) return ERR_DEVICE_NOT_CONNECTED;


    context = new GretioWebContext();

    return 0L;
}

long PTAPI PassThruClose(unsigned long DeviceID) {

    if (!ready) return ERR_DEVICE_NOT_CONNECTED;


    return 0L;
}


long PTAPI PassThruConnect(unsigned long DeviceID, unsigned long ProtocolID,
    unsigned long Flags, unsigned long BaudRate, unsigned long* pChannelID) {

    if (!ready) return ERR_DEVICE_NOT_CONNECTED;

    //logger.outfile << "PTCONNECT: " << DeviceID << " PRTOCOL: " << ProtocolID << std::endl;


    return 0L;
}

long PTAPI PassThruDisconnect(unsigned long ChannelID) {
    if (!ready) return ERR_DEVICE_NOT_CONNECTED;

    //logger.outfile << "PTDISCONNECT: " << ChannelID << std::endl;


    return 0L;
}

long PTAPI PassThruReadMsgs(unsigned long ChannelID, PASSTHRU_MSG* pMsg, unsigned long* pNumMsgs, unsigned long Timeout) {
    if (!ready) return ERR_DEVICE_NOT_CONNECTED;
    unsigned long preCallNumMsgs = *pNumMsgs;

   // logger.outfile << "PTREADMSG: " << ChannelID << " NUM: " << *pNumMsgs << " TO: " << Timeout << std::endl;



    return 0L;
}

long PTAPI PassThruWriteMsgs(unsigned long ChannelID, PASSTHRU_MSG* pMsg, unsigned long* pNumMsgs, unsigned long Timeout) {
    if (!ready) return ERR_DEVICE_NOT_CONNECTED;
    unsigned long preCallNumMsgs = *pNumMsgs;
    //auto res = LocalWriteMsgs(ChannelID, pMsg, pNumMsgs, Timeout);

    //logger.outfile << "PTWRTMSG: " << ChannelID << " NUM: " << *pNumMsgs << " TO: " << Timeout << std::endl;

    for (int x = 0; x < pMsg->DataSize; x++) {
        //logger.outfile << pMsg->Data[x];
    }
    //logger.outfile << std::endl;

    return 0L;
}

long PTAPI PassThruStartPeriodicMsg(unsigned long ChannelID, PASSTHRU_MSG* pMsg, unsigned long* pMsgID, unsigned long TimeInterval) {
    if (!ready) return ERR_DEVICE_NOT_CONNECTED;
    //auto res = LocalStartPeriodicMsg(ChannelID, pMsg, pMsgID, TimeInterval);

    //logger.outfile << "PTWRTMSGPERIODIC: " << ChannelID << " PID: " << *pMsgID << " TI: " << TimeInterval << std::endl;

    for (int x = 0; x < pMsg->DataSize; x++) {
        //logger.outfile << pMsg->Data[x];
    }
    //logger.outfile << std::endl;

    return 0L;
}

long PTAPI PassThruStopPeriodicMsg(unsigned long ChannelID, unsigned long MsgID) {
    if (!ready) return ERR_DEVICE_NOT_CONNECTED;
    //auto res = LocalStopPeriodicMsg(ChannelID, MsgID);

    //logger.outfile << "PTSTPPERIODIC: " << ChannelID << " PID: " << MsgID << std::endl;


    return 0L;

}

long PTAPI PassThruStartMsgFilter(unsigned long ChannelID, unsigned long FilterType, PASSTHRU_MSG* pMaskMsg,
    PASSTHRU_MSG* pPatternMsg, PASSTHRU_MSG* pFlowControlMsg, unsigned long* pFilterID) {
    if (!ready) return ERR_DEVICE_NOT_CONNECTED;
    //auto res = LocalStartMsgFilter(ChannelID, FilterType, pMaskMsg, pPatternMsg, pFlowControlMsg, pFilterID);

    //logger.outfile << "FILTER: " << ChannelID << " FID: " << *pFilterID << " TYPE: " << FilterType << std::endl;


    return 0L;
}

long PTAPI PassThruStopMsgFilter(unsigned long ChannelID, unsigned long FilterID) {
    if (!ready) return ERR_DEVICE_NOT_CONNECTED;
    //auto res = LocalStopMsgFilter(ChannelID, FilterID);

    //logger.outfile << "FILTER: " << ChannelID << " FID: " << FilterID << std::endl;


    return 0L;
}

long PTAPI PassThruSetProgrammingVoltage(unsigned long DeviceID, unsigned long PinNumber, unsigned long Voltage) {
   if (!ready) return ERR_DEVICE_NOT_CONNECTED;
    //auto res = LocalSetProgrammingVoltage(DeviceID, PinNumber, Voltage);

    //logger.outfile << "PROGVOLTAGE" << std::endl;


    return 0L;
}

long PTAPI PassThruReadVersion(unsigned long DeviceID, char* pFirmwareVersion, char* pDllVersion, char* pApiVersion) {
    if (!ready) return ERR_DEVICE_NOT_CONNECTED;
    //auto res = LocalReadVersion(DeviceID, pFirmwareVersion, pDllVersion, pApiVersion);

    //logger.outfile << "READVERSION" << std::endl;


    return 0L;
}

long PTAPI PassThruGetLastError(char* pErrorDescription) {
    if (!ready) return ERR_DEVICE_NOT_CONNECTED;
    //auto res = LocalGetLastError(pErrorDescription);

    //logger.outfile << "GETLASTERROR" << std::endl;


    return 0L;
}

long PTAPI PassThruIoctl(unsigned long ChannelID, unsigned long IoctlID, void* pInput, void* pOutput) {
    if (!ready) return ERR_DEVICE_NOT_CONNECTED;
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

