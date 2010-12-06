#include "AxisMapper.h"

AxisMapper::AxisMapper()
{
    //ctor
}

AxisMapper::AxisMapper(wxString dtype, wxString did, wxString dname, wxString etype, wxString eid, wxString axis, wxString deadZone, wxString multiplier, wxString exponent):
m_Device(dtype, did, dname), m_Event(etype, eid), m_Axis(axis), m_DeadZone(deadZone), m_Multiplier(multiplier), m_Exponent(exponent)
{
    //ctor
}

AxisMapper::~AxisMapper()
{
    //dtor
}

AxisMapper::AxisMapper(const AxisMapper& other):
m_Device(other.m_Device), m_Event(other.m_Event), m_Axis(other.m_Axis), m_DeadZone(other.m_DeadZone), m_Multiplier(other.m_Multiplier), m_Exponent(other.m_Exponent)
{
    //copy ctor
}

AxisMapper& AxisMapper::operator=(const AxisMapper& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    m_Device = rhs.m_Device;
    m_Event = rhs.m_Event;
    m_Axis = rhs.m_Axis;
    m_DeadZone = rhs.m_DeadZone;
    m_Multiplier = rhs.m_Multiplier;
    m_Exponent = rhs.m_Exponent;
    return *this;
}
