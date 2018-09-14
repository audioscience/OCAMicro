
#include "OcaLiteIyoLevelActuator.h"

OcaLiteIyoLevelActuator *OcaLiteIyoLevelActuator::build_new(::OcaONo objectNumber, bool is_input, ::OcaUint16 input_index)
{
    char role_str[64];
    snprintf(role_str, sizeof(role_str), "asi:ctrl:analog.%s.%hu.level", is_input ? "in" : "out", input_index);
    return new OcaLiteIyoLevelActuator(objectNumber, ::OcaLiteString(role_str), 0, 0, is_input, input_index);
}

OcaLiteIyoLevelActuator::OcaLiteIyoLevelActuator(::OcaONo objectNumber, const ::OcaLiteString& role, ::OcaDB minGain, ::OcaDB maxGain, bool is_input, ::OcaUint16 input_index)
	:	::OcaLiteGain(objectNumber,
	                 static_cast< ::OcaBoolean>(false),
	                 role,
	                 ::OcaLiteList< ::OcaLitePort>(),
	                 minGain, maxGain),
		m_gain_value(0),
		m_is_input(is_input),
		m_input_index(input_index)
{
}

::OcaLiteStatus OcaLiteIyoLevelActuator::GetGainValue(::OcaDB& gain) const
{
	return OCASTATUS_OK;
}

::OcaLiteStatus OcaLiteIyoLevelActuator::SetGainValue(::OcaDB gain)
{
	return OCASTATUS_OK;
}
