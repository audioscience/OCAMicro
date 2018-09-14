/*  By downloading or using this file, the user agrees to be bound by the terms of the license 
 *  agreement located at http://ocaalliance.com/EULA as an original contracting party.
 */

// OCALiteOCCFactory.cpp : Simple stub that creates and destroys the Lite objects for the reference design.
//


#include <HostInterfaceLite/OCA/OCF/OcfLiteHostInterface.h>
#include <HostInterfaceLite/OCA/OCP.1/Ocp1LiteHostInterface.h>
#include <OCC/ControlClasses/Workers/BlocksAndMatrices/OcaLiteBlock.h>

#include "OcaLiteStreamNetworkDante.h"
#include "DanteLiteHostInterface.h"
#include "OcaLiteDanteMediaClock.h"
#include "OcaLiteDanteLvlSensor.h"

#include "OcaLiteIyoPhantomActuator.h"
#include "OcaLiteIyoLevelActuator.h"

#define kNumberObjects    (2)
static bool mInitialized = false;
static OcaLiteRoot *m_Objects[kNumberObjects];
OcaLiteRoot **m_Meters = NULL;
int m_NumberOfMeters = 0;
int m_NumberOfInputMeters = 0;
int m_NumberOfOutputMeters = 0;
::OcaONo m_OcaMeterObjStart;


class OcaLiteNodeBlock : public :: OcaLiteBlock
{
public:
    OcaLiteNodeBlock(::OcaONo objectNumber, ::OcaBoolean lockable, const ::OcaLiteString& role, const ::OcaLiteList< ::OcaLitePort>& ports);
};

OcaLiteNodeBlock::OcaLiteNodeBlock(::OcaONo objectNumber, ::OcaBoolean lockable, const ::OcaLiteString& role, const ::OcaLiteList< ::OcaLitePort>& ports)
: ::OcaLiteBlock(objectNumber, lockable, role, ports, objectNumber)
{
}

OcaLiteStatus OCALiteOCCFactoryCreate()
{
    OcaLiteStatus status(OCASTATUS_OK);
    int obj(0);
    if(mInitialized)
        return OCASTATUS_BAD_METHOD;
    // Obtain our I/O channel counts
    UINT16 nrRxChannels;
    UINT16 nrTxChannels;
    bool bResult = DanteLiteHostInterfaceGetNumberOfChannels(nrRxChannels, nrTxChannels);
    if (!bResult)
    {
        printf("Unable to get Channel I/O count, assuming 0-0\n");
        nrRxChannels = 0;
        nrTxChannels = 0;
    }
    // Add meters to total number of objects 
    int numberObjects = kNumberObjects;
    m_Meters = new OcaLiteRoot *[nrRxChannels + nrTxChannels];
    // Dante subscriptions manager
    ::OcaONo streamStartONo(static_cast< ::OcaONo>(0x02000));
    OcaLiteStreamNetworkDante* streamNetwork = new ::OcaLiteStreamNetworkDante(streamStartONo, 
                                                                                static_cast< ::OcaBoolean>(true),
                                                                                ::OcaLiteString("OcaLiteStreamNetworkDante"),
                                                                                ::OcaLiteNetworkDanteNodeID(::OcaLiteString(DanteLiteHostInterfaceGetDeviceName())));
    ::OcaLiteBlock::GetRootBlock().AddObject(*streamNetwork);
    streamStartONo++;
    m_Objects[obj++] = streamNetwork;
    ::OcaONo streamEndONo;
    ::OcaONo numberOfStreams = 0;
    if (streamNetwork->InitSignalChannels(streamStartONo, streamEndONo))
    {
        numberOfStreams = streamEndONo - streamStartONo;
        printf("(%s): OcaLiteStreamNetworkDante::InitSignalChannels from 0x%x to 0x%x (%d)\n", DanteLiteHostInterfaceGetDeviceName(), streamStartONo, streamEndONo, numberOfStreams);
    } else {
        printf("(%s): OcaLiteStreamNetworkDante::InitSignalChannels FAILED\n", DanteLiteHostInterfaceGetDeviceName());
    }
    // Dante Media Clock
    OcaLiteMediaClockRate rate0(static_cast< ::OcaFrequency>(44100.0), 
                                static_cast< ::OcaFrequency>(0.0), 
                                static_cast< ::OcaFrequency>(0.441), 
                                static_cast< ::OcaFrequency>(0.0));
    OcaLiteMediaClockRate rate1(static_cast< ::OcaFrequency>(48000.0),
                                static_cast< ::OcaFrequency>(0.0), 
                                static_cast< ::OcaFrequency>(0.480), 
                                static_cast< ::OcaFrequency>(0.0));
    OcaLiteMediaClockRate rate2(static_cast< ::OcaFrequency>(88200.0), 
                                static_cast< ::OcaFrequency>(0.0), 
                                static_cast< ::OcaFrequency>(0.882), 
                                static_cast< ::OcaFrequency>(0.0));
    OcaLiteMediaClockRate rate3(static_cast< ::OcaFrequency>(96000.0), 
                                static_cast< ::OcaFrequency>(0.0), 
                                static_cast< ::OcaFrequency>(0.960), 
                                static_cast< ::OcaFrequency>(0.0));
    OcaLiteMediaClockRate rate4(static_cast< ::OcaFrequency>(176400.0), 
                                static_cast< ::OcaFrequency>(0.0), 
                                static_cast< ::OcaFrequency>(1.764), 
                                static_cast< ::OcaFrequency>(0.0));
    OcaLiteMediaClockRate rate5(static_cast< ::OcaFrequency>(192000.0), 
                                static_cast< ::OcaFrequency>(0.0), 
                                static_cast< ::OcaFrequency>(1.920), 
                                static_cast< ::OcaFrequency>(0.0));
    OcaLiteList< ::OcaLiteMediaClockRate> clockList;
    pDanteDeviceClocking clock = DanteLiteHostInterfaceGetDanteDanteDeviceClocking();
    pDanteClockStatus clockStatus = DanteLiteHostInterfaceGetDanteDanteDanteClockStatus();
    // sanity check on clocks
    OcaLiteMediaClockRate currentRate;
    if(!clock->NumberOfSupportedRates) {
        // we don't have any supported rates, so we make one up
        clockList.Add(rate1);
        currentRate = rate1;
    } else {
        if(clock->NumberOfSupportedRates > 6)
            clock->NumberOfSupportedRates = 6;
        // add the supported rates
        int numberRates = 0;
        for(unsigned i = 0; i < clock->NumberOfSupportedRates; i++) {
            switch(clock->SupportedSampleRates[i]) {
            case 44100:
                clockList.Add(rate0);
                numberRates++;
                break;
            case 48000:
                clockList.Add(rate1);
                numberRates++;
                break;
            case 88200:
                clockList.Add(rate2);
                numberRates++;
                break;
            case 96000:
                clockList.Add(rate3);
                numberRates++;
                break;
            case 176400:
                clockList.Add(rate4);
                numberRates++;
                break;
            case 192000:
                clockList.Add(rate5);
                numberRates++;
                break;
            default:
                printf("Media Clock: Unsupported rate: %d\n", clock->SupportedSampleRates[i]);
                break;
            }
        }
        // now get the current rate
        switch(clock->CurrentSampleRate) {
        case 44100:
            currentRate = rate0;
            break;
        case 48000:
            currentRate = rate1;
            break;
        case 88200:
            currentRate = rate2;
            break;
        case 96000:
            currentRate = rate3;
            break;
        case 176400:
            currentRate = rate4;
            break;
        case 192000:
            currentRate = rate5;
            break;
        default:
            printf("Media Clock: Invalid Current rate: %d\n", clock->CurrentSampleRate);
            currentRate = rate1;
            break;
        }
    }
    DanteLiteOcaMediaClock *mediaClock = new DanteLiteOcaMediaClock(streamEndONo,
                                                                static_cast< ::OcaBoolean>(true),
                                                                ::OcaLiteString("DanteMediaClock"),
                                                                OCAMEDIACLOCKTYPE_INTERNAL,
                                                                static_cast< ::OcaUint16>(0),
                                                                clockList, currentRate);
    mediaClock->Initialize();
    m_Objects[obj++] = mediaClock;
    streamEndONo++;

#ifdef DANTE_CM_METERING_SUBSCRIPTIONS
    m_OcaMeterObjStart = streamEndONo;

    ::OcaONo meterONo = streamStartONo+100;
    ::OcaONo levelONo = streamStartONo+200;
    ::OcaONo phantomONo = streamStartONo+300;
    // Now create the meter objects, physical inputs first (network Tx)
    int meterStart = 0;
    OcaLiteList<OcaLitePort> gPorts;
    for(int i = 0; i < nrTxChannels; i++) {
        char name[32];
        #if 0
        sprintf(name, "asi:node:dante.out.%d", i+1);
        // Create container block
        OcaLiteBlock *blk = new OcaLiteNodeBlock( static_cast< ::OcaONo>(streamEndONo+100),
                                                        static_cast< ::OcaBoolean>(false),
                                                        static_cast<OcaLiteString>(name),
                                                        gPorts);
        //reinterpret_cast<OcaLiteBlock *>(OcaLiteBlock::GetRootBlock().GetObject(100))->AddObject(*blk);
        //::OcaLiteBlock::GetRootBlock().AddObject(*blk);
        #endif
        sprintf(name, "asi:ctrl:dante.out.%d.meter", i+1);
        // create object
        OCALiteDanteLvlSensor *lvl = new OCALiteDanteLvlSensor( static_cast< ::OcaONo>(meterONo++),
                                                        static_cast< ::OcaBoolean>(false),
                                                        static_cast<OcaLiteString>(name),
                                                        gPorts,
                                                        static_cast< ::OcaDB>(-128),
                                                        static_cast< ::OcaDB>(1));
        //blk->AddObject(*lvl);
        m_Meters[meterStart] = lvl;
        // initialize
        reinterpret_cast<OCALiteDanteLvlSensor *>(m_Meters[meterStart])->Initialize();

        OcaLiteIyoPhantomActuator *ph = OcaLiteIyoPhantomActuator::build_new(phantomONo++, i+1);
        ::OcaLiteBlock::GetRootBlock().AddObject(*ph);

        OcaLiteIyoLevelActuator *l = OcaLiteIyoLevelActuator::build_new(levelONo++, true, i+1);
        ::OcaLiteBlock::GetRootBlock().AddObject(*l);

        // increment
        meterStart++;
    }
    // Physical Outputs next (network Rx)
    for(int i = 0; i < nrRxChannels; i++) {
        char name[32];
        #if 0
        sprintf(name, "asi:node:dante.in.%d", i+1);
        // Create container block
        OcaLiteBlock *blk = new OcaLiteNodeBlock( static_cast< ::OcaONo>(streamEndONo+200),
                                                        static_cast< ::OcaBoolean>(false),
                                                        static_cast<OcaLiteString>(name),
                                                        gPorts);
        //reinterpret_cast<OcaLiteBlock *>(OcaLiteBlock::GetRootBlock().GetObject(100))->AddObject(*blk);
        //::OcaLiteBlock::GetRootBlock().AddObject(*blk);
        #endif
        sprintf(name, "asi:ctrl:dante.in.%d.meter", i+1);
        // create object
        OCALiteDanteLvlSensor *lvl = new OCALiteDanteLvlSensor( static_cast< ::OcaONo>(meterONo++),
                                                        static_cast< ::OcaBoolean>(false),
                                                        static_cast<OcaLiteString>(name),
                                                        gPorts,
                                                        static_cast< ::OcaDB>(-128),
                                                        static_cast< ::OcaDB>(1));
        //blk->AddObject(*lvl);
        m_Meters[meterStart] = lvl;
        // initialize
        reinterpret_cast<OCALiteDanteLvlSensor *>(m_Meters[meterStart])->Initialize();

        OcaLiteIyoLevelActuator *l = OcaLiteIyoLevelActuator::build_new(levelONo++, false, i+1);
        ::OcaLiteBlock::GetRootBlock().AddObject(*l);

        // increment
        meterStart++;
    }

#if 0
    OcaLiteList<OcaLitePort> gPortsb;
    OcaLiteBlock *blk = new OcaLiteNodeBlock( static_cast< ::OcaONo>(streamEndONo++),
                                                    static_cast< ::OcaBoolean>(false),
                                                    static_cast<OcaLiteString>("Block 1"),
                                                    gPortsb);
    ::OcaLiteBlock::GetRootBlock().AddObject(*blk);
#endif
    m_NumberOfMeters = meterStart;
    m_NumberOfInputMeters  = nrTxChannels;
    m_NumberOfOutputMeters = nrRxChannels;
#endif // DANTE_CM_METERING_SUBSCRIPTIONS

    assert(obj == numberObjects);
    printf("OCALiteOCCFactory(), Created %d objects and %d (%d, %d) meters attached to the Root block\n", 
            obj, m_NumberOfMeters, m_NumberOfInputMeters, m_NumberOfOutputMeters);
    mInitialized = true;
    return status;
}

OcaLiteStatus OCALiteOCCFactoryDestroy()
{
    OcaLiteStatus status(OCASTATUS_OK);
    // Remove objects
    for(int i = 0; i < kNumberObjects; i++) {
        // De-register with our device block
        reinterpret_cast<OcaLiteBlock *>(OcaLiteBlock::GetRootBlock().GetObject(100))->RemoveObject(m_Objects[i]->GetObjectNumber());
        delete (m_Objects[i]);
    }
    // Remove meters
    for(int i = 0; i < m_NumberOfMeters; i++) {
        // De-register with our device block
        reinterpret_cast<OcaLiteBlock *>(OcaLiteBlock::GetRootBlock().GetObject(100))->RemoveObject(m_Meters[i]->GetObjectNumber());
        delete (m_Meters[i]);
    }
    m_Meters = NULL;
    mInitialized = false;
    return status;
}

#ifdef DANTE_CM_METERING_SUBSCRIPTIONS
OcaLiteStatus OcaLiteFactoryDanteDeviceMeterUpdate(bool allMeters, int nTx, void *tx, int nRx, void *rx, int reference)
{
    OcaUint8 *txMeters, *rxMeters, value;
    OcaLiteStatus status(OCASTATUS_OK);
    txMeters = (OcaUint8 *)tx;
    rxMeters = (OcaUint8 *)rx;

    if(nTx) {
        // Physical inputs to Brooklyn (network output streams)
        OcaUint32 start(0), end(0), mtr(0), objbase(0);
        if(!allMeters) {
            // only one meter
            start = reference;
            end = reference;
        } else {
            start = 0;
            end = nTx > m_NumberOfInputMeters ? m_NumberOfInputMeters : nTx;
        }
        for(mtr = start; mtr < end; mtr++) {
            // work out object number
            objbase = mtr;
            value = txMeters[mtr];
            reinterpret_cast<OCALiteDanteLvlSensor *>(m_Meters[objbase])->updateMeter(value);
        }
    }
    if(nRx) {
        // Physical outputs from Brooklyn (network input streams)
        OcaUint32 start(0), end(0), mtr(0), objbase(0);
        if(!allMeters) {
            // only one meter
            start = reference;
            end = reference;
        } else {
            start = 0;
            end = nRx > m_NumberOfOutputMeters ? m_NumberOfOutputMeters : nRx;
        }
        for(mtr = start; mtr < end; mtr++) {
            // work out object number
            objbase = m_NumberOfInputMeters + mtr;
            value = rxMeters[mtr];
            reinterpret_cast<OCALiteDanteLvlSensor *>(m_Meters[objbase])->updateMeter(value);
        }
    }
    return status;
}

// Go through the meters and process if there's a value change
// NOTE: Metering is only functional with declaration of DANTE_CM_METERING_SUBSCRIPTIONS
void RunEventPropertyChanges(void)
{
    static OcaInt32 mChannelCounter = 0;
    if (mChannelCounter < static_cast< OcaInt32>(m_NumberOfInputMeters > m_NumberOfOutputMeters ? m_NumberOfInputMeters : m_NumberOfOutputMeters))
    {
        if(mChannelCounter < m_NumberOfInputMeters)
            reinterpret_cast<OCALiteDanteLvlSensor *>(m_Meters[mChannelCounter])->Run();
        if(mChannelCounter < m_NumberOfOutputMeters)
            reinterpret_cast<OCALiteDanteLvlSensor *>(m_Meters[mChannelCounter + m_NumberOfInputMeters])->Run();
        mChannelCounter++;
    }
    else
    {
        mChannelCounter = 0;
    }

}

extern "C" {

// our metering callback from the Dante API
void conmon_cb_metering_message(conmon_client_t * client, 
                                conmon_channel_type_t type, 
                                conmon_channel_direction_t channel_direction, 
                                const conmon_message_head_t * head, 
                                const conmon_message_body_t * body)
{
    const conmon_metering_message_peak_t * tx_peaks = NULL;
    const conmon_metering_message_peak_t * rx_peaks = NULL;
    conmon_metering_message_version_t version;
    uint16_t num_txchannels;
    uint16_t num_rxchannels;
    conmon_instance_id_t instance_id;
    aud_error_t result;
    aud_errbuf_t errbuf;

    const char * name;

    if(!mInitialized)
        return;

    if (!conmon_vendor_id_equals(conmon_message_head_get_vendor_id(head), CONMON_VENDOR_ID_AUDINATE)) {
        return;
    }
    
    conmon_message_head_get_instance_id(head, &instance_id);
    name = conmon_client_device_name_for_instance_id(client, &instance_id);


    result = conmon_metering_message_parse(body, &version, &num_txchannels, &num_rxchannels);
    if (result != AUD_SUCCESS) {
        printf("Error parsing metering message header: %s\n", aud_error_message(result, errbuf));
        return;
    }
    if(num_txchannels)
        tx_peaks = conmon_metering_message_get_peaks_const(body, CONMON_CHANNEL_DIRECTION_TX);
    if(num_rxchannels)
        rx_peaks = conmon_metering_message_get_peaks_const(body, CONMON_CHANNEL_DIRECTION_RX);
    // Call the metering handler
    OcaLiteFactoryDanteDeviceMeterUpdate(true, num_txchannels, (void *)tx_peaks, num_rxchannels, (void *)rx_peaks, 0);
}

}

#endif // DANTE_CM_METERING_SUBSCRIPTIONS
