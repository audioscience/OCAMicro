
#ifndef OCALITEIYOPHANTOMACTUATOR_H
#define OCALITEIYOPHANTOMACTUATOR_H

// ---- Include system wide include files ----
#include <OCC/ControlClasses/Workers/Actuators/OcaLiteBooleanActuator.h>

// ---- Include local include files ----

// ---- Referenced classes and types ----

// ---- Helper types and constants ----

// ---- Helper functions ----

// ---- Class Definition ---
/**
 * Handles Phantom power for the Iyo.
 */
class OcaLiteIyoPhantomActuator : public ::OcaLiteBooleanActuator
{
public:
    /**
     * Constructor
     *
     * @param[in] objectNumber  The objectNumber.
     * @param[in] relay        The relay being controlled
     */
    OcaLiteIyoPhantomActuator(::OcaONo objectNumber, const ::OcaLiteString& role, ::OcaUint16 input_index);

    /**
     * Destructor
     */
    virtual ~OcaLiteIyoPhantomActuator() {}

    static OcaLiteIyoPhantomActuator *build_new(::OcaONo objectNumber, ::OcaUint16 input_index);

protected:

    virtual ::OcaLiteStatus SetSettingValue(::OcaBoolean setting);

    virtual ::OcaLiteStatus GetSetting(::OcaBoolean& setting) const
    {
        setting = m_setting;

        return OCASTATUS_OK;
    }
    

private:

    /** Private copy constructor to prevent copying. */
    OcaLiteIyoPhantomActuator(const ::OcaLiteIyoPhantomActuator&);
    /** Private assignment operator to prevent assignment. */
    ::OcaLiteIyoPhantomActuator& operator=(const ::OcaLiteIyoPhantomActuator& source);

    /** The boolean value */
    ::OcaBoolean m_setting;
    // the channel    
    ::OcaUint16 m_input_index;
};

#endif //OCALITEIYOPHANTOMACTUATOR_H
