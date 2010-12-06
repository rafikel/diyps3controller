#include "Trigger.h"

Trigger::Trigger()
{
    //ctor
}

Trigger::Trigger(wxString dtype, wxString did, wxString dname, wxString eid):m_Device(dtype, did, dname), m_Event(eid)
{
    //ctor
}

Trigger::~Trigger()
{
    //dtor
}

Trigger::Trigger(const Trigger& other):m_Device(other.m_Device), m_Event(other.m_Event)
{
    //copy ctor
}

Trigger& Trigger::operator=(const Trigger& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    m_Device = rhs.m_Device;
    m_Event = rhs.m_Event;
    return *this;
}
