/*
 * config_reader.c
 *
 *  Created on: 6 mai 2011
 *      Author: matlo
 */

#include "config_reader.h"
#include "config.h"
#include "conversion.h"
#include <limits.h>
#include <dirent.h>
#include <libxml/xmlreader.h>

#ifdef WIN32
#include <sys/stat.h>
#define LINE_MAX 1024
#endif

#ifndef WIN32
extern char* homedir;
#endif

extern int mean_axis_value;
extern int merge_all_devices;
extern const char* joystickName[MAX_DEVICES];
extern int joystickVirtualIndex[MAX_DEVICES];
extern const char* mouseName[MAX_DEVICES];
extern int mouseVirtualIndex[MAX_DEVICES];
extern const char* keyboardName[MAX_DEVICES];
extern int keyboardVirtualIndex[MAX_DEVICES];
extern s_trigger triggers[MAX_CONTROLLERS][MAX_CONFIGURATIONS];
extern s_intensity left_intensity[MAX_CONTROLLERS][MAX_CONFIGURATIONS];
extern s_intensity right_intensity[MAX_CONTROLLERS][MAX_CONFIGURATIONS];
extern s_mapper* keyboard_buttons[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];
extern s_mapper* mouse_buttons[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];
extern s_mapper* mouse_axis[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];
extern s_mouse_cal mouse_cal[MAX_DEVICES][MAX_CONFIGURATIONS];
extern s_mapper* joystick_buttons[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];
extern s_mapper* joystick_axis[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];

/*
 * These variables are used to read the configuration.
 */
static unsigned int r_controller_id;
static unsigned int r_config_id;
static e_event_type r_event_type;
static unsigned int r_event_id;
static int r_threshold;
static unsigned int r_deadZone;
static double r_multiplier;
static double r_exponent;
static e_shape r_shape;
static e_device_type r_device_type;
static int r_device_id;
static char r_device_name[128];
static int r_config_dpi[MAX_CONTROLLERS];

static int GetDeviceId()
{
  int i;
  int device_id = 0;

  if(r_device_type == E_DEVICE_TYPE_JOYSTICK)
  {
    for (i = 0; i < MAX_DEVICES && joystickName[i]; ++i)
    {
      if (!strcmp(r_device_name, joystickName[i]))
      {
        if (r_device_id == joystickVirtualIndex[i])
        {
          device_id = i;
          break;
        }
      }
    }
    if(i == MAX_DEVICES || !joystickName[i])
    {
      printf("joystick not found: %s %d\n", r_device_name, r_device_id);
      device_id = -1;
    }
  }
  else if(merge_all_devices)
  {
    device_id = 0;
  }
  else if(!strlen(r_device_name))
  {
    printf("multiple mice and keyboard are merged\n");
    merge_all_devices = 1;
  }
  else
  {
    if(r_device_type == E_DEVICE_TYPE_MOUSE)
    {
      for (i = 0; i < MAX_DEVICES && mouseName[i]; ++i)
      {
        if (!strcmp(r_device_name, mouseName[i]))
        {
          if (r_device_id == mouseVirtualIndex[i])
          {
            device_id = i;
            break;
          }
        }
      }
      if(i == MAX_DEVICES || !mouseName[i])
      {
        printf("mouse not found: %s %d\n", r_device_name, r_device_id);
        device_id = -1;
      }
    }
    else if(r_device_type == E_DEVICE_TYPE_KEYBOARD)
    {
      for (i = 0; i < MAX_DEVICES && keyboardName[i]; ++i)
      {
        if (!strcmp(r_device_name, keyboardName[i]))
        {
          if (r_device_id == keyboardVirtualIndex[i])
          {
            device_id = i;
            break;
          }
        }
      }
      if(i == MAX_DEVICES || !keyboardName[i])
      {
        printf("keyboard not found: %s %d\n", r_device_name, r_device_id);
        device_id = -1;
      }
    }
  }

  return device_id;
}

static int GetDeviceTypeProp(xmlNode * a_node)
{
  int ret = 0;
  char* type;

  type = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_TYPE);

  if (!strncmp(type, X_ATTR_VALUE_KEYBOARD, strlen(X_ATTR_VALUE_KEYBOARD)))
  {
    r_device_type = E_DEVICE_TYPE_KEYBOARD;
  }
  else if (!strncmp(type, X_ATTR_VALUE_MOUSE, strlen(X_ATTR_VALUE_MOUSE)))
  {
    r_device_type = E_DEVICE_TYPE_MOUSE;
  }
  else if (!strncmp(type, X_ATTR_VALUE_JOYSTICK, strlen(X_ATTR_VALUE_JOYSTICK)))
  {
    r_device_type = E_DEVICE_TYPE_JOYSTICK;
  }
  else
  {
    r_device_type = E_DEVICE_TYPE_UNKNOWN;
  }

  xmlFree(type);
  return ret;
}

static int GetEventId(xmlNode * a_node, char* attr_label)
{
  char* event_id = (char*) xmlGetProp(a_node, (xmlChar*) attr_label);

  if(!event_id) return -1;

  switch(r_device_type)
  {
    case E_DEVICE_TYPE_KEYBOARD:
      r_event_id = get_key_from_buffer(event_id);
      break;
    case E_DEVICE_TYPE_JOYSTICK:
      r_event_id = atoi(event_id);
      break;
    case E_DEVICE_TYPE_MOUSE:
      r_event_id = get_mouse_event_id_from_buffer(event_id);
      break;
    default:
      break;
  }

  xmlFree(event_id);
  return 0;
}

int GetIntProp(xmlNode * a_node, char* a_attr, int* a_int)
{
  char* val;
  int ret;

  val = (char*)xmlGetProp(a_node, (xmlChar*) a_attr);

  if(val)
  {
    *a_int = atoi(val);
    ret = 0;
  }
  else
  {
    ret = -1;
  }

  xmlFree(val);
  return ret;
}

int GetUnsignedIntProp(xmlNode * a_node, char* a_attr, unsigned int* a_uint)
{
  char* val;
  int ret;

  val = (char*)xmlGetProp(a_node, (xmlChar*) a_attr);

  if(val)
  {
    *a_uint = atoi(val);
    ret = 0;
  }
  else
  {
    ret = -1;
  }

  xmlFree(val);
  return ret;
}

int GetDoubleProp(xmlNode * a_node, char* a_attr, double* a_double)
{
  char* val;
  int ret;

  val = (char*)xmlGetProp(a_node, (xmlChar*) a_attr);

  if(val)
  {
    *a_double = strtod(val, NULL);
    ret = 0;
  }
  else
  {
    ret = -1;
  }

  xmlFree(val);
  return ret;
}

static int ProcessEventElement(xmlNode * a_node)
{
  int ret = 0;
  char* type;
  char* shape;

  type = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_TYPE);
  if (!strncmp(type, X_ATTR_VALUE_BUTTON, strlen(X_ATTR_VALUE_BUTTON)))
  {
    r_event_type = E_EVENT_TYPE_BUTTON;
  }
  else if (!strncmp(type, X_ATTR_VALUE_AXIS, strlen(X_ATTR_VALUE_AXIS)))
  {
    r_event_type = E_EVENT_TYPE_AXIS;

    ret = GetUnsignedIntProp(a_node, X_ATTR_DEADZONE, &r_deadZone);
    if(ret == -1)
    {
      r_deadZone = 0;
    }
    ret = GetDoubleProp(a_node, X_ATTR_MULTIPLIER, &r_multiplier);
    if(ret == -1)
    {
      r_multiplier = 1;
    }
    ret = GetDoubleProp(a_node, X_ATTR_EXPONENT, &r_exponent);
    if(ret == -1)
    {
      r_exponent = 1;
      ret = 0;
    }
    shape = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_SHAPE);
    r_shape = E_SHAPE_CIRCLE;//default value
    if(shape)
    {
      if (!strncmp(shape, X_ATTR_VALUE_RECTANGLE, strlen(X_ATTR_VALUE_RECTANGLE)))
      {
        r_shape = E_SHAPE_RECTANGLE;
      }
    }
    xmlFree(shape);
  }
  else if (!strncmp(type, X_ATTR_VALUE_AXIS_DOWN, strlen(X_ATTR_VALUE_AXIS_DOWN)))
  {
    r_event_type = E_EVENT_TYPE_AXIS_DOWN;
    ret = GetIntProp(a_node, X_ATTR_THRESHOLD, &r_threshold);
  }
  else if (!strncmp(type, X_ATTR_VALUE_AXIS_UP, strlen(X_ATTR_VALUE_AXIS_UP)))
  {
    r_event_type = E_EVENT_TYPE_AXIS_UP;
    ret = GetIntProp(a_node, X_ATTR_THRESHOLD, &r_threshold);
  }

  ret = GetEventId(a_node, X_ATTR_ID);

  xmlFree(type);
  return ret;
}

static int ProcessDeviceElement(xmlNode * a_node)
{
  int ret = 0;
  char* prop;

  ret = GetDeviceTypeProp(a_node);

  if(ret != -1)
  {
    ret = GetIntProp(a_node, X_ATTR_ID, &r_device_id);

    prop = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_NAME);
    if(prop)
    {
      strncpy(r_device_name, prop, sizeof(r_device_name));
    }
    xmlFree(prop);

    r_device_id = GetDeviceId();
  }

  return ret;
}

static s_mapper** get_mapper_table()
{
  s_mapper** pp_mapper = NULL;

  if(r_device_id < 0) return NULL;

  switch(r_device_type)
  {
    case E_DEVICE_TYPE_KEYBOARD:
      pp_mapper = &(keyboard_buttons[r_device_id][r_controller_id][r_config_id]);
      break;
    case E_DEVICE_TYPE_MOUSE:
      switch(r_event_type)
      {
        case E_EVENT_TYPE_BUTTON:
          pp_mapper = &(mouse_buttons[r_device_id][r_controller_id][r_config_id]);
          break;
        case E_EVENT_TYPE_AXIS:
          pp_mapper = &(mouse_axis[r_device_id][r_controller_id][r_config_id]);
          break;
        default:
          break;
      }
      break;
    case E_DEVICE_TYPE_JOYSTICK:
      switch(r_event_type)
      {
        case E_EVENT_TYPE_BUTTON:
          pp_mapper = &(joystick_buttons[r_device_id][r_controller_id][r_config_id]);
          break;
        case E_EVENT_TYPE_AXIS:
          pp_mapper = &(joystick_axis[r_device_id][r_controller_id][r_config_id]);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  return pp_mapper;
}

static void allocate_mapper(s_mapper** pp_mapper)
{
  if(*pp_mapper)
  {
    *pp_mapper = realloc(*pp_mapper, ((*pp_mapper)->nb_mappers+1)*sizeof(s_mapper));
    memset(*pp_mapper+(*pp_mapper)->nb_mappers, 0x00, sizeof(s_mapper));
  }
  else
  {
    *pp_mapper = calloc(1, sizeof(s_mapper));
  }
  (*pp_mapper)->nb_mappers++;
}

static int ProcessAxisElement(xmlNode * a_node)
{
  int ret = 0;
  xmlNode* cur_node = NULL;
  s_mapper** pp_mapper = NULL;
  s_mapper* p_mapper = NULL;
  s_axis_index aindex;
  char* aid;

  aid = (char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_ID);

  aindex = get_axis_index_from_name(aid);

  xmlFree(aid);

  for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_DEVICE))
      {
        ret = ProcessDeviceElement(cur_node);
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

  for (cur_node = cur_node->next; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_EVENT))
      {
        ret = ProcessEventElement(cur_node);
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

  if(ret != -1)
  {
    pp_mapper = get_mapper_table();

    if(pp_mapper)
    {
      allocate_mapper(pp_mapper);

      p_mapper = *pp_mapper;

      p_mapper = p_mapper+p_mapper->nb_mappers-1;

      p_mapper->controller_button = -1;
      p_mapper->controller_button_axis = aindex.baindex;
      p_mapper->controller_thumbstick_axis_value = aindex.tavalue;
      p_mapper->controller_thumbstick = aindex.tindex;
      p_mapper->controller_thumbstick_axis = aindex.taindex;

      switch(r_event_type)
      {
        case E_EVENT_TYPE_BUTTON:
          p_mapper->button = r_event_id;
          break;
        case E_EVENT_TYPE_AXIS:
          p_mapper->axis = r_event_id;
          p_mapper->dead_zone = r_deadZone;
          p_mapper->multiplier = r_multiplier;
          p_mapper->exponent = r_exponent;
          p_mapper->shape = r_shape;
          break;
        default:
          break;
      }
    }
  }

  return ret;
}

static int ProcessButtonElement(xmlNode * a_node)
{
  int ret = 0;
  xmlNode* cur_node = NULL;
  char* bid;
  e_sixaxis_button_index bindex;
  s_mapper* p_mapper = NULL;
  s_mapper** pp_mapper = NULL;

  bid = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_ID);

  ret = get_button_index_from_name(bid);

  xmlFree(bid);

  if(ret != -1)
  {
    bindex = ret;
  }

  for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_DEVICE))
      {
        ret = ProcessDeviceElement(cur_node);
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

  for (cur_node = cur_node->next; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_EVENT))
      {
        ret = ProcessEventElement(cur_node);
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

  if(ret != -1)
  {
    pp_mapper = get_mapper_table();

    if(pp_mapper)
    {
      allocate_mapper(pp_mapper);

      p_mapper = *pp_mapper;

      p_mapper = p_mapper + p_mapper->nb_mappers - 1;

      p_mapper->controller_button = bindex;
      p_mapper->controller_thumbstick = -1;

      switch (r_event_type)
      {
        case E_EVENT_TYPE_BUTTON:
          p_mapper->button = r_event_id;
          break;
        case E_EVENT_TYPE_AXIS_DOWN:
        case E_EVENT_TYPE_AXIS_UP:
          p_mapper->axis = r_event_id;
          p_mapper->threshold = r_threshold;
          break;
        default:
          break;
      }
    }
  }

  return ret;
}

static int ProcessAxisMapElement(xmlNode * a_node)
{
  int ret = 0;
  xmlNode* cur_node = NULL;

  for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_AXIS))
      {
        ret = ProcessAxisElement(cur_node);
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

  for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_BUTTON))
      {
        ret = ProcessButtonElement(cur_node);
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
  char* device_name;
  char* r_switch_back;
  int switch_back = 0;
  int delay = 0;

  ret = GetDeviceTypeProp(a_node);

  if(ret != -1)
  {
    device_name = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_NAME);
    if(device_name)
    {
      strncpy(r_device_name, device_name, sizeof(r_device_name));
    }
    xmlFree(device_name);

    ret = GetIntProp(a_node, X_ATTR_ID, &r_device_id);

    if(ret != -1)
    {
      if((r_device_id = GetDeviceId()) < 0)
      {
        ret = -1;
      }
      else
      {
        ret = GetEventId(a_node, X_ATTR_BUTTON_ID);
      }
    }

    //Optional
    r_switch_back = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_SWITCH_BACK);
    if(r_switch_back)
    {
      if(!strncmp(r_switch_back, X_ATTR_VALUE_YES, sizeof(X_ATTR_VALUE_YES)))
      {
        switch_back = 1;
      }
    }
    xmlFree(r_switch_back);

    //Optional
    GetIntProp(a_node, X_ATTR_DELAY, &delay);
  }

  if(ret != -1)
  {
    triggers[r_controller_id][r_config_id].button = r_event_id;
    triggers[r_controller_id][r_config_id].device_id = r_device_id;
    triggers[r_controller_id][r_config_id].device_type = r_device_type;
    triggers[r_controller_id][r_config_id].switch_back = switch_back;
    triggers[r_controller_id][r_config_id].delay = delay;
  }

  return ret;
}

static int ProcessUpDownElement(xmlNode * a_node, int* device_type, int* device_id, int* button)
{
  int ret = 0;
  char* device_name;

  ret = GetDeviceTypeProp(a_node);

  if(ret != -1 && r_device_type != E_DEVICE_TYPE_UNKNOWN)
  {
    device_name = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_NAME);
    if(device_name)
    {
      strncpy(r_device_name, device_name, sizeof(r_device_name));
    }
    xmlFree(device_name);

    ret = GetIntProp(a_node, X_ATTR_ID, &r_device_id);

    if(ret != -1)
    {
      if((r_device_id = GetDeviceId()) < 0)
      {
        ret = -1;
      }
      else
      {
        ret = GetEventId(a_node, X_ATTR_BUTTON_ID);
      }
    }

    if(ret != -1)
    {
      *button = r_event_id;
      *device_id = r_device_id;
      *device_type = r_device_type;
    }
  }

  return ret;
}

static int ProcessIntensityElement(xmlNode * a_node, s_intensity* intensity)
{
  xmlNode* cur_node = NULL;
  int ret = 0;
  char* shape;
  unsigned int steps;

  for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_UP))
      {
        ret = ProcessUpDownElement(cur_node, &intensity->device_up_type, &intensity->device_up_id, &intensity->up_button);
      }
      else if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_DOWN))
      {
        ret = ProcessUpDownElement(cur_node, &intensity->device_down_type, &intensity->device_down_id, &intensity->down_button);
      }
      else
      {
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }

  if(ret != -1 && (intensity->device_down_id != -1 || intensity->device_up_id != -1))
  {
    ret = GetUnsignedIntProp(a_node, X_ATTR_DEADZONE, &intensity->dead_zone);

    if(ret != -1)
    {
      shape = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_SHAPE);
      if(shape)
      {
        if (!strncmp(shape, X_ATTR_VALUE_RECTANGLE, strlen(X_ATTR_VALUE_RECTANGLE)))
        {
          r_shape = E_SHAPE_RECTANGLE;
        }
        else
        {
          intensity->shape = E_SHAPE_CIRCLE;
        }
      }
      else
      {
        ret = -1;
      }
      xmlFree(shape);

      if(ret != -1)
      {
        ret = GetUnsignedIntProp(a_node, X_ATTR_STEPS, &steps);

        if(ret != -1)
        {
          intensity->step = (double)(mean_axis_value - intensity->dead_zone) / steps;
        }
      }
    }
  }

  return ret;
}

static int ProcessIntensityListElement(xmlNode * a_node)
{
  xmlNode* cur_node = NULL;
  int ret = 0;
  char* control;

  for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_INTENSITY))
      {
        control = (char*) xmlGetProp(cur_node, (xmlChar*) X_ATTR_CONTROL);
        if(!strcmp(control, "left_stick"))
        {
          ret = ProcessIntensityElement(cur_node, &left_intensity[r_controller_id][r_config_id]);
        }
        else if(!strcmp(control, "right_stick"))
        {
          ret = ProcessIntensityElement(cur_node, &right_intensity[r_controller_id][r_config_id]);
        }
        xmlFree(control);
      }
      else
      {
        break;
      }
    }
  }

  return ret;
}

static int ProcessConfigurationElement(xmlNode * a_node)
{
  int ret = 0;
  xmlNode* cur_node = NULL;

  ret = GetUnsignedIntProp(a_node, X_ATTR_ID, &r_config_id);

  if(ret != -1)
  {
    r_config_id--;

    if (r_config_id >= MAX_CONFIGURATIONS || r_config_id < 0)
    {
      printf("bad configuration id: %d\n", r_config_id);
      ret = -1;
    }
  }

  cur_node = a_node->children;

  for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_TRIGGER))
      {
        ret = ProcessTriggerElement(cur_node);
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

  left_intensity[r_controller_id][r_config_id].device_up_id = -1;
  left_intensity[r_controller_id][r_config_id].device_down_id = -1;
  left_intensity[r_controller_id][r_config_id].up_button = -1;
  left_intensity[r_controller_id][r_config_id].down_button = -1;
  left_intensity[r_controller_id][r_config_id].value = mean_axis_value;
  left_intensity[r_controller_id][r_config_id].shape = E_SHAPE_RECTANGLE;

  right_intensity[r_controller_id][r_config_id].device_up_id = -1;
  right_intensity[r_controller_id][r_config_id].device_down_id = -1;
  right_intensity[r_controller_id][r_config_id].up_button = -1;
  right_intensity[r_controller_id][r_config_id].down_button = -1;
  right_intensity[r_controller_id][r_config_id].value = mean_axis_value;
  right_intensity[r_controller_id][r_config_id].shape = E_SHAPE_RECTANGLE;

  for (cur_node = cur_node->next; cur_node; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_INTENSITY_LIST))
      {
        ret = ProcessIntensityListElement(cur_node);
        break;
      }
      else
      {
        cur_node = cur_node->prev;
        break;
      }
    }
  }

  for (cur_node = cur_node->next; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_BUTTON_MAP))
      {
        ret = ProcessButtonMapElement(cur_node);
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

  for (cur_node = cur_node->next; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_AXIS_MAP))
      {
        ret = ProcessAxisMapElement(cur_node);
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

  return ret;
}

static int ProcessControllerElement(xmlNode * a_node)
{
  xmlNode* cur_node = NULL;
  int ret = 0;

  ret = GetUnsignedIntProp(a_node, X_ATTR_ID, &r_controller_id);

  if(ret != -1)
  {
    r_controller_id--;

    if (r_controller_id >= MAX_CONTROLLERS || r_controller_id < 0)
    {
      printf("bad controller id: %d\n", r_controller_id);
      ret = -1;
    }
  }

  if(ret != -1)
  {
    /* optional */
    GetIntProp(a_node, X_ATTR_DPI, r_config_dpi+r_controller_id);
  }

  for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_CONFIGURATION))
      {
        ret = ProcessConfigurationElement(cur_node);
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
      for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
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
static int read_file(char* file_path)
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

#ifdef WIN32
  if(!xmlFree) xmlMemGet(&xmlFree,&xmlMalloc,&xmlRealloc,NULL);
#endif

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

  return ret;
}

extern int current_mouse;

static void read_calibration()
{
  int i, j, k;
  s_mapper* p_mapper;

  current_mouse = -1;

  for(i=0; i<MAX_DEVICES; ++i)
  {
    for(j=0; j<MAX_CONTROLLERS; ++j)
    {
      for(k=0; k<MAX_CONFIGURATIONS; ++k)
      {
        for(p_mapper = mouse_axis[i][j][k]; p_mapper && p_mapper<mouse_axis[i][j][k]+mouse_axis[i][j][k]->nb_mappers; p_mapper++)
        {
          if(current_mouse < 0)
          {
            current_mouse = i;
          }
          if(p_mapper->axis == 0)
          {
            mouse_cal[i][k].mx = &p_mapper->multiplier;
            mouse_cal[i][k].ex = &p_mapper->exponent;
            mouse_cal[i][k].dzx = &p_mapper->dead_zone;
            mouse_cal[i][k].dzs = &p_mapper->shape;
            mouse_cal[i][k].rd = DEFAULT_RADIUS;
            mouse_cal[i][k].dpi = r_config_dpi[j];
          }
          else
          {
            mouse_cal[i][k].my = &p_mapper->multiplier;
            mouse_cal[i][k].ey = &p_mapper->exponent;
            mouse_cal[i][k].dzy = &p_mapper->dead_zone;
          }
        }
      }
    }
  }

  if(current_mouse < 0)
  {
    current_mouse = 0;
  }
}

void free_config()
{
  int i, j, k;
  for(i=0; i<MAX_DEVICES; ++i)
  {
    for(j=0; j<MAX_CONTROLLERS; ++j)
    {
      for(k=0; k<MAX_CONFIGURATIONS; ++k)
      {
        free(keyboard_buttons[i][j][k]);
        free(mouse_buttons[i][j][k]);
        free(mouse_axis[i][j][k]);
        free(joystick_buttons[i][j][k]);
        free(joystick_axis[i][j][k]);
        keyboard_buttons[i][j][k] = NULL;
        mouse_buttons[i][j][k] = NULL;
        mouse_axis[i][j][k] = NULL;
        joystick_buttons[i][j][k] = NULL;
        joystick_axis[i][j][k] = NULL;
      }
    }
  }
  memset(mouse_cal, 0x00, sizeof(mouse_cal));
}

/*
 * This function loads a config file.
 */
void read_config_file(const char* file)
{
  char file_path[PATH_MAX];

#ifndef WIN32
  snprintf(file_path, sizeof(file_path), "%s/%s/%s", homedir, CONFIG_DIR, file);
#else
  snprintf(file_path, sizeof(file_path), "%s/%s", CONFIG_DIR, file);
#endif

  if(read_file(file_path) == -1)
  {
    printf("Bad config file: %s\n", file_path);
    exit(-1);
  }

  read_calibration();
}

/*
 * This function reads all filenames in the config directory
 * and ask the user which one he wants to load.
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
#ifdef WIN32
  struct stat buf;
  char path[PATH_MAX];
#endif

#ifndef WIN32
  snprintf(file_path, sizeof(file_path), "%s/%s", homedir, CONFIG_DIR);
#else
  snprintf(file_path, sizeof(file_path), "%s", CONFIG_DIR);
#endif
  dirp = opendir(file_path);
  if (dirp == NULL)
  {
    printf("Warning: can't open config directory %s\n", file_path);
    exit(-1);
  }

  while ((d = readdir(dirp)))
  {
#ifndef WIN32
      if (d->d_type == DT_REG)
      {
        nb_filenames++;
        filenames = realloc(filenames, nb_filenames*sizeof(char*));
        filenames[nb_filenames-1] = calloc(strlen(d->d_name)+1, sizeof(char));
        strncpy(filenames[nb_filenames-1], d->d_name, strlen(d->d_name));
      }
#else
      snprintf(path, sizeof(file_path), "%s/%s", file_path, d->d_name);
      if(stat(file_path, &buf) == 0)
      {
        if(S_ISREG(buf.st_mode))
        {
          nb_filenames++;
          filenames = realloc(filenames, nb_filenames*sizeof(char*));
          filenames[nb_filenames-1] = calloc(strlen(d->d_name)+1, sizeof(char));
          strncpy(filenames[nb_filenames-1], d->d_name, strlen(d->d_name));
        }
      }
#endif
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

  read_config_file(filenames[selected]);

  for(i=0; i<nb_filenames; ++i)
  {
    free(filenames[i]);
  }
  free(filenames);

  return 1;
}

