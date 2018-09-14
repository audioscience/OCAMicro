/*  By downloading or using this file, the user agrees to be bound by the terms of the license 
 *  agreement located at http://ocaalliance.com/EULA as an original contracting party.
 */

// OCALite.cpp : Defines the entry point for the console application.
//


#include <HostInterfaceLite/OCA/OCF/OcfLiteHostInterface.h>
#include <HostInterfaceLite/OCA/OCF/Timer/IOcfLiteTimer.h>
#include <HostInterfaceLite/OCA/OCP.1/Ocp1LiteHostInterface.h>
#include <OCC/ControlClasses/Workers/BlocksAndMatrices/OcaLiteBlock.h>
#include <OCC/ControlClasses/Managers/OcaLiteDeviceManager.h>
#include <OCC/ControlClasses/Managers/OcaLiteNetworkManager.h>
#include <OCC/ControlClasses/Managers/OcaLiteSubscriptionManager.h>
#include <OCC/ControlClasses/Managers/OcaLiteFirmwareManager.h>
#include <OCC/ControlClasses/Managers/OcaLiteMediaClockManager.h>
#include <OCF/OcaLiteCommandHandler.h>
#include <OCP.1/Ocp1LiteNetwork.h>
#include <OCP.1/Ocp1LiteNetworkSystemInterfaceID.h>
#include <StandardLib/StandardLib.h>

//#include "OcaLiteStreamNetworkDante.h"
//#include "DanteLiteHostInterface.h"

#ifdef OCA_RUN
extern void Ocp1LiteServiceRun();
#else
extern int Ocp1LiteServiceGetSocket();
extern void Ocp1LiteServiceRunWithFdSet(fd_set* readSet);
#endif // OCA_RUN

void InitializePool();
void ShutdownPool();

extern OcaLiteStatus OCALiteOCCFactoryCreate();
extern OcaLiteStatus OCALiteOCCFactoryDestroy();
extern void RunEventPropertyChanges(void);

int main(int argc, const char* argv[])
{
    std::string manufacturer, name, version;
    char *devNameOCA;
    manufacturer.assign("OCAAlliance");
    name.assign("OCAMicroTestDevice");
    version.assign("1.0.0");
    // set device information
    OcfLiteConfigureSetModelGUID(0x123456, 0x01);
    OcfLiteConfigureSetModelDescription(manufacturer, name, version);
    // Initialize the host interfaces
    bool bSuccess = ::OcfLiteHostInterfaceInitialize();
    bSuccess = bSuccess && ::Ocp1LiteHostInterfaceInitialize();
    // Are we connecting to a local or remote Dante device?
    if(argc > 1)
    {
        printf("Device Name passed in command line\n");
        devNameOCA = (char *)argv[1];
    }
    else
    {
        devNameOCA = (char *)"testdev";
    }
    printf("Trying to connect to <%s>\n", devNameOCA);

    static_cast<void>(::OcaLiteBlock::GetRootBlock());
    bSuccess = bSuccess && static_cast<bool>(::OcaLiteNetworkManager::GetInstance().Initialize());
    bSuccess = bSuccess && static_cast<bool>(::OcaLiteSubscriptionManager::GetInstance().SetNrEvents(1/*OCA_NR_EVENTS*/));
    bSuccess = bSuccess && static_cast<bool>(::OcaLiteSubscriptionManager::GetInstance().Initialize());
    bSuccess = bSuccess && static_cast<bool>(::OcaLiteDeviceManager::GetInstance().Initialize());
    bSuccess = bSuccess && static_cast<bool>(::OcaLiteFirmwareManager::GetInstance().Initialize());
    bSuccess = bSuccess && static_cast<bool>(::OcaLiteMediaClockManager::GetInstance().Initialize());

    if (bSuccess)
    {
        Ocp1LiteNetworkSystemInterfaceID interfaceId = ::Ocp1LiteNetworkSystemInterfaceID(static_cast< ::OcaUint32>(0));
        std::vector<std::string> txtRecords;
        txtRecords.push_back(name.c_str());
        name += "@";
        ::OcaLiteString nodeId = ::OcaLiteString(name.c_str() + OcfLiteConfigureGetDeviceName());
        ::Ocp1LiteNetwork* ocp1Network = new ::Ocp1LiteNetwork(static_cast< ::OcaONo>(0x1000), static_cast< ::OcaBoolean>(true), 
                                                               ::OcaLiteString("Ocp1LiteNetwork"), ::Ocp1LiteNetworkNodeID(nodeId),
                                                               interfaceId, txtRecords, ::OcaLiteString("local"), OcaInt16(65000U));
        if (ocp1Network->Initialize())
        {
            OCALiteOCCFactoryCreate();
            if (::OcaLiteBlock::GetRootBlock().AddObject(*ocp1Network))
            {
                OcaLiteStatus rc(ocp1Network->Startup());
                if (OCASTATUS_OK == rc)
                {
                    bSuccess = bSuccess && ::OcaLiteCommandHandler::GetInstance().Initialize();
                    ::OcaLiteDeviceManager::GetInstance().SetErrorAndOperationalState(static_cast< ::OcaBoolean>(!bSuccess), ::OcaLiteDeviceManager::OCA_OPSTATE_OPERATIONAL);
                    ::OcaLiteDeviceManager::GetInstance().SetEnabled(static_cast< ::OcaBoolean>(bSuccess));

                    INT32 highestFd;
                    OcfLiteSelectableSet readSet;
                    OcfLiteSelectableSet writeSet;
                    OcfLiteSelectableSet exceptSet;
                    bool pendingMessageAvailable(false);
                    while (bSuccess)
                    {
#ifdef OCA_RUN
                        ::OcaLiteCommandHandler::GetInstance().RunWithTimeout(100);
                        Ocp1LiteServiceRun();
#else
                        FD_ZERO(&readSet);
                        FD_ZERO(&writeSet);
                        FD_ZERO(&exceptSet);

                        pendingMessageAvailable = ::OcaLiteCommandHandler::GetInstance().AddSelectables(highestFd, readSet, writeSet, exceptSet);

                        int serviceSocket = Ocp1LiteServiceGetSocket();
                        if (-1 != serviceSocket)
                        {
                            FD_SET(serviceSocket, &readSet);
                            if (serviceSocket > highestFd)
                            {
                                highestFd = serviceSocket;
                            }
                        }
                        timeval timeout = { 0, pendingMessageAvailable ? 0 : 10000};
                        select(highestFd + 1, &readSet, &writeSet, &exceptSet, &timeout);

                        // We always need to Run OCA because of keep alive handling.
                        ::OcaLiteCommandHandler::GetInstance().RunWithSelectSet(readSet, writeSet, exceptSet);

                        if (FD_ISSET(serviceSocket, &readSet))
                        {
                            // Service only needs to run when something usefull to do.
                            Ocp1LiteServiceRunWithFdSet(&readSet);
                        }

#endif // OCA_RUN
                    }
                }
            }
            OCALiteOCCFactoryDestroy();
        }
    }
    return 0;
}

