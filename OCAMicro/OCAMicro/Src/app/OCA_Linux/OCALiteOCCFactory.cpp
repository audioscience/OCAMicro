/*  By downloading or using this file, the user agrees to be bound by the terms of the license 
 *  agreement located at http://ocaalliance.com/EULA as an original contracting party.
 */

// OCALiteOCCFactory.cpp : Simple stub that creates and destroys the Lite objects for the reference design.
//


#include <HostInterfaceLite/OCA/OCF/OcfLiteHostInterface.h>
#include <HostInterfaceLite/OCA/OCP.1/Ocp1LiteHostInterface.h>
#include <OCC/ControlClasses/Workers/BlocksAndMatrices/OcaLiteBlock.h>

//#include "OcaLiteStreamNetworkDante.h"
//#include "DanteLiteHostInterface.h"
//#include "OcaLiteDanteMediaClock.h"
//#include "OcaLiteDanteLvlSensor.h"

#define kNumberObjects    (2)
static bool mInitialized = false;
static OcaLiteRoot *m_Objects[kNumberObjects];
OcaLiteRoot **m_Meters = NULL;
int m_NumberOfMeters = 0;
int m_NumberOfInputMeters = 0;
int m_NumberOfOutputMeters = 0;
::OcaONo m_OcaMeterObjStart;

OcaLiteStatus OCALiteOCCFactoryCreate()
{
    OcaLiteStatus status(OCASTATUS_OK);
    if(mInitialized)
        return OCASTATUS_BAD_METHOD;

    mInitialized = true;
    return status;
}

OcaLiteStatus OCALiteOCCFactoryDestroy()
{
    OcaLiteStatus status(OCASTATUS_OK);

    mInitialized = false;
    return status;
}
