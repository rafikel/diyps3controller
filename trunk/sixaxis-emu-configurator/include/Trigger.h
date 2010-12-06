#ifndef TRIGGER_H
#define TRIGGER_H

#include <Device.h>
#include <Event.h>

class Trigger
{
    public:
        Trigger();
        Trigger(wxString dtype, wxString did, wxString dname, wxString eid);
        virtual ~Trigger();
        Trigger(const Trigger& other);
        Trigger& operator=(const Trigger& other);
        Device* GetDevice() { return &m_Device; }
        void SetDevice(Device val) { m_Device = val; }
        Event* GetEvent() { return &m_Event; }
        void SetEvent(Event val) { m_Event = val; }
    protected:
    private:
        Device m_Device;
        Event m_Event;
};

#endif // TRIGGER_H
