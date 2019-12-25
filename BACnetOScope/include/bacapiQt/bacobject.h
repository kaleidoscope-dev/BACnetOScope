#ifndef BACOBJECT_H
#define BACOBJECT_H


// TODO: ove dvije definicije moraju biti opisane ili u bacdef ili u bacapi
// Ovaj file nebi trebao niti postojati. Ubaciti ovo u drugi file. 
//#define BACAPI_OBJECT_COUNT 60  -- prebacio u enum BACnetObjectType
#define BACAPI_COMPLEX_PROPERTY_COUNT 111




#define BACAPI_LINK_OBJECT_PROP_COUNT 11

#define BACAPI_ACCUMULATOR_PROP_COUNT 43 // 2016
#define BACAPI_ANALOG_INPUT_PROP_COUNT 40 // 2016
#define BACAPI_ANALOG_OUTPUT_PROP_COUNT 43 // 2016
#define BACAPI_ANALOG_VALUE_PROP_COUNT 44 // 2016
#define BACAPI_AVERAGING_PROP_COUNT 23 // 2016
#define BACAPI_BINARY_INPUT_PROP_COUNT 37 // 2016
#define BACAPI_BINARY_OUTPUT_PROP_COUNT 45 // 2016
#define BACAPI_BINARY_VALUE_PROP_COUNT 43 // 2016
#define BACAPI_CALENDAR_PROP_COUNT 10 // 2016
#define BACAPI_COMMAND_PROP_COUNT 26 // 2016
#define BACAPI_DEVICE_PROP_COUNT 71  // 2016
#define BACAPI_EVENT_ENROLLMENT_PROP_COUNT 28  // 2016
#define BACAPI_FILE_PROP_COUNT 15 // 2016
#define BACAPI_GROUP_PROP_COUNT 10 // 2016
#define BACAPI_LIFE_SAFETY_POINT_PROP_COUNT 41 // 2016
#define BACAPI_LIFE_SAFETY_ZONE_PROP_COUNT 39 // 2016
#define BACAPI_LOOP_PROP_COUNT 49 // 2016
#define BACAPI_MSI_PROP_COUNT 32 // 2016
#define BACAPI_MSO_PROP_COUNT 38 // 2016
#define BACAPI_MSV_PROP_COUNT 37 // 2016
#define BACAPI_NOTIFICATION_CLASS_PROP_COUNT 23 // 2016
#define BACAPI_PROGRAM_PROP_COUNT 27 // 2016
#define BACAPI_PULSE_CONVERTER_PROP_COUNT 40 // 2016
#define BACAPI_SCHEDULE_PROP_COUNT 28 // 2016
#define BACAPI_TREND_LOG_PROP_COUNT 41 // 2016
#define BACAPI_ACCESS_DOOR_PROP_COUNT 46 // 2016
#define BACAPI_EVENT_LOG_PROP_COUNT 33  // 2016
#define BACAPI_LOAD_CONTROL_PROP_COUNT 37 // 2016
#define BACAPI_STRUCTURED_VIEW_PROP_COUNT 16 // 2016
#define BACAPI_TREND_LOG_MULTIPLE_PROP_COUNT 39 // 2016
#define BACAPI_ACCESS_POINT_PROP_COUNT 54  // 2016
#define BACAPI_ACCESS_ZONE_PROP_COUNT 42 // 2016
#define BACAPI_ACCESS_USER_PROP_COUNT 19 //2016
#define BACAPI_ACCESS_RIGHTS_PROP_COUNT 16
#define BACAPI_ACCESS_CREDENTIAL_PROP_COUNT 30 // 2016
#define BACAPI_CREDENTIAL_DATA_INPUT_PROP_COUNT 25 // 2016
#define BACAPI_CHARACTERSTRING_VALUE_PROP_COUNT 35 // 2016
#define BACAPI_DATETIME_VALUE_PROP_COUNT 30  // 2016
#define BACAPI_LARGE_ANALOG_VALUE_PROP_COUNT 44 // 2016
#define BACAPI_BITSTRING_VALUE_PROP_COUNT 36 // 2016
#define BACAPI_OCTETSTRING_VALUE_PROP_COUNT 21 // 2016
#define BACAPI_TIME_VALUE_PROP_COUNT 29 // 2016
#define BACAPI_INTEGER_VALUE_PROP_COUNT 44 //2016
#define BACAPI_POSITIVE_INTEGER_VALUE_PROP_COUNT 44 //2016
#define BACAPI_DATE_VALUE_PROP_COUNT 29 // 2016
#define BACAPI_DATETIME_PATTERN_VALUE_PROP_COUNT 30 // 2016
#define BACAPI_TIME_PATTERN_VALUE_PROP_COUNT 29
#define BACAPI_DATE_PATTERN_VALUE_PROP_COUNT 20 // 2016
#define BACAPI_NETWORK_SECURITY_PROP_COUNT 19 // 2016
#define BACAPI_GLOBAL_GROUP_PROP_COUNT 35 // 2016
#define BACAPI_NOTIFICATION_FORWARDER_PROP_COUNT 17 // 2016
#define BACAPI_LIGHTING_OUTPUT_PROP_COUNT 37 // 2016
#define BACAPI_BINARY_LIGHTING_OUTPUT_PROP_COUNT 38  // 2016
#define BACAPI_NETWORK_PORT_PROP_COUNT 72  // 2016
#define BACAPI_TIMER_PROP_COUNT 40 // 2016
#define BACAPI_ELEVATOR_GROUP_PROP_COUNT 14 // 2016
#define BACAPI_LIFT_PROP_COUNT 51 // 2016
#define BACAPI_ESCALATOR_PROP_COUNT 35 // 2016
#define BACAPI_ALERT_ENROLLMENT_PROP_COUNT 20 // 2016
#define BACAPI_CHANNEL_PROP_COUNT 30  // 2016


//#define BACAPI_NETWORK_PORT_PROP_COUNT 27


#define BACAPI_SCRIPT_PROP_COUNT 7



#define BACAPI_PRIMITIVE_TYPE_TEST_PROP_COUNT 298

//#define BACAPI_PRIMITIVE_TYPE_TEST_PROP_COUNT 50




extern BACNET_PROPERTY_ID BACAPI_ComplexPropertyList[];
extern BACNET_OBJECT_TYPE BACAPI_Object_List[]; 

// 2016 - 60 Objekata - 25.1.2018
extern BACNET_PROPERTY_ID BACAPI_Object_Analog_Input[];                // 12.2 - 40
extern BACNET_PROPERTY_ID BACAPI_Object_Analog_Output[];               // 12.3
extern BACNET_PROPERTY_ID BACAPI_Object_Analog_Value[];                // 12.4
extern BACNET_PROPERTY_ID BACAPI_Object_Averaging[];                   // 12.5
extern BACNET_PROPERTY_ID BACAPI_Object_Binary_Input[];                // 12.6
extern BACNET_PROPERTY_ID BACAPI_Object_Binary_Output[];               // 12.7
extern BACNET_PROPERTY_ID BACAPI_Object_Binary_Value[];                // 12.8
extern BACNET_PROPERTY_ID BACAPI_Object_Calendar[];                    // 12.9
extern BACNET_PROPERTY_ID BACAPI_Object_Command[];                     // 12.10
extern BACNET_PROPERTY_ID BACAPI_Object_Device[];                      // 12.11
extern BACNET_PROPERTY_ID BACAPI_Object_Event_Enrollment[];            // 12.12
extern BACNET_PROPERTY_ID BACAPI_Object_File[];                        // 12.13
extern BACNET_PROPERTY_ID BACAPI_Object_Group[];                       // 12.14
extern BACNET_PROPERTY_ID BACAPI_Object_Life_Safety_Point[];           // 12.15
extern BACNET_PROPERTY_ID BACAPI_Object_Life_Safety_Zone[];            // 12.16
extern BACNET_PROPERTY_ID BACAPI_Object_Loop[];                        // 12.17
extern BACNET_PROPERTY_ID BACAPI_Object_Multi_State_Input[];           // 12.18
extern BACNET_PROPERTY_ID BACAPI_Object_Multi_State_Output[];          // 12.19
extern BACNET_PROPERTY_ID BACAPI_Object_Multi_State_Value[];           // 12.20
extern BACNET_PROPERTY_ID BACAPI_Object_Notification_Class[];          // 12.21
extern BACNET_PROPERTY_ID BACAPI_Object_Program[];                     // 12.22
extern BACNET_PROPERTY_ID BACAPI_Object_Pulse_Converter[];             // 12.23
extern BACNET_PROPERTY_ID BACAPI_Object_Schedule[];                    // 12.24
extern BACNET_PROPERTY_ID BACAPI_Object_Trend_Log[];                   // 12.25
extern BACNET_PROPERTY_ID BACAPI_Object_Access_Door[];                 // 12.26
extern BACNET_PROPERTY_ID BACAPI_Object_Event_Log[];                   // 12.27
extern BACNET_PROPERTY_ID BACAPI_Object_Load_Control[];                // 12.28
extern BACNET_PROPERTY_ID BACAPI_Object_Structured_View[];             // 12.29
extern BACNET_PROPERTY_ID BACAPI_Object_Trend_Log_Multiple[];          // 12.30
extern BACNET_PROPERTY_ID BACAPI_Object_Access_Point[];                // 12.31
extern BACNET_PROPERTY_ID BACAPI_Object_Access_Zone[];                 // 12.32
extern BACNET_PROPERTY_ID BACAPI_Object_Access_User[];                 // 12.23
extern BACNET_PROPERTY_ID BACAPI_Object_Access_Rights[];               // 12.34
extern BACNET_PROPERTY_ID BACAPI_Object_Access_Credential[];           // 12.35
extern BACNET_PROPERTY_ID BACAPI_Object_Credential_Data_Input[];       // 12.36
extern BACNET_PROPERTY_ID BACAPI_Object_CharacterString_Value[];       // 12.37
extern BACNET_PROPERTY_ID BACAPI_Object_DateTime_Value[];              // 12.38
extern BACNET_PROPERTY_ID BACAPI_Object_Large_Analog_Value[];          // 12.39
extern BACNET_PROPERTY_ID BACAPI_Object_BitString_Value[];             // 12.40
extern BACNET_PROPERTY_ID BACAPI_Object_OctetString_Value[];           // 12.41
extern BACNET_PROPERTY_ID BACAPI_Object_Time_Value[];                  // 12.42
extern BACNET_PROPERTY_ID BACAPI_Object_Integer_Value[];               // 12.43
extern BACNET_PROPERTY_ID BACAPI_Object_Positive_Integer_Value[];      // 12.44
extern BACNET_PROPERTY_ID BACAPI_Object_Date_Value[];                  // 12.45
extern BACNET_PROPERTY_ID BACAPI_Object_DateTime_Pattern_Value[];      // 12.46
extern BACNET_PROPERTY_ID BACAPI_Object_Time_Pattern_Value[];          // 12.47
extern BACNET_PROPERTY_ID BACAPI_Object_Date_Pattern_Value[];          // 12.48
extern BACNET_PROPERTY_ID BACAPI_Object_Network_Security[];            // 12.49
extern BACNET_PROPERTY_ID BACAPI_Object_Global_Group[];                // 12.50
extern BACNET_PROPERTY_ID BACAPI_Object_Notification_Forwarder[];      // 12.51
extern BACNET_PROPERTY_ID BACAPI_Object_Alert_Enrollment[];            // 12.52
extern BACNET_PROPERTY_ID BACAPI_Object_Channel[];                     // 12.53
extern BACNET_PROPERTY_ID BACAPI_Object_Lighting_Output[];             // 12.54
extern BACNET_PROPERTY_ID BACAPI_Object_Binary_Lighting_Output[];      // 12.55
extern BACNET_PROPERTY_ID BACAPI_Object_Network_Port[];                // 12.56
extern BACNET_PROPERTY_ID BACAPI_Object_Timer[];                       // 12.57
extern BACNET_PROPERTY_ID BACAPI_Object_Elevator_Group[];              // 12.58
extern BACNET_PROPERTY_ID BACAPI_Object_Lift[];                        // 12.59
extern BACNET_PROPERTY_ID BACAPI_Object_Escalator[];                   // 12.60
extern BACNET_PROPERTY_ID BACAPI_Object_Accumulator[];                 // 12.61  - 43


extern BACNET_PROPERTY_ID BACAPI_Object_Link_Object[];
extern BACNET_PROPERTY_ID BACAPI_Object_Script_Object[];
extern BACNET_PROPERTY_ID BACAPI_Object_Primitive_Type_Test[];
extern BACNET_PROPERTY_ID BACAPI_Object_All_Propertyies[];



#endif