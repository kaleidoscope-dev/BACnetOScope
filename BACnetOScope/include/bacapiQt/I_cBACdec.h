#pragma once
#include "bacnet.h"
#include <string>


// kodiranje / dekoriranje niskog nivoa


using namespace std;



class I_cBACdec
{
 public:
        I_cBACdec();
       ~I_cBACdec();


 int encode_unsigned16(uint8_t * apdu, uint16_t value);
 int encode_unsigned32( uint8_t * apdu,uint32_t value);
 int encode_unsigned24(uint8_t * apdu,uint32_t value);
 int encode_signed8(uint8_t * apdu,int8_t value);
 int encode_signed16( uint8_t * apdu, int16_t value);
 int encode_signed24( uint8_t * apdu,int32_t value);
 int encode_signed32( uint8_t * apdu,int32_t value);


 int encode_context_unsigned(uint8_t * apdu, uint8_t tag_number, uint32_t value);
 int encode_context_character_string(uint8_t * apdu,uint8_t tag_number, BACNET_CHARACTER_STRING *str);
 int encode_context_object_id(  uint8_t * apdu,uint8_t tag_number,int object_type,uint32_t instance);
 int encode_context_enumerated(uint8_t * apdu,uint8_t tag_number,uint32_t value);
 int encode_context_null(uint8_t * apdu,uint8_t tag_number);
 int encode_context_boolean( uint8_t * apdu, uint8_t tag_number,bool boolean_value);
 int encode_context_signed( uint8_t * apdu,uint8_t tag_number,int32_t value);
 int encode_context_real(uint8_t * apdu, uint8_t tag_number, float value);
 int encode_context_double(uint8_t * apdu,uint8_t tag_number, double value);
 int encode_context_octet_string( uint8_t * apdu,uint8_t tag_number,BACNET_OCTET_STRING * octet_string);
 int encode_context_bitstring( uint8_t * apdu,uint8_t tag_number, BACNET_BIT_STRING * bit_string);
 int encode_context_date(uint8_t * apdu,uint8_t tag_number, BACNET_DATE * bdate);
 int encode_context_time(uint8_t * apdu, uint8_t tag_number, BACNET_TIME * btime);
 int encode_context_timestamp(uint8_t * apdu, uint8_t tag_number, BACNET_TIMESTAMP *timestamp);



 int encode_application_boolean( uint8_t * apdu, bool boolean_value);
 int encode_application_unsigned(    uint8_t * apdu,uint32_t value);
 int encode_application_signed( uint8_t * apdu, int32_t value);
 int encode_application_real(uint8_t * apdu,float value);
 int encode_application_double(uint8_t * apdu,double value);
 int encode_application_octet_string( uint8_t * apdu,BACNET_OCTET_STRING * octet_string);
 int encode_application_octet_string_range( uint8_t * apdu, uint16_t start_pos, uint32_t octet_count, BACNET_OCTET_STRING *octet_string);
 int encode_application_bitstring( uint8_t * apdu, BACNET_BIT_STRING * bit_string);
 int encode_application_enumerated( uint8_t * apdu,uint32_t value);
 int encode_application_object_id( uint8_t * apdu,int object_type,uint32_t instance);
 int encode_application_date( uint8_t * apdu, BACNET_DATE * bdate);
 int encode_application_time(uint8_t * apdu,BACNET_TIME * btime);

 int encode_bacnet_signed(uint8_t * apdu,int32_t value);
 int encode_bacnet_enumerated( uint8_t * apdu,uint32_t value);
 int encode_tag(uint8_t * apdu, uint8_t tag_number,bool context_specific,uint32_t len_value_type);
 int encode_bacnet_unsigned(uint8_t * apdu,uint32_t value);
 int encode_bacnet_object_id(uint8_t * apdu,int object_type,uint32_t instance);
 //int encode_bacnet_character_string(uint8_t * apdu, string char_string, BACNET_TEXT_ENCODING encoding);

 int encode_octet_string( uint8_t * apdu,BACNET_OCTET_STRING * octet_string);
 int encode_opening_tag(uint8_t * apdu,uint8_t tag_number);
 int encode_closing_tag( uint8_t * apdu,uint8_t tag_number);




 int  decode_real(uint8_t * apdu, float *real_value);
 int  decode_double(uint8_t * apdu,double *double_value);
 int  decode_signed32(uint8_t * apdu,int32_t * value);
 int  decode_signed24(uint8_t * apdu,int32_t * value);
 int  decode_signed16(uint8_t * apdu,int32_t * value);
 int  decode_signed8(uint8_t * apdu,int32_t * value);
 int  decode_unsigned16(uint8_t * apdu,  uint16_t * value);
 int  decode_unsigned8(uint8_t * apdu,  uint8_t * value);
 int  decode_unsigned32(uint8_t * apdu,uint32_t * value);
 int  decode_unsigned( uint8_t * apdu, uint32_t len_value,uint32_t * value);
 int  decode_unsigned24(  uint8_t * apdu,uint32_t * value);


 bool decode_context_boolean(uint8_t * apdu);
 int  decode_context_bitstring( uint8_t * apdu, uint8_t tag_number, BACNET_BIT_STRING * bit_string);
 int  decode_context_real( uint8_t * apdu,uint8_t tag_number,float *real_value);
 int  decode_context_enumerated(uint8_t * apdu, uint8_t tag_value,uint32_t * value);
 int  decode_context_unsigned(uint8_t * apdu,uint8_t tag_number,uint32_t * value);
 int decode_context_signed( uint8_t * apdu,uint8_t tag_number, int32_t *value);
 int  decode_context_object_id( uint8_t * apdu,uint8_t tag_number,uint16_t * object_type,uint32_t * instance);
 int  decode_context_bacnet_time(uint8_t * apdu,uint8_t tag_number,BACNET_TIME * btime);
 int decode_context_character_string(uint8_t * apdu,uint8_t tag_number, BACNET_CHARACTER_STRING *txt);




 bool decode_is_context_tag_with_length(uint8_t * apdu,uint8_t tag_number,int *tag_length);


 int  bacapp_decode_context_property_state( uint8_t * apdu, uint8_t tag_number,BACNET_PROPERTY_STATE * value);
 bool decode_is_context_tag(uint8_t * apdu,uint8_t tag_number);

 bool decode_is_opening_tag_number( uint8_t * apdu, uint8_t tag_number);
 bool decode_boolean(uint32_t len_value);
 int  decode_signed(uint8_t * apdu, uint32_t len_value,int32_t * value);
 int  decode_real_safe( uint8_t * apdu, uint32_t len_value, float *real_value);
 int  decode_double_safe(uint8_t * apdu,uint32_t len_value,double *double_value);
 int  decode_octet_string( uint8_t * apdu,uint32_t len_value, BACNET_OCTET_STRING * octet_string);
 int  decode_bitstring(uint8_t * apdu,uint32_t len_value,BACNET_BIT_STRING * bit_string);
 int  decode_object_id_safe( uint8_t * apdu, uint32_t len_value,uint16_t * object_type,uint32_t * instance);

 bool decode_is_opening_tag(uint8_t * apdu);
 bool decode_is_closing_tag(uint8_t * apdu);

 int decode_tag_number(uint8_t * apdu,uint8_t * tag_number);
 int decode_tag_number_and_value(uint8_t * apdu, uint8_t * tag_number,  uint32_t * value);

 int decode_object_id(uint8_t * apdu,uint16_t * object_type, uint32_t * instance);
 int decode_enumerated(uint8_t * apdu, uint32_t len_value,uint32_t * value);
 int decode_character_string(uint8_t * apdu, uint32_t len_value,  string *char_string);

 string decode_standard_character_string(uint8_t * apdu, uint32_t len_value);
 int decode_standard_character_string(uint8_t * apdu, uint32_t len_value,  string &ptr_string);
//int decode_standard_character_string16( uint8_t * apdu, uint32_t len_value,  char *char_string);  // HACK
 int decode_standard_character_string(uint8_t * apdu, uint32_t len_value,  wstring *ptr_wstring);



 uint8_t encode_max_segs_max_apdu(int max_segs,int max_apdu);
 static uint8_t byte_reverse_bits( uint8_t in_byte);
 //const char *bactext_object_type_name( unsigned index);

// const char *indtext_by_index_split_default( INDTEXT_DATA * data_list, unsigned index, unsigned split_index,const char *before_split_default_name, const char *default_name);
// const char *indtext_by_index_default( INDTEXT_DATA * data_list,unsigned index, const char *default_string);

 int bacapp_encode_data(uint8_t * apdu,BACNET_APPLICATION_DATA_VALUE * value);
 int bacapp_encode_context_data_value(uint8_t * apdu,uint8_t context_tag_number,BACNET_APPLICATION_DATA_VALUE * value);
 int bacapp_encode_application_data(uint8_t * apdu, BACNET_APPLICATION_DATA_VALUE * value);

 int bacapp_decode_application_data(uint8_t * apdu, unsigned max_apdu_len, BACNET_APPLICATION_DATA_VALUE * value);
 int bacapp_decode_data(uint8_t * apdu,uint8_t tag_data_type,uint32_t len_value_type,BACNET_APPLICATION_DATA_VALUE * value);
 int bacapp_decode_property_state( uint8_t * apdu,BACNET_PROPERTY_STATE * value);


 uint8_t bitstring_bytes_used(  BACNET_BIT_STRING * bit_string);
 bool bitstring_set_bits_used(BACNET_BIT_STRING * bit_string,uint8_t bytes_used,uint8_t unused_bits);
 uint8_t bitstring_bits_used( BACNET_BIT_STRING * bit_string);
 uint8_t bitstring_octet(BACNET_BIT_STRING * bit_string,uint8_t octet_index);
 bool bitstring_bit( BACNET_BIT_STRING * bit_string,uint8_t bit_number);
 void bitstring_init( BACNET_BIT_STRING * bit_string);
 bool bitstring_set_octet( BACNET_BIT_STRING * bit_string,uint8_t index,uint8_t octet);


int encode_bacnet_real(float value, uint8_t * apdu);
int encode_bacnet_double( double value, uint8_t * apdu);
int encode_bitstring(uint8_t * apdu, BACNET_BIT_STRING * bit_string);
bool decode_is_closing_tag_number(uint8_t * apdu, uint8_t tag_number);
int bacapp_decode_context_data( uint8_t * apdu,unsigned max_apdu_len,BACNET_APPLICATION_DATA_VALUE * value, BACNET_PROPERTY_ID property);
BACNET_APPLICATION_TAG bacapp_context_tag_type(BACNET_PROPERTY_ID property,uint8_t tag_number);
//bool bacapp_print_value(FILE * stream, BACNET_APPLICATION_DATA_VALUE * value, BACNET_PROPERTY_ID property);
int bacapp_data_len(uint8_t * apdu, unsigned max_apdu_len,BACNET_PROPERTY_ID property);
int decode_max_segs(uint8_t octet);
int decode_max_apdu( uint8_t octet);
int rpm_decode_object_id(uint8_t * apdu,unsigned apdu_len,BACNET_OBJECT_TYPE * object_type,uint32_t * object_instance);


int bacerror_decode_error_class_and_code( uint8_t * apdu,unsigned apdu_len,BACNET_ERROR_CLASS * error_class,BACNET_ERROR_CODE * error_code);
int bacerror_create_object_decode_error_class_and_code( uint8_t * apdu,unsigned apdu_len,BACNET_ERROR_CLASS * error_class,BACNET_ERROR_CODE * error_code);


int encode_bacnet_date( uint8_t * apdu,BACNET_DATE * bdate);
int encode_bacnet_time(uint8_t * apdu, BACNET_TIME * btime);

int decode_date(uint8_t * apdu, BACNET_DATE * bdate);
int decode_bacnet_time( uint8_t * apdu,BACNET_TIME * btime);


int decode_bacnet_time_safe(uint8_t * apdu,uint32_t len_value,BACNET_TIME * btime);
int decode_date_safe( uint8_t * apdu,uint32_t len_value,BACNET_DATE * bdate);



 size_t octetstring_length( BACNET_OCTET_STRING * octet_string);
 //uint8_t *octetstring_value(BACNET_OCTET_STRING * octet_string);
 void octetstring_init(BACNET_OCTET_STRING * octet_string,uint8_t * value,size_t length);



int bacapp_decode_context_device_obj_property_ref( uint8_t * apdu, uint8_t tag_number,BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE * value);
int bacapp_decode_device_obj_property_ref(uint8_t * apdu,BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE * value);
int bacapp_encode_device_obj_property_ref(uint8_t * apdu,BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE * value);

int bacapp_encode_device_obj_ref(uint8_t * apdu, BACNET_DEVICE_OBJECT_REFERENCE * value);
int bacapp_decode_device_obj_ref(uint8_t * apdu,BACNET_DEVICE_OBJECT_REFERENCE * value);
int bacapp_encode_obj_property_ref( uint8_t * apdu,BACNET_OBJECT_PROPERTY_REFERENCE * value);
int bacapp_decode_obj_property_ref( uint8_t * apdu,BACNET_OBJECT_PROPERTY_REFERENCE * value);

int decode_application_datetime( uint8_t * apdu,BACNET_DATE_TIME * datetime);
int encode_application_datetime(uint8_t * apdu,BACNET_DATE_TIME * datetime);
int decode_application_date(uint8_t * apdu, BACNET_DATE * bdate);
int decode_application_time(uint8_t * apdu, BACNET_TIME * btime);

int encode_recipient(uint8_t * apdu,BACNET_RECIPIENT * destination);
int decode_recipient(uint8_t * apdu,BACNET_RECIPIENT * destination);
int encode_context_bacnet_address(uint8_t * apdu,uint8_t tag_number,BACNET_ADDRESS * destination);
int decode_context_bacnet_address(uint8_t * apdu,uint8_t tag_number,BACNET_ADDRESS * destination);
int decode_bacnet_address(uint8_t * apdu, BACNET_ADDRESS * destination);
int encode_destination(uint8_t * apdu, BACNET_DESTINATION * destination);
int decode_destination(uint8_t * apdu,int max_apdu_len, BACNET_DESTINATION * destination);

int encode_daterange(uint8_t * apdu,BACNET_DATE_RANGE * bdaterange);
int decode_daterange(uint8_t * apdu,BACNET_DATE_RANGE * bdaterange);

int bacapp_decode_timestamp( uint8_t * apdu, BACNET_TIMESTAMP * value, BACNET_PROPERTY_ID prop_id);
int bacapp_decode_context_datetime(uint8_t * apdu,uint8_t tag_number,BACNET_DATE_TIME * value);
int bacapp_decode_datetime(uint8_t * apdu,BACNET_DATE_TIME * value);
int decode_cov_subscription(uint8_t * apdu,int max_apdu_len,BACNET_COV_SUBSCRIPTION * covs);
int decode_context_recipient_process(uint8_t * apdu,uint8_t tag_number,BACNET_RECIPIENT_PROCESS * destination);
int bacapp_decode_context_obj_property_ref(uint8_t * apdu,uint8_t tag_number,BACNET_OBJECT_PROPERTY_REFERENCE * value);
int decode_context_boolean2(uint8_t * apdu,uint8_t tag_number,bool * boolean_value);
int decode_recipient_process(uint8_t * apdu,BACNET_RECIPIENT_PROCESS * destination);
int decode_context_recipient( uint8_t * apdu,uint8_t tag_number,BACNET_RECIPIENT * destination);

int encode_calendar_entry(uint8_t * apdu, int max_apdu_len, BACNET_CALENDAR_ENTRY * entry);
int decode_calendar_entry(uint8_t * apdu, int max_apdu_len, BACNET_CALENDAR_ENTRY * entry);

int decode_context_date(uint8_t * apdu, uint8_t tag_number, BACNET_DATE * bdate);

int encode_context_daterange( uint8_t * apdu,uint8_t tag_number,BACNET_DATE_RANGE * bdaterange);
int decode_context_daterange( uint8_t * apdu,uint8_t tag_number,BACNET_DATE_RANGE * bdaterange);

int encode_context_datetime(uint8_t * apdu,uint8_t tag_number,BACNET_DATE_TIME);
int decode_context_datetime(uint8_t * apdu,uint8_t tag_number,BACNET_DATE_TIME);


int encode_context_weenkday(uint8_t * apdu,uint8_t tag_number,BACNET_WEEKNDAY * bweeknday);
int decode_context_weeknday(uint8_t * apdu,uint8_t tag_number,BACNET_WEEKNDAY * bweeknday);

int encode_weeknday(uint8_t * apdu,BACNET_WEEKNDAY * bweeknday);
int decode_weeknday(uint8_t * apdu,BACNET_WEEKNDAY * bweeknday);

int decode_priority_value( uint8_t * apdu,unsigned max_apdu_len, BACNET_APPLICATION_DATA_VALUE * value, BACNET_PROPERTY_ID prop);
int bacapp_decode_generic_property( uint8_t * apdu,int max_apdu_len,BACNET_APPLICATION_DATA_VALUE * value,BACNET_PROPERTY_ID prop);

int encode_weekly_schedule(uint8_t * apdu,BACNET_WEEKLY_SCHEDULE * week);
int decode_weekly_schedule(uint8_t * apdu,int max_apdu_len, BACNET_WEEKLY_SCHEDULE * week);

int encode_daily_schedule(uint8_t * apdu,int max_apdu_len,BACNET_DAILY_SCHEDULE * day);
int decode_daily_schedule(uint8_t * apdu,int max_apdu_len,BACNET_DAILY_SCHEDULE * day);

int encode_context_time_values( uint8_t * apdu,int max_apdu_len,uint8_t tag_number,vector<BACNET_TIME_VALUE>*daySchedule);
int decode_context_time_values( uint8_t * apdu,int max_apdu_len,uint8_t tag_number,vector<BACNET_TIME_VALUE>*daySchedule);

int encode_time_and_value( uint8_t * apdu, int max_apdu_len, BACNET_TIME_VALUE * timevalue);
int decode_time_and_value( uint8_t * apdu, int max_apdu_len, BACNET_TIME_VALUE * timevalue);


int decode_special_event(uint8_t * apdu,int max_apdu_len, BACNET_SPECIAL_EVENT * special);
int decode_context_calendar_entry(uint8_t * apdu,int max_apdu_len,uint8_t tag_number,BACNET_CALENDAR_ENTRY * entry);
int decode_read_access_specification(uint8_t * apdu,int max_apdu_len,BACNET_READ_ACCESS_SPECIFICATION * entry);
int bacapp_decode_property_ref(uint8_t * apdu,BACNET_PROPERTY_REF * value);
int decode_device_address_binding(uint8_t *apdu, BACNET_ADDRESS_BINDING *addressBind);
int decode_action_command_list(uint8_t *apdu, BACNET_ACTION_LIST *actionCommandList);
int decode_action_text(uint8_t *apdu, BACNET_ACTION_TEXT *actionText);
int decode_event_parameters(uint8_t *apdu, BACNET_EVENT_PAREMETER *eventParam);
int decode_scale(uint8_t *apdu, BACNET_SCALE *scale);
int decode_pre_scale(uint8_t *apdu, BACNET_PRESCALE *preScale);
int decode_shed_level(uint8_t *apdu, BACNET_SHED_LEVEL *shedLevel);

int encode_exception_schedule(uint8_t *apdu, BACNET_SPECIAL_EVENT *special_event);


int encode_application_character_string( uint8_t * apdu, BACNET_CHARACTER_STRING *char_str);
int encode_bacnet_character_string(uint8_t * apdu, BACNET_CHARACTER_STRING *char_str);


int encode_bacnet_host_port(uint8_t *apdu, BACNET_HOST_NPORT *port);
int decode_bacnet_host_port(uint8_t *apdu, BACNET_HOST_NPORT *port);

int BigToLittleEndian(char *str, int size);

void bitstring_set_bit(BACNET_BIT_STRING *bit_string,uint8_t bit_number,bool value);

int decode_context_octet_string(uint8_t *apdu, uint8_t tag, BACNET_OCTET_STRING *ostr);

int decode_value_source(uint8_t *apdu, BACNET_VALUE_SOURCE *valsrc);
int decode_notif_sub(uint8_t *apdu, BACNET_EVENT_NOTIFICATION_SUBSCRIPTION *sub);


};



extern I_cBACdec *gDec;

