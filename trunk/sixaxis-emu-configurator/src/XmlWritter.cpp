#include "XmlWritter.h"

XmlWritter::XmlWritter()
{
    //ctor
    m_ConfigurationFile = NULL;
}

XmlWritter::XmlWritter(ConfigurationFile* configFile)
{
    //ctor
    m_ConfigurationFile = configFile;
    m_CurrentController = 0;
    m_CurrentConfiguration = 0;
}

XmlWritter::~XmlWritter()
{
    //dtor
}

void XmlWritter::CreateEventNode(xmlNodePtr parent_node)
{

}

void XmlWritter::CreateDeviceNode(xmlNodePtr parent_node)
{

}

void XmlWritter::CreateAxisMapNode(xmlNodePtr parent_node)
{

}

void XmlWritter::CreateButtonMapNode(xmlNodePtr parent_node)
{

}

void XmlWritter::CreateTriggerNode(xmlNodePtr parent_node)
{
    xmlNodePtr node;
    Trigger* trigger = m_ConfigurationFile->GetController(m_CurrentController)->GetConfiguration(m_CurrentConfiguration)->GetTrigger();

    node = xmlNewChild(parent_node, NULL, BAD_CAST X_NODE_TRIGGER, NULL);

    //xmlNewProp(node, BAD_CAST X_ATTR_DEVICE_TYPE, BAD_CAST trigger->GetDevice()->GetType().mb_str());
}

void XmlWritter::CreateConfigurationNodes(xmlNodePtr parent_node)
{
    xmlNodePtr node;
    char id[2];
    int i;

    for(i=0; i<MAX_CONFIGURATIONS; ++i)
    {
        m_CurrentConfiguration = i;

        node = xmlNewChild(parent_node, NULL, BAD_CAST X_NODE_CONFIGURATION, NULL);

        snprintf(id, sizeof(id), "%hhu", i);

        xmlNewProp(node, BAD_CAST "id", BAD_CAST id);

        CreateTriggerNode(node);

        CreateButtonMapNode(node);

        CreateAxisMapNode(node);
    }
}

void XmlWritter::CreateControllerNodes(xmlNodePtr parent_node)
{
    xmlNodePtr node;
    char id[2];
    int i;

    for(i=0; i<MAX_CONTROLLERS; ++i)
    {
        m_CurrentController = i;

        node = xmlNewChild(parent_node, NULL, BAD_CAST X_NODE_CONTROLLER, NULL);

        snprintf(id, sizeof(id), "%hhu", i);

        xmlNewProp(node, BAD_CAST "id", BAD_CAST id);

        CreateConfigurationNodes(node);
    }
}

void XmlWritter::WriteConfigFile()
{
    xmlDocPtr doc = NULL;       /* document pointer */
    xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;/* node pointers */
    char buff[256];
    int i, j;

    LIBXML_TEST_VERSION;

    /*
     * Creates a new document, a node and set it as a root node
     */
    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST X_NODE_ROOT);
    xmlDocSetRootElement(doc, root_node);

    CreateControllerNodes(root_node);

    /*
     * Dumping document to stdio or file
     */
    xmlSaveFormatFileEnc(m_ConfigurationFile->GetFilePath().mb_str(), doc, "UTF-8", 1);

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();
}
