/* Zastarjelo znace, rest.kandidat
*/
#ifndef BACNET_H
#define BACNET_H


// verzija protokola opisana ovdje
#define BACNET_PROTOCOL_VERSION 1
#define BACNET_PROTOCOL_REVISION 12



#include "bacdef.h"
#include "mstpdef.h"
#include "npdu.h"
#include <vector>


#define BACNET_API_TYPE_CLIENT 0
#define BACNET_API_TYP_SERVER 1

// tip inicijalizacije klijent/server?
#define BACNET_API_TYPE BACNET_API_TYP_SERVER // ili client

#define BACNET_DLT_TYPE_BACIP 0
#define BACNET_DLT_TYPE_ETH802 1
#define BACNET_DLT_TYPE_ALL 2


#define BACNET_WEEKDAY_MONDAY    0
#define BACNET_WEEKDAY_TUESDAY   1
#define BACNET_WEEKDAY_WEDNESDAY 2
#define BACNET_WEEKDAY_THURSDAY  3
#define BACNET_WEEKDAY_FRIDAY    4
#define BACNET_WEEKDAY_SATURDAY  5
#define BACNET_WEEKDAY_SUNDAY    6


#define BACNET_RECIPIENT_TYPE_OID 0
#define BACNET_RECIPIENT_TYPE_ADR 1


#define BACNET_MAX_SEGMENTS 32

#define BACNET_MAX_BBMD 10



/*/ BACnetRecipient */
typedef struct BACnet_Recipient {
    /*/ Flag : choice between [0] BACnetObjectIdentifier, [1] BACnetAddress */
    uint8_t TagRecipientType;
    union {
        /*/ ObjectIdentifier : the recipient is an object to be discovered */
        BACNET_OBJECT_ID Device;
        /*/ Address : the complete recipient address */
        BACNET_ADDRESS Address;
    } RecipientType;
} BACNET_RECIPIENT;

typedef struct BACNET_RECIPIENT_PROCESS
{
  BACNET_RECIPIENT recipient;
  uint32_t recipientProcessId;
}BACNET_RECIPIENT_PROCESS;


typedef struct BACnetObjectPropertyReference
{
 BACNET_OBJECT_ID oid;
 BACNET_PROPERTY_ID propertyId;
 uint32_t propertyArrayIndex;   // optional
 //BACnetObjectPropertyReference *next;

}BACNET_OBJECT_PROPERTY_REFERENCE;


typedef struct BACNET_COV_SUBSCRIPTION
{
 BACNET_RECIPIENT_PROCESS recipientProcess;
 BACNET_OBJECT_PROPERTY_REFERENCE monitoredProperty;
 bool confirmed;
 uint32_t timeRemaining;
 float covIncrement;
}BACNET_COV_SUBSCRIPTION;




///*/ BACnetCOVSubscription */
//typedef struct BACnet_COV_Subscription {
//    BACNET_RECIPIENT_PROCESS recipient;
//    BACNET_OBJECT_PROPERTY_REFERENCE monitoredPropertyReference;
//    bool issueConfirmedNotifications;
//    uint32_t timeRemaining;
//    float covIncrement;
//} BACNET_COV_SUBSCRIPTION;
//




typedef struct BACnetAddressBinding
{
 //BACNET_OBJECT_ID device;
 BACNET_OBJECT_ID deviceOid;   // device OID
 //BACNET_PROPERTY_ID property;
 //uint16_t arrayIndex;
 BACNET_ADDRESS deviceAddress;  // device address
 //BACnetAddressBinding *next;
 //uint32_t srcAddress;   //originalna adresa servera
 //uint16_t netNum;
}BACNET_ADDRESS_BINDING;


typedef struct BACnetDeviceObjectPropertyReference{
    BACNET_OBJECT_ID objectIdentifier;
    BACNET_PROPERTY_ID propertyIdentifier;
    uint32_t arrayIndex;
    BACNET_OBJECT_ID deviceIndentifier;
} BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE;


typedef struct BACnet_Device_Object_Reference {
    BACNET_OBJECT_ID deviceIndentifier; /* Optional (0, 0) */
    BACNET_OBJECT_ID objectIdentifier;
    BACnet_Device_Object_Reference *next;
} BACNET_DEVICE_OBJECT_REFERENCE;



typedef struct BACnet_Destination{
    /*/ BACnetDaysOfWeek : the set of dats of the week on which  */
    /*/ this destination may be used between from and to time */
    BACNET_BIT_STRING ValidDays;
    /*/ Begin of the time window (inclusive) during which the destination is viable */
    BACNET_TIME FromTime;
    /*/ End of the time window (inclusive) during which the destination is viable */
    BACNET_TIME ToTime;
    /*/ The destination device(s) to receive notifications */
    BACNET_RECIPIENT Recipient;
    /*/ Handle of a process within the recipient device */
    uint32_t ProcessIdentifier;
    /*/ True if confirmed notifications are to be sent, false if unconfirmed notifications */
    bool IssueConfirmedNotifications;
    /*/ BACnetEventTransitionBits : 3 flags that indicate the transitions  */
    /*/ {to-offnormal, to-fault, to-normal } for which this recipient is suitable */
    BACNET_BIT_STRING Transitions;

    BACnet_Destination *next;
} BACNET_DESTINATION;


typedef struct BACnet_Date_Range {
    BACNET_DATE startdate;
    BACNET_DATE enddate;
} BACNET_DATE_RANGE;

//
//typedef struct BACnetTimeStamp
//{
// BACNET_TIME time;
// uint32_t sequenceNum;
// BACNET_DATE_TIME dateTime;
//}BACNET_TIME_STAMP;


/* week and days */
typedef struct BACnet_Weeknday {
    uint8_t month;      /* 1=Jan 13=odd 14=even FF=any */
    uint8_t weekofmonth;        /* 1=days 1-7, 2=days 8-14, 3=days 15-21, 4=days 22-28, 5=days 29-31, 6=last 7 days, FF=any week */
    uint8_t dayofweek;  /* 1=Monday-7=Sunday, FF=any */
} BACNET_WEEKNDAY;



/*/ One small value for the BACnetTimeValue */
typedef struct BACnet_Short_Application_Data_Value {
    uint8_t tag;        /* application tag data type */
    struct {
        bool Boolean;
        uint32_t Unsigned_Int;
        int32_t Signed_Int;
        float Real;
        uint32_t Enumerated;
        BACNET_CHARACTER_STRING character_string;
        double Double;
    } type;
} BACNET_SHORT_APPLICATION_DATA_VALUE;


typedef struct BACnet_Time_Value {
    BACNET_TIME Time;
    BACNET_SHORT_APPLICATION_DATA_VALUE Value;
} BACNET_TIME_VALUE;

#define MAX_DAY_SCHEDULE_VALUES 40   // deprecated - ne koristiti vise nigdje i izbrisati sve reference na ovaj def


typedef struct BACnet_Daily_Schedule {
    //uint8_t dayValueCount;
    //BACNET_TIME_VALUE daySchedule[MAX_DAY_SCHEDULE_VALUES];     /* null value means no timevalue */
    vector<BACNET_TIME_VALUE>vDaySchedule;
} BACNET_DAILY_SCHEDULE;

/*/ weekly-schedule [123] SEQUENCE SIZE(7) OF BACnetDailySchedule OPTIONAL */
typedef struct BACnet_Weekly_Schedule {
    BACNET_DAILY_SCHEDULE weeklySchedule[7];
} BACNET_WEEKLY_SCHEDULE;




/*/ BACnetCalendarEntry */
typedef struct BACnet_Calendar_Entry {
    /*/ Flag : choice between [0] Date [1] DateRange [2] Weeknday */
    uint8_t TagEntryType;
    union {
        /*date  [0] Date,  */
        BACNET_DATE date;
        /*dateRange [1] BACnetDateRange,  */
        BACNET_DATE_RANGE dateRange;
        /*weekNDay [2] BACnetWeekNDay */
        BACNET_WEEKNDAY weekNDay;
    } Entry;

  BACnet_Calendar_Entry *next;
} BACNET_CALENDAR_ENTRY;




#define MAX_SPECIAL_EVENT_VALUES 255

/*/BACnetSpecialEvent */
typedef struct BACnet_Special_Event {
    /*/ Flag : choice between [0] calendarEntry [1] ObjectIdentifier */
    uint8_t TagSpecialEventType;
    union {
        /*   calendarEntry  [0] BACnetCalendarEntry,  */
        BACNET_CALENDAR_ENTRY calendarEntry;
        /*   calendarReference [1] BACnetObjectIdentifier  */
        BACNET_OBJECT_ID calendarReference;
    } period;
    /* listOfTimeValues [2] SEQUENCE OF BACnetTimeValue,  */
    //BACNET_TIME_VALUE listOfTimeValues[MAX_SPECIAL_EVENT_VALUES];
    vector<BACNET_TIME_VALUE>vTimeValueList;
    /* eventPriority [3] Unsigned (1..16) */
    uint8_t eventPriority;
} BACNET_SPECIAL_EVENT;


typedef struct BACnet_Property_Ref {
    BACNET_PROPERTY_ID propertyIdentifier;
    uint32_t arrayIndex;        /* Optional */
} BACNET_PROPERTY_REF;



#define MAX_LIST_OF_PROPERTY_REFERENCES 100
#define EMPTY_PROPERTY_REFERENCE_ID ((BACNET_PROPERTY_ID)(~0))



typedef struct BACnet_Read_Access_Specification {
    BACNET_OBJECT_ID objectIdentifier;
    BACNET_PROPERTY_REF listOfPropertyReferences[MAX_LIST_OF_PROPERTY_REFERENCES];
    BACnet_Read_Access_Specification *next;
} BACNET_READ_ACCESS_SPECIFICATION;






 /*
BACnetActionCommand ::= SEQUENCE {
 deviceIdentifier  [0] BACnetObjectIdentifier OPTIONAL,
 objectIdentifier  [1] BACnetObjectIdentifier,
propertyIdentifier  [2] BACnetPropertyIdentifier,
 propertyArrayIndex [3] Unsigned OPTIONAL,   --used only with array datatype
 propertyValue  [4] ABSTRACT-SYNTAX.&Type,
  priority      [5] Unsigned (1..16) OPTIONAL, --used only when property is commandable
 postDelay  [6] Unsigned OPTIONAL,
 quitOnFailure  [7] BOOLEAN,
 writeSuccessful  [8] BOOLEAN
 }
 */

 typedef struct BACnetActionProperty
{
  BACNET_APPLICATION_DATA_VALUE *value;
  uint16_t index;
  uint8_t priority;
  BACNET_PROPERTY_ID property;
}BACNET_ACTION_PROPERTY;

typedef struct BACnetActionCommand
{
 BACNET_OBJECT_ID device;
 BACNET_OBJECT_ID object;
 BACNET_ACTION_PROPERTY *property;
 uint8_t postDelay;
 bool quitOnFail;
 bool writeSuccessful;
}BACNET_ACTION_COMMAND;


typedef struct BACnetActionList
{
 vector<BACNET_ACTION_COMMAND>vBACnetActionCommand;
}BACNET_ACTION_LIST;

typedef struct BACnetActionText
{
 BACNET_APPLICATION_DATA_VALUE value;
 BACnetActionText *next;
}BACNET_ACTION_TEXT;



typedef enum BACnetPropertyStateType{
    BOOLEAN_VALUE,
    BINARY_VALUE,
    EVENT_TYPE,
    POLARITY,
    PROGRAM_CHANGE,
    PROGRAM_STATE,
    REASON_FOR_HALT,
    RELIABILITY,
    STATE,
    SYSTEM_STATUS,
    UNITS,
    UNSIGNED_VALUE,
    LIFE_SAFETY_MODE,
    LIFE_SAFETY_STATE,
} BACNET_PROPERTY_STATE_TYPE;




typedef struct BACnetPropertyState{
    BACNET_PROPERTY_STATE_TYPE tag;
    union {
        bool booleanValue;
        BACNET_BINARY_PV binaryValue;
        BACNET_EVENT_TYPE eventType;
        BACNET_POLARITY polarity;
        BACNET_PROGRAM_REQUEST programChange;
        BACNET_PROGRAM_STATE programState;
        BACNET_PROGRAM_ERROR programError;
        BACNET_RELIABILITY reliability;
        BACNET_EVENT_STATE state;
        BACNET_DEVICE_STATUS systemStatus;
        BACNET_ENGINEERING_UNITS units;
        uint32_t unsignedValue;
        BACNET_LIFE_SAFETY_MODE lifeSafetyMode;
        BACNET_LIFE_SAFETY_STATE lifeSafetyState;
    } state;
} BACNET_PROPERTY_STATE;

typedef enum BACnetChangeOfValueType
{
 CHANGE_OF_VALUE_BITS,
 CHANGE_OF_VALUE_REAL
} CHANGE_OF_VALUE_TYPE;




typedef struct BACnetEventParameter
{
 union {
        struct {
                BACNET_BIT_STRING bitString;
                BACNET_BIT_STRING statusFlags;
               }changeOfBitString;

        struct {
                BACNET_PROPERTY_STATE newState;
                BACNET_BIT_STRING statusFlags;
               }changeOfState;

        struct {
                union {
                       BACNET_BIT_STRING changedBits;
                       float changeValue;
                      } newValue;
                CHANGE_OF_VALUE_TYPE tag;
                BACNET_BIT_STRING statusFlags;
               } changeOfValue;

        struct {
                float referenceValue;
                BACNET_BIT_STRING statusFlags;
                float setPointValue;
                float errorLimit;
               }floatingLimit;

        struct {
                float exceedingValue;
                BACNET_BIT_STRING *statusFlags;
                float deadband;
                float exceededLimit;
               }outOfRange;

        struct {
                BACNET_LIFE_SAFETY_STATE newState;
                BACNET_LIFE_SAFETY_MODE newMode;
                BACNET_BIT_STRING statusFlags;
                BACNET_LIFE_SAFETY_OPERATION operationExpected;
               }changeOfLifeSafety;

        struct {
                BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE bufferProperty;
                uint32_t previousNotification;
                uint32_t currentNotification;
               }bufferReady;

        struct {
                uint32_t exceedingValue;
                BACNET_BIT_STRING statusFlags;
                uint32_t exceededLimit;
               }unsignedRange;
        struct {
                bool presentValue;
                BACNET_BIT_STRING *statusFlags;
                bool feedbackValue;
               }commandFaliure;

        struct {
                uint32_t vendorId;
                uint32_t extendedEventType;
                uint8_t *buffer;
               }extendedEvent;

    } notificationParams;
 uint8_t paramTag;
}BACNET_EVENT_PAREMETER;


typedef struct BACnetScale
{
 uint8_t type;
 float floatScale;
 uint16_t intScale;
}BACNET_SCALE;


typedef struct BACnetPreScale
{
 uint16_t multiplier;
 uint16_t moduloDivide;
}BACNET_PRESCALE;






typedef enum BACnetLogStatus
{
 LOG_DISABLED = 0,
 BUFFER_PURGED = 1,
 LOG_INTERRUPTED = 2
}BACNET_LOG_STATUS;

typedef struct BACnetLogRecord
{
 BACNET_DATE_TIME timeStamp;     // [0]
 uint8_t recType;                // [1]
 BACNET_STATUS_FLAGS statusFlags;  // [2]

 union {
        BACNET_LOG_STATUS logStatus;   // [0]
        bool Boolean;                  // [1]   recType...
        float Real;                    // [2]   recType...
        uint32_t Enum;                 // [3]   ...
        uint32_t Unsigned;             // [4]   ...
        int32_t Signed;                // [5]
        BACNET_BIT_STRING Bit_String;  // [6]
        // NULL VALUE                     [7]   // null, ne hendlamo ovo
        //BACNET_ERROR error;          // [8]   // trenutno ne podrzavamo error
        float Time_Change;             // [9]
        // any_value - ABSTRACT.any    // [10]  // opcionalni ne handlamo ovo
       }logDatum;
}BACNET_LOG_RECORD;


typedef union BACnetLogDataType
{
        bool Boolean;                  // [0]   recType...
        float Real;                    // [1]   recType...
        uint32_t Enum;                 // [2]   ...
        uint32_t Unsigned;             // [3]   ...
        int32_t Signed;                // [4]
        BACNET_BIT_STRING Bit_String;  // [5]
        // NULL VALUE                     [6]   // null, netreba specificirati ovdje
        // failure          ;          // [7]   // ne podrzavamo za sada
        // any_value - ABSTRACT.any    // [8]  // opcionalni ne handlamo ovo
}BACNET_LOG_DATA_TYPE;

typedef struct BACnetLogData
{
 BACNET_LOG_STATUS logStatus; // [0]
 uint8_t recType;

 vector<BACNET_LOG_DATA_TYPE>vLogData;

 float timeChange;
}BACNET_LOG_DATA;

typedef struct BACnetLogMultipleRecord
{
 BACNET_DATE_TIME timeStamp;
 BACNET_LOG_DATA logData;
}BACNET_LOG_MULTIPLE_RECORD;


typedef struct BACnetEventLogRecord
{
 BACNET_DATE_TIME timeStamp;
 uint8_t type;

 union {
        BACNET_LOG_STATUS logStatus;   // [0]
        // notification [1]
        float time_change; //  [2];
       }logDatum;

}BACNET_EVENT_LOG_RECORD;





typedef enum BACnetAccumulatorStatus
{
 ACCU_STATUS_NORMAL = 0,
 ACCU_STATUS_STARTING = 1,
 ACCU_STATUS_RECOVERED = 2,
 ACCU_STATUS_ABNORMAL = 3,
 ACCU_STATUS_FAILED = 4,
 ACCU_STATUS_MAX = 5,
}BACNET_ACCUMULATOR_STATUS;

typedef struct BACnetAccumulatorRecord
{
 BACNET_DATE_TIME timeStamp;  // [0]
 uint32_t presentValue;       // [1]
 uint32_t accumulatedValue;   // [2]
 BACNET_ACCUMULATOR_STATUS accuStatus;   // [3];
}BACNET_ACCUMULATOR_RECORD;


 // TODO: FaultParameter struct standard 2012 stranica 682
typedef struct FaultParameter
{
 int emtpy;
}FAULT_PARAMETER;


typedef struct BACnetFaultLifeSafety
{
 BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE ref;
 vector <BACNET_LIFE_SAFETY_STATE>listOfLifeStates;
}BACNET_FAULT_LIFE_SAFETY;


typedef struct BACnetFaultState
{
 vector<BACNET_PROPERTY_STATE>listOfStates;
}BACNET_FAULT_STATE;

typedef struct BACnetFaultExtended
{
 uint16_t vendor_id;  // [0]
 uint8_t extended_fault_type;  // [1]
 vector<FAULT_PARAMETER>faultParameterList;  // [2] // TODO:
}BACNET_FAULT_EXTENDED;

typedef struct BACnetFaultParameter
{
 BACNET_FAULT_TYPE type;
 struct {
         vector<string>listOfFaultValues; // [1]. Svaki string je odvojen s [0] max 10
         vector<BACNET_FAULT_EXTENDED>listOfExtendedFaults; // [2]
         vector<BACNET_FAULT_LIFE_SAFETY>listOfLifeStates; // [3]
         vector<BACNET_FAULT_STATE>listOfStates;  // [4]
         vector<BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE>listOfFlagsReferences; // [5]
       }value;

}BACNET_FAULT_PARAMETER;

typedef struct BACnetShedLevel
{
 uint8_t type;
 union {
        uint16_t percent;
        uint16_t level;
        float amount;
       }value;

}BACNET_SHED_LEVEL;


typedef struct BACnetPolicy
{
 BACNET_DEVICE_OBJECT_REFERENCE credential_data_input;  // [0]
 uint16_t index;  // [1]
}BACNET_POLICY;

typedef struct BACnetAuthenticationPolicy
{
 vector<BACNET_POLICY>vPolicy; //[0]
 bool order_enforced;  // [1]
 uint16_t timeout; // [2]
}BACNET_AUTHENTICATION_POLICY;


typedef struct BACnetAccessRule
{
 uint32_t timeRangeSpecifier;  // open/close [0], enum_context [0] - specified, [1] always
 BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE timeRange;  // OPTIONAL [1];
 uint32_t locationSpecifier; // open/close [2], enum_context [0] - specified, [1] all
 BACNET_DEVICE_OBJECT_REFERENCE location;  // OPTIONAL [3];
 bool enable; // [4]
}BACNET_ACCESS_RULE;




typedef struct BACnetAuthenticationFactor
{
 BACNET_AUTHENTICATION_FACTOR_TYPE factor_type;
 uint16_t format_class;
 BACNET_OCTET_STRING ostr;
}BACNET_AUTHENTICATION_FACTOR;

typedef struct BACnetCredentialAuthenticationFactor
{
 BACNET_ACCESS_AUTH_FACTOR_DISABLE disable; // [0]
 BACNET_AUTHENTICATION_FACTOR auth_factor;  // [1]
}BACNET_CREDENTIAL_AUTH_FACTOR;


typedef struct BACnetAssignedAccessRights
{
 BACNET_DEVICE_OBJECT_REFERENCE ref;
 bool enable;
}BACNET_ASSIGNED_ACCESS_RIGHTS;


typedef struct BACnetAuthenticationFactorFormat
{
 BACNET_AUTHENTICATION_FACTOR_TYPE formal_type;
 uint16_t vendor_id;
 uint16_t vendor_format;
}BACNET_AUTHENTICATION_FACTOR_FORMAT;


typedef struct BACnetNetworkSecurityPolicy
{
 uint8_t port_id;
 BACNET_SECURITY_POLICY sec_level;
}BACNET_NETWORK_SECURITY_POLICY;

typedef struct BACnetKeyIdentifier
{
 uint8_t algorithm;
 uint8_t key_id;
}BACNET_KEY_IDENTIFIER;

typedef struct BACnetSecurityKeySet
{
 uint8_t key_revision;
 BACNET_DATE_TIME activation_time;
 BACNET_DATE_TIME expiration_time;
 vector<BACNET_KEY_IDENTIFIER>key_id;
}BACNET_SECURITY_KEY_SET;



typedef struct BACnetEventNotificationSubscription
{
 BACNET_RECIPIENT recipient; // [0]
 uint32_t process_id;  // [1]
 bool issueConfirmedNotifications; // [2]
 uint16_t timeRemaining;
}BACNET_EVENT_NOTIFICATION_SUBSCRIPTION;


typedef struct BACnetPortPermission
{
 uint8_t port_id;
 bool enabled;
}BACNET_PORT_PERMISSION;


typedef struct BACnetLightingCommand
{
 BACNET_LIGHTING_OPERATION operation;
 float target_level;
 float ramp_ratel;
 float step_increment;
 uint32_t fade_time;
 uint32_t priority;
}BACNET_LIGHTING_COMMAND;




typedef struct BACnetHostAddress
{
 BACNET_HOST_ADDRESS_TAG tag;
     struct {
             BACNET_OCTET_STRING ip_address;
             BACNET_CHARACTER_STRING name;
           }type;
 
}BACNET_HOST_ADDRESS;


typedef struct BACnetHostNPort
{
 BACNET_HOST_ADDRESS address;
 uint16_t port;
}BACNET_HOST_NPORT;


typedef struct BACnetBDTEntry
{
 BACNET_HOST_NPORT bbmd_address;  //[0]
 BACNET_OCTET_STRING broadcast_mask; //[1] - OPTIONAL:   present if BAC/IP, absent if BAC/IPV6
}BACNET_BDT_ENTRY;


typedef struct BACnetFDTEntry
{
 BACNET_OCTET_STRING ip_address;
 uint16_t ttl;
 uint16_t rem_ttl;
}BACNET_FDT_ENTRY;


typedef struct BACnetVMACEntry
{
 BACNET_OCTET_STRING virtual_mac_address;
 BACNET_OCTET_STRING native_mac_address;
}BACNET_VMAC_ENTRY;


typedef struct BACnetRouterEntry
{
 uint16_t network_number;
 BACNET_OCTET_STRING mac_address;
 uint32_t status;
 uint8_t performance_index;    // - optional
}BACNET_ROUTER_ENTRY;


typedef struct BACnetValueSource
{
 uint8_t tag;
 BACNET_ADDRESS adr;
 BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE ref;
}BACNET_VALUE_SOURCE;


typedef struct BACNET_COMPLEX_OBJECT
{
 vector<BACNET_DEVICE_OBJECT_REFERENCE*>vDeviceObjectReference;
 BACNET_DATE_TIME dateTime;
 vector<BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE*>vDeviceObjectPropertyReference;
 vector<BACNET_OBJECT_PROPERTY_REFERENCE*>vObjectPropertyReference;
 vector<BACNET_RECIPIENT*>vRecipientList;
 vector<BACNET_DESTINATION*>vDestinationList;
 BACNET_DATE_RANGE dateRange;
 vector<BACNET_TIMESTAMP*>vTimeStamps;
 vector<BACNET_COV_SUBSCRIPTION*>vActiveCOVSubs;
 vector<BACNET_CALENDAR_ENTRY*>vCalendarEntryList;
 BACNET_WEEKLY_SCHEDULE weeklySchedule;
 vector<BACNET_SPECIAL_EVENT>vSpecialEventList;
 BACNET_READ_ACCESS_SPECIFICATION *readAccessSpec;
 vector<BACNET_ADDRESS_BINDING*>vBACnetAddressBindingList;
 vector<BACNET_ACTION_LIST*>vBACnetActionList;
 BACNET_ACTION_TEXT *actionText;
 BACNET_EVENT_PAREMETER eventParam;
 BACNET_SCALE scale;
 BACNET_PRESCALE preScale;
 BACNET_SHED_LEVEL shedLevel;
 vector<BACNET_LOG_RECORD*>vLogRecordList;
 vector<BACNET_LOG_MULTIPLE_RECORD*>vLogMultipleList;
 vector<BACNET_EVENT_LOG_RECORD*>vEventLogRecordList;
 BACNET_ACCUMULATOR_RECORD accuRecord;  // TODO: ovo treba biti lista vrijednosti
 BACNET_FAULT_PARAMETER faultParam;
 vector<BACNET_AUTHENTICATION_POLICY>vPolicyList;
 vector<BACNET_ACCESS_RULE>vRuleList;
 vector<BACNET_CREDENTIAL_AUTH_FACTOR>vCredentialAuthFactorList;
 vector<BACNET_ASSIGNED_ACCESS_RIGHTS>vAssignedAccessRights;
 vector<BACNET_AUTHENTICATION_FACTOR_FORMAT>vAuthenticationFormatList;
 vector<BACNET_NETWORK_SECURITY_POLICY>vSecurityPolicyList;
 vector<BACNET_SECURITY_KEY_SET>vSecurityKeySet;  // samo 2
 vector<BACNET_EVENT_NOTIFICATION_SUBSCRIPTION>vNotificationSubList;
 vector<BACNET_PORT_PERMISSION>vPortPermissionList;
 vector<BACNET_LIGHTING_COMMAND>vLightingCommandList;
 BACNET_HOST_NPORT *hostnPort;
 vector<BACNET_BDT_ENTRY*>vBDTEntry;
 vector<BACNET_FDT_ENTRY*>vFDTEntry;
 vector<BACNET_VMAC_ENTRY*>vVMACEntry;
 vector<BACNET_ROUTER_ENTRY*>vRouterEntry;
 vector<BACNET_VALUE_SOURCE*>vValueSource;
 
 
}BACNET_COMPLEX_OBJECT;



typedef struct BACNET_ERROR
{
 BACNET_ERROR_CODE errCode;
 BACNET_ERROR_CLASS errClass;
 //string err_msg;   Mozda ce lookout srati ovdje. Ali ovo nesmije biti tu.
}BACNET_ERROR;




typedef struct BACNET_OBJECT_PROPERTY
{
  BACNET_PROPERTY_ID property;
  vector<BACNET_APPLICATION_DATA_VALUE*>vValueList;
  BACNET_APPLICATION_DATA_VALUE *value;  // legacy code. Maknuti value i sitaviti samo vValueList
  BACNET_COMPLEX_OBJECT complexValue;    // samo kod specificnih complex vrijednosti. Vidi definiciju gore.
  bool readOnly;
  uint16_t index;
  uint8_t priority;
  bool propertyAccessError;   // log_buffer kod tren loga npr moze vratiti [5e][5f] tag s read-access-denied.
  BACNET_ERROR bac_err;  // samo ako je propertyAccessError == true
}BACNET_OBJECT_PROPERTY;


typedef struct
{
 BACNET_OBJECT_ID oid;
 bool eof;
 int access_type;
 int start_pos;
 uint32_t octet_count;
 BACNET_OCTET_STRING data;
}BACNET_FILE;

// bacnet object
typedef struct BACNET_OBJECT
{
 BACNET_OBJECT_ID oid;
 string name;
 vector<BACNET_OBJECT_PROPERTY*>vProperties;
 int currentTimeDelay;  // koristi se kod intrinsickog reporta za pracenje
 BACNET_FILE bac_file; // koristi se samo ako je oid.type == FILE za pohranu octet podataka u fileu
                       // TODO: preoblikovati u nesto drugo
}BACNET_OBJECT;




typedef struct BACNET_OBJECT_SUBSCRIPTION
{
 int processId;
 BACNET_OBJECT *object;
 uint32_t lifetime;
 bool confirmed;
 BACNET_PROPERTY_ID property;
 bool covIncrementPresent;
 float covIncrement;


}BACNET_OBJECT_SUBSCRIPTION;



typedef struct BACNET_DEVICE
{
 BACNET_OBJECT_ID oid;
 uint16_t vendorId;
 BACNET_SEGMENTATION segmentation;
 unsigned int maximumAPDU;
 BACNET_NPDU_DATA *npduData;
 vector<BACNET_OBJECT*>vObjects;
}BACNET_DEVICE;




typedef struct DESTINATION_NETWORK
{
 uint16_t net_num;
 //BACNET_DATALINK data_link;
}DESTINATION_NETWORK;



typedef struct BACnetRouter
{
 //BACNET_DEVICE *device;
 BACNET_ADDRESS *router_address;
 BACNET_DATALINK data_link;
 vector<uint16_t>vDestinationNetworkList;
}BACNET_ROUTER;


// Confirmed Service struktura
typedef struct BACNET_SERVICE_CHOICE_CONFIRMED
{
  bool segmented_message;
  bool more_follows;
  bool segmented_response_accepted;
  int max_segs;
  int max_resp;
  uint8_t invoke_id;
  uint8_t sequence_number;
  uint8_t proposed_window_number;
  BACNET_CONFIRMED_SERVICE serviceChoice;
}BACNET_SERVICE_CHOICE_CONFIRMED;




typedef struct BACNET_CONFIRMED_MESSAGE
{
 int invokeId;
 uint16_t objectType;
 uint32_t objectInstance;
 BACNET_MESSAGE_PRIORITY priority;
 BACNET_MESSAGE_CLASS messageClass;
 uint32_t numValue;
 string strValue;
 string message;
}BACNET_CONFIRMED_MESSAGE;





// GetAlarmSummary service struktura
//typedef struct BACNET_ALARM
//{
// uint32_t instance;
// uint16_t type;
// BACNET_EVENT_STATE eventState;
// uint8_t transitionBits;
//}BACNET_ALARM;



// GetEventInformation service struktura
typedef struct BACNET_EVENT
{
 uint32_t objectInstance;
 uint16_t objectType;
 BACNET_EVENT_STATE eventState;
 uint8_t transitionBits;
 BACNET_TIMESTAMP eventTimeStamp[3];
 BACNET_NOTIFY_TYPE notifyType;
 uint8_t eventEnable;
 uint32_t eventPriority[3];
}BACNET_EVENT;


// GetEnrollmentSummary service struktura
typedef struct BACNET_ENROLLMENT
{
 uint32_t objectInstance;
 uint16_t objectType;
 BACNET_EVENT_TYPE eventType;
 BACNET_EVENT_STATE eventState;
 BACNET_MESSAGE_PRIORITY minPriority;
 BACNET_MESSAGE_PRIORITY maxPriority;
 uint32_t notificationClass;
}BACNET_ENROLLMENT;


typedef struct BACNET_LIST_ELEMENT
{
 uint16_t type;
 uint32_t instance;
 BACNET_PROPERTY_ID propertyId;
 uint16_t arrayIndex;

}BACNET_LIST_ELEMENT;



typedef struct BACNET_ACK_ALARM
{
 uint32_t ackProcessId;
 BACNET_OBJECT_ID event_oid;
 BACNET_EVENT_STATE eventState;
 BACNET_TIMESTAMP timeStamp;
 string ackSource;
 BACNET_TIMESTAMP ackTime;
}BACNET_ACK_ALARM;

typedef struct BACNET_EVENT_NOTIFICATION
{
 uint32_t processId;
 BACNET_OBJECT_ID deviceId;
 BACNET_OBJECT_ID objectId;
 BACNET_TIMESTAMP timeStamp;
 uint32_t notificationClass;
 uint8_t priority;
 BACNET_EVENT_TYPE eventType;
 BACNET_APPLICATION_DATA_VALUE messageText;
 BACNET_NOTIFY_TYPE notifyType;
 bool ackReq;
 BACNET_EVENT_STATE fromState;
 BACNET_EVENT_STATE toState;
 BACNET_EVENT_PAREMETER eventParams;
 BACNET_ADDRESS *source_device_address;
}BACNET_EVENT_NOTIFICATION;


// koristi se kod GetEnrollmentSummary servisa
typedef enum BACnetAckedFilter{
 FILTER_ALL = 0,
 FILTER_ACKED = 1,
 FILTER_NOT_ACKED = 2,
}BACNET_ACKED_FILTER;


//typedef struct BACNET_ENROLLMENT_SUMMARY
//{
// BACNET_ACKED_FILTER ackedFilter;
// struct {
//         uint32_t deviceId;
//         uint16_t deviceType;
//         uint32_t processId;
//        }recipients;
//
// BACNET_EVENT_STATE eventState;
// BACNET_EVENT_TYPE eventType;
//
// uint16_t minPrioroty;
// uint16_t maxPriority;
//
// uint32_t notificationClass;
//}BACNET_ENROLLMENT_SUMMARY;





typedef struct OBJECT_ERROR
{
 int invokeId;
 string fromAdr;
 BACNET_PROPERTY_ID property;
 BACNET_OBJECT_TYPE object;
 uint32_t instance;
}OBJECT_ERROR;


typedef struct BACNET_LIFE_SAFETY
{
 uint32_t processId;
 string source;
 BACNET_LIFE_SAFETY_OPERATION request;
 uint32_t instance;
 uint32_t type;
}BACNET_LIFE_SAFETY;


typedef struct BACNET_PRIVATE_TRANSFER
{
 uint32_t vendorId;
 uint32_t serviceNumber;
 void *serviceParameters; // lokalna stvar
}BACNET_PRIVATE_TRANSFER;


typedef struct BACNET_ATOMIC_READ_WRITE
{
 uint32_t fileInstance;
 bool streamAccess;
 int startPos;
 int octetCount;
 uint32_t recordCount;
 uint8_t *fileData;
 bool eof;
}BACNET_ATOMIC_READ_WRITE;

typedef enum BACnetReadRangeChoice
{
 RANGE_BY_POSITION = 0,
 RANGE_BY_SEQUENCE = 1,
 RANGE_BY_TIME = 2
}BACNET_RANGE_CHOICE;

typedef struct BACnetRange
{
 uint32_t index;
 uint32_t seqNumber;
 BACNET_DATE_TIME datetime;
 int count;
}BACNET_RANGE;


typedef struct BACNET_READ_RANGE
{
 BACNET_OBJECT_ID oid;
 BACNET_PROPERTY_ID property;
 uint32_t arrayIndex;
 BACNET_RANGE_CHOICE requestType;
 BACNET_RANGE rangeParam;
 BACNET_BIT_STRING resultFlags;
}BACNET_READ_RANGE;



typedef struct BACNET_TEXT_TEXT_PAIR
{
 const char *text;
 const char *name;
}BACNET_TEXT_TEXT_PAIR;

typedef struct BACNET_INDEX_TEXT_PAIR
{
 uint16_t index;
 const char *name;
}BACNET_INDEX_TEXT_PAIR;


typedef struct BACnet_COV_Value
{
 BACNET_PROPERTY_ID property;
 BACNET_APPLICATION_DATA_VALUE *value;
}BACNET_COV_VALUE;


typedef struct BACNET_COV_NOTIFICATION
{
 uint32_t processId;
 BACNET_OBJECT_ID object;
 BACNET_OBJECT_ID device;
 uint32_t seconds;
 vector<BACNET_COV_VALUE>vValueList;
}BACNET_COV_NOTIFICATION;




typedef struct BACNET_DEVICE_RANGE
{
 uint32_t from;
 uint32_t to;
}BACNET_DEVICE_RANGE;


/*
SubscribeCOV-Request ::= SEQUENCE {
        subscriberProcessIdentifier  [0] Unsigned32,
        monitoredObjectIdentifier    [1] BACnetObjectIdentifier,
        issueConfirmedNotifications  [2] BOOLEAN OPTIONAL,
        lifetime                     [3] Unsigned OPTIONAL
        }
*/
//
//typedef struct BACnet_COV_Data {
//    uint32_t subscriberProcessIdentifier;
//    uint32_t initiatingDeviceIdentifier;
//    BACNET_OBJECT_ID monitoredObjectIdentifier;
//    uint32_t timeRemaining;     /* seconds */
//    /* simple linked list of values */
//    BACNET_PROPERTY_VALUE *listOfValues;
//} BACNET_COV_DATA;
//
//

typedef struct BCL_MONITOR_PROPERTY
{
 BACNET_DEVICE *dev;
 BACNET_OBJECT_PROPERTY *prop;
 BACNET_OBJECT *owner;
}BCL_MONITOR_PROPERTY;



typedef struct EVENT_INFORMATION
{
 BACNET_OBJECT_ID oid;
 BACNET_EVENT_STATE eventState;
 BACNET_TIMESTAMP eventTimeStamps[3];
 BACNET_NOTIFY_TYPE notifyType;
 uint16_t eventPriorities[3];
 BACNET_BIT_STRING bstrAckTrans;
 BACNET_BIT_STRING bstrEventEnable;
}EVENT_INFORMATION;


typedef struct BACNET_EVENT_INFORMATION
{
 vector<EVENT_INFORMATION>vEventInformationList;
 bool more_events;
}BACNET_EVENT_INFORMATION;





typedef struct BACnet_Alarm_Summary
{
 BACNET_OBJECT_ID oid;
 BACNET_EVENT_STATE alarmState;
 BACNET_BIT_STRING bstrTransBits;
}BACNET_ALARM_SUMMARY;


typedef struct BACnet_Alarm
{
 vector<BACNET_ALARM_SUMMARY>vAlarmSummary;
}BACNET_ALARM;



typedef struct BACnetSimpleACK
{
 //uint16_t invokeId;
 uint16_t serviceChoice;
}BACNET_SIMPLE_ACK;


typedef struct BACnetComplexACK
{
 uint16_t invokeId;
 uint16_t serviceChoice;
 BACNET_OBJECT_ID oid;
 BACNET_OBJECT_PROPERTY *prop;
}BACNET_COMPLEX_ACK;


typedef struct BACnetSegmentACK
{
 BACNET_PDU_TYPE pduType;   // [0]
 uint8_t reserved;          // [1]
 bool negativeACK;          // [2]  - out of order packet?
 bool server;               // [3]  - send by server?
 uint8_t invokeId;          // [4]
 uint8_t sequenceNum;       // [5]
 uint8_t windowSize;        // [6]
}BACNET_SEGMENT_ACK;



typedef struct EnrollmentSummary
{
 BACNET_OBJECT_ID oid;
 BACNET_EVENT_TYPE event_type;
 BACNET_EVENT_STATE event_state;
 uint16_t priority;
 uint16_t notification_class;
}ENROLLMENT_SUMMARY;

typedef struct BACnetEnrollmentSummary
{
 vector<ENROLLMENT_SUMMARY>vEnrollmentSummary;
}BACNET_ENROLLMENT_SUMMARY;


typedef enum BACnetSupportedService{
 ACK_ALARM = 0,
 CONFIRMED_COV_NOTIF = 1,
 CONFIRMED_EVENT_NOTIF = 2,
 GET_ALARM_SUMM = 3,
 GET_ENROLL_SUMM = 4,
 SUBSCRIBE_COV = 5,
 ATOMIC_READ = 6,
 ATOMIC_WRITE = 7,
 ADD_LIST_ELEM = 8,
 REM_LIST_ELEM = 9,
 CREATE_OBJECT = 10,
 DELETE_OBJECT = 11,
 READ_PROP = 12,
 READ_PROP_COND = 13,
 READ_PROP_MULTI = 14,
 WRITE_PROP = 15,
 WRITE_PROP_MULTI = 16,
 DEV_COMM_CONTROL = 17,
 CONF_PRIVATE_TRANS = 18,
 CONF_TEXT_MSG = 19,
 REINIT_DEVICE = 20,
 VT_OPEN = 21,
 VC_CLOSE = 22,
 VT_DATA = 23,
 AUTHENTICATE = 24,
 REQUEST_KEY = 25,
 I_AM = 26,
 I_HAVE = 27,
 UNCONF_COV_NOTIF = 28,
 UNCONF_EVENT_NOTIF = 29,
 UNCONF_PRIV_TRANS = 30,
 UNCONF_TEXT_MSG = 31,
 TIME_SYNC = 32,
 WHO_HAS = 33,
 WHO_IS = 34,
 READ_RANGE = 35,
 UTC_TIME_SYNC = 36,
 LIFE_SAFETY_OP = 37,
 SUBSCRIBE_COV_PROP = 38,
 GET_EVENT_INFO = 39,
 SERVICE_SUPPORTED_WRITE_GROUP = 40
}BACNET_SUPPORTED_SERVICE;










#endif //BACNET_H
