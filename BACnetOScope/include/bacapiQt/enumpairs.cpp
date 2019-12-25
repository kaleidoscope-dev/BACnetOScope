#include "bacapi.h"
#include "namedef.h"


vector<BACAPI_ENUM_STRUCT>vEnumList;

void BACAPI_InitEnumTypes(void)
{
 BACAPI_ENUM_STRUCT es;
// OBJECT_ANALOG_INPUT
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_ANALOG_INPUT;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_ANALOG_INPUT;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);


 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_ANALOG_INPUT;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetEngUnitsName;
 es.propid = PROP_UNITS;
 es.obj_type = OBJECT_ANALOG_INPUT;
 es.count = UNITS_COUNT;
 vEnumList.push_back(es);
 
 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_ANALOG_INPUT;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);
 
 
 //OBJECT_ANALOG_OUTPUT -----------------------------------------
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_ANALOG_OUTPUT;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_ANALOG_OUTPUT;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);
 
 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_ANALOG_OUTPUT;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es);
  
 es.pair = bacnetEngUnitsName;
 es.propid = PROP_UNITS;
 es.obj_type = OBJECT_ANALOG_OUTPUT;
 es.count = UNITS_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_ANALOG_OUTPUT;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);




 //OBJECT_ANALOG_VALUE  --------------------------
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_ANALOG_VALUE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_ANALOG_VALUE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_ANALOG_VALUE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEngUnitsName;
 es.propid = PROP_UNITS;
 es.obj_type = OBJECT_ANALOG_VALUE;
 es.count = UNITS_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_ANALOG_VALUE;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);


// OBJECT_BINARY_INPUT --------------------------
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_BINARY_INPUT;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetBinaryPVName;
 es.propid = PROP_PRESENT_VALUE;
 es.obj_type = OBJECT_BINARY_INPUT;
 es.count = 2;
 vEnumList.push_back(es);

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_BINARY_INPUT;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_BINARY_INPUT;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetPolarityName;
 es.propid = PROP_POLARITY;
 es.obj_type = OBJECT_BINARY_INPUT;
 es.count = 2;
 vEnumList.push_back(es); 

 es.pair = bacnetBinaryPVName;
 es.propid = PROP_ALARM_VALUE;
 es.obj_type = OBJECT_BINARY_INPUT;
 es.count = 2;
 vEnumList.push_back(es);
 
 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_BINARY_INPUT;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es); 
 

// OBJECT_BINARY_OUTPUT --------------------------
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_BINARY_OUTPUT;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetBinaryPVName;
 es.propid = PROP_PRESENT_VALUE;
 es.obj_type = OBJECT_BINARY_OUTPUT;
 es.count = 2;
 vEnumList.push_back(es);

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_BINARY_OUTPUT;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_BINARY_OUTPUT;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetPolarityName;
 es.propid = PROP_POLARITY;
 es.obj_type = OBJECT_BINARY_OUTPUT;
 es.count = 2;
 vEnumList.push_back(es); 

 es.pair = bacnetBinaryPVName;
 es.propid = PROP_RELINQUISH_DEFAULT;
 es.obj_type = OBJECT_BINARY_OUTPUT;
 es.count = 2;
 vEnumList.push_back(es);
 
 es.pair = bacnetBinaryPVName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_BINARY_OUTPUT;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es); 
 
 

// OBJECT_BINARY_VALUE--------------------------
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_BINARY_OUTPUT;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetBinaryPVName;
 es.propid = PROP_PRESENT_VALUE;
 es.obj_type = OBJECT_BINARY_VALUE;
 es.count = 2;
 vEnumList.push_back(es);

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_BINARY_VALUE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_BINARY_VALUE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetBinaryPVName;
 es.propid = PROP_RELINQUISH_DEFAULT;
 es.obj_type = OBJECT_BINARY_VALUE;
 es.count = 2;
 vEnumList.push_back(es);

 es.pair = bacnetBinaryPVName;
 es.propid = PROP_ALARM_VALUE;
 es.obj_type = OBJECT_BINARY_VALUE;
 es.count = 2;
 vEnumList.push_back(es);
 
 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_BINARY_VALUE;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es); 
 
// OBJECT_CALENDAR ----------------------------------
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_CALENDAR;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);

// OBJECT_COMMAND ---------------------------------------
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_COMMAND;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);

// OBJECT_FILE ---------------------------------------
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_FILE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetFileAccessMethodName;
 es.propid = PROP_FILE_ACCESS_METHOD;
 es.obj_type = OBJECT_FILE;
 es.count = 2;
 vEnumList.push_back(es);

// OBJECT_MULTI_STATE_INPUT -------------------------------
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_MULTI_STATE_INPUT;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_MULTI_STATE_INPUT;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_MULTI_STATE_INPUT;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_MULTI_STATE_INPUT;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);

// OBJECT_MULTI_STATE_OUTPUT -------------------------------
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_MULTI_STATE_OUTPUT;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_MULTI_STATE_OUTPUT;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_MULTI_STATE_OUTPUT;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_MULTI_STATE_OUTPUT;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);

// OBJECT_MULTI_STATE_VALUE -------------------------------
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_MULTI_STATE_VALUE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_MULTI_STATE_VALUE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_MULTI_STATE_VALUE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_MULTI_STATE_VALUE;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);
 
// OBJECT_SCHEDULE ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_SCHEDULE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);  

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_SCHEDULE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_SCHEDULE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_SCHEDULE;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);

// OBJECT_TRENDLOG ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_TREND_LOG;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);  

 es.pair = bacnetLoggingTypeName;
 es.propid = PROP_LOGGING_TYPE;
 es.obj_type = OBJECT_TREND_LOG;
 es.count = LOGGING_TYPE_COUNT;
 vEnumList.push_back(es);  

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_TREND_LOG;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_TREND_LOG;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_TREND_LOG;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);

// OBJECT_ACCUMULATOR ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_ACCUMULATOR;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);  

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_ACCUMULATOR;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_ACCUMULATOR;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEngUnitsName;
 es.propid = PROP_UNITS;
 es.obj_type = OBJECT_ACCUMULATOR;
 es.count = UNITS_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_TREND_LOG;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);

// OBJECT_AVERAGING ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_AVERAGING;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);  

// OBJECT_EVENT_ENROLLMENT ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_EVENT_ENROLLMENT;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventTypeName;
 es.propid = PROP_EVENT_TYPE;
 es.obj_type = OBJECT_EVENT_ENROLLMENT;
 es.count = EVENT_TYPE_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_EVENT_ENROLLMENT;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_EVENT_ENROLLMENT;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_EVENT_ENROLLMENT;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetFaultTypeName;
 es.propid = PROP_FAULT_TYPE;
 es.obj_type = OBJECT_EVENT_ENROLLMENT;
 es.count = FAULT_TYPE_COUNT;
 vEnumList.push_back(es); 
 
// OBJECT_LIFE_SAFETY_POINT ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_LIFE_SAFETY_POINT;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetLifeSafetyStateName;
 es.propid = PROP_PRESENT_VALUE;
 es.obj_type = OBJECT_LIFE_SAFETY_POINT;
 es.count = LIFE_SAFETY_STATE_COUNT;
 vEnumList.push_back(es);  
 
 es.pair = bacnetLifeSafetyStateName;
 es.propid = PROP_TRACKING_VALUE;
 es.obj_type = OBJECT_LIFE_SAFETY_POINT;
 es.count = LIFE_SAFETY_STATE_COUNT;
 vEnumList.push_back(es);  

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_LIFE_SAFETY_POINT;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_LIFE_SAFETY_POINT;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetLifeSafetyModeName;
 es.propid = PROP_MODE;
 es.obj_type = OBJECT_LIFE_SAFETY_POINT;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_LIFE_SAFETY_POINT;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetSilencedStateName;
 es.propid = PROP_SILENCED;
 es.obj_type = OBJECT_LIFE_SAFETY_POINT;
 es.count = 4;
 vEnumList.push_back(es);

 es.pair = bacnetLifeSafeteOperationName;
 es.propid = PROP_OPERATION_EXPECTED;
 es.obj_type = OBJECT_LIFE_SAFETY_POINT;
 es.count = LIFE_OPERATION_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetMaintenanceName;
 es.propid = PROP_MAINTENANCE_REQUIRED;
 es.obj_type = OBJECT_LIFE_SAFETY_POINT;
 es.count = 4;
 vEnumList.push_back(es);

 es.pair = bacnetEngUnitsName;
 es.propid = PROP_UNITS;
 es.obj_type = OBJECT_LIFE_SAFETY_POINT;
 es.count = UNITS_COUNT;
 vEnumList.push_back(es); 
 
// OBJECT_LIFE_SAFETY_ZONE ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_LIFE_SAFETY_ZONE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetLifeSafetyStateName;
 es.propid = PROP_PRESENT_VALUE;
 es.obj_type = OBJECT_LIFE_SAFETY_ZONE;
 es.count = LIFE_SAFETY_STATE_COUNT;
 vEnumList.push_back(es);  
 
 es.pair = bacnetLifeSafetyStateName;
 es.propid = PROP_TRACKING_VALUE;
 es.obj_type = OBJECT_LIFE_SAFETY_ZONE;
 es.count = LIFE_SAFETY_STATE_COUNT;
 vEnumList.push_back(es);  

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_LIFE_SAFETY_ZONE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_LIFE_SAFETY_ZONE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetLifeSafetyModeName;
 es.propid = PROP_MODE;
 es.obj_type = OBJECT_LIFE_SAFETY_ZONE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_LIFE_SAFETY_ZONE;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetSilencedStateName;
 es.propid = PROP_SILENCED;
 es.obj_type = OBJECT_LIFE_SAFETY_ZONE;
 es.count = 4;
 vEnumList.push_back(es);

 es.pair = bacnetLifeSafeteOperationName;
 es.propid = PROP_OPERATION_EXPECTED;
 es.obj_type = OBJECT_LIFE_SAFETY_ZONE;
 es.count = LIFE_OPERATION_COUNT;
 vEnumList.push_back(es);


// OBJECT_LOOP ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_LOOP;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_LOOP;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_LOOP;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEngUnitsName;
 es.propid = PROP_OUTPUT_UNITS;
 es.obj_type = OBJECT_LOOP;
 es.count = UNITS_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEngUnitsName;
 es.propid = PROP_CONTROLLED_VARIABLE_UNITS;
 es.obj_type = OBJECT_LOOP;
 es.count = UNITS_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEngUnitsName;
 es.propid = PROP_PROPORTIONAL_CONSTANT_UNITS;
 es.obj_type = OBJECT_LOOP;
 es.count = UNITS_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEngUnitsName;
 es.propid = PROP_INTEGRAL_CONSTANT_UNITS;
 es.obj_type = OBJECT_LOOP;
 es.count = UNITS_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEngUnitsName;
 es.propid = PROP_DERIVATIVE_CONSTANT_UNITS;
 es.obj_type = OBJECT_LOOP;
 es.count = UNITS_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_LOOP;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);
 
// OBJECT_NOTIFICATION_CLASS ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_NOTIFICATION_CLASS;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 

// OBJECT_PROGRAM ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_PROGRAM;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetProgramStateName;
 es.propid = PROP_PROGRAM_STATE;
 es.obj_type = OBJECT_PROGRAM;
 es.count = 6;
 vEnumList.push_back(es);  
 
 es.pair = bacnetProgramRequestName;
 es.propid = PROP_PROGRAM_CHANGE;
 es.obj_type = OBJECT_PROGRAM;
 es.count = 6;
 vEnumList.push_back(es);   
 
 es.pair = bacnetProgramErrorName;
 es.propid = PROP_REASON_FOR_HALT;
 es.obj_type = OBJECT_PROGRAM;
 es.count = 5;
 vEnumList.push_back(es);    
 
 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_PROGRAM;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_PROGRAM;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);  

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_PROGRAM;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es); 
 
 
 
// OBJECT_PULSE_CONVERTER ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_PULSE_CONVERTER;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);  
 
 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_PULSE_CONVERTER;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);  
 
 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_PULSE_CONVERTER;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es);  
 
 es.pair = bacnetEngUnitsName;
 es.propid = PROP_UNITS;
 es.obj_type = OBJECT_PULSE_CONVERTER;
 es.count = UNITS_COUNT;
 vEnumList.push_back(es);  

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_PROGRAM;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);  
 
// OBJECT_ACCESS_DOOR ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_ACCESS_DOOR;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);   
 
 es.pair = bacnetDoorValueName;
 es.propid = PROP_PRESENT_VALUE;
 es.obj_type = OBJECT_ACCESS_DOOR;
 es.count = 4;
 vEnumList.push_back(es);   
 
 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_ACCESS_DOOR;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);  
 
 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_ACCESS_DOOR;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es);  
 
 es.pair = bacnetDoorValueName;
 es.propid = PROP_RELINQUISH_DEFAULT;
 es.obj_type = OBJECT_ACCESS_DOOR;
 es.count = 4;
 vEnumList.push_back(es);  
 
 es.pair = bacnetDoorStatusName;
 es.propid = PROP_DOOR_STATUS;
 es.obj_type = OBJECT_ACCESS_DOOR;
 es.count = 5;
 vEnumList.push_back(es);   
 
 es.pair = bacnetLockStatusName;
 es.propid = PROP_LOCK_STATUS;
 es.obj_type = OBJECT_ACCESS_DOOR;
 es.count = 5;
 vEnumList.push_back(es);   
  
 es.pair = bacnetDoorSecuredStatusName;
 es.propid = PROP_SECURED_STATUS;
 es.obj_type = OBJECT_ACCESS_DOOR;
 es.count = 3;
 vEnumList.push_back(es);    
 
 es.pair = bacnetMaintenanceName;
 es.propid = PROP_MAINTENANCE_REQUIRED;
 es.obj_type = OBJECT_ACCESS_DOOR;
 es.count = 4;
 vEnumList.push_back(es); 

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_ACCESS_DOOR;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);   
 

// OBJECT_EVENT_LOG -----------------------------------  
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_EVENT_LOG;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_EVENT_LOG;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_EVENT_LOG;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es);     

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_EVENT_LOG;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);    
 
 
 
// OBJECT_LOAD_CONTROL -----------------------------------  
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_LOAD_CONTROL;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetShedStateName;
 es.propid = PROP_PRESENT_VALUE;
 es.obj_type = OBJECT_LOAD_CONTROL;
 es.count = 4;
 vEnumList.push_back(es);  

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_LOAD_CONTROL;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_LOAD_CONTROL;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es);     

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_LOAD_CONTROL;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);    
 

// OBJECT_TREND_LOG_MULTIPLE -----------------------------------  
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_TREND_LOG_MULTIPLE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);  

 es.pair = bacnetLoggingTypeName;
 es.propid = PROP_LOGGING_TYPE;
 es.obj_type = OBJECT_TREND_LOG_MULTIPLE;
 es.count = LOGGING_TYPE_COUNT;
 vEnumList.push_back(es);  

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_TREND_LOG_MULTIPLE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_TREND_LOG_MULTIPLE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_TREND_LOG_MULTIPLE;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);



// OBJECT_ACCESS_POINT ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_ACCESS_POINT;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);  

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_ACCESS_POINT;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_ACCESS_POINT;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetAuthenticationStatusName;
 es.propid = PROP_AUTHENTICATION_STATUS;
 es.obj_type = OBJECT_ACCESS_POINT;
 es.count = 7;
 vEnumList.push_back(es); 
 
 es.pair = bacnetAuthorizationModeName;
 es.propid = PROP_AUTHORIZATION_MODE;
 es.obj_type = OBJECT_ACCESS_POINT;
 es.count = 7;
 vEnumList.push_back(es);  
 
 es.pair = bacnetAccessEventName;
 es.propid = PROP_ACCESS_EVENT;
 es.obj_type = OBJECT_ACCESS_POINT;
 es.count = 54;       // PROVJERITI OVO
 vEnumList.push_back(es);  
  
 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_ACCESS_POINT;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es); 


// OBJECT_ACCESS_ZONE ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_ACCESS_ZONE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);  
 
 es.pair = bacnetAccessZoneOccupancyStateName;
 es.propid = PROP_OCCUPANCY_STATE;
 es.obj_type = OBJECT_ACCESS_ZONE;
 es.count = 8;
 vEnumList.push_back(es);  

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_ACCESS_ZONE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_ACCESS_ZONE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetAccessPassbackModeName;
 es.propid = PROP_PASSBACK_MODE;
 es.obj_type = OBJECT_ACCESS_ZONE;
 es.count = 3;
 vEnumList.push_back(es); 
  
 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_ACCESS_POINT;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);  
 

// OBJECT_ACCESS_USER ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_ACCESS_USER;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);   

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_ACCESS_USER;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es);  
 
 es.pair = bacnetAccessUserTypeName;
 es.propid = PROP_USER_TYPE;
 es.obj_type = OBJECT_ACCESS_USER;
 es.count = 3;   // PROVJERITI
 vEnumList.push_back(es);   
 


// OBJECT_ACCESS_RIGHTS ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_ACCESS_RIGHTS;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);   

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_ACCESS_RIGHTS;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es);  
 
// OBJECT_ACCESS_CREDENTIAL ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_ACCESS_CREDENTIAL;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es);    

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_ACCESS_CREDENTIAL;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetBinaryPVName;
 es.propid = PROP_CREDENTIAL_STATUS;
 es.obj_type = OBJECT_ACCESS_CREDENTIAL;
 es.count = 2;
 vEnumList.push_back(es);

 es.pair = bacnetAccessCredentialDisableName;
 es.propid = PROP_CREDENTIAL_DISABLE;
 es.obj_type = OBJECT_ACCESS_CREDENTIAL;
 es.count = 4;
 vEnumList.push_back(es);
 
 es.pair = bacnetAuthorizationExemptions;
 es.propid = PROP_AUTHORIZATION_EXEMPTIONS;
 es.obj_type = OBJECT_ACCESS_CREDENTIAL;
 es.count = 6;
 vEnumList.push_back(es);
 //
 es.pair = bacnetAccessCredentialDisableReason;
 es.propid = PROP_REASON_FOR_DISABLE;
 es.obj_type = OBJECT_ACCESS_CREDENTIAL;
 es.count = 9;
 vEnumList.push_back(es);
 //
  

// OBJECT_CREDENTIAL_DATA_INPUT ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_CREDENTIAL_DATA_INPUT;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_CREDENTIAL_DATA_INPUT;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_CREDENTIAL_DATA_INPUT;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_CREDENTIAL_DATA_INPUT;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);

// OBJECT_CHARACTERSTRING_VALUE ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_CHARACTERSTRING_VALUE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_CHARACTERSTRING_VALUE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_CHARACTERSTRING_VALUE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_CHARACTERSTRING_VALUE;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);

// OBJECT_DATETIME_VALUE ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_DATETIME_VALUE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_DATETIME_VALUE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_DATETIME_VALUE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);
 
 
// OBJECT_BITSTRING_VALUE ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_BITSTRING_VALUE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_BITSTRING_VALUE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_BITSTRING_VALUE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_BITSTRING_VALUE;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es); 


// OBJECT_OCTETSTRING_VALUE ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_OCTETSTRING_VALUE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_OCTETSTRING_VALUE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_OCTETSTRING_VALUE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

// OBJECT_TIME_VALUE ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_TIME_VALUE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_TIME_VALUE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_TIME_VALUE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);


 //OBJECT_LARGE_ANALOG_VALUE  --------------------------
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_LARGE_ANALOG_VALUE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_LARGE_ANALOG_VALUE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_LARGE_ANALOG_VALUE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEngUnitsName;
 es.propid = PROP_UNITS;
 es.obj_type = OBJECT_LARGE_ANALOG_VALUE;
 es.count = UNITS_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_LARGE_ANALOG_VALUE;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);


 //OBJECT_INTEGER_VALUE  --------------------------
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_INTEGER_VALUE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_INTEGER_VALUE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_INTEGER_VALUE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEngUnitsName;
 es.propid = PROP_UNITS;
 es.obj_type = OBJECT_INTEGER_VALUE;
 es.count = UNITS_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_INTEGER_VALUE;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);

 //OBJECT_POSITIVE_INTEGER_VALUE  --------------------------
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_POSITIVE_INTEGER_VALUE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_POSITIVE_INTEGER_VALUE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_POSITIVE_INTEGER_VALUE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEngUnitsName;
 es.propid = PROP_UNITS;
 es.obj_type = OBJECT_POSITIVE_INTEGER_VALUE;
 es.count = UNITS_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_POSITIVE_INTEGER_VALUE;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);
 
 
// OBJECT_DATE_VALUE ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_DATE_VALUE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_DATE_VALUE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_DATE_VALUE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es); 
 
// OBJECT_DATETIME_PATTERN_VALUE ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_DATETIME_PATTERN_VALUE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_DATETIME_PATTERN_VALUE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_DATETIME_PATTERN_VALUE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);  

// OBJECT_TIME_PATTERN_VALUE ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_TIME_PATTERN_VALUE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_TIME_PATTERN_VALUE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_TIME_PATTERN_VALUE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);  

// OBJECT_DATE_PATTERN_VALUE ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_DATE_PATTERN_VALUE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_DATE_PATTERN_VALUE;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_DATE_PATTERN_VALUE;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);  

// OBJECT_NETWORK_SECURITY ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_NETWORK_SECURITY;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetSecurityLevelName;
 es.propid = PROP_BASE_DEVICE_SECURITY_POLICY;
 es.obj_type = OBJECT_NETWORK_SECURITY;
 es.count = 6;
 vEnumList.push_back(es); 
 
// OBJECT_GLOBAL_GROUP ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_GLOBAL_GROUP;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_GLOBAL_GROUP;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_GLOBAL_GROUP;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_GLOBAL_GROUP;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es); 

// OBJECT_NOTIFICATION_FORWARDER ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_NOTIFICATION_FORWARDER;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_NOTIFICATION_FORWARDER;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

// OBJECT_ALERT_ENROLLMENT ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_ALERT_ENROLLMENT;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_ALERT_ENROLLMENT;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_ALERT_ENROLLMENT;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es); 


// OBJECT_CHANNEL ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_CHANNEL;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetWriteStatusName;
 es.propid = PROP_WRITE_STATUS;
 es.obj_type = OBJECT_CHANNEL;
 es.count = 4;
 vEnumList.push_back(es); 

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_CHANNEL;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_CHANNEL;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_CHANNEL;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es); 


// OBJECT_LIGHTING_OUTPUT ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_LIGHTING_OUTPUT;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetLightingInProgressName;
 es.propid = PROP_IN_PROGRESS;
 es.obj_type = OBJECT_LIGHTING_OUTPUT;
 es.count = 5;
 vEnumList.push_back(es);  

 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_LIGHTING_OUTPUT;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetLightingTransitionName;
 es.propid = PROP_TRANSITION;
 es.obj_type = OBJECT_LIGHTING_OUTPUT;
 es.count = 3;
 vEnumList.push_back(es); 

// OBJECT_GROUP ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_GROUP;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 


// OBJECT_STRUCTURED_VIEW ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_STRUCTURED_VIEW;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacneNodeTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_STRUCTURED_VIEW;
 es.count = 12;
 vEnumList.push_back(es); 

// OBJECT_DEVICE ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_DEVICE;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetDeviceStatusName;
 es.propid = PROP_SYSTEM_STATUS;
 es.obj_type = OBJECT_DEVICE;
 es.count = 5;
 vEnumList.push_back(es); 
 
 es.pair = bacnetSegmentationName;
 es.propid = PROP_SEGMENTATION_SUPPORTED;
 es.obj_type = OBJECT_DEVICE;
 es.count = 4;
 vEnumList.push_back(es);  

 es.pair = bacnetBackupStateName;
 es.propid = PROP_BACKUP_AND_RESTORE_STATE;
 es.obj_type = OBJECT_DEVICE;
 es.count = 7;
 vEnumList.push_back(es);  

 es.pair = bacnetRestartReasonName;
 es.propid = PROP_LAST_RESTART_REASON;
 es.obj_type = OBJECT_DEVICE;
 es.count = 8;
 vEnumList.push_back(es); 



// OBJECT_NETWORK_PORT ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_NETWORK_PORT;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_NETWORK_PORT;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es);  
 
 
 es.pair = networkTypeName;
 es.propid = PROP_NETWORK_TYPE;
 es.obj_type = OBJECT_NETWORK_PORT;
 es.count = NETWORK_TYPE_COUNT;
 vEnumList.push_back(es);  


 es.pair = bacnetNNQualityName;
 es.propid = PROP_NETWORK_NUMBER_QUALITY;
 es.obj_type = OBJECT_NETWORK_PORT;
 es.count = QUALITY_COUNT;
 vEnumList.push_back(es);  


 es.pair = protocolLevelName;
 es.propid = PROP_PROTOCOL_LEVEL;
 es.obj_type = OBJECT_NETWORK_PORT;
 es.count = PROTOCOL_LEVEL_COUNT;
 vEnumList.push_back(es);  

 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_NETWORK_PORT;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);  
 
 es.pair = networkPortCommandName;
 es.propid = PROP_COMMAND;
 es.obj_type = OBJECT_NETWORK_PORT;
 es.count = PORT_COMMAND_COUNT;
 vEnumList.push_back(es);  
 

 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_NETWORK_PORT;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);
 
 es.pair = bacnetIPModeName;
 es.propid = PROP_BACNET_IP_MODE;
 es.obj_type = OBJECT_NETWORK_PORT;
 es.count = IP_MODE_COUNT;
 vEnumList.push_back(es);
  

 es.pair = bacnetIPModeName;
 es.propid = PROP_BACNET_IPV6_MODE;
 es.obj_type = OBJECT_NETWORK_PORT;
 es.count = IP_MODE_COUNT;
 vEnumList.push_back(es);
 




// OBJECT_BINARY_LIGHTING_OUTPUT ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_BINARY_LIGHTING_OUTPUT;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_BINARY_LIGHTING_OUTPUT;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es);  
 
  
  
 es.pair = binaryLightingPVName;
 es.propid = PROP_FEEDBACK_VALUE;
 es.obj_type = OBJECT_BINARY_LIGHTING_OUTPUT;
 es.count = BINARY_LIHGTING_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = binaryLightingPVName;
 es.propid = PROP_RELINQUISH_DEFAULT;
 es.obj_type = OBJECT_BINARY_LIGHTING_OUTPUT;
 es.count = BINARY_LIHGTING_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetPolarityName;
 es.propid = PROP_POLARITY;
 es.obj_type = OBJECT_BINARY_LIGHTING_OUTPUT;
 es.count = 2;
 vEnumList.push_back(es);  


 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_BINARY_LIGHTING_OUTPUT;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);  
 


// OBJECT_ELEVATOR_GROUP ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_ELEVATOR_GROUP;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 


 es.pair = liftGroupModeName;
 es.propid = PROP_GROUP_MODE;
 es.obj_type = OBJECT_ELEVATOR_GROUP;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 
 

// OBJECT_LIFT ----------------------------------- 
 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_LIFT;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 


 es.pair = liftCarDirectionName;
 es.propid = PROP_CAR_MOVING_DIRECTION;
 es.obj_type = OBJECT_LIFT;
 es.count = DIRECTION_COUNT;
 vEnumList.push_back(es); 


 es.pair = liftCarDirectionName;
 es.propid = PROP_CAR_ASSIGNED_DIRECTION;
 es.obj_type = OBJECT_LIFT;
 es.count = DIRECTION_COUNT;
 vEnumList.push_back(es); 



 es.pair = liftCarDirectionName;
 es.propid = PROP_CAR_DOOR_COMMAND;
 es.obj_type = OBJECT_LIFT;
 es.count = DIRECTION_COUNT;
 vEnumList.push_back(es); 

 es.pair = bacnetLiftCarMode;
 es.propid = PROP_CAR_MODE;
 es.obj_type = OBJECT_LIFT;
 es.count = CAR_MODE_COUNT;
 vEnumList.push_back(es); 
 
 
 
 es.pair = bacnetEngUnitsName;
 es.propid = PROP_CAR_LOAD_UNITS;
 es.obj_type = OBJECT_LIFT;
 es.count = UNITS_COUNT;
 vEnumList.push_back(es);
 
 
 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_LIFT;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es);  
 

 
 es.pair = bacnetLiftCarDriveStatus;
 es.propid = PROP_CAR_DRIVE_STATUS;
 es.obj_type = OBJECT_LIFT;
 es.count = DRIVE_STATUS_COUNT;
 vEnumList.push_back(es);  


 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_LIFT;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);  

// OBJECT_ESCALATOR ----------------------------------- 

 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_ESCALATOR;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 



 es.pair = bacnetEscalatorOperationDirection;
 es.propid = PROP_OPERATION_DIRECTION;
 es.obj_type = OBJECT_ESCALATOR;
 es.count = OPERATION_DIRECTION_COUNT;
 vEnumList.push_back(es); 
 

 es.pair = bacnetEscalatorMode;
 es.propid = PROP_ESCALATOR_MODE;
 es.obj_type = OBJECT_ESCALATOR;
 es.count = ESCALATOR_MODE_COUNT;
 vEnumList.push_back(es); 
 
 es.pair = bacnetEventStateName;
 es.propid = PROP_EVENT_STATE;
 es.obj_type = OBJECT_ESCALATOR;
 es.count = EVENT_STATE_COUNT;
 vEnumList.push_back(es);
 
 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_ESCALATOR;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);  
 
 
  
 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_ESCALATOR;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es);  
 
 
 

// OBJECT_TIMER -----------------------------------  

 es.pair = bacnetObjectTypeName;
 es.propid = PROP_OBJECT_TYPE;
 es.obj_type = OBJECT_TIMER;
 es.count = BACNET_OBJECT_COUNT;
 vEnumList.push_back(es); 


  
 es.pair = bacnetReliabilityName;
 es.propid = PROP_RELIABILITY;
 es.obj_type = OBJECT_TIMER;
 es.count = RELIABILITY_COUNT;
 vEnumList.push_back(es);  
 

 
 es.pair = bacnetNotifyTypeName;
 es.propid = PROP_NOTIFY_TYPE;
 es.obj_type = OBJECT_TIMER;
 es.count = NOTIFY_TYPE_COUNT;
 vEnumList.push_back(es);  
 
}






