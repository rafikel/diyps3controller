#ifndef EVENT_CATCHER_H
#define EVENT_CATCHER_H

#define MAX_DEVICES 256

class event_catcher
{
    public:
        event_catcher();
        virtual ~event_catcher();
        wxString GetDeviceType() { return m_DeviceType; }
        wxString GetDeviceName() { return m_DeviceName; }
        wxString GetDeviceId() { return m_DeviceId; }
        wxString GetEventType() { return m_EventType; }
        wxString GetEventId() { return m_EventId; }
        void run(wxString device_type, wxString event_type);
    protected:
    private:
        void init();
        wxString m_DeviceType;
        wxString m_DeviceName;
        wxString m_DeviceId;
        wxString m_EventType;
        wxString m_EventId;
        wxString joystickName[MAX_DEVICES];
        int joystickVirtualIndex[MAX_DEVICES];
        int joystickNbButton[MAX_DEVICES];
        int joystickSixaxis[MAX_DEVICES];
        wxString mouseName[MAX_DEVICES];
        int mouseVirtualIndex[MAX_DEVICES];
        wxString keyboardName[MAX_DEVICES];
        int keyboardVirtualIndex[MAX_DEVICES];
        unsigned int done;
};

#endif // EVENT_CATCHER_H
