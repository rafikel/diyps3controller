#include "Intensity.h"

Intensity::Intensity()
{
    //ctor
}

Intensity::Intensity(wxString dtype, wxString did, wxString dname, wxString eid, unsigned char steps):m_Device(dtype, did, dname), m_Event(eid), m_steps(steps)
{
    //ctor
}

Intensity::~Intensity()
{
    //dtor
}

Intensity::Intensity(const Intensity& other):m_Device(other.m_Device), m_Event(other.m_Event), m_steps(other.m_steps)
{
    //copy ctor
}

Intensity& Intensity::operator=(const Intensity& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    m_Device = rhs.m_Device;
    m_Event = rhs.m_Event;
    m_steps = rhs.m_steps;
    return *this;
}
