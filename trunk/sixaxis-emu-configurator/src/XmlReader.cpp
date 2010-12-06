#include "XmlReader.h"
#include <stdexcept>
using namespace std;

XmlReader::XmlReader()
{
    //ctor
    m_ConfigurationFile = NULL;
}

XmlReader::XmlReader(ConfigurationFile* configFile)
{
    //ctor
    m_ConfigurationFile = configFile;
}

XmlReader::~XmlReader()
{
    //dtor
}

void XmlReader::ProcessEventElement(xmlNode * a_node)
{
    wxString type;
    wxString id;
    wxString threshold;
    wxString deadZone;
    wxString multiplier;
    wxString exponent;

    type = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_TYPE), wxConvUTF8);
    id = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_ID), wxConvUTF8);
    threshold = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_THRESHOLD), wxConvUTF8);
    deadZone = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_DEADZONE), wxConvUTF8);
    multiplier = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_MULTIPLIER), wxConvUTF8);
    exponent = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_EXPONENT), wxConvUTF8);

    m_TempEvent.SetType(type);
    m_TempEvent.SetId(id);
    m_TempEvent.SetThreshold(threshold);
    m_TempEvent.SetDeadZone(deadZone);
    m_TempEvent.SetMultiplier(multiplier);
    m_TempEvent.SetExponent(exponent);
}

void XmlReader::ProcessDeviceElement(xmlNode * a_node)
{
    wxString type;
    wxString id;
    wxString name;

    type = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_TYPE), wxConvUTF8);
    id = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_ID), wxConvUTF8);
    name = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_NAME), wxConvUTF8);

    m_TempDevice.SetType(type);
    m_TempDevice.SetId(id);
    m_TempDevice.SetName(name);
}

void XmlReader::ProcessAxisElement(xmlNode * a_node)
{
    xmlNode* cur_node = NULL;
    wxString id;

    id = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_ID), wxConvUTF8);

    for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_DEVICE))
            {
                ProcessDeviceElement(cur_node);
                break;
            }
            else
            {
                string message(string("bad element name: ") + string((char*)cur_node->name));
                throw invalid_argument(message);
            }
        }
    }

    if(!cur_node)
    {
        string message(string("missing device element"));
        throw invalid_argument(message);
    }

    for (cur_node = cur_node->next; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_EVENT))
            {
                ProcessEventElement(cur_node);
                break;
            }
            else
            {
                string message(string("bad element name: ") + string((char*)cur_node->name));
                throw invalid_argument(message);
            }
        }
    }

    if(!cur_node)
    {
        string message(string("missing event element"));
        throw invalid_argument(message);
    }

    m_TempAxisMapper.SetAxis(id);
    m_TempAxisMapper.SetDevice(m_TempDevice);
    m_TempAxisMapper.SetEvent(m_TempEvent);
}

void XmlReader::ProcessButtonElement(xmlNode * a_node)
{
    xmlNode* cur_node = NULL;
    wxString id;

    id = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_ID), wxConvUTF8);

    for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_DEVICE))
            {
                ProcessDeviceElement(cur_node);
                break;
            }
            else
            {
                string message(string("bad element name: ") + string((char*)cur_node->name));
                throw invalid_argument(message);
            }
        }
    }

    if(!cur_node)
    {
        string message(string("missing device element"));
        throw invalid_argument(message);
    }

    for (cur_node = cur_node->next; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_EVENT))
            {
                ProcessEventElement(cur_node);
                break;
            }
            else
            {
                string message(string("bad element name: ") + string((char*)cur_node->name));
                throw invalid_argument(message);
            }
        }
    }

    if(!cur_node)
    {
        string message(string("missing event element"));
        throw invalid_argument(message);
    }

    m_TempButtonMapper.SetButton(id);
    m_TempButtonMapper.SetDevice(m_TempDevice);
    m_TempButtonMapper.SetEvent(m_TempEvent);
}

void XmlReader::ProcessAxisMapElement(xmlNode * a_node)
{
    xmlNode* cur_node = NULL;

    for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_AXIS))
            {
                ProcessAxisElement(cur_node);
            }
            else
            {
                string message(string("bad element name: ") + string((char*)cur_node->name));
                throw invalid_argument(message);
            }
        }
    }
    m_TempConfiguration.GetAxisMapperList()->push_back(m_TempAxisMapper);
}

void XmlReader::ProcessButtonMapElement(xmlNode * a_node)
{
    xmlNode* cur_node = NULL;

    for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_BUTTON))
            {
                ProcessButtonElement(cur_node);
            }
            else
            {
                string message(string("bad element name: ") + string((char*)cur_node->name));
                throw invalid_argument(message);
            }
        }
    }
    m_TempConfiguration.GetButtonMapperList()->push_back(m_TempButtonMapper);
}

void XmlReader::ProcessTriggerElement(xmlNode * a_node)
{
    wxString device_type;
    wxString device_id;
    wxString device_name;
    wxString button_id;

    device_type = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_DEVICE_TYPE), wxConvUTF8);
    device_id = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_DEVICE_ID), wxConvUTF8);
    device_name = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_DEVICE_NAME), wxConvUTF8);
    button_id = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_BUTTON_ID), wxConvUTF8);

    m_TempTrigger.SetDevice(Device(device_type, device_id, device_name));
    m_TempTrigger.SetEvent(Event(button_id));

    m_TempConfiguration.SetTrigger(m_TempTrigger);
}

void XmlReader::ProcessConfigurationElement(xmlNode * a_node)
{
    xmlNode* cur_node = NULL;
    unsigned char config_index;
    wxString id;

    id = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_ID), wxConvUTF8);

    config_index = wxAtoi(id) - 1;

    if(config_index >= MAX_CONFIGURATIONS || config_index < 0)
    {
        string message(string("bad configuration id: ") + string(id.mb_str()));
        throw invalid_argument(message);
    }

    cur_node = a_node->children;

    for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_TRIGGER))
            {
                ProcessTriggerElement(cur_node);
                break;
            }
            else
            {
                string message(string("bad element name: ") + string((char*)cur_node->name));
                throw invalid_argument(message);
            }
        }
    }

    if(!cur_node)
    {
        string message(string("missing trigger element"));
        throw invalid_argument(message);
    }

    for (cur_node = cur_node->next; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_BUTTON_MAP))
            {
                ProcessButtonMapElement(cur_node);
                break;
            }
            else
            {
                string message(string("bad element name: ") + string((char*)cur_node->name));
                throw invalid_argument(message);
            }
        }
    }

    if(!cur_node)
    {
        string message(string("missing button_map element"));
        throw invalid_argument(message);
    }

    for (cur_node = cur_node->next; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_AXIS_MAP))
            {
                ProcessAxisMapElement(cur_node);
                break;
            }
            else
            {
                string message(string("bad element name: ") + string((char*)cur_node->name));
                throw invalid_argument(message);
            }
        }
    }

    if(!cur_node)
    {
        string message(string("missing axis_map element"));
        throw invalid_argument(message);
    }

    m_TempController.SetConfiguration(m_TempConfiguration, config_index);
}

void XmlReader::ProcessControllerElement(xmlNode * a_node)
{
    xmlNode* cur_node = NULL;
    unsigned char controller_index;
    wxString id;

    id = wxString((char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_ID), wxConvUTF8);

    controller_index = wxAtoi(id) - 1;

    if(controller_index >= MAX_CONTROLLERS || controller_index < 0)
    {
        string message(string("bad controller id: ") + string(id.mb_str()));
        throw invalid_argument(message);
    }

    for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_CONFIGURATION))
            {
                ProcessConfigurationElement(cur_node);
            }
            else
            {
                string message(string("bad element name: ") + string((char*)cur_node->name));
                throw invalid_argument(message);
            }
        }
    }
    m_TempConfigurationFile.SetController(m_TempController, controller_index);
}

void XmlReader::ProcessRootElement(xmlNode * a_node)
{
  xmlNode *cur_node = NULL;

  if (a_node->type == XML_ELEMENT_NODE)
  {
    if (xmlStrEqual(a_node->name, (xmlChar*) X_NODE_ROOT))
    {
      for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
      {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
          if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_CONTROLLER))
          {
            ProcessControllerElement(cur_node);
          }
          else
          {
            string message(string("bad element name: ") + string((char*)cur_node->name));
            throw invalid_argument(message);
          }
        }
      }
    }
    else
    {
      string message(string("bad element name: ") + string((char*)a_node->name));
      throw invalid_argument(message);
    }
  }
}

void XmlReader::ReadConfigFile(wxString filePath)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    try
    {
        /*parse the file and get the DOM */
        doc = xmlReadFile(filePath.mb_str(), NULL, 0);

        if (doc != NULL)
        {
            /*Get the root element node */
            root_element = xmlDocGetRootElement(doc);

            if(root_element != NULL)
            {
                ProcessRootElement(root_element);
                *m_ConfigurationFile = m_TempConfigurationFile;
            }
            else
            {
                throw invalid_argument("error: no root element");
            }
        }
        else
        {
            throw invalid_argument("error: could not parse file ");
        }
    }
    catch(exception& e)
    {
        wxMessageBox(wxString(e.what(), wxConvUTF8));
    }

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();
}
