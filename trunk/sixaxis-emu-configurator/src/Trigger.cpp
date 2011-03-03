#include "Trigger.h"

Trigger::Trigger()
{
    //ctor
}

Trigger::Trigger(wxString dtype, wxString did, wxString dname, wxString eid, wxString switchback):m_Device(dtype, did, dname), m_Event(eid), m_SwitchBack(switchback)
{
    //ctor
}

Trigger::~Trigger()
{
    //dtor
}

Trigger::Trigger(const Trigger& other):m_Device(other.m_Device), m_Event(other.m_Event), m_SwitchBack(other.m_SwitchBack)
{
    //copy ctor
}

Trigger& Trigger::operator=(const Trigger& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    m_Device = rhs.m_Device;
    m_Event = rhs.m_Event;
    m_SwitchBack = rhs.m_SwitchBack;
    return *this;
}
