#include "Controller.h"

Controller::Controller()
{
    //ctor
}

Controller::~Controller()
{
    //dtor
}

Controller::Controller(const Controller& other)
{
    for(unsigned int i=0; i<sizeof(m_Configurations)/sizeof(Configuration); ++i)
    {
        m_Configurations[i] = other.m_Configurations[i];
    }
}

Controller& Controller::operator=(const Controller& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    for(unsigned int i=0; i<sizeof(m_Configurations)/sizeof(Configuration); ++i)
    {
        m_Configurations[i] = rhs.m_Configurations[i];
    }
    return *this;
}
