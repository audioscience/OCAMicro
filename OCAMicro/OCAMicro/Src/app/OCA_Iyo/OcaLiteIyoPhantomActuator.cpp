
// ---- Include system wide include files ----

// ---- Include local include files ----
#include "OcaLiteIyoPhantomActuator.h"

// ---- Helper types and constants ----

// ---- Helper functions ----

// ---- Local data ----

// ---- Class Implementation ----

OcaLiteIyoPhantomActuator *OcaLiteIyoPhantomActuator::build_new(::OcaONo objectNumber, ::OcaUint16 input_index)
{
    char role_str[64];
    snprintf(role_str, sizeof(role_str), "asi:ctrl:analog.in.%hu.phantom", input_index);
    return new OcaLiteIyoPhantomActuator(objectNumber, ::OcaLiteString(role_str), input_index);
}

OcaLiteIyoPhantomActuator::OcaLiteIyoPhantomActuator(::OcaONo objectNumber, const ::OcaLiteString& role, ::OcaUint16 input_index)
    :   OcaLiteBooleanActuator(objectNumber,
                             static_cast< ::OcaBoolean>(true),
                             role,
                             ::OcaLiteList< ::OcaLitePort>()),
        m_input_index(input_index)
{
}

::OcaLiteStatus OcaLiteIyoPhantomActuator::SetSettingValue(::OcaBoolean setting)
{
    ::OcaLiteStatus rc(OCASTATUS_OK);

    bool changed = m_setting != setting;
    m_setting = setting;
    if (changed) {
        // do something
    }
    return rc;
}
