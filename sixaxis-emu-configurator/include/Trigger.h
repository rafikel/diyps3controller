#ifndef TRIGGER_H
#define TRIGGER_H

#include <Device.h>
#include <Event.h>

class Trigger
{
    public:
        Trigger();
        Trigger(wxString dtype, wxString did, wxString dname, wxString eid, wxString switchback, unsigned short delay);
        virtual ~Trigger();
        Trigger(const Trigger& other);
        Trigger& operator=(const Trigger& other);
        Device* GetDevice() { return &m_Device; }
        void SetDevice(Device val) { m_Device = val; }
        Event* GetEvent() { return &m_Event; }
        void SetEvent(Event val) { m_Event = val; }
        wxString GetSwitchBack() { return m_SwitchBack; }
        void SetSwitchBack(wxString val) { m_SwitchBack = val; }
        unsigned short GetDelay() { return m_Delay; }
        void SetDelay(unsigned short val) { m_Delay = val; }
    protected:
    private:
        Device m_Device;
        Event m_Event;
        wxString m_SwitchBack;
        unsigned short m_Delay;
};

#endif // TRIGGER_H
