#ifndef XMLREADER_H
#define XMLREADER_H

#include <ConfigurationFile.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlstring.h>

#define X_NODE_ROOT "root"
#define X_NODE_CONTROLLER "controller"
#define X_NODE_CONFIGURATION "configuration"

#define X_NODE_TRIGGER "trigger"
#define X_NODE_BUTTON_MAP "button_map"
#define X_NODE_AXIS_MAP "axis_map"

#define X_NODE_DEVICE "device"
#define X_NODE_EVENT "event"
#define X_NODE_AXIS "axis"
#define X_NODE_BUTTON "button"

#define X_ATTR_ID "id"
#define X_ATTR_TYPE "type"
#define X_ATTR_NAME "name"
#define X_ATTR_BUTTON_ID "button_id"
#define X_ATTR_THRESHOLD "threshold"
#define X_ATTR_DEADZONE "dead_zone"
#define X_ATTR_MULTIPLIER "multiplier"
#define X_ATTR_EXPONENT "exponent"

class XmlReader
{
    public:
        XmlReader();
        XmlReader(ConfigurationFile* configFile);
        virtual ~XmlReader();
        void ReadConfigFile(wxString filePath);
    protected:
    private:
        void ProcessRootElement(xmlNode * a_node);
        void ProcessControllerElement(xmlNode * a_node);
        void ProcessConfigurationElement(xmlNode * a_node);
        void ProcessTriggerElement(xmlNode * a_node);
        void ProcessButtonMapElement(xmlNode * a_node);
        void ProcessAxisMapElement(xmlNode * a_node);
        void ProcessButtonElement(xmlNode * a_node);
        void ProcessAxisElement(xmlNode * a_node);
        void ProcessDeviceElement(xmlNode * a_node);
        void ProcessEventElement(xmlNode * a_node);
        Event m_TempEvent;
        Device m_TempDevice;
        ButtonMapper m_TempButtonMapper;
        AxisMapper m_TempAxisMapper;
        Trigger m_TempTrigger;
        Configuration m_TempConfiguration;
        Controller m_TempController;
        ConfigurationFile m_TempConfigurationFile;
        ConfigurationFile* m_ConfigurationFile;
};

#endif // XMLREADER_H
