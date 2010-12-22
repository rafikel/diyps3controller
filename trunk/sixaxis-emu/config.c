#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <libxml/xmlreader.h>
#include <string.h>
#include "config.h"
#include <stdlib.h>

static inline int GetUnsignedIntProp(xmlNode * a_node, char* a_attr, unsigned int* a_uint)
{
  char* val;

  val = (char*)xmlGetProp(a_node, (xmlChar*) a_attr);

  if(val)
  {
    *a_uint = atoi(val);
    return 0;
  }
  else
  {
    return -1;
  }
}

static int ProcessEventElement(xmlNode * a_node)
{
  int ret = 0;
  char* type;
  unsigned int id;
  unsigned int threshold;
  unsigned int deadZone;
  unsigned int multiplier;
  unsigned int exponent;

  type = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_TYPE);
  ret = GetUnsignedIntProp(a_node, X_ATTR_ID, &id);
  ret = GetUnsignedIntProp(a_node, X_ATTR_THRESHOLD, &threshold);
  ret = GetUnsignedIntProp(a_node, X_ATTR_DEADZONE, &deadZone);
  ret = GetUnsignedIntProp(a_node, X_ATTR_MULTIPLIER, &multiplier);
  ret = GetUnsignedIntProp(a_node, X_ATTR_EXPONENT, &exponent);

  return ret;
}

static int ProcessDeviceElement(xmlNode * a_node)
{
  int ret = 0;
  char* type;
  char* id;
  char* name;

  type = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_TYPE);
  id = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_ID);
  name = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_NAME);

  //td
  return ret;
}

static int ProcessAxisElement(xmlNode * a_node)
{
  int ret = 0;
  xmlNode* cur_node = NULL;
  unsigned int aid;

  ret = GetUnsignedIntProp(a_node, X_ATTR_ID, &aid);

  for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
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
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }

  if (!cur_node)
  {
    printf("missing device element");
    ret = -1;
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
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }

  if (!cur_node)
  {
    printf("missing event element");
    ret = -1;
  }

  //td
  return ret;
}

static int ProcessButtonElement(xmlNode * a_node)
{
  int ret = 0;
  xmlNode* cur_node = NULL;
  unsigned int bid;

  ret = GetUnsignedIntProp(a_node, X_ATTR_ID, &bid);

  for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
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
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }

  if (!cur_node)
  {
    printf("missing device element");
    ret = -1;
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
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }

  if (!cur_node)
  {
    printf("missing event element");
    ret = -1;
  }

  //td
  return ret;
}

static int ProcessAxisMapElement(xmlNode * a_node)
{
  int ret = 0;
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
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }
  return ret;
}

static int ProcessButtonMapElement(xmlNode * a_node)
{
  int ret = 0;
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
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }
  return ret;
}

static int ProcessTriggerElement(xmlNode * a_node)
{
  int ret = 0;
  char* device_type;
  unsigned int did;
  char* device_name;
  unsigned int bid;

  device_type = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_TYPE);
  ret = GetUnsignedIntProp(a_node, X_ATTR_ID, &did);
  device_name = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_NAME);
  ret = GetUnsignedIntProp(a_node, X_ATTR_ID, &bid);

  //td
  return ret;
}

static int ProcessConfigurationElement(xmlNode * a_node)
{
  int ret = 0;
  xmlNode* cur_node = NULL;
  unsigned int config_index;

  ret = GetUnsignedIntProp(a_node, X_ATTR_ID, &config_index);

  if(ret != -1)
  {
    if (config_index >= MAX_CONFIGURATIONS || config_index < 0)
    {
      printf("bad configuration id: %d\n", config_index);
      ret = -1;
    }
  }

  config_index--;

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
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }

  if (!cur_node)
  {
    printf("missing trigger element");
    ret = -1;
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
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }

  if (!cur_node)
  {
    printf("missing button_map element");
    ret = -1;
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
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }

  if (!cur_node)
  {
    printf("missing axis_map element");
    ret = -1;
  }

  //td
  return ret;
}

static int ProcessControllerElement(xmlNode * a_node)
{
  xmlNode* cur_node = NULL;
  unsigned int controller_index;
  int ret = 0;

  ret = GetUnsignedIntProp(a_node, X_ATTR_ID, &controller_index);

  if(ret != -1)
  {
    if (controller_index >= MAX_CONTROLLERS || controller_index < 0)
    {
      printf("bad controller id: %d\n", controller_index);
      ret = -1;
    }
  }

  controller_index--;

  for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_CONFIGURATION))
      {
        ProcessConfigurationElement(cur_node);
      }
      else
      {
        ret = -1;
        printf("bad element name: %s\n", cur_node->name);
      }
    }
  }
  return ret;
}

static int ProcessRootElement(xmlNode * a_node)
{
  xmlNode *cur_node = NULL;
  int ret = 0;

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
            ret = ProcessControllerElement(cur_node);
          }
          else
          {
            ret = -1;
            printf("bad element name: %s\n", cur_node->name);
          }
        }
      }
    }
    else
    {
      ret = -1;
      printf("bad element name: %s\n", a_node->name);
    }
  }
  return ret;
}

/*
 * This function loads a config file.
 */
static int read_config_file(char* file_path)
{
  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;
  int ret = 0;

  /*
   * this initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  LIBXML_TEST_VERSION

  /*parse the file and get the DOM */
  doc = xmlReadFile(file_path, NULL, 0);

  if (doc != NULL)
  {
    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    if(root_element != NULL)
    {
      ret = ProcessRootElement(root_element);
    }
    else
    {
      ret = -1;
      printf("error: no root element\n");
    }
  }
  else
  {
    ret = -1;
    printf("error: could not parse file %s\n", file_path);
  }

  /*free the document */
  xmlFreeDoc(doc);

  /*
   *Free the global variables that may
   *have been allocated by the parser.
   */
  xmlCleanupParser();

  return ret;

}

/*
 * This function reads all filenames in the config directory
 * and ask the user which one he wans to load.
 */
int read_config_dir()
{
  DIR *dirp;
  struct dirent *d;
  char file_path[PATH_MAX];
  unsigned int i;
  char line[LINE_MAX];
  unsigned int selected = 0;
  unsigned int nb_filenames = 0;
  char** filenames = NULL;

  dirp = opendir(CONFIG_DIR);
  if (dirp == NULL)
    return 0;

  while ((d = readdir(dirp)))
  {
    if (d->d_type == DT_REG)
    {
      nb_filenames++;
      filenames = realloc(filenames, nb_filenames*sizeof(char*));
      filenames[nb_filenames-1] = calloc(strlen(d->d_name)+1, sizeof(char));
      strncpy(filenames[nb_filenames-1], d->d_name, strlen(d->d_name));
    }
  }

  closedir(dirp);

  if(!nb_filenames)
  {
    printf("error: no config file found\n");
    return 0;
  }

  printf("Please choose the config file to load:\n");
  for(i=0; i<nb_filenames; ++i)
  {
    printf("\t%d - %s\n", i, filenames[i]);
  }

  fgets(line, sizeof(line), stdin);
  while(sscanf(line, "%d\n", &selected) != 1 || selected >= nb_filenames)
  {
    printf("Wrong value!");
    fgets(line, sizeof(line), stdin);
  }

  printf("Selected config file: %s\n", filenames[selected]);

  sprintf(file_path, "%s/%s", CONFIG_DIR, filenames[selected]);
  if(read_config_file(file_path) == -1)
  {
    printf("Bad config file: %s\n", filenames[selected]);
    exit(-1);
  }

  for(i=0; i<nb_filenames; ++i)
  {
    free(filenames[i]);
  }
  free(filenames);

  return 1;
}
