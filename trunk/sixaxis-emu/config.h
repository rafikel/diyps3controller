/*
 * config.h
 *
 *  Created on: 30 oct. 2010
 *      Author: matlo
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define MAX_CONTROLLERS 7
#define MAX_CONFIGURATIONS 4

#define CONFIG_DIR "./config"

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

int read_config_dir();

#endif /* CONFIG_H_ */
