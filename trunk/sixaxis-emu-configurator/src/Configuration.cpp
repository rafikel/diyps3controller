#include "Configuration.h"

Configuration::Configuration()
{
    //ctor
}

Configuration::~Configuration()
{
    //dtor
}

Configuration::Configuration(const Configuration& other):m_Trigger(other.m_Trigger), m_LeftIntensity(other.m_LeftIntensity),
    m_RightIntensity(other.m_RightIntensity), m_ButtonMappers(other.m_ButtonMappers), m_AxisMappers(other.m_AxisMappers)
{
    //copy ctor
}

Configuration& Configuration::operator=(const Configuration& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    m_Trigger = rhs.m_Trigger;
    m_LeftIntensity = rhs.m_LeftIntensity;
    m_RightIntensity = rhs.m_RightIntensity;
    m_ButtonMappers = rhs.m_ButtonMappers;
    m_AxisMappers = rhs.m_AxisMappers;
    return *this;
}
