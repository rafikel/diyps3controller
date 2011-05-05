#ifndef INTENSITY_H
#define INTENSITY_H

#include <Device.h>
#include <Event.h>

class Intensity
{
    public:
        Intensity();
        Intensity(wxString dtype, wxString did, wxString dname, wxString eid, unsigned char steps);
        virtual ~Intensity();
        Intensity(const Intensity& other);
        Intensity& operator=(const Intensity& other);
        Device* GetDevice() { return &m_Device; }
        void SetDevice(Device val) { m_Device = val; }
        Event* GetEvent() { return &m_Event; }
        void SetEvent(Event val) { m_Event = val; }
        unsigned char GetSteps() { return m_steps; }
        void SetSteps(unsigned char val) { m_steps = val; }
    protected:
    private:
        Device m_Device;
        Event m_Event;
        unsigned char m_steps;
};

#endif // INTENSITY_H
