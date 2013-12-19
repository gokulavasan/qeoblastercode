#ifndef CONFIG_STRUCTS_H
#define CONFIG_STRUCTS_H

enum value_type_t
{
	STRING,
	INTEGER,
	DOUBLE
};

union value_t
{
	char str_value[256];
	int int_value;
	double double_value;
};

enum compare_type_t
{
	EQUAL,
	LESS,
	GREATER
};

struct config_t
{
	int cause_device_id; // id of device that triggers the event
	bool cause_state_event; // true if we are looking for a state event
	char cause_name[256]; // if a state event, then it's the state name, otherwise it's the signal name
	
	//**** following is used for state changes only *****************************
	compare_type_t compare_type; // what kind of comparison are we comparing the state value to?
	value_type_t cause_value_type; // what data type is the value we are comparing?
	value_t cause_value; // what value are we looking to compare with?
	//***************************************************************************

	int effect_device_id;
	bool effect_state_change; // true if the event causes a state change, otherwise it causes a signal
	char effect_name[256]; // if a state change effect, name of the state, otherwise name of the signal

	//****** following is for effects that change state only ******************
	value_type_t effect_value_type; // what data type is the effect state change?
	value_t effect_value; // what value are setting the effect state to?
	//*************************************************************************
};

#endif CONFIG_STRUCTS_H
