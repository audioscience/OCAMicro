
#ifndef OCALITEIYOLEVELACTUATOR_H
#define OCALITEIYOLEVELACTUATOR_H

// ---- Include system wide include files ----
#include <OCC/ControlClasses/Workers/Actuators/OcaLiteGain.h>

// ---- Include local include files ----

// ---- Referenced classes and types ----

// ---- Helper types and constants ----

// ---- Helper functions ----

// ---- Class Definition ---
/**
 * Handles line level for the Iyo.
 */
class OcaLiteIyoLevelActuator : public ::OcaLiteGain
{
public:
    /**
     * Constructor
     *
     * @param[in] objectNumber  The objectNumber.
     * @param[in] relay        The relay being controlled
     */
    OcaLiteIyoLevelActuator(::OcaONo objectNumber, const ::OcaLiteString& role, ::OcaDB minGain, ::OcaDB maxGain, bool is_input, ::OcaUint16 input_index);

    /**
     * Destructor
     */
    virtual ~OcaLiteIyoLevelActuator() {}

    static OcaLiteIyoLevelActuator *build_new(::OcaONo objectNumber, bool is_input, ::OcaUint16 input_index);

protected:

    ::OcaLiteStatus GetGainValue(::OcaDB& gain) const;

    ::OcaLiteStatus SetGainValue(::OcaDB gain);
    

private:

    /** Private copy constructor to prevent copying. */
    OcaLiteIyoLevelActuator(const ::OcaLiteIyoLevelActuator&);
    /** Private assignment operator to prevent assignment. */
    ::OcaLiteIyoLevelActuator& operator=(const ::OcaLiteIyoLevelActuator& source);

    ::OcaDB m_gain_value;
    bool m_is_input;
    ::OcaUint16 m_input_index;
};

#endif //OCALITEIYOLEVELACTUATOR_H
