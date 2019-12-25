#ifndef BACAPI_H
#define BACAPI_H

/* bacapi.h"

   definicije globalnih struktura, enumeracija i funkcija koje se koriste ili interno
   ili od strane korisnika u BACapi-u.

   Nedokumentirane funkcije se ne koriste direktno vec su namjenjene internom radu API-u
*/


#include "bacnet.h"
#include "bacevent.h"
#include "bacobject.h"
#include "namedef.h"
#include "bacerr.h"
#include <string>

#if defined BACAPI_BUILD_ARM9 || defined BACAPI_BUILD_LINUX
#include "pevents.h"
using namespace neosmart;
#endif

#ifdef BACAPI_BUILD_WIN32
#include <winsock2.h>
//#include <stdafx.h>
#endif

//-----
//#define MAGIC_NUMBER_HEX 0x279
//#define MAGIC_NUMBER_HEX 0x279
//#define MAGIC_NUMBER_DEC 633

#define MAGIC_NUMBER_HEX 0x283
#define MAGIC_NUMBER_DEC 643


//using namespace std;


// provjeriti ove definicije


#define BACAPI_PROPERTY_COUNT MAX_ASHRAE_PROPERTY_COUNT

#define MAX_BACNET_ERROR_CLASS 65535  // sve nakon 64 propriatery
#define MAX_BACNET_ERROR_CODE 65535 // isto ko i gore


// NOTE: ove definicije bi trebalo provjeriti gdje se i kako upotrebljuju
#define MAX_BACNET_SUPPORTED_SERVICES 40
#define MAX_BACNET_SUPPORTED_OBJECTS 21
#define MAX_BACNET_STATUS_FLAGS 4
#define MAX_BACNET_MESSAGE_PRIORITY 4
#define MAX_BACNET_EVENT_STATE 5
#define MAX_BACNET_WEEKDAYS 6
#define MAX_BACNET_EVENT_TRANSITION_BITS 3
#define MAX_BACNET_REJECT_CODE 11
#define MAX_BACNET_ABORT_REASON 4
#define MAX_BACNET_ENGINEERING_UNITS 190
#define MAX_BACNET_SEGMENTATION 3
#define MAX_BACAPI_ERROR 9
#define MAX_BACNET_PRIORITY 17
#define MAX_BACNET_VENDOR_ID 621
#define MAX_BACAPI_ERROR_STATES -11 // ???


// NOTE: ???
#define PROP_PROPRIATERY_DOP_REFERENCE 511 +12


extern bool BACAPI_segmentedFlag;



// I_cBACClient
typedef struct BACAPI_SubscribeCOV
{
 uint32_t processId;
 BACNET_OBJECT_ID oid;
 bool confirmedNotification;
 uint32_t lifetime;
 BACNET_PROPERTY_ID propId;
 float increment;
 bool cancel;
}BACAPI_SUBSCRIBE_COV;

// I_cBACClient
typedef struct BACAPI_EnrollmentSummary
{
 uint8_t ack_filter;
 vector<BACNET_RECIPIENT_PROCESS>vEnrollmentList;
 BACNET_EVENT_STATE state;
 BACNET_EVENT_TYPE type;
 uint8_t priority_min;
 uint8_t priority_max;
 uint16_t notification_class;
}BACAPI_ENROLLMENT_SUMMARY;


// Upotrebljuje se kod rezervacije Invoke ID-a za sada se koristi samo iz Lookouta
// da bi se razlikovali razliciti tipovi Invokova.
// TODO: Maknuti ovo i omoguciti user-definirane tipove invokova
typedef enum BACAPI_Invoke_Type
{
 INVOKE_TYPE_NORMAL,
 INVOKE_TYPE_MONITOR,
 INVOKE_TYPE_STRESS_TEST,
 INVOKE_TYPE_PID,
 INVOKE_TYPE_BTL_TEST
}BACAPI_INVOKE_TYPE;


// BACAPI_INVOKE_ID
// Svaki BACnet servis koji ima povratnu informaciju koristi InvokeId uin8_t broj
// da bi se povezali zahtjevi i odgovori na asinhronim implementacijama BACnet-a
// Ova strujktura grupira za korisnika device objekt i/ili property.
// Koristi se u grupi s BACAPI_ReserveInvokeId, BACAPI_GetInvokeId i BACAPI_ReleaseInvokeId
typedef struct BACAPI_InvokeID
{
 BACNET_DEVICE *dev;
 BACNET_OBJECT *obj;
 BACNET_OBJECT_PROPERTY *prop;
 uint8_t invokeId;
 bool active_;
 BACAPI_INVOKE_TYPE invoke_type;
}BACAPI_INVOKE_ID;




// ove dvije strukture koriste se kod programiranja device-a s API-em
// da bi prenjeli informacije dobivene s PollEvent u device.
typedef enum
{
 BACAPI_REQUEST_NULL = 0,
 BACAPI_REQUEST_WHO_IS = 1,
 BACAPI_REQUEST_READ_PROPERTY = 2,
 BACAPI_REQUEST_READ_PROPERTY_MULTI = 3,
 BACAPI_REQUEST_WRITE_PROPERTY = 4,
 BACAPI_REQUEST_I_AM = 5,
 BACAPI_REQUEST_RP_ACK = 6,
 BACAPI_REQUEST_CREATE_OBJECT = 7,
 BACAPI_REQUEST_DELETE_OBJECT = 8,
 BACAPI_REQUEST_WHO_HAS = 9,
 BACAPI_REQUEST_ATOMIC_READ_FILE = 10,
 BACAPI_REQUEST_READ_RANGE = 11,
 BACAPI_REQUEST_ATOMIC_WRITE_FILE = 12,
}BACAPI_REQUEST_TYPE;



typedef struct BACAPI_REQUEST
{
 BACAPI_REQUEST_TYPE requestType;
 int invokeId;  // uint8_t
 BACNET_DATALINK srcDataLink;
 std::string extra;
    struct {
           BACNET_DEVICE_RANGE deviceRange;  // WHO_IS
           BACNET_OBJECT_PROPERTY_REFERENCE *opRef;
           BACNET_OBJECT_ID oid;
           struct {
                   BACNET_OBJECT *object;
                   BACNET_OBJECT_PROPERTY *property;
                   BACNET_OBJECT_ID oid;
                  }objectProperty;

           uint32_t test;
           BACNET_DEVICE *ptrDevice;
           BACNET_ADDRESS *srcAdr;
           BACNET_FILE *bac_file;
           BACNET_READ_RANGE *range_data;
          }request;
}BACAPI_REQUEST;



// Kod segmentiranog citanja
typedef struct BACAPI_Segment
{
 uint8_t sequence_number;
 uint8_t window_size;
 uint8_t *data;
 size_t datalen;
}BACAPI_SEGMENT;





/* BACAPI_EnumType, BACAPI_EnumStruct i  vEnumList
   su interne strukture nativne za korisnik ne smije imati pristup ovome
*/
typedef enum BACAPI_EnumType
{
 BACAPI_ENUM_TYPE_EVENT_STATE,
 BACAPI_ENUM_TYPE_COUNT
}BACAPI_ENUM_TYPE;



typedef struct BACAPI_EnumStruct
{
 BACNET_INDEX_TEXT_PAIR *pair;
 //vector<BACNET_PROPERTY_ID>vPropertyList;  // ako se propery nalazi na ovoj listi vrati ovaj par
 BACNET_PROPERTY_ID propid;
 BACNET_OBJECT_TYPE obj_type;
 uint16_t count;
}BACAPI_ENUM_STRUCT;



typedef struct BACAPI_ROUTER_INFO
{
 BACNET_DATALINK datalink;
 BACNET_ADDRESS address;
}BACAPI_ROUTER_INFO;




typedef struct BACAPI_DNET_NUMBER
{
 uint16_t dnet;   // dnet
 BACAPI_ROUTER_INFO  router_info;  // router_info i adresa routera za ovaj destination number
}BACAPI_DNET_NUMBER;



typedef struct BACAPI_ROUTER_PORT
{
 BACNET_DATALINK datalink;
 uint16_t dnet;   // asociran s ovim portom. Mogli samo ga staviti i u vDNETList ali ovako se lakse snalazimo
 //BACNET_ADDRESS *adr;
 uint8_t port_id;
 vector<BACAPI_DNET_NUMBER>vDNETList;  // adresa i lista dnet-a na koji se routa
}BACAPI_ROUTER_PORT;


// vEnumList se ovdje otkriva prema ostatku API-a. Definicija se nalazi u enumpairs.cpp
extern vector<BACAPI_ENUM_STRUCT>vEnumList;


// NOTE: exspozane varijable za korisnicku upotrebu.
// TODO: mozda nebi trebali imati ovo
extern BACNET_ADDRESS *BACAPI_MyAddress;
extern BACNET_ADDRESS *BACAPI_BroadcastAddress;
extern BACNET_ADDRESS *BACAPI_GlobalBroadcastAddress;

/* BACAPI_Init();
   Stvorit ce 256 invokova i inicirati ce enumeracijske parove.
   Ovo je prva funkcija koja se pokrece kada se radi s API-em.
*/
extern void BACAPI_Init(void);
/* BACAPI_Fini();
   Oslobodit ce sve invokove. Mora se pozvati pri izlazu iz programa.
*/
extern void BACAPI_Fini(void);
extern void BACAPI_InitEnumTypes(void);  // nedokumentirana, zove se samo iz BACAPI_Init()

/* BACAPI_GetBACnetObjectCount()
   vraca MAX_ASHRAE_OBJECT_COUNT
*/
extern uint16_t BACAPI_GetBACnetObjectCount(void);


/* BACAPI_GetBACnetPropertyCount()
   vraca MAX_ASHRAE_PROPERTY_COUNT
*/
extern uint16_t BACAPI_GetBACnetPropertyCount(void);

/* Nedokumentirane, ne koristiti
*/
extern void BACAPI_PrintOctetData(string filename, uint8_t *data, size_t data_len);
extern string BACAPI_GetOctetData(uint8_t *data, size_t data_len);

// konverzacije
extern int BACAPI_ToInt(string s);
extern float BACAPI_ToFloat(string s);
extern double BACAPI_ToDouble(string s);
extern uint32_t BACAPI_ToUint32(string s);

// TODO: testirati ove konverzije
extern string BACAPI_ToString(int n);
extern string BACAPI_ToString8(uint8_t n);
extern string BACAPI_ToString16(uint16_t n);
extern string BACAPI_ToString32(uint32_t n);
extern string BACAPI_ToStringr(double n);
extern string BACAPI_ToStringf(float n);

// Provjeravaju da li je vrijeme u dozvoljenim granicama npr n<0,  n>59
extern bool BACAPI_IsCorrectTime(BACNET_TIME *tm);
extern bool BACAPI_IsCorrectDate(BACNET_DATE *date);
extern void BACAPI_CorrectBACnetTime(BACNET_TIME *tm);
extern void BACAPI_CorrectBACnetDate(BACNET_DATE *dt);

// nedokumentirana, ne koristiti
extern int BACAPI_GetBACnetObjectType(string name);

// stvaranje BACnet adresa
extern BACNET_ADDRESS *BACAPI_CreateBACnetAddress(void);
extern BACNET_ADDRESS *BACAPI_CreateBACnetAddress(uint32_t ipadr, uint16_t port, uint16_t netnum);  // kod ip adrese
extern BACNET_ADDRESS *BACAPI_CreateBACnetAddress(unsigned char *adr, uint16_t port, uint16_t netnum);  // kod ethernet adrese
extern BACNET_ADDRESS *BACAPI_CreateBACnetAddress(unsigned char *adr, uint16_t netnum);  // kod ethernet adrese
extern BACNET_ADDRESS *BACAPI_CreateBACnetAddress(BACNET_ADDRESS *adr);  // kod ethernet adrese
extern BACNET_ADDRESS *BACAPI_CreateBACnetAddress(uint8_t src, uint16_t netnum); // kod mstp adrese
extern BACNET_ADDRESS *BACAPI_CreateBACnetAddress(const char *s, uint16_t port, uint16_t nn);
extern void BACAPI_CopyBACnetAddress(BACNET_ADDRESS *to, BACNET_ADDRESS *from);

// Prazne, nedokumentirane. Ne koristiti
extern void BACAPI_SetMyAddress(unsigned char *adr, uint16_t port, uint16_t netnum);
extern void BACAPI_SetBroadcastAddress(unsigned char *adr);


// Legacy. Ne koristiti.
extern uint32_t BACAPI_GetBroadcastAddress(uint8_t port);

/* BACAPI_GetAddressStr();
  Vraca adresni char string izveden iz uin32_t
*/
extern char *BACAPI_GetAddressStr(uint32_t adr);

/* BACAPI_GetAddressStr();
  Vraca uint32_t izveden iz tockane IP adresse npr "10.0.1.22"
*/
extern uint32_t BACAPI_GetAddressUint32(const char *adr);

/* BACAPI_GetAddressStr();
  Vraca uint32_t izveden iz MAC IP adrese npr. aa.bb.dd.cc
*/
extern uint32_t BACAPI_GetAddressUint32(uint8_t *adr);

/* BACAPI_GetAddressStr();
  Vraca adresni string u sprintf(tmp,"%X %X %X %X: %X%X") formatu
*/
extern string BACAPI_GetAddressString(BACNET_ADDRESS *adr);

/* BACAPI_GetAddressStr();
  Vraca adresni string u sprintf(tmp,"%X ") formatu ovisno o adr_len
  TODO: adr_len  se nalazi u BACNET_ADDRESS. maknuti ovaj parametar
*/
extern string BACAPI_GetAddressString(BACNET_ADDRESS *adr, int adr_len);


/* BACAPI_GetAddressStringIP();
  Vraca adresni string iz bacnet adrese u IP formatu. npr "10.0.1.22"
*/
extern string BACAPI_GetAddressStringIP(BACNET_ADDRESS *adr);

/* BACAPI_GetAddressStringIP();
  Vraca adresni string iz uint8_t u IP formatu npr "10.0.1.22"
*/
extern string BACAPI_GetAddressStringIP(uint8_t *adr);


/* BACAPI_PollEvent();
   Poll funkcija koju korisnik koristi da provjeri da li postoji BACAPI_EVENT na event stacku.
   Svaki event koji se dogodi unutar API-a ulazit ce na event stack.
   User pollira event stack za evente. Npr. User pozove WhoIs servis iz I_cBACClient,
   i svi I-Am detektirani odgovori pojavit ce se u ovom stacku.

   BACAPI_PollEvent() se mora koristiti iz glavne programske petlje ili korisnickog event handlera.
*/
extern int BACAPI_PollEvent(BACAPI_Event *event);

/* BACAPI_PushEvent();

   Koristi se iz service handlera da bi se gurali BACnet eventi na event stack. Korisnik
   Slusa evente koristeci BACAPI_PollEvent().
   Funkcija je interna za API i korisnik je nesmije koristiti
*/
extern size_t BACAPI_PushEvent(BACAPI_Event *event);

// error
// Legacy funkcije, vise ne ne koriste.
extern void BACAPI_ResetLastError();
extern void BACAPI_SetLastError(BACAPI_ERROR err);
extern void BACAPI_SetBACnetError(BACNET_ERROR_CLASS errClass, BACNET_ERROR_CODE errCode);
extern BACNET_ERROR *BACAPI_GetBACnetError(void);
extern BACAPI_ERROR BACAPI_GetLastError(void);


// Nedokumentirane funkcije iz stare verzije API-a. mozda rade, mozda ne.
extern int BACAPI_SetObjectPropertyValueString(BACNET_OBJECT *object, BACNET_PROPERTY_ID prop_id, string value);
extern int BACAPI_SetObjectPropertyValueReal(BACNET_OBJECT *object, BACNET_PROPERTY_ID prop_id, float value);
extern int BACAPI_SetObjectPropertyValueDouble(BACNET_OBJECT *object, BACNET_PROPERTY_ID prop_id, double value);
extern int BACAPI_SetObjectPropertyValueEnum(BACNET_OBJECT *object, BACNET_PROPERTY_ID prop_id, uint32_t value);
extern int BACAPI_SetObjectPropertyValueBool(BACNET_OBJECT *object, BACNET_PROPERTY_ID prop_id, bool value);
extern int BACAPI_SetObjectPropertyValueUint(BACNET_OBJECT *object, BACNET_PROPERTY_ID prop_id, uint32_t value);

// pomocne funkcije za postavljanje vrijednosti u property objekte
//extern BACNET_OBJECT_PROPERTY *BACAPI_SetPropertyValue(BACNET_PROPERTY_ID propid, float val);
//extern BACNET_OBJECT_PROPERTY *BACAPI_SetPropertyValue(BACNET_PROPERTY_ID propid, int val);
//extern BACNET_OBJECT_PROPERTY *BACAPI_SetPropertyValue(BACNET_PROPERTY_ID propid, char *val);
//extern BACNET_OBJECT_PROPERTY *BACAPI_SetPropertyValue(BACNET_APPLICATION_DATA_VALUE *val);
extern bool BACAPI_SetApplicationDataValue(string str_value, BACNET_APPLICATION_DATA_VALUE *val);
extern BACNET_OBJECT_PROPERTY *BACAPI_CreateObjectProperty(BACNET_OBJECT_ID oid, BACNET_PROPERTY_ID propid); //UVJEK KORISTITI OVO ZA STVARANJE PROPERTIJA
extern BACNET_OBJECT_PROPERTY *BACAPI_CreateObjectProperty(void); // ako ce se manualno ispuniti: UVJEK KORISTITI OVO ZA STVARANJE PROPERTIJA
extern BACNET_APPLICATION_DATA_VALUE *BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG apptag);


// Legacy, ne koristiti
extern void BACAPI_DefaultComplexValue(BACNET_OBJECT_PROPERTY *prop);
extern void BACAPI_DefaultNormalValue(BACNET_OBJECT_ID oid, BACNET_OBJECT_PROPERTY *prop);

/* BACAPI_GetApplicationTag()
   Svi ne-komplexsni BACnet property imaju tip podatka koji koriste. Tipovi su podjeljeni u 12 kategorija
   opisanih u bacdef.h ova funkcija vraca tip podatka za property u odnosu na OID jer zbog nekog razloga
   BINARY objekti imaju PRESENT_VALUE specificiran kao enum, COMMAND kao UINT a svi drugi kao REAL osim
   PROP_GROUP ili tamo neki koji ima PRESENT_VALUE specificiran kao complex value.
*/
extern BACNET_APPLICATION_TAG BACAPI_GetApplicationTag(BACNET_OBJECT_TYPE type, BACNET_PROPERTY_ID propid);


/* BACAPI_CopyBitString()
   Kopira bit_string iz *from u *to
   TODO: sigurno postoji nacin da se ovo bolje napravi.
*/
extern void BACAPI_CopyBitString(BACNET_BIT_STRING *to, BACNET_BIT_STRING *from);


// Nedokumentirana, nepotrebna, ne koristiti
extern void BACAPI_SetCharacterString(BACNET_CHARACTER_STRING *str, char *txt, int len);




/* BACAPI_GetApplicationTag()
   Vraca true ako je property complex property
*/
extern bool BACAPI_IsComplexProperty(BACNET_PROPERTY_ID propid);


/* BACAPI_DeleteComplexProperty()
   Brise alocirani complex property.
   TODO: Provjeriti da li se sve brise
*/
extern void BACAPI_DeleteComplexProperty(BACNET_OBJECT_PROPERTY *prop);

/* BACAPI_IsSpecialProperty
   Vraca true ako je property poseban. npr. PROP_GROUP ima komplexni PRESENT_VALUE property itd.
*/
extern bool BACAPI_IsSpecialProperty(BACNET_OBJECT_TYPE type, BACNET_PROPERTY_ID prop_id);

/* BACAPI_IsPropriateryProperty
   Vraca true ako je property propriatery
*/
extern bool BACAPI_IsPropriateryProperty(BACNET_PROPERTY_ID propid);

/* BACAPI_GetObject
   Vraca objekt ako postoji u device-u, ako ne onda NULL
*/
extern BACNET_OBJECT *BACAPI_GetObject(BACNET_DEVICE *device, BACNET_OBJECT_ID oid);
extern BACNET_OBJECT *BACAPI_GetObject(BACNET_DEVICE *device, uint16_t type, uint32_t instance);

/* BACAPI_GetPropertyList
   Vraca pointer na listu propertija kako su opisani u bacobject.h
*/
extern BACNET_PROPERTY_ID *BACAPI_GetPropertyList(BACNET_OBJECT_TYPE object);

/* BACAPI_GetPropertyCount
   Vraca broj propertija kako su definirani u bacobject.h
*/
extern int BACAPI_GetPropertyCount(BACNET_OBJECT_TYPE object);



/* BACAPI_CreateNPDUData
   Pomocne funkcije za stvaranje NPDU strukture u odnosu na parametre
*/
extern BACNET_NPDU_DATA *BACAPI_CreateNPDUData(BACNET_ADDRESS *src, BACNET_ADDRESS *dest);
extern BACNET_NPDU_DATA *BACAPI_CreateNPDUData(BACNET_NPDU_DATA *src);
extern BACNET_NPDU_DATA *BACAPI_CreateNPDUData(BACNET_ADDRESS *src, BACNET_ADDRESS *dest, BACNET_ADDRESS *remote, BACNET_DATALINK dlink);

/* BACAPI_CreateBACnetDevice
   Pomocne funkcije za stvaranje BACNET_DEVICE strukture u odnosu na parametre
*/
extern BACNET_DEVICE *BACAPI_CreateBACnetDevice(void);
extern BACNET_DEVICE *BACAPI_CreateBACnetDevice(BACNET_DEVICE *dev);
extern BACNET_DEVICE *BACAPI_CreateBACnetDevice(uint32_t instance,uint16_t maxApdu, BACNET_SEGMENTATION seg, uint16_t vendor);  // stvori BACNET_DEVICE i nuliraj sve vrijednosti


/* BACAPI_BACnetObjectId();
   Vraca BACNET_OBJECT_ID strukturu za type i instance
*/
extern BACNET_OBJECT_ID BACAPI_BACnetObjectId(uint16_t type, uint32_t instance);

/* BACAPI_CreateBACnetObject();
   Stvara i vraca nulirani BACNET_OBJECT
*/
extern BACNET_OBJECT *BACAPI_CreateBACnetObject(uint16_t type, uint32_t instance);

/* Funkcije koje brisu stvorene objekte, devicove, propertije itd.
*/
extern void BACAPI_DeleteDevice(BACNET_DEVICE *device);
extern void BACAPI_DeleteObjectList(BACNET_DEVICE *device);
extern void BACAPI_DeleteObject(BACNET_OBJECT *object);
extern void BACAPI_DeleteProperties(BACNET_OBJECT *object);
extern void BACAPI_DeleteObjectProperty(BACNET_OBJECT_PROPERTY *prop);
extern void BACAPI_DeletePropertyValue(BACNET_APPLICATION_DATA_VALUE *value);
extern void BACAPI_DeletePropertyValues(BACNET_OBJECT_PROPERTY *prop);
extern void BACAPI_DeleteObjectPropertyValues(BACNET_OBJECT_PROPERTY *prop);
extern void BACAPI_DeleteApplicationDataValue(BACNET_APPLICATION_DATA_VALUE *value);
extern void BACAPI_DeleteBACnetAlarm(BACNET_ALARM *alarm);
extern void BACAPI_DeleteCOVValue(BACNET_COV_VALUE *val);
extern void BACAPI_DeleteNPDUData(BACNET_NPDU_DATA *npdu);
extern void BACAPI_DeleteProperty(BACNET_OBJECT *object, BACNET_PROPERTY_ID propertyId);
extern void BACAPI_DeleteObjectPropRef(BACNET_OBJECT_PROPERTY_REFERENCE *ref);  //zove se iz  BACAPI_DeleteComplexProperty
extern void BACAPI_DeleteDeviceObjectPropRef(BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE *ref);  //zove se iz  BACAPI_DeleteComplexProperty
extern void BACAPI_DeleteDeviceObjectRef(BACNET_OBJECT_PROPERTY *prop);  //zove se iz  BACAPI_DeleteComplexProperty
extern void BACAPI_DeleteObjectPropertyRef(BACNET_OBJECT_PROPERTY *prop);
extern void BACAPI_DeleteEnumList(BACNET_APPLICATION_DATA_VALUE *val);
extern void BACAPI_DeleteTimeStamp(BACNET_TIMESTAMP *stamp);
extern void BACAPI_DeleteRecipientList(BACNET_OBJECT_PROPERTY *property);
extern void BACAPI_DeleteDestination(BACNET_DESTINATION *dest);
extern void BACAPI_DeleteWeeklySchedule(BACNET_WEEKLY_SCHEDULE *sch);
extern void BACAPI_DeleteCalendarEntry(BACNET_CALENDAR_ENTRY *entry);
extern void BACAPI_DeleteReadAccessSpec(BACNET_READ_ACCESS_SPECIFICATION *spec);
extern void BACAPI_DeleteBACnetActionList(BACNET_OBJECT_PROPERTY *property);
extern void BACAPI_DeleteBACnetActionCommand(BACNET_ACTION_COMMAND *command);
extern void BACAPI_DeleteActionText(BACNET_ACTION_TEXT *text);
extern void BACAPI_DeleteActionLists(BACNET_OBJECT_PROPERTY *prop);
extern void BACAPI_DeleteActionProperty(BACNET_ACTION_PROPERTY *prop);
extern void BACAPI_DeleteSpecialEvent(BACNET_OBJECT_PROPERTY *prop);
extern void BACAPI_DeleteDeviceAddressBinding(BACNET_ADDRESS_BINDING *adr_bind);
extern void BACAPI_DeleteActionCommandList(BACNET_ACTION_LIST *list);
extern void BACAPI_DeleteCOVNotification(BACNET_COV_NOTIFICATION *cov);


/* Funkcije koje kopiraju device, obj, prop
*/
extern void BACAPI_CopyBACnetDevice(BACNET_DEVICE *to, BACNET_DEVICE *from);
extern void BACAPI_CopyBACnetObject(BACNET_OBJECT *to, BACNET_OBJECT *from);
extern void BACAPI_CopyBACnetProperty(BACNET_OBJECT_PROPERTY *to, BACNET_OBJECT_PROPERTY *from);
extern void BACAPI_CopyPropertyNormalValue(BACNET_OBJECT_PROPERTY *to, BACNET_OBJECT_PROPERTY *from);
extern void BACAPI_CopyPropertyValue(BACNET_OBJECT_PROPERTY *to, BACNET_OBJECT_PROPERTY *from);
extern void BACAPI_CopyPropertyComplexValue(BACNET_OBJECT_PROPERTY *to, BACNET_OBJECT_PROPERTY *from);


// char /wide char pomocne funkcije
char *BACAPI_utf16_to_utf8(const wchar_t *str, int len);
wchar_t *BACAPI_utf8_to_utf16(const char *str);
void BACAPI_utf8_free(char *str);
extern void BACAPI_ZeroString(char *str, int size);






extern void BACAPI_CreateDateRange(BACNET_OBJECT_PROPERTY *prop);


// TODO: complex
//extern void BACAPI_CreateDeviceObjectPropertyReference(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateDateTime(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateDeviceObjectReference(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateSetPointObjectPropertyReference(BACNET_OBJECT_PROPERTY *prop);   // PROVJERITI KOJI JE OVO TIP
//extern void BACAPI_CreateObjectPropertyReference(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateRecipient(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateBACDectination(BACNET_OBJECT_PROPERTY *prop);

//extern void BACAPI_CreateTimeStamps(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateCOVSubscriptions(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateCalendarEntry(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreatePriorityList(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateWeeklySchedule(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateSpecialEvent(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateReadAccessSpecification(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateDeviceAddressBind(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateBACnetActionList(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateActionText(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateEnumList(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateEventParameters(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateLogBuffer(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateScale(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreatePreScale(BACNET_OBJECT_PROPERTY *prop);
//extern void BACAPI_CreateShedLevel(BACNET_OBJECT_PROPERTY *prop);






/*
  Ovaj set funkcija vraca string imena za parametar.
  npr. BACAPI_BACnetObjectName(0); ce vratiti "Analog Input"
*/
extern const char *BACAPI_BACnetObjectName(uint16_t type);
extern const char *BACAPI_BACnetPropertyName(BACNET_PROPERTY_ID id);
extern const char *BACAPI_BACnetPropertyNormalName(BACNET_PROPERTY_ID id);
extern const char *BACAPI_BACnetErrorClassName(BACNET_ERROR_CLASS errclass);
extern const char *BACAPI_BACnetErrorCodeName(BACNET_ERROR_CODE errcode);
extern const char *BACAPI_BACnetConfirmedServiceName(BACNET_CONFIRMED_SERVICE service);
extern const char *BACAPI_BACnetUncomfirmedServiceName(BACNET_UNCONFIRMED_SERVICE service);
extern const char *BACAPI_BACnetServiceName(BACNET_PDU_TYPE type, uint8_t service);
extern const char *BACAPI_BACnetSupportedServicesName(uint8_t service);
extern const char *BACAPI_BACnetSupportedObjectsName(BACNET_PROPERTY_ID object);
extern const char *BACAPI_BACnetStatusFlagsName(uint8_t flag);
extern const char *BACAPI_BACnetEventStateName(BACNET_EVENT_STATE event);
extern const char *BACAPI_BACnetNotifyTypeName(BACNET_NOTIFY_TYPE type);
extern const char *BACAPI_BACnetWeekdayName(uint8_t day);
extern const char *BACAPI_BACnetTransBitsName(BACNET_EVENT_TRANSITION_BITS bit);
extern const char *BACAPI_BACnetEngUnitsName(BACNET_ENGINEERING_UNITS unit);
extern const char *BACAPI_BACnetRejectCodeName(BACNET_REJECT_CODE code);
extern const char *BACAPI_BACnetAbortReasonName(BACNET_ABORT_REASON abort);
extern const char *BACAPI_BACnetSegmentationName(BACNET_SEGMENTATION seg);
extern const char *BACAPI_BACnetPriorityName(BACNET_MESSAGE_PRIORITY prio);
extern const char *BACAPI_BACnetVendorName(int vendorid);
extern const char *BACAPI_BACnetAccumulatorStatusName(BACNET_ACCUMULATOR_STATUS status);
extern const char *BACAPI_BACnetFaultTypeName(BACNET_FAULT_TYPE type);
extern const char *BACAPI_BACnetLifeSafetyStateName(BACNET_LIFE_SAFETY_STATE state);
extern const char *BACAPI_BACnetLifeSafetyModeName(BACNET_LIFE_SAFETY_MODE mode);
extern const char *BACAPI_BACnetApplicationTagName(BACNET_APPLICATION_TAG tag);
extern const char *BACAPI_BACnetReinitStateName(BACNET_REINIT_STATE state);
extern const char *BACAPI_BACnetDCCStateName(BACNET_DCC_STATE state);
extern const char *BACAPI_BACnetSeverity(BACAPI_SEVERITY state);
extern const char *BACAPI_BACnetDataLinkName(BACNET_DATALINK link);
extern const char *BACAPI_BACnetEventTypeName(BACNET_EVENT_TYPE type);
extern const char *BACAPI_BACnetPDUTypeName(BACNET_PDU_TYPE type);
extern const char *BACAPI_BACnetFileAccessTypeName(int at);
extern const char *BACAPI_BACnetEOFName(bool eof);
extern const char *BACAPI_MACName(BACNET_ADDRESS *adr);
extern const char *BACAPI_BACnetMSTPFrameTypeName(MSTP_FRAME_TYPE ftype);
extern const char *BACAPI_BACnetMSTPMasterStateName(MSTP_MASTER_STATE state);
extern const char *BACAPI_BACnetRouterRejectMessageName(BACNET_ROUTER_REJECT_MESSAGE msg);
extern const char *BACAPI_BACnetReliabilityName(BACNET_RELIABILITY rel);
extern const char *BACAPI_BACnetBinaryLightingPVName(BACNET_BINARY_LIGHTING_PV pv);
extern const char *BACAPI_BACnetLiftGroupModeName(BACNET_LIFT_GROUP_MODE mode);
extern const char *BACAPI_BACnetLiftDirectionName(BACNET_LIFT_CAR_DIRECTION dir);
extern const char *BACAPI_BACnetNNQualityName(BACNET_NN_QUALITY q);
extern const char *BACAPI_BACnetAccessAuthFactorDisableName(BACNET_ACCESS_AUTH_FACTOR_DISABLE fact);
extern const char *BACAPI_BACnetAuthenticationFactorTypeName(BACNET_AUTHENTICATION_FACTOR_TYPE type);
extern const char *BACAPI_BACnetAuthorizationExemptionsName(BACNET_AUTHORIZATION_EXEMPTIONS e);
extern const char *BACAPI_BACnetAccessCredentialDisableReasonName(BACNET_ACCESS_CREDENTIAL_DISABLE_REASON n);






/* BACAPI_GetObjectString()
   Vraca object/instance string npr "Analog Input, 1"
*/
extern string BACAPI_GetObjectString(BACNET_OBJECT_ID oid);

// Interna funkcija.
extern BACAPI_Event *BACAPI_CreateEvent(void);


extern void BACAPI_DeleteEventMessage(BACAPI_MESSAGE *msg);


/* BACAPI_ReleaseEvent()
  Nakon sto se dobije poruka s event stacka mora se raspustiti s ovom funkcijom
*/
extern void BACAPI_ReleaseEvent(BACAPI_Event *event);


/* BACAPI_GetBitState()
  Vraca 0/1 za specificirani bit i bstr
*/
extern bool BACAPI_GetBitState(BACNET_BIT_STRING *bstr, int bit);

/* BACAPI_GetObjectProperty()
  Vraca BACNET_OBJECT_PROPERTY property za uneseni parametar. Ako property ne postoji u objektu
  vraca NULL
*/
extern BACNET_OBJECT_PROPERTY *BACAPI_GetObjectProperty(BACNET_PROPERTY_ID propId, BACNET_OBJECT *obj);
extern BACNET_OBJECT_PROPERTY *BACAPI_GetObjectProperty(int num, BACNET_OBJECT *obj);

/* BACAPI_GetStringValue()
  Vraca string value za specificirani parametar.
*/
extern string BACAPI_GetStringValue( BACNET_APPLICATION_DATA_VALUE *value);

// koristi se samo iz LUE i vraca samo prvi vrijedsnost (vValueList[0])
extern string BACAPI_GetStringValue( BACNET_OBJECT_PROPERTY *prop);

/* BACAPI_GetShortStringValue()
 Vraca string value za specificirani parametar.
*/
extern string BACAPI_GetShortStringValue( BACNET_SHORT_APPLICATION_DATA_VALUE *value);

extern const char *BACAPI_GetErrorString(BACAPI_ERROR err);


extern BACNET_TIMESTAMP *BACAPI_CreateNewTimeStamp(BACNET_OBJECT_PROPERTY *prop);


extern BACNET_APPLICATION_DATA_VALUE *BACAPI_GetArrayElement(BACNET_APPLICATION_DATA_VALUE *value, int element);

extern void BACAPI_SetSupportedService(BACNET_BIT_STRING *bstr, BACNET_SUPPORTED_SERVICE service);
extern void BACAPI_SetSupportedObject(BACNET_BIT_STRING *bstr, BACNET_OBJECT_TYPE object_type);

// INVOKE
extern vector<BACAPI_INVOKE_ID*>vInvokeList;
extern BACAPI_INVOKE_ID *BACAPI_ReserveInvokeId(BACNET_DEVICE *dev,BACNET_OBJECT *obj, BACNET_OBJECT_PROPERTY *prop, BACAPI_INVOKE_TYPE type = INVOKE_TYPE_NORMAL);
extern void BACAPI_ReleaseInvokeId(BACAPI_INVOKE_ID *invoke);
extern BACAPI_INVOKE_ID *BACAPI_GetInvoke(uint8_t invokeid);
extern void BACAPI_ReleaseInvokeIdAll(void);


//
extern void BACAPI_IsArrayProperty(BACNET_OBJECT_TYPE object_type, BACNET_PROPERTY_ID property);
extern bool BACAPI_IsCommandableObject(BACNET_OBJECT_TYPE object_type);


// date/time - upotrebljavamo drugacije strukture za win32 i linux
// trebalo bi ujediniti ovo
//extern void BACAPI_SysDateToBACnetDate(BACNET_DATE *bac_date, SYSTEMTIME *sys_date);
//extern void BACAPI_BACnetDateToSysDate(SYSTEMTIME *sys_date, BACNET_DATE *bac_date);
//extern void BACAPI_SysTimeToBACnetTime(BACNET_TIME *bac_time, SYSTEMTIME *sys_time);
//extern void BACAPI_BACnetTimeToSysTime(SYSTEMTIME *sys_time, BACNET_TIME *bac_time);

extern void BACAPI_SysDateToBACnetDate(BACNET_DATE *bac_date, struct tm *tt_date);
extern void BACAPI_BACnetDateToSysDate(struct tm  *tt_date, BACNET_DATE *bac_date);
extern void BACAPI_SysTimeToBACnetTime(BACNET_TIME *bac_time, struct tm  *tt_time);
extern void BACAPI_BACnetTimeToSysTime(struct tm  *tt_time, BACNET_TIME *bac_time);
extern void BACAPI_BACnetDateTimeToSysDateTime(struct tm  *tt_datetime, BACNET_DATE_TIME *bac_datetime);
//extern void BACAPI_BACnetDateTimeToSysDateTime(SYSTEMTIME* sys_datetime, BACNET_DATE_TIME *bac_datetime);

BACNET_OCTET_STRING *BACAPI_CreateOctetString(uint8_t *source, uint32_t len);
extern int BACAPI_CopyBACnetOctetString(BACNET_OCTET_STRING *to, BACNET_OCTET_STRING *from);
extern void BACAPI_InitBACnetOctetString(BACNET_OCTET_STRING *ostr, uint8_t *source, uint32_t len);
extern uint8_t *BACAPI_GetOctetStringValue(BACNET_OCTET_STRING *ostr);

// pointer na count log rekorda koji se traze iz log_buffer propertija
extern BACNET_LOG_RECORD **BACAPI_GetLogRecordList(BACNET_OBJECT_PROPERTY *prop, BACNET_READ_RANGE *range);
extern BACNET_LOG_MULTIPLE_RECORD **BACAPI_GetLogMultipleRecordList(BACNET_OBJECT_PROPERTY *prop, BACNET_READ_RANGE *range);

extern int BACAPI_GetPropertyId(string prop_name);


extern BACAPI_ENUM_STRUCT *BACAPI_GetEnumerationList(BACNET_OBJECT_TYPE type, BACNET_PROPERTY_ID propid);


extern BACNET_APPLICATION_DATA_VALUE *BACAPI_GetPropertyValue(BACNET_OBJECT_PROPERTY *prop);

uint8_t BACAPI_CRC_Header(uint8_t dataValue,uint8_t crcValue);
uint16_t BACAPI_CRC_Data(uint8_t dataValue,uint16_t crcValue);

void BACAPI_print_buffer(uint8_t *buffer, int len);



extern int BACAPI_CopyBACnetFile(BACNET_FILE *to, BACNET_FILE *from);
extern int BACAPI_AppendBACnetFile(BACNET_FILE *to, BACNET_FILE *from);

extern void BACAPI_ClearBACnetFile(BACNET_FILE *bac_file);
extern int BACAPI_LoadBACnetFile(BACNET_FILE *bac_file, string filename);


extern BACAPI_PACKET *BACAPI_CreatePacket(uint8_t *apdu, BACNET_NPDU_DATA *npdu,  int apdulen, uint8_t bvlc_len);
extern void BACAPI_ReleasePacket(BACAPI_PACKET *packet);


extern bool BACAPI_CompareAddressData(BACNET_ADDRESS *adr1, BACNET_ADDRESS *adr2);

extern BACNET_APPLICATION_DATA_VALUE *BACAPI_GetApplicationDataValue(BACNET_OBJECT_PROPERTY *prop, uint8_t val_num);



extern uint8_t BACAPI_ascii2hex( char inASCII );
extern uint32_t BACAPI_ascii2hex_block( uint8_t * outMessage, int32_t out_len, const char * inMessage );



#ifdef BACAPI_BUILD_WIN32
extern HWND mainHWND_;
extern HANDLE receiveFrameFlag;   // receive frame state machine
extern HANDLE receiveFIFOBufferFlag;
#else
//extern neosmart_event_t receiveFrameFlag;
//extern neosmart_event_t receivePacketFlag;


#endif

#if defined BACAPI_BUILD_ARM9 || defined BACAPI_BUILD_LINUX

extern void BACAPI_GetAbsoluteTime(struct timespec *abstime, unsigned long milliseconds);
#endif // BACAPI_BUILD_LINUX


#endif




