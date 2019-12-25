#include "I_cBACdec.h"
#include "bacapi.h"
#include <string.h>
#include "I_cBACLog.h"

I_cBACdec *gDec = NULL;


static const char *ASHRAE_Reserved_String = "Reserved for Use by ASHRAE";
static const char *Vendor_Proprietary_String = "Vendor Proprietary Value";




I_cBACdec::I_cBACdec()
{
}


I_cBACdec::~I_cBACdec()
{
}



int I_cBACdec::decode_unsigned16(uint8_t * apdu,  uint16_t * value)
{
    if (value) {
        *value = (uint16_t) ((((uint16_t) apdu[0]) << 8) & 0xff00);
        *value |= ((uint16_t) (((uint16_t) apdu[1]) & 0x00ff));
    }

    return 2;
}




int I_cBACdec::decode_unsigned8(uint8_t * apdu,  uint8_t * value)
{
    if (value)
       {
        *value |= ((uint8_t) (((uint8_t) apdu[0]) & 0x00ff));
       }

    return 1;
}






int I_cBACdec::decode_tag_number(uint8_t * apdu,uint8_t * tag_number)
{
    int len = 1;        /* return value */

    /* decode the tag number first */
    if (IS_EXTENDED_TAG_NUMBER(apdu[0])) {
        /* extended tag */
        if (tag_number) {
            *tag_number = apdu[1];
        }
        len++;
    } else {
        if (tag_number) {
            *tag_number = (uint8_t) (apdu[0] >> 4);
        }
    }

    return len;
}





/* from clause 20.2.1.3.2 Constructed Data */
/* returns the number of apdu bytes consumed */
int I_cBACdec::decode_tag_number_and_value(uint8_t * apdu, uint8_t * tag_number,  uint32_t * value)
{
    int len = 1;
    uint16_t value16;
    uint32_t value32;

    len = decode_tag_number(&apdu[0], tag_number);
    if (IS_EXTENDED_VALUE(apdu[0])) {
        /* tagged as uint32_t */
        if (apdu[len] == 255) {
            len++;
            len += decode_unsigned32(&apdu[len], &value32);
            if (value) {
                *value = value32;
            }
        }
        /* tagged as uint16_t */
        else if (apdu[len] == 254) {
            len++;
            len += decode_unsigned16(&apdu[len], &value16);
            if (value) {
                *value = value16;
            }
        }
        /* no tag - must be uint8_t */
        else {
            if (value) {
                *value = apdu[len];
            }
            len++;
        }
    } else if (decode_is_opening_tag(&apdu[0]) && value) {
        *value = 0;
    } else if (decode_is_closing_tag(&apdu[0]) && value) {
        /* closing tag */
        *value = 0;
    } else if (value) {
        /* small value */
        *value = apdu[0] & 0x07;
    }

    return len;
}





int I_cBACdec::decode_unsigned32(uint8_t * apdu,uint32_t * value)
{
    if (value) {
        *value = ((uint32_t) ((((uint32_t) apdu[0]) << 24) & 0xff000000));
        *value |= ((uint32_t) ((((uint32_t) apdu[1]) << 16) & 0x00ff0000));
        *value |= ((uint32_t) ((((uint32_t) apdu[2]) << 8) & 0x0000ff00));
        *value |= ((uint32_t) (((uint32_t) apdu[3]) & 0x000000ff));
    }

    return 4;
}




bool I_cBACdec::decode_is_opening_tag(uint8_t * apdu)
{
    return (bool) ((apdu[0] & 0x07) == 6);
}




/* from clause 20.2.1.3.2 Constructed Data */
/* returns the number of apdu bytes consumed */
bool I_cBACdec::decode_is_closing_tag(uint8_t * apdu)
{
    return (bool) ((apdu[0] & 0x07) == 7);
}





/* from clause 20.2.14 Encoding of an Object Identifier Value */
/* returns the number of apdu bytes consumed */
int I_cBACdec::decode_object_id(uint8_t * apdu,uint16_t * object_type, uint32_t * instance)
{
    uint32_t value = 0;
    int len = 0;

    len = decode_unsigned32(apdu, &value);
    *object_type = (uint16_t) (((value >> BACNET_INSTANCE_BITS) & BACNET_MAX_OBJECT));

    *instance = (value & BACNET_MAX_INSTANCE);

    return len;
}




/* from clause 20.2.4 Encoding of an Unsigned Integer Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::decode_unsigned( uint8_t * apdu, uint32_t len_value,uint32_t * value)
{
    uint16_t unsigned16_value = 0;

    if (value) {
        switch (len_value) {
            case 1:
                *value = apdu[0];
                break;
            case 2:
                decode_unsigned16(&apdu[0], &unsigned16_value);
                *value = unsigned16_value;
                break;
            case 3:
                decode_unsigned24(&apdu[0], value);
                break;
            case 4:
                decode_unsigned32(&apdu[0], value);
                break;
            default:
                *value = 0;
                break;
        }
    }

    return (int) len_value;
}




int I_cBACdec::decode_unsigned24(uint8_t * apdu,uint32_t * value)
{
    if (value) {
        *value = ((uint32_t) ((((uint32_t) apdu[0]) << 16) & 0x00ff0000));
        *value |= (uint32_t) ((((uint32_t) apdu[1]) << 8) & 0x0000ff00);
        *value |= ((uint32_t) (((uint32_t) apdu[2]) & 0x000000ff));
    }

    return 3;
}




/* from clause 20.2.11 Encoding of an Enumerated Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::decode_enumerated(uint8_t * apdu, uint32_t len_value,uint32_t * value)
{
    uint32_t unsigned_value = 0;
    int len;

    len = decode_unsigned(apdu, len_value, &unsigned_value);
    if (value) {
        *value = unsigned_value;
    }

    return len;
}






int I_cBACdec::encode_unsigned16(uint8_t * apdu, uint16_t value)
{
    apdu[0] = (uint8_t) ((value & 0xff00) >> 8);
    apdu[1] = (uint8_t) (value & 0x00ff);

    return 2;
}



int I_cBACdec::encode_unsigned32( uint8_t * apdu,uint32_t value)
{
    apdu[0] = (uint8_t) ((value & 0xff000000) >> 24);
    apdu[1] = (uint8_t) ((value & 0x00ff0000) >> 16);
    apdu[2] = (uint8_t) ((value & 0x0000ff00) >> 8);
    apdu[3] = (uint8_t) (value & 0x000000ff);

    return 4;
}




/* from clause 20.2.4 Encoding of an Unsigned Integer Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_context_unsigned(uint8_t * apdu, uint8_t tag_number, uint32_t value)
{
    int len = 0;

    /* length of unsigned is variable, as per 20.2.4 */
    if (value < 0x100) {
        len = 1;
    } else if (value < 0x10000) {
        len = 2;
    } else if (value < 0x1000000) {
        len = 3;
    } else {
        len = 4;
    }

    len = encode_tag(&apdu[0], tag_number, true, len);
    len += encode_bacnet_unsigned(&apdu[len], value);

    return len;
}



int I_cBACdec::encode_context_character_string(uint8_t * apdu,uint8_t tag_number, BACNET_CHARACTER_STRING *str)
{
    int len = 0;
    int string_len = 0;

    if (str->value8.size() > 0)
       {
		string_len =(int)str->value8.size();

		len += encode_tag(&apdu[0], tag_number, true, (uint32_t) string_len+1); // enkoding

		if ((len + string_len) < MAX_PDU)
		   {
			len += encode_bacnet_character_string(&apdu[len], str);
		   } else {
		    	   len = 0;
		          }
	   }

    return len;
}



/* from clause 20.2.9 Encoding of a Character String Value */
/* returns the number of apdu bytes consumed */
//int I_cBACdec::encode_bacnet_character_string(uint8_t * apdu, string char_string, BACNET_TEXT_ENCODING encoding)
//{
//    int len, i;
//    char *pString;
//
//    len = (int) char_string.size();
//    apdu[0] = encoding;  // 20.2.9 Encoding of characted string value
//    pString = (char*)char_string.c_str();
//
//    for (i = 0; i < len; i++) {
//        apdu[1 + i] = (uint8_t) pString[i];
//    }
//
//    return len /* for encoding */ ;
//}


// MOJA -- OBAVEZNO PROVJERITI za CHAR_SET_UCS2
int I_cBACdec::encode_bacnet_character_string(uint8_t * apdu, BACNET_CHARACTER_STRING *char_str)
{
    int len= 0 , i;
    char *pString;

    apdu[len++] = char_str->encoding;  // 20.2.9 Encoding of characted string value

    switch(char_str->encoding)
          {
		   case CHAR_SET_UCS2:
		        //memcpy(&apdu[1],char_str->value16.c_str(),char_str->value16.size());
		        return char_str->value16.size();

		   case CHAR_SET_JISC_6226:
		   case CHAR_SET_ISO8859:
		   case CHAR_SET_ANSI_X34:   // normalni char set
                memcpy(&apdu[len],char_str->value8.c_str(),char_str->value8.size());
                len += char_str->value8.size();
           default:
           break;
          }


 return len;
}




/* from clause 20.2.14 Encoding of an Object Identifier Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_context_object_id(  uint8_t *apdu, uint8_t tag_number, int object_type,uint32_t instance)
{
    int len = 0;

    /* length of object id is 4 octets, as per 20.2.14 */

    len = encode_tag(&apdu[0], tag_number, true, 4);
    len += encode_bacnet_object_id(&apdu[len], object_type, instance);

    return len;
}




/* from clause 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_tag(uint8_t * apdu, uint8_t tag_number,bool context_specific,uint32_t len_value_type)
{
    int len = 1;        /* return value */

    apdu[0] = 0;
    if (context_specific)
        apdu[0] = 0x08;

    /* additional tag byte after this byte */
    /* for extended tag byte */
    if (tag_number <= 14) {
        apdu[0] |= (tag_number << 4);
    } else {
        apdu[0] |= 0xF0;
        apdu[1] = tag_number;
        len++;
    }

    /* NOTE: additional len byte(s) after extended tag byte */
    /* if larger than 4 */
    if (len_value_type <= 4) {
        apdu[0] |= len_value_type;
    } else {
        apdu[0] |= 5;
        if (len_value_type <= 253) {
            apdu[len++] = (uint8_t) len_value_type;
        } else if (len_value_type <= 65535) {
            apdu[len++] = 254;
            len += encode_unsigned16(&apdu[len], (uint16_t) len_value_type);
        } else {
            apdu[len++] = 255;
            len += encode_unsigned32(&apdu[len], len_value_type);
        }
    }

    return len;
}




/* from clause 20.2.4 Encoding of an Unsigned Integer Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_bacnet_unsigned(uint8_t * apdu,uint32_t value)
{
    int len = 0;        /* return value */

    if (value < 0x100) {
        apdu[0] = (uint8_t) value;
        len = 1;
    } else if (value < 0x10000) {
        len = encode_unsigned16(&apdu[0], (uint16_t) value);
    } else if (value < 0x1000000) {
        len = encode_unsigned24(&apdu[0], value);
    } else {
        len = encode_unsigned32(&apdu[0], value);
    }

    return len;
}




/* from clause 20.2.14 Encoding of an Object Identifier Value */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_bacnet_object_id(uint8_t * apdu,int object_type,uint32_t instance)
{
    uint32_t value = 0;
    uint32_t type = 0;
    int len = 0;

    type = (uint32_t) object_type;
    value =
        ((type & BACNET_MAX_OBJECT) << BACNET_INSTANCE_BITS) | (instance &
        BACNET_MAX_INSTANCE);
    len = encode_unsigned32(apdu, value);

    return len;
}




int I_cBACdec::encode_unsigned24(uint8_t * apdu,uint32_t value)
{
    apdu[0] = (uint8_t) ((value & 0xff0000) >> 16);
    apdu[1] = (uint8_t) ((value & 0x00ff00) >> 8);
    apdu[2] = (uint8_t) (value & 0x0000ff);

    return 3;
}



/* from clause 20.2.9 Encoding of a Character String Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
//int I_cBACdec::decode_character_string(uint8_t * apdu, uint32_t len_value,  BACNET_CHARACTER_STRING * char_string)
//{
//    int len = 0;        /* return value */
//    bool status = false;
//
//    status =
//        characterstring_init(char_string, apdu[0], (char *) &apdu[1],len_value - 1);
//
//    if (status) {
//        len = (int) len_value;
//    }
//
//    return len;
//}

int I_cBACdec::decode_character_string(uint8_t * apdu, uint32_t len_value,  string *char_string)
{
 for (uint32_t c=0,i=1;c<len_value-1;c++,i++)
     {
      char_string->push_back(apdu[i]);
     }

 return len_value;
}



int I_cBACdec::decode_standard_character_string(uint8_t * apdu, uint32_t len_value,  string &ptr_string)
{
 for (uint32_t c=0;c<len_value-1;c++)
     {
      //ptr_string->push_back(apdu[c]);
      ptr_string += apdu[c];
     }

 return len_value;
}



string I_cBACdec::decode_standard_character_string(uint8_t * apdu, uint32_t len_value)
{
 string s;
 for (uint32_t c=0;c<len_value-1;c++)
     {
      //ptr_string->push_back(apdu[c]);
      s += apdu[c];
     }

 return s;
}







int I_cBACdec::decode_standard_character_string(uint8_t * apdu, uint32_t len_value,  wstring *ptr_wstring)
{
 for (int c=0;c<len_value-1;c+=2)
     {
      wchar_t wtmp = 0;
      wtmp = apdu[c]<<8;
      wtmp = apdu[c+1];
      ptr_wstring->push_back(wtmp);
     }

 return len_value;
}



#define CHARACTER_STRING_CAPACITY (MAX_CHARACTER_STRING_BYTES - 1)

//
//bool I_cBACdec::characterstring_init(BACNET_CHARACTER_STRING * char_string,uint8_t encoding,const char *value,size_t length)
//{
//    bool status = false;        /* return value */
//    size_t i;   /* counter */
//
//    if (char_string) {
//        char_string->length = 0;
//        char_string->encoding = encoding;
//        /* save a byte at the end for NULL -
//           note: assumes printable characters */
//        if (length <= CHARACTER_STRING_CAPACITY) {
//            if (value) {
//                for (i = 0; i < MAX_CHARACTER_STRING_BYTES; i++) {
//                    if (i < length) {
//                        char_string->value[char_string->length] = value[i];
//                        char_string->length++;
//                    } else {
//                        char_string->value[i] = 0;
//                    }
//                }
//            } else {
//                for (i = 0; i < MAX_CHARACTER_STRING_BYTES; i++) {
//                    char_string->value[i] = 0;
//                }
//            }
//            status = true;
//        }
//    }
//
//    return status;
//}
//


/* from clause 20.2.11 Encoding of an Enumerated Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_context_enumerated(uint8_t * apdu,uint8_t tag_number,uint32_t value)
{
    int len = 0;        /* return value */

    /* length of enumerated is variable, as per 20.2.11 */
    if (value < 0x100) {
        len = 1;
    } else if (value < 0x10000) {
        len = 2;
    } else if (value < 0x1000000) {
        len = 3;
    } else {
        len = 4;
    }

    len = encode_tag(&apdu[0], tag_number, true, (uint32_t) len);
    len += encode_bacnet_enumerated(&apdu[len], value);

    return len;
}



/* from clause 20.1.2.4 max-segments-accepted */
/* and clause 20.1.2.5 max-APDU-length-accepted */
/* returns the encoded octet */
uint8_t I_cBACdec::encode_max_segs_max_apdu(int max_segs,int max_apdu)
{
    uint8_t octet = 0;

    if (max_segs < 2)
        octet = 0;
    else if (max_segs < 4)
        octet = 0x10;
    else if (max_segs < 8)
        octet = 0x20;
    else if (max_segs < 16)
        octet = 0x30;
    else if (max_segs < 32)
        octet = 0x40;
    else if (max_segs < 64)
        octet = 0x50;
    else if (max_segs == 64)
        octet = 0x60;
    else
        octet = 0x70;

    /* max_apdu must be 50 octets minimum */
    if (max_apdu <= 50)
        octet |= 0x00;
    else if (max_apdu <= 128)
        octet |= 0x01;
    /*fits in a LonTalk frame */
    else if (max_apdu <= 206)
        octet |= 0x02;
    /*fits in an ARCNET or MS/TP frame */
    else if (max_apdu <= 480)
        octet |= 0x03;
    else if (max_apdu <= 1024)
        octet |= 0x04;
    /* fits in an ISO 8802-3 frame */
    else if (max_apdu <= 1476)
        octet |= 0x05;

    return octet;
}



/* from clause 20.2.11 Encoding of an Enumerated Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_bacnet_enumerated( uint8_t * apdu,uint32_t value)
{
    return encode_bacnet_unsigned(apdu, value);
}





/* from clause 20.2.1.3.2 Constructed Data */
/* returns true if the tag is context specific and matches */
bool I_cBACdec::decode_is_context_tag(uint8_t * apdu,uint8_t tag_number)
{
    uint8_t my_tag_number = 0;

    decode_tag_number(apdu, &my_tag_number);

    return (bool) (IS_CONTEXT_SPECIFIC(*apdu) &&
        (my_tag_number == tag_number));
}



/* from clause 20.2.1.3.2 Constructed Data */
/* returns the true if the tag matches */
bool I_cBACdec::decode_is_opening_tag_number( uint8_t * apdu, uint8_t tag_number)
{
    uint8_t my_tag_number = 0;
    bool opening_tag = false;

    opening_tag = (bool) ((apdu[0] & 0x07) == 6);
    decode_tag_number(apdu, &my_tag_number);

    return (bool) (opening_tag && (my_tag_number == tag_number));
}





int I_cBACdec::bacapp_decode_application_data(uint8_t * apdu, unsigned max_apdu_len, BACNET_APPLICATION_DATA_VALUE * value)
{
    int len = 0;
    int tag_len = 0;
    uint8_t tag_number = 0;
    uint32_t len_value_type = 0;


    /* FIXME: use max_apdu_len! */
    max_apdu_len = max_apdu_len;
    if (apdu && value && !IS_CONTEXT_SPECIFIC(*apdu))
       {
        value->context_specific = false;
        tag_len = decode_tag_number_and_value(&apdu[0], &tag_number, &len_value_type);

        if (tag_len)
           {
            len += tag_len;
            value->application_tag = (BACNET_APPLICATION_TAG)tag_number;
            int ret = bacapp_decode_data(&apdu[len], tag_number, len_value_type,value);
            if (ret < 0)
               {
                return ret;
               }
            len += ret;
           }
        return len;
       }

    return -1;
}




/* decode the data and store it into value->
   Return the number of octets consumed. */
int I_cBACdec::bacapp_decode_data(uint8_t * apdu,uint8_t tag_data_type,uint32_t len_value_type,BACNET_APPLICATION_DATA_VALUE * value)
{
    int len = 0;

    if (apdu && value) {
        switch (tag_data_type) {
            case BACNET_APPLICATION_TAG_NULL:
                /* nothing else to do */
                break;
            case BACNET_APPLICATION_TAG_BOOL:
                value->type.Boolean = decode_boolean(len_value_type);
                break;
            case BACNET_APPLICATION_TAG_UINT:
                len =
                    decode_unsigned(&apdu[0], len_value_type,
                    &value->type.Unsigned_Int);
                break;
            case BACNET_APPLICATION_TAG_SINT:
                len =
                    decode_signed(&apdu[0], len_value_type,
                    &value->type.Signed_Int);
                break;
            case BACNET_APPLICATION_TAG_REAL:
                len =
                    decode_real_safe(&apdu[0], len_value_type,
                    &(value->type.Real));
                break;
            case BACNET_APPLICATION_TAG_DOUBLE:
                len =decode_double_safe(&apdu[0], len_value_type,&(value->type.Double));
                break;
            case BACNET_APPLICATION_TAG_OSTRING:
             len = decode_octet_string(&apdu[0], len_value_type, &value->type.Octet_String);
             break;
            case BACNET_APPLICATION_TAG_CSTRING:
            {
                int a = apdu[0];
                value->type.character_String.encoding = (BACNET_TEXT_ENCODING)a;
                switch (value->type.character_String.encoding)
                       {
                        case CHAR_SET_MS_DBCS:
                        case CHAR_SET_UCS2:
                        {
                         uint8_t *tmp = &apdu[1];
                         len = decode_standard_character_string(&apdu[1], len_value_type,  &value->type.character_String.value16);
                         break;
                        }

                        case CHAR_SET_ISO8859:
                        case CHAR_SET_ANSI_X34:
                        case CHAR_SET_JISC_6226:

                        {
                         //string s = decode_standard_character_string(&apdu[1], len_value_type);
                         //value->type.character_String.value8 = s;
													value->type.character_String.value8 = decode_standard_character_string(&apdu[1], len_value_type);
												 
												 len += len_value_type;

                         //value->type.character_String.value8 = new char[len_value_type];
                         //len = decode_standard_character_string(&apdu[1], len_value_type,  &value->type.character_String.value8);
                         //len = decode_standard_character_string(&apdu[1], len_value_type,  &value->type.character_String.value8);
                         break;
                        }

                        case CHAR_SET_UCS4:
                         gLog->AppendLog("BACDEC: Nepodrzani Text: CHAR_SET_UCS4");
                         break;

					    default:
						 return -1;  // ne podrzavamp ostale za sada
						break;
                       }
                break;
               }
            case BACNET_APPLICATION_TAG_BSTRING:
                len = decode_bitstring(&apdu[0], len_value_type, &value->type.Bit_String);


                break;

            case BACNET_APPLICATION_TAG_ENUM:
                len =
                    decode_enumerated(&apdu[0], len_value_type,
                    &value->type.Enumerated);
                break;

            case BACNET_APPLICATION_TAG_DATE:
                len =
                    this->decode_date_safe(&apdu[0], len_value_type,
                    &value->type.Date);
                break;

            case BACNET_APPLICATION_TAG_TIME:
                len =
                    decode_bacnet_time_safe(&apdu[0], len_value_type,
                    &value->type.Time);
                break;

            case BACNET_APPLICATION_TAG_OID:
                {
                    uint16_t object_type = 0;
                    uint32_t instance = 0;
                    len =
                        decode_object_id_safe(&apdu[0], len_value_type,
                        &object_type, &instance);
                    value->type.Object_Id.type = object_type;
                    value->type.Object_Id.instance = instance;
                }
                break;

            default:
                break;
        }
    }

    if (len == 0 && tag_data_type != BACNET_APPLICATION_TAG_NULL &&
        tag_data_type != BACNET_APPLICATION_TAG_BOOL) {
        value->application_tag = MAX_BACNET_APPLICATION_TAG;
    }
    return len;
}





int I_cBACdec::decode_bacnet_time_safe(uint8_t * apdu,uint32_t len_value,BACNET_TIME * btime)
{
    if (len_value != 4) {
        btime->hour = 0;
        btime->hundredths = 0;
        btime->minute = 0;
        btime->sec = 0;
        return len_value;
    } else {
        return decode_bacnet_time(apdu, btime);
    }
}



int I_cBACdec::decode_date_safe( uint8_t * apdu,uint32_t len_value,BACNET_DATE * bdate)
{
    if (len_value != 4) {
        bdate->day = 0;
        bdate->month = 0;
        bdate->wday = 0;
        bdate->year = 0;
        return len_value;
    } else {
        return decode_date(apdu, bdate);
    }
}



/* returns the length. */
size_t I_cBACdec::octetstring_length( BACNET_OCTET_STRING * octet_string)
{
    size_t length = 0;

    if (octet_string) {
        /* FIXME: validate length is within bounds? */
        length = octet_string->vValue.size();
    }

    return length;
}



/* returns the length.  Returns the value in parameter. */

/*

NE KORISTI SE VISE  octet_string je vector

uint8_t *I_cBACdec::octetstring_value(BACNET_OCTET_STRING * octet_string)
{
    uint8_t *value = NULL;

    if (octet_string) {
        value = Octet_String.value;
    }

    return value;
}
*/

//
///* Returns the value-> */
//char *I_cBACdec::characterstring_value( BACNET_CHARACTER_STRING * char_string)
//{
//    char *value = NULL;
//
//    if (char_string) {
//        value = char_string->value;
//    }
//
//    return value;
//}
//
///* returns the length. */
//size_t I_cBACdec::characterstring_length(BACNET_CHARACTER_STRING * char_string)
//{
//    size_t length = 0;
//
//    if (char_string) {
//        /* FIXME: validate length is within bounds? */
//        length = char_string->length;
//    }
//
//    return length;
//}



/* from clause 20.2.3 Encoding of a Boolean Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
bool I_cBACdec::decode_boolean(uint32_t len_value)
{
    bool boolean_value = false;

    if (len_value) {
        boolean_value = true;
    }

    return boolean_value;
}



/* from clause 20.2.5 Encoding of a Signed Integer Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::decode_signed(uint8_t * apdu, uint32_t len_value,int32_t * value)
{
    if (value) {
        switch (len_value) {
            case 1:
                decode_signed8(&apdu[0], value);
                break;
            case 2:
                decode_signed16(&apdu[0], value);
                break;
            case 3:
                decode_signed24(&apdu[0], value);
                break;
            case 4:
                decode_signed32(&apdu[0], value);
                break;
            default:
                *value = 0;
                break;
        }
    }

    return (int) len_value;
}


/* from clause 20.2.6 Encoding of a Real Number Value */
/* returns the number of apdu bytes consumed */
int I_cBACdec::decode_real(uint8_t * apdu, float *real_value)
{
    union {
        uint8_t byte[4];
        float real_value;
    } my_data;

    /* NOTE: assumes the compiler stores float as IEEE-754 float */
#if BIG_ENDIAN
    my_data.byte[0] = apdu[0];
    my_data.byte[1] = apdu[1];
    my_data.byte[2] = apdu[2];
    my_data.byte[3] = apdu[3];
#else
    my_data.byte[0] = apdu[3];
    my_data.byte[1] = apdu[2];
    my_data.byte[2] = apdu[1];
    my_data.byte[3] = apdu[0];
#endif

    *real_value = my_data.real_value;

    return 4;
}


int I_cBACdec::decode_real_safe( uint8_t * apdu, uint32_t len_value, float *real_value)
{
    if (len_value != 4) {
        *real_value = 0.0f;
        return len_value;
    } else {
        return decode_real(apdu, real_value);
    }
}


int I_cBACdec::decode_double_safe(uint8_t * apdu,uint32_t len_value,double *double_value)
{
    if (len_value != 8) {
        *double_value = 0.0;
        return len_value;
    } else {
        return decode_double(apdu, double_value);
    }
}


/* from clause 20.2.8 Encoding of an Octet String Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::decode_octet_string( uint8_t * apdu,uint32_t len_value, BACNET_OCTET_STRING * octet_string)
{
    int len = 0;        /* return value */
    bool status = false;

    for (uint32_t c=0;c<len_value;c++)
        {
         octet_string->vValue.push_back(apdu[c]);
        }

    len = (int)len_value;
    return len;
}



/* from clause 20.2.10 Encoding of a Bit String Value */
/* returns the number of apdu bytes consumed */
int I_cBACdec::decode_bitstring(uint8_t * apdu,uint32_t len_value,BACNET_BIT_STRING *bit_string)
{
    int len = 0;
    uint8_t unused_bits = 0;
    uint32_t i = 0;
    uint32_t bytes_used = 0;


    bitstring_init(bit_string);

    if (len_value) {
        /* the first octet contains the unused bits */
        bytes_used = len_value - 1;
        if (bytes_used <= MAX_BITSTRING_BYTES) {
            len = 1;
            for (i = 0; i < bytes_used; i++) {
                bitstring_set_octet(bit_string, (uint8_t) i,
                    byte_reverse_bits(apdu[len++]));
            }
            unused_bits = (uint8_t) (apdu[0] & 0x07);
            bitstring_set_bits_used(bit_string, (uint8_t) bytes_used,
                unused_bits);
        }
    }

    return len;
}


int I_cBACdec::decode_object_id_safe( uint8_t * apdu, uint32_t len_value,uint16_t * object_type,uint32_t * instance)
{
    if (len_value != 4) {
        return 0;
    } else {
        return decode_object_id(apdu, object_type, instance);
    }
}






int I_cBACdec::decode_signed8(uint8_t * apdu,int32_t * value)
{
    if (value) {
        /* negative - bit 7 is set */
        if (apdu[0] & 0x80)
            *value = 0xFFFFFF00;
        else
            *value = 0;
        *value |= ((int32_t) (((int32_t) apdu[0]) & 0x000000ff));
    }

    return 1;
}

int I_cBACdec::decode_signed16(uint8_t * apdu,int32_t * value)
{
    if (value) {
        /* negative - bit 7 is set */
        if (apdu[0] & 0x80)
            *value = 0xFFFF0000;
        else
            *value = 0;
        *value |= ((int32_t) ((((int32_t) apdu[0]) << 8) & 0x0000ff00));
        *value |= ((int32_t) (((int32_t) apdu[1]) & 0x000000ff));
    }

    return 2;
}


int I_cBACdec::decode_signed24(uint8_t * apdu,int32_t * value)
{
    if (value) {
        /* negative - bit 7 is set */
        if (apdu[0] & 0x80)
            *value = 0xFF000000;
        else
            *value = 0;
        *value |= ((int32_t) ((((int32_t) apdu[0]) << 16) & 0x00ff0000));
        *value |= ((int32_t) ((((int32_t) apdu[1]) << 8) & 0x0000ff00));
        *value |= ((int32_t) (((int32_t) apdu[2]) & 0x000000ff));
    }

    return 3;
}


int I_cBACdec::decode_signed32(uint8_t * apdu,int32_t * value)
{
    if (value) {
        *value = ((int32_t) ((((int32_t) apdu[0]) << 24) & 0xff000000));
        *value |= ((int32_t) ((((int32_t) apdu[1]) << 16) & 0x00ff0000));
        *value |= ((int32_t) ((((int32_t) apdu[2]) << 8) & 0x0000ff00));
        *value |= ((int32_t) (((int32_t) apdu[3]) & 0x000000ff));
    }

    return 4;
}




/* from clause 20.2.7 Encoding of a Double Precision Real Number Value */
/* returns the number of apdu bytes consumed */
int I_cBACdec::decode_double(uint8_t * apdu,double *double_value)
{
    union {
        uint8_t byte[8];
        double double_value;
    } my_data;

    /* NOTE: assumes the compiler stores float as IEEE-754 float */
#if BIG_ENDIAN
    my_data.byte[0] = apdu[0];
    my_data.byte[1] = apdu[1];
    my_data.byte[2] = apdu[2];
    my_data.byte[3] = apdu[3];
    my_data.byte[4] = apdu[4];
    my_data.byte[5] = apdu[5];
    my_data.byte[6] = apdu[6];
    my_data.byte[7] = apdu[7];
#else
    my_data.byte[0] = apdu[7];
    my_data.byte[1] = apdu[6];
    my_data.byte[2] = apdu[5];
    my_data.byte[3] = apdu[4];
    my_data.byte[4] = apdu[3];
    my_data.byte[5] = apdu[2];
    my_data.byte[6] = apdu[1];
    my_data.byte[7] = apdu[0];
#endif

    *double_value = my_data.double_value;

    return 8;
}



// vraca NULL ako je string veci od dozvoljenog.
// inicira Octet_String.value na 0 ako value == NULL
// inace stvara Octet_String.value i inicira na value



void I_cBACdec::octetstring_init(BACNET_OCTET_STRING *octet_string,uint8_t *value,size_t length)
{
 for (uint32_t c=0;c<length;c++)
     {
      octet_string->vValue.push_back(value[c]);
     }
}







void I_cBACdec::bitstring_init( BACNET_BIT_STRING * bit_string)
{
    int i;

    bit_string->bits_used = 0;
    for (i = 0; i < MAX_BITSTRING_BYTES; i++) {
        bit_string->value[i] = 0;
    }
}





bool I_cBACdec::bitstring_set_octet( BACNET_BIT_STRING * bit_string,uint8_t index,uint8_t octet)
{
    bool status = false;

    if (bit_string) {
        if (index < MAX_BITSTRING_BYTES) {
            bit_string->value[index] = octet;
            status = true;
        }
    }

    return status;
}



uint8_t I_cBACdec::byte_reverse_bits( uint8_t in_byte)
{
    uint8_t out_byte = 0;

    if (in_byte & SET_BIT(0)) {
        out_byte |= SET_BIT(7);
    }
    if (in_byte & SET_BIT(1)) {
        out_byte |= SET_BIT(6);
    }
    if (in_byte & SET_BIT(2)) {
        out_byte |= SET_BIT(5);
    }
    if (in_byte & SET_BIT(3)) {
        out_byte |= SET_BIT(4);
    }
    if (in_byte & SET_BIT(4)) {
        out_byte |= SET_BIT(3);
    }
    if (in_byte & SET_BIT(5)) {
        out_byte |= SET_BIT(2);
    }
    if (in_byte & SET_BIT(6)) {
        out_byte |= SET_BIT(1);
    }
    if (in_byte & SET_BIT(7)) {
        out_byte |= SET_BIT(0);
    }

    return out_byte;
}



bool I_cBACdec::bitstring_set_bits_used(BACNET_BIT_STRING * bit_string,uint8_t bytes_used,uint8_t unused_bits)
{
    bool status = false;

    if (bit_string) {
        /* FIXME: check that bytes_used is at least one? */
        bit_string->bits_used = bytes_used * 8;
        bit_string->bits_used -= unused_bits;
        status = true;
    }

    return status;
}

/*
const char *I_cBACdec::bactext_object_type_name(unsigned index)
{
 switch(index)
       {
        case OBJECT_ANALOG_INPUT:
          return "OBJECT_ANALOG_INPUT";
        case OBJECT_ANALOG_OUTPUT:
          return "OBJECT_ANALOG_OUTPUT";
        case OBJECT_ANALOG_VALUE :
          return "OBJECT_ANALOG_VALUE";
        case OBJECT_BINARY_INPUT:
          return "OBJECT_BINARY_INPUT";
        case OBJECT_BINARY_OUTPUT :
          return "OBJECT_BINARY_OUTPUT";
        case OBJECT_BINARY_VALUE:
          return "OBJECT_BINARY_VALUE";
        case OBJECT_CALENDAR :
          return "OBJECT_CALENDAR";
        case OBJECT_COMMAND:
          return "OBJECT_COMMAND";
        case OBJECT_DEVICE:
          return "OBJECT_DEVICE";
        case OBJECT_EVENT_ENROLLMENT:
          return "OBJECT_EVENT_ENROLLMENT";
        case OBJECT_FILE:
          return "OBJECT_FILE";
        case OBJECT_GROUP :
          return "OBJECT_GROUP";
        case OBJECT_LOOP:
          return "OBJECT_LOOP";
        case OBJECT_MULTI_STATE_INPUT:
          return "OBJECT_MULTI_STATE_INPUT";
        case OBJECT_MULTI_STATE_OUTPUT :
          return "OBJECT_MULTI_STATE_OUTPUT";
        case OBJECT_NOTIFICATION_CLASS :
          return "OBJECT_NOTIFICATION_CLASS";
        case OBJECT_PROGRAM :
          return "OBJECT_PROGRAM";
        case OBJECT_SCHEDULE:
          return "OBJECT_SCHEDULE";
        case OBJECT_AVERAGING :
          return "OBJECT_AVERAGING";
        case OBJECT_MULTI_STATE_VALUE :
          return "OBJECT_MULTI_STATE_VALUE";
        case OBJECT_TREND_LOG :
          return "OBJECT_TREND_LOG";
        case OBJECT_LIFE_SAFETY_POINT:
          return "OBJECT_LIFE_SAFETY_POINT";
        case OBJECT_LIFE_SAFETY_ZONE :
          return "OBJECT_LIFE_SAFETY_ZONE";
        case OBJECT_ACCUMULATOR:
          return "OBJECT_ACCUMULATOR";
        case OBJECT_PULSE_CONVERTER :
          return "OBJECT_PULSE_CONVERTER";
        case OBJECT_EVENT_LOG :
          return "OBJECT_EVENT_LOG";
        case OBJECT_GLOBAL_GROUP:
          return "OBJECT_GLOBAL_GROUP";
        case OBJECT_TREND_LOG_MULTIPLE:
          return "OBJECT_TREND_LOG_MULTIPLE";
        case OBJECT_LOAD_CONTROL:
          return "OBJECT_LOAD_CONTROL";
        case OBJECT_STRUCTURED_VIEW:
          return "OBJECT_STRUCTURED_VIEW";
        case OBJECT_ACCESS_DOOR :
          return "OBJECT_ACCESS_DOOR";
        case OBJECT_LIGHTING_OUTPUT :
          return "OBJECT_LIGHTING_OUTPUT";
        case OBJECT_ACCESS_CREDENTIAL :
          return "OBJECT_ACCESS_CREDENTIAL";
        case OBJECT_ACCESS_POINT :
          return "OBJECT_ACCESS_POINT";
        case OBJECT_ACCESS_RIGHTS:
          return "OBJECT_ACCESS_RIGHTS";
        case OBJECT_ACCESS_USER :
          return "OBJECT_ACCESS_USER";
        case OBJECT_ACCESS_ZONE :
          return "OBJECT_ACCESS_ZONE";
        case OBJECT_CREDENTIAL_DATA_INPUT:
          return "OBJECT_CREDENTIAL_DATA_INPUT";
        default:
          return NULL;
       }

}
 */


//
//
//const char *I_cBACdec::indtext_by_index_split_default( INDTEXT_DATA * data_list, unsigned index, unsigned split_index,const char *before_split_default_name, const char *default_name)
//{
//    if (index < split_index)
//        return indtext_by_index_default(data_list, index,before_split_default_name);
//
//    else
//        return indtext_by_index_default(data_list, index, default_name);
//}
//
//
//
//
//const char *I_cBACdec::indtext_by_index_default( INDTEXT_DATA * data_list,unsigned index, const char *default_string)
//{
//    const char *pString = NULL;
//
//    if (data_list) {
//        while (data_list->pString) {
//            if (data_list->index == index) {
//                pString = data_list->pString;
//                break;
//            }
//            data_list++;
//        }
//    }
//
//    return pString ? pString : default_string;
//}




int I_cBACdec::bacapp_encode_data(uint8_t * apdu,BACNET_APPLICATION_DATA_VALUE * value)
{
    int apdu_len = 0;   /* total length of the apdu, return value */

    if (value && apdu) {
        if (value->context_specific) {
            apdu_len =
                bacapp_encode_context_data_value(&apdu[0], value->context_tag,
                value);
        } else {
            apdu_len = bacapp_encode_application_data(&apdu[0], value);
        }
    }

    return apdu_len;
}




int I_cBACdec::bacapp_encode_context_data_value(uint8_t * apdu,uint8_t context_tag_number,BACNET_APPLICATION_DATA_VALUE * value)
{
    int apdu_len = 0;   /* total length of the apdu, return value */

    if (value && apdu) {
        switch (value->application_tag) {
#if defined (BACAPP_NULL)
            case BACNET_APPLICATION_TAG_NULL:
                apdu_len = encode_context_null(&apdu[0], context_tag_number);
                break;
#endif
#if defined (BACAPP_BOOLEAN)
            case BACNET_APPLICATION_TAG_BOOL:
                apdu_len =
                    encode_context_boolean(&apdu[0], context_tag_number,
                    value->type.Boolean);
                break;
#endif
#if defined (BACAPP_UNSIGNED)
            case BACNET_APPLICATION_TAG_UINT:
                apdu_len =
                    encode_context_unsigned(&apdu[0], context_tag_number,
                    value->type.Unsigned_Int);
                break;
#endif
#if defined (BACAPP_SIGNED)
            case BACNET_APPLICATION_TAG_SINT:
                apdu_len =
                    encode_context_signed(&apdu[0], context_tag_number,
                    value->type.Signed_Int);
                break;
#endif
#if defined (BACAPP_REAL)
            case BACNET_APPLICATION_TAG_REAL:
                apdu_len =
                    encode_context_real(&apdu[0], context_tag_number,
                    value->type.Real);
                break;
#endif
#if defined (BACAPP_DOUBLE)
            case BACNET_APPLICATION_TAG_DOUBLE:
                apdu_len =
                    encode_context_double(&apdu[0], context_tag_number,
                    value->type.Double);
                break;
#endif
#if defined (BACAPP_OCTET_STRING)
            case BACNET_APPLICATION_TAG_OSTRING:
                apdu_len =
                    encode_context_octet_string(&apdu[0], context_tag_number,&value->type.Octet_String);

                break;
#endif
#if defined (BACAPP_CHARACTER_STRING)
            case BACNET_APPLICATION_TAG_CSTRING:
                //apdu_len =
                //    encode_context_character_string(&apdu[0], context_tag_number, (char*)&value->type.character_String.value);

                break;
#endif
#if defined (BACAPP_BIT_STRING)
            case BACNET_APPLICATION_TAG_BSTRING:
                apdu_len =
                    encode_context_bitstring(&apdu[0], context_tag_number,
                    &value->type.Bit_String);
                break;
#endif
#if defined (BACAPP_ENUMERATED)
            case BACNET_APPLICATION_TAG_ENUM:
                apdu_len =
                    encode_context_enumerated(&apdu[0], context_tag_number,
                    value->type.Enumerated);
                break;
#endif
#if defined (BACAPP_DATE)
            case BACNET_APPLICATION_TAG_DATE:
                 //apdu_len = encode_context_date(&apdu[0], context_tag_number,
                //
                //    &value->type.Date);
                break;
#endif
#if defined (BACAPP_TIME)
            case BACNET_APPLICATION_TAG_TIME:
                //apdu_len =
                //    encode_context_time(&apdu[0], context_tag_number,
                //    &value->type.Time);
                break;
#endif
#if defined (BACAPP_OBJECT_ID)
            case BACNET_APPLICATION_TAG_OID:
                apdu_len =
                    encode_context_object_id(&apdu[0], context_tag_number,
                    (int) value->type.Object_Id.type,
                    value->type.Object_Id.instance);
                break;
#endif
            default:
                break;
        }
    }

    return apdu_len;
}




int I_cBACdec::bacapp_encode_application_data(uint8_t * apdu, BACNET_APPLICATION_DATA_VALUE * value)
{
    int apdu_len = 0;   /* total length of the apdu, return value */

    if (value && apdu) {
        switch (value->application_tag) {
#if defined (BACAPP_NULL)
            case BACNET_APPLICATION_TAG_NULL:
                apdu[0] = value->application_tag;
                apdu_len++;
                break;
#endif
#if defined (BACAPP_BOOLEAN)
            case BACNET_APPLICATION_TAG_BOOL:
                apdu_len =
                    encode_application_boolean(&apdu[0], value->type.Boolean);
                break;
#endif
#if defined (BACAPP_UNSIGNED)
            case BACNET_APPLICATION_TAG_UINT:
                apdu_len =
                    encode_application_unsigned(&apdu[0],
                    value->type.Unsigned_Int);
                break;
#endif
#if defined (BACAPP_SIGNED)
            case BACNET_APPLICATION_TAG_SINT:
                apdu_len =
                    encode_application_signed(&apdu[0],
                    value->type.Signed_Int);
                break;
#endif
#if defined (BACAPP_REAL)
            case BACNET_APPLICATION_TAG_REAL:
                apdu_len = encode_application_real(&apdu[0], value->type.Real);
                break;
#endif
#if defined (BACAPP_DOUBLE)
            case BACNET_APPLICATION_TAG_DOUBLE:
                apdu_len =
                    encode_application_double(&apdu[0], value->type.Double);
                break;
#endif
#if defined (BACAPP_OCTET_STRING)
            case BACNET_APPLICATION_TAG_OSTRING:
                apdu_len =
                    encode_application_octet_string(&apdu[0],&value->type.Octet_String);

                break;
#endif
#if defined (BACAPP_CHARACTER_STRING)
            case BACNET_APPLICATION_TAG_CSTRING:
                apdu_len =
                    encode_application_character_string(&apdu[0], &value->type.character_String);

                break;
#endif
#if defined (BACAPP_BIT_STRING)
            case BACNET_APPLICATION_TAG_BSTRING:
                apdu_len =
                    encode_application_bitstring(&apdu[0],
                    &value->type.Bit_String);
                break;
#endif
#if defined (BACAPP_ENUMERATED)
            case BACNET_APPLICATION_TAG_ENUM:
                apdu_len =
                    encode_application_enumerated(&apdu[0],
                    value->type.Enumerated);
                break;
#endif
#if defined (BACAPP_DATE)
            case BACNET_APPLICATION_TAG_DATE:
                //apdu_len =
                //    encode_application_date(&apdu[0], &value->type.Date);
                break;
#endif
#if defined (BACAPP_TIME)
            case BACNET_APPLICATION_TAG_TIME:
                //apdu_len =
                //    encode_application_time(&apdu[0], &value->type.Time);
                break;
#endif
#if defined (BACAPP_OBJECT_ID)
            case BACNET_APPLICATION_TAG_OID:
                apdu_len =
                    encode_application_object_id(&apdu[0],
                    (int) value->type.Object_Id.type,
                    value->type.Object_Id.instance);
                break;
#endif
            default:
                break;
        }
    }

    return apdu_len;
}


int I_cBACdec::encode_context_null(uint8_t * apdu,uint8_t tag_number)
{
    return encode_tag(&apdu[0], tag_number, true, 0);
}


/* context tagged is encoded differently */
int I_cBACdec::encode_context_boolean( uint8_t * apdu, uint8_t tag_number,bool boolean_value)
{
    int len = 0;        /* return value */

    len = encode_tag(&apdu[0], (uint8_t) tag_number, true, 1);
    apdu[len] = (bool) (boolean_value ? 1 : 0);
    len++;

    return len;
}


/* from clause 20.2.4 Encoding of an Unsigned Integer Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
//int I_cBACdec::encode_context_unsigned( uint8_t * apdu, uint8_t tag_number,uint32_t value)
//{
//    int len = 0;
//
//    /* length of unsigned is variable, as per 20.2.4 */
//    if (value < 0x100) {
//        len = 1;
//    } else if (value < 0x10000) {
//        len = 2;
//    } else if (value < 0x1000000) {
//        len = 3;
//    } else {
//        len = 4;
//    }
//
//    len = encode_tag(&apdu[0], tag_number, true, len);
//    len += encode_bacnet_unsigned(&apdu[len], value);
//
//    return len;
//}


/* from clause 20.2.5 Encoding of a Signed Integer Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_context_signed( uint8_t * apdu,uint8_t tag_number,int32_t value)
{
    int len = 0;        /* return value */

    /* length of signed int is variable, as per 20.2.11 */
    if ((value >= -128) && (value < 128)) {
        len = 1;
    } else if ((value >= -32768) && (value < 32768)) {
        len = 2;
    } else if ((value > -8388608) && (value < 8388608)) {
        len = 3;
    } else {
        len = 4;
    }

    len = encode_tag(&apdu[0], tag_number, true, (uint32_t) len);
    len += encode_bacnet_signed(&apdu[len], value);

    return len;
}




int I_cBACdec::encode_context_real(uint8_t * apdu, uint8_t tag_number, float value)
{
    int len = 0;

    /* length of double is 4 octets, as per 20.2.6 */
    len = encode_tag(&apdu[0], tag_number, true, 4);
    len += encode_bacnet_real(value, &apdu[len]);
    return len;
}




int I_cBACdec::encode_context_double(uint8_t * apdu,uint8_t tag_number, double value)
{
    int len = 0;

    /* length of double is 8 octets, as per 20.2.7 */
    len = encode_tag(&apdu[0], tag_number, true, 8);
    len += encode_bacnet_double(value, &apdu[len]);

    return len;
}





/* from clause 20.2.8 Encoding of an Octet String Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_context_octet_string( uint8_t * apdu,uint8_t tag_number,BACNET_OCTET_STRING * octet_string)
{
    int apdu_len = 0;

    if (apdu && octet_string) {
        apdu_len =
            encode_tag(&apdu[0], tag_number, true, (uint32_t)octetstring_length(octet_string));

        if ((apdu_len + octetstring_length(octet_string)) < MAX_APDU) {
            apdu_len += encode_octet_string(&apdu[apdu_len], octet_string);
        } else {
            apdu_len = 0;
        }
    }

    return apdu_len;
}




//
//int I_cBACdec::encode_context_character_string( uint8_t * apdu, uint8_t tag_number,BACNET_CHARACTER_STRING * char_string)
//{
//    int len = 0;
//    int string_len = 0;
//
//    string_len =
//        (int) characterstring_length(char_string) + 1 /* for encoding */ ;
//    len += encode_tag(&apdu[0], tag_number, true, (uint32_t) string_len);
//    if ((len + string_len) < MAX_APDU) {
//        len += encode_bacnet_character_string(&apdu[len], char_string);
//    } else {
//        len = 0;
//    }
//
//    return len;
//}



int I_cBACdec::encode_context_bitstring( uint8_t * apdu,uint8_t tag_number, BACNET_BIT_STRING * bit_string)
{
    int len = 0;
    uint32_t bit_string_encoded_length = 1;     /* 1 for the bits remaining octet */

    /* bit string may use more than 1 octet for the tag, so find out how many */
    bit_string_encoded_length += bitstring_bytes_used(bit_string);
    len = encode_tag(&apdu[0], tag_number, true, bit_string_encoded_length);
    len += encode_bitstring(&apdu[len], bit_string);

    return len;
}


/* from clause 20.2.3 Encoding of a Boolean Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_application_boolean( uint8_t * apdu, bool boolean_value)
{
    int len = 0;
    uint32_t len_value = 0;

    if (boolean_value) {
        len_value = 1;
    }
    len =
        encode_tag(&apdu[0], BACNET_APPLICATION_TAG_BOOL, false, len_value);

    return len;
}


/* from clause 20.2.4 Encoding of an Unsigned Integer Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_application_unsigned(    uint8_t * apdu,uint32_t value)
{
    int len = 0;

    len = encode_bacnet_unsigned(&apdu[1], value);
    len +=
        encode_tag(&apdu[0], BACNET_APPLICATION_TAG_UINT, false,
        (uint32_t) len);

    return len;
}


/* from clause 20.2.5 Encoding of a Signed Integer Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_application_signed( uint8_t * apdu, int32_t value)
{
    int len = 0;        /* return value */

    /* assumes that the tag only consumes 1 octet */
    len = encode_bacnet_signed(&apdu[1], value);
    len +=
        encode_tag(&apdu[0], BACNET_APPLICATION_TAG_SINT, false,
        (uint32_t) len);

    return len;
}


/* from clause 20.2.6 Encoding of a Real Number Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_application_real(uint8_t * apdu,float value)
{
    int len = 0;

    /* assumes that the tag only consumes 1 octet */
    len = encode_bacnet_real(value, &apdu[1]);
    len +=
        encode_tag(&apdu[0], BACNET_APPLICATION_TAG_REAL, false,
        (uint32_t) len);

    return len;
}



/* from clause 20.2.7 Encoding of a Double Precision Real Number Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_application_double(uint8_t * apdu,double value)
{
    int len = 0;

    /* assumes that the tag only consumes 2 octet */
    len = encode_bacnet_double(value, &apdu[2]);

    len +=
        encode_tag(&apdu[0], BACNET_APPLICATION_TAG_DOUBLE, false,
        (uint32_t) len);

    return len;
}




/* from clause 20.2.8 Encoding of an Octet String Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_application_octet_string( uint8_t * apdu,BACNET_OCTET_STRING * octet_string)
{
    int apdu_len = 0;

    if (octet_string) {
        apdu_len =
            encode_tag(&apdu[0], BACNET_APPLICATION_TAG_OSTRING, false, octet_string->vValue.size());

        /* FIXME: probably need to pass in the length of the APDU
           to bounds check since it might not be the only data chunk */
        if ((apdu_len + octet_string->vValue.size()) < MAX_APDU) {
            apdu_len += encode_octet_string(&apdu[apdu_len], octet_string);
        } else {
            apdu_len = 0;
        }
    }

    return apdu_len;
}


int I_cBACdec::encode_application_octet_string_range( uint8_t * apdu, uint16_t start_pos, uint32_t octet_count, BACNET_OCTET_STRING *octet_string)
{
 int apdu_len = 0;
 if (octet_string)
    {
     apdu_len = encode_tag(&apdu[0], BACNET_APPLICATION_TAG_OSTRING, false, octet_count);
     // napravi novi octet string i napuni ga od start_pos do octet count
     BACNET_OCTET_STRING ostr;
     for (int c=0;c<octet_count;c++)
         {
          ostr.vValue.push_back(octet_string->vValue[start_pos+c]);
         }
     // kodiraj octet string
     apdu_len += encode_octet_string(&apdu[apdu_len], &ostr);
    }
 return apdu_len;
}





/* from clause 20.2.9 Encoding of a Character String Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
//int I_cBACdec::encode_application_character_string( uint8_t * apdu, char *char_string, BACNET_TEXT_ENCODING encoding, int strlen)
//{
//    //int len = 0;
//    //
//    //len = encode_tag(&apdu[0], BACNET_APPLICATION_TAG_CSTRING, false, (uint32_t) strlen+1);   // strlen +1 . '\0'.
//    //
//    //
//    //if ((len + string_len) < MAX_APDU)
//    //   {
//    //    len += encode_bacnet_character_string(&apdu[len], (char_string),encoding);
//    //   } else {
//    //           len = 0;
//    //          }
//
//    //return len+1;    // +1 . '\0'.
//}




// MOJA!!
int I_cBACdec::encode_application_character_string( uint8_t * apdu, BACNET_CHARACTER_STRING *char_str)
{
 int len = 0;
 int i;
 char *pString;

 switch(char_str->encoding)
       {
		case CHAR_SET_UCS2:
		     {
             len = encode_tag(&apdu[0], BACNET_APPLICATION_TAG_CSTRING, false, (uint32_t) char_str->value16.size()*2+1);  // len + tag
             apdu[len++] = char_str->encoding;  // 20.2.9 Encoding of characted string value
             int byte_len = char_str->value16.size();

             for (int c=0;c<byte_len;c++)
                 {
                  wchar_t wtmp = char_str->value16[c];
                  apdu[len++] = wtmp<<8;
                  apdu[len++] = wtmp;
                 }
		     return len;
		    }

        case CHAR_SET_JISC_6226:
		case CHAR_SET_ISO8859:
		case CHAR_SET_ANSI_X34:   // normalni char set
             len = encode_tag(&apdu[0], BACNET_APPLICATION_TAG_CSTRING, false, (uint32_t) char_str->value8.size()+1);  // tag + len
             apdu[len++] = char_str->encoding;  // 20.2.9 Encoding of characted string value
             memcpy(&apdu[len],char_str->value8.c_str(),char_str->value8.size());
             return len + char_str->value8.size();

        default:
        break;
       }


 return len;
}






int I_cBACdec::encode_application_bitstring( uint8_t * apdu, BACNET_BIT_STRING * bit_string)
{
    int len = 0;
    uint32_t bit_string_encoded_length = 1;     /* 1 for the bits remaining octet */

    /* bit string may use more than 1 octet for the tag, so find out how many */
    bit_string_encoded_length += bitstring_bytes_used(bit_string);
    len =
        encode_tag(&apdu[0], BACNET_APPLICATION_TAG_BSTRING, false,bit_string_encoded_length);

    len += encode_bitstring(&apdu[len], bit_string);

    return len;
}


/* from clause 20.2.11 Encoding of an Enumerated Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_application_enumerated( uint8_t * apdu,uint32_t value)
{
    int len = 0;        /* return value */

    /* assumes that the tag only consumes 1 octet */
    len = encode_bacnet_enumerated(&apdu[1], value);
    len +=
        encode_tag(&apdu[0], BACNET_APPLICATION_TAG_ENUM, false,
        (uint32_t) len);

    return len;
}


/* from clause 20.2.14 Encoding of an Object Identifier Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_application_object_id( uint8_t * apdu,int object_type,uint32_t instance)
{
    int len = 0;

    /* assumes that the tag only consumes 1 octet */
    len = encode_bacnet_object_id(&apdu[1], object_type, instance);
    len +=
        encode_tag(&apdu[0], BACNET_APPLICATION_TAG_OID, false,
        (uint32_t) len);

    return len;
}



/* from clause 20.2.5 Encoding of a Signed Integer Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_bacnet_signed(uint8_t * apdu,int32_t value)
{
    int len = 0;        /* return value */

    /* don't encode the leading X'FF' or X'00' of the two's compliment.
       That is, the first octet of any multi-octet encoded value shall
       not be X'00' if the most significant bit (bit 7) of the second
       octet is 0, and the first octet shall not be X'FF' if the most
       significant bit of the second octet is 1. */
    if ((value >= -128) && (value < 128)) {
        len = encode_signed8(&apdu[0], (int8_t) value);
    } else if ((value >= -32768) && (value < 32768)) {
        len = encode_signed16(&apdu[0], (int16_t) value);
    } else if ((value > -8388608) && (value < 8388608)) {
        len = encode_signed24(&apdu[0], value);
    } else {
        len = encode_signed32(&apdu[0], value);
    }

    return len;
}






int I_cBACdec::encode_signed8(uint8_t * apdu,int8_t value)
{
    apdu[0] = (uint8_t) value;

    return 1;
}



int I_cBACdec::encode_signed16( uint8_t * apdu, int16_t value)
{
    apdu[0] = (uint8_t) ((value & 0xff00) >> 8);
    apdu[1] = (uint8_t) (value & 0x00ff);

    return 2;
}



int I_cBACdec::encode_signed24( uint8_t * apdu,int32_t value)
{
    apdu[0] = (uint8_t) ((value & 0xff0000) >> 16);
    apdu[1] = (uint8_t) ((value & 0x00ff00) >> 8);
    apdu[2] = (uint8_t) (value & 0x0000ff);

    return 3;
}



int I_cBACdec::encode_signed32( uint8_t * apdu,int32_t value)
{
    apdu[0] = (uint8_t) ((value & 0xff000000) >> 24);
    apdu[1] = (uint8_t) ((value & 0x00ff0000) >> 16);
    apdu[2] = (uint8_t) ((value & 0x0000ff00) >> 8);
    apdu[3] = (uint8_t) (value & 0x000000ff);

    return 4;
}





/* from clause 20.2.6 Encoding of a Real Number Value */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_bacnet_real(float value, uint8_t * apdu)
{
    union {
        uint8_t byte[4];
        float real_value;
    } my_data;

    /* NOTE: assumes the compiler stores float as IEEE-754 float */
    my_data.real_value = value;
#if BIG_ENDIAN
    apdu[0] = my_data.byte[0];
    apdu[1] = my_data.byte[1];
    apdu[2] = my_data.byte[2];
    apdu[3] = my_data.byte[3];
#else
    apdu[0] = my_data.byte[3];
    apdu[1] = my_data.byte[2];
    apdu[2] = my_data.byte[1];
    apdu[3] = my_data.byte[0];
#endif

    return 4;
}



/* returns the number of bytes that a bit string is using */
uint8_t I_cBACdec::bitstring_bytes_used(  BACNET_BIT_STRING * bit_string)
{
    uint8_t len = 0;    /* return value */
    uint8_t used_bytes = 0;
    uint8_t last_bit = 0;

    if (bit_string->bits_used) {
        last_bit = bit_string->bits_used - 1;
        used_bytes = last_bit / 8;
        /* add one for the first byte */
        used_bytes++;
        len = used_bytes;
    }

    return len;
}



/* from clause 20.2.7 Encoding of a Double Precision Real Number Value */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_bacnet_double( double value, uint8_t * apdu)
{
    union {
        uint8_t byte[8];
        double double_value;
    } my_data;

    /* NOTE: assumes the compiler stores float as IEEE-754 float */
    my_data.double_value = value;
#if BIG_ENDIAN
    apdu[0] = my_data.byte[0];
    apdu[1] = my_data.byte[1];
    apdu[2] = my_data.byte[2];
    apdu[3] = my_data.byte[3];
    apdu[4] = my_data.byte[4];
    apdu[5] = my_data.byte[5];
    apdu[6] = my_data.byte[6];
    apdu[7] = my_data.byte[7];
#else
    apdu[0] = my_data.byte[7];
    apdu[1] = my_data.byte[6];
    apdu[2] = my_data.byte[5];
    apdu[3] = my_data.byte[4];
    apdu[4] = my_data.byte[3];
    apdu[5] = my_data.byte[2];
    apdu[6] = my_data.byte[1];
    apdu[7] = my_data.byte[0];
#endif

    return 8;
}




/* from clause 20.2.10 Encoding of a Bit String Value */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_bitstring(uint8_t * apdu, BACNET_BIT_STRING * bit_string)
{
    int len = 0;
    uint8_t remaining_used_bits = 0;
    uint8_t used_bytes = 0;
    uint8_t i = 0;

    /* if the bit string is empty, then the first octet shall be zero */
    if (bitstring_bits_used(bit_string) == 0) {
        apdu[len++] = 0;
    } else {
        used_bytes = bitstring_bytes_used(bit_string);
        remaining_used_bits =
            (uint8_t) (bitstring_bits_used(bit_string) - ((used_bytes -
                    1) * 8));
        /* number of unused bits in the subsequent final octet */
        apdu[len++] = (uint8_t) (8 - remaining_used_bits);
        for (i = 0; i < used_bytes; i++) {
            apdu[len++] = byte_reverse_bits(bitstring_octet(bit_string, i));
        }
    }

    return len;
}



uint8_t I_cBACdec::bitstring_bits_used( BACNET_BIT_STRING * bit_string)
{
    return bit_string->bits_used;
}




uint8_t I_cBACdec::bitstring_octet(BACNET_BIT_STRING * bit_string,uint8_t octet_index)
{
    uint8_t octet = 0;

    if (bit_string) {
        if (octet_index < MAX_BITSTRING_BYTES) {
            octet = bit_string->value[octet_index];
        }
    }

    return octet;
}



/* from clause 20.2.8 Encoding of an Octet String Value */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_octet_string( uint8_t * apdu,BACNET_OCTET_STRING * octet_string)
{
 int len = 0;
 for (uint32_t i = 0; i < octet_string->vValue.size(); i++)
     {
      apdu[len++] = octet_string->vValue[i];
     }
 return len;
}





/* from clause 20.2.1.3.2 Constructed Data */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_opening_tag(uint8_t * apdu,uint8_t tag_number)
{
    int len = 1;

    /* set class field to context specific */
    apdu[0] = SET_BIT(3);
    /* additional tag byte after this byte for extended tag byte */
    if (tag_number <= 14) {
        apdu[0] |= (tag_number << 4);
    } else {
        apdu[0] |= 0xF0;
        apdu[1] = tag_number;
        len++;
    }
    /* set type field to opening tag */
    apdu[0] |= 6;

    return len;
}




/* from clause 20.2.1.3.2 Constructed Data */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_closing_tag( uint8_t * apdu,uint8_t tag_number)
{
    int len = 1;

    /* set class field to context specific */
    apdu[0] = SET_BIT(3);
    /* additional tag byte after this byte for extended tag byte */
    if (tag_number <= 14) {
        apdu[0] |= (tag_number << 4);
    } else {
        apdu[0] |= 0xF0;
        apdu[1] = tag_number;
        len++;
    }
    /* set type field to closing tag */
    apdu[0] |= 7;

    return len;
}



/* from clause 20.2.1.3.2 Constructed Data */
/* returns the number of apdu bytes consumed */
bool I_cBACdec::decode_is_closing_tag_number(uint8_t * apdu, uint8_t tag_number)
{
    uint8_t my_tag_number = 0;
    bool closing_tag = false;

    closing_tag = (bool) ((apdu[0] & 0x07) == 7);
    decode_tag_number(apdu, &my_tag_number);

    return (bool) (closing_tag && (my_tag_number == tag_number));
}




int I_cBACdec::bacapp_decode_context_data( uint8_t * apdu,unsigned max_apdu_len,BACNET_APPLICATION_DATA_VALUE * value, BACNET_PROPERTY_ID property)
{
    int apdu_len = 0, len = 0;
    int tag_len = 0;
    uint8_t tag_number = 0;
    uint32_t len_value_type = 0;

    /* FIXME: use max_apdu_len! */
    max_apdu_len = max_apdu_len;
    if (apdu && value && IS_CONTEXT_SPECIFIC(*apdu)) {
        value->context_specific = true;
        tag_len =
            decode_tag_number_and_value(&apdu[0], &tag_number, &len_value_type);

        if (tag_len) {
            apdu_len = tag_len;
            value->context_tag = tag_number;
            value->application_tag = bacapp_context_tag_type(property, tag_number);
            if (value->application_tag < MAX_BACNET_APPLICATION_TAG) {
                len =
                    bacapp_decode_data(&apdu[apdu_len], value->application_tag,
                    len_value_type, value);
                apdu_len += len;
            } else {
                /* FIXME: what now? */
                apdu_len = 0;
            }
        }
        //value->next = NULL;
    }

    return apdu_len;
}




/* returns the fixed tag type for certain context tagged properties */
BACNET_APPLICATION_TAG I_cBACdec::bacapp_context_tag_type(BACNET_PROPERTY_ID property,uint8_t tag_number)
{
    BACNET_APPLICATION_TAG tag = MAX_BACNET_APPLICATION_TAG;

    switch (property) {
        case PROP_ACTUAL_SHED_LEVEL:
        case PROP_REQUESTED_SHED_LEVEL:
        case PROP_EXPECTED_SHED_LEVEL:
            switch (tag_number) {
                case 0:
                case 1:
                    tag = BACNET_APPLICATION_TAG_UINT;
                    break;
                case 2:
                    tag = BACNET_APPLICATION_TAG_REAL;
                    break;
                default:
                    break;
            }
            break;
        case PROP_ACTION:
            switch (tag_number) {
                case 0:
                case 1:
                    tag = BACNET_APPLICATION_TAG_OID;
                    break;
                case 2:
                    tag = BACNET_APPLICATION_TAG_ENUM;
                    break;
                case 3:
                case 5:
                case 6:
                    tag = BACNET_APPLICATION_TAG_UINT;
                    break;
                case 7:
                case 8:
                    tag = BACNET_APPLICATION_TAG_BOOL;
                    break;
                case 4:        /* propertyValue: abstract syntax */
                default:
                    break;
            }
            break;
        case PROP_LIST_OF_GROUP_MEMBERS:
            switch (tag_number) {
                case 0:
                    tag = BACNET_APPLICATION_TAG_OID;
                    break;
                default:
                    break;
            }
            break;
        case PROP_EXCEPTION_SCHEDULE:
            switch (tag_number) {
                case 1:
                    tag = BACNET_APPLICATION_TAG_OID;
                    break;
                case 3:
                    tag = BACNET_APPLICATION_TAG_UINT;
                    break;
                case 0:        /* calendarEntry: abstract syntax + context */
                case 2:        /* list of BACnetTimeValue: abstract syntax */
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return tag;
}

//
//
//bool I_cBACdec::bacapp_print_value(FILE * stream, BACNET_APPLICATION_DATA_VALUE * value, BACNET_PROPERTY_ID property)
//{
//    bool status = true; /*return value */
//    size_t len = 0, i = 0;
//    char *char_str;
//    uint8_t *octet_str;
//
//    if (value) {
//        switch (value->tag) {
//            case BACNET_APPLICATION_TAG_NULL:
//                fprintf(stream, "Null");
//                break;
//            case BACNET_APPLICATION_TAG_BOOL:
//                fprintf(stream, "%s", value->type.Boolean ? "TRUE" : "FALSE");
//                break;
//            case BACNET_APPLICATION_TAG_UINT:
//                fprintf(stream, "%u", value->type.Unsigned_Int);
//                break;
//            case BACNET_APPLICATION_TAG_SINT:
//                fprintf(stream, "%d", value->type.Signed_Int);
//                break;
//            case BACNET_APPLICATION_TAG_REAL:
//                fprintf(stream, "%f", (double) value->type.Real);
//                break;
//#if defined (BACAPP_DOUBLE)
//            case BACNET_APPLICATION_TAG_DOUBLE:
//                fprintf(stream, "%f", value->type.Double);
//                break;
//#endif
//            case BACNET_APPLICATION_TAG_OSTRING:
//                len = octetstring_length(&value->type.Octet_String);
//                octet_str = octetstring_value(&value->type.Octet_String);
//                for (i = 0; i < len; i++) {
//                    fprintf(stream, "%02X", *octet_str);
//                    octet_str++;
//                }
//                break;
//            case BACNET_APPLICATION_TAG_CSTRING:
//                len = characterstring_length(&value->type.Character_String);
//                char_str =
//                    characterstring_value(&value->type.Character_String);
//                fprintf(stream, "\"");
//                for (i = 0; i < len; i++) {
//                    if (isprint(*((unsigned char *) char_str))) {
//                        fprintf(stream, "%c", *char_str);
//                    } else {
//                        fprintf(stream, ".");
//                    }
//                    char_str++;
//                }
//                fprintf(stream, "\"");
//                break;
//            case BACNET_APPLICATION_TAG_BSTRING:
//                len = bitstring_bits_used(&value->type.Bit_String);
//                fprintf(stream, "{");
//                for (i = 0; i < len; i++) {
//                    fprintf(stream, "%s",
//                        bitstring_bit(&value->type.Bit_String,
//                            (uint8_t) i) ? "true" : "false");
//                    if (i < len - 1)
//                        fprintf(stream, ",");
//                }
//                fprintf(stream, "}");
//                break;
//            case BACNET_APPLICATION_TAG_ENUM:
//                switch (property) {
//                    case PROP_OBJECT_TYPE:
//                        if (value->type.Enumerated < MAX_ASHRAE_OBJECT_COUNT) {
//                            fprintf(stream, "%s",
//                                bactext_object_type_name(value->type.
//                                    Enumerated));
//                        } else if (value->type.Enumerated < 128) {
//                            fprintf(stream, "reserved %u",
//                                value->type.Enumerated);
//                        } else {
//                            fprintf(stream, "proprietary %u",
//                                value->type.Enumerated);
//                        }
//                        break;
//                    case PROP_EVENT_STATE:
//                        fprintf(stream, "%s",
//                            bactext_event_state_name(value->type.Enumerated));
//                        break;
//                    case PROP_UNITS:
//                        if (value->type.Enumerated < 256) {
//                            fprintf(stream, "%s",
//                                bactext_engineering_unit_name(value->
//                                    type.Enumerated));
//                        } else {
//                            fprintf(stream, "proprietary %u",
//                                value->type.Enumerated);
//                        }
//                        break;
//                    case PROP_PRESENT_VALUE:
//                        fprintf(stream, "%s",
//                            bactext_binary_present_value_name(value->
//                                type.Enumerated));
//                        break;
//                    case PROP_RELIABILITY:
//                        fprintf(stream, "%s",
//                            bactext_reliability_name(value->type.Enumerated));
//                        break;
//                    case PROP_SYSTEM_STATUS:
//                        fprintf(stream, "%s",
//                            bactext_device_status_name(value->
//                                type.Enumerated));
//                        break;
//                    case PROP_SEGMENTATION_SUPPORTED:
//                        fprintf(stream, "%s",
//                            bactext_segmentation_name(value->type.Enumerated));
//                        break;
//                    default:
//                        fprintf(stream, "%u", value->type.Enumerated);
//                        break;
//                }
//                break;
//            case BACNET_APPLICATION_TAG_DATE:
//                fprintf(stream, "%s, %s ",
//                    bactext_day_of_week_name(value->type.Date.wday),
//                    bactext_month_name(value->type.Date.month));
//                if (value->type.Date.day == 255) {
//                    fprintf(stream, "(unspecified), ");
//                } else {
//                    fprintf(stream, "%u, ", (unsigned) value->type.Date.day);
//                }
//                if (value->type.Date.year == 255) {
//                    fprintf(stream, "(unspecified), ");
//                } else {
//                    fprintf(stream, "%u", (unsigned) value->type.Date.year);
//                }
//                break;
//            case BACNET_APPLICATION_TAG_TIME:
//                if (value->type.Time.hour == 255) {
//                    fprintf(stream, "**:");
//                } else {
//                    fprintf(stream, "%02u:", (unsigned) value->type.Time.hour);
//                }
//                if (value->type.Time.min == 255) {
//                    fprintf(stream, "**:");
//                } else {
//                    fprintf(stream, "%02u:", (unsigned) value->type.Time.min);
//                }
//                if (value->type.Time.sec == 255) {
//                    fprintf(stream, "**.");
//                } else {
//                    fprintf(stream, "%02u.", (unsigned) value->type.Time.sec);
//                }
//                if (value->type.Time.hundredths == 255) {
//                    fprintf(stream, "**");
//                } else {
//                    fprintf(stream, "%02u",
//                        (unsigned) value->type.Time.hundredths);
//                }
//                break;
//            case BACNET_APPLICATION_TAG_OID:
//                if (value->type.Object_Id.type < MAX_ASHRAE_OBJECT_COUNT) {
//                    fprintf(stream, "(%s, %u)",
//                        bactext_object_type_name(value->type.Object_Id.type),
//                        value->type.Object_Id.instance);
//                } else if (value->type.Object_Id.type < 128) {
//                    fprintf(stream, "(reserved %u, %u)",
//                        value->type.Object_Id.type,
//                        value->type.Object_Id.instance);
//                } else {
//                    fprintf(stream, "(proprietary %u, %u)",
//                        value->type.Object_Id.type,
//                        value->type.Object_Id.instance);
//                }
//                break;
//            default:
//                status = false;
//                break;
//        }
//    }
//
//    return status;
//}




bool I_cBACdec::bitstring_bit( BACNET_BIT_STRING * bit_string,uint8_t bit_number)
{
    bool value = false;
    uint8_t byte_number = bit_number / 8;
    uint8_t bit_mask = 1;

    if (bit_number < (MAX_BITSTRING_BYTES * 8))
       {
        bit_mask = bit_mask << (bit_number - (byte_number * 8));
        if (bit_string->value[byte_number] & bit_mask)
           {
            value = true;
           }
       }

    return value;
}








/* returns the length of data between an opening tag and a closing tag.
   Expects that the first octet contain the opening tag.
   Include a value property identifier for context specific data
   such as the value received in a WriteProperty request */
int I_cBACdec::bacapp_data_len(uint8_t * apdu, unsigned max_apdu_len,BACNET_PROPERTY_ID property)
{
    int len = 0;
    int total_len = 0;
    int apdu_len = 0;
    uint8_t tag_number = 0;
    uint8_t opening_tag_number = 0;
    uint8_t opening_tag_number_counter = 0;
    uint32_t value = 0;
    BACNET_APPLICATION_DATA_VALUE application_value;

    if (decode_is_opening_tag(&apdu[0]))
       {
        len = decode_tag_number_and_value(&apdu[apdu_len], &tag_number, &value);

        apdu_len += len;
        opening_tag_number = tag_number;
        opening_tag_number_counter = 1;

        while (opening_tag_number_counter)   // zatvara se kada naidje na closing tag number
              {
               if (decode_is_opening_tag(&apdu[apdu_len]))
                  {
                   len = decode_tag_number_and_value(&apdu[apdu_len], &tag_number, &value);


                   if (tag_number == opening_tag_number)
                      {
                       opening_tag_number_counter++;
                      }
                 } else if (decode_is_closing_tag(&apdu[apdu_len]))
                           {
                            len = decode_tag_number_and_value(&apdu[apdu_len], &tag_number,  &value);


                           if (tag_number == opening_tag_number)
                              {
                               opening_tag_number_counter--;
                              }
                 } else if (IS_CONTEXT_SPECIFIC(apdu[apdu_len]))
                           {
                            /* context-specific tagged data */
                            len = bacapp_decode_context_data(&apdu[apdu_len],max_apdu_len - apdu_len, &application_value, property);
                 } else {
                         /* application tagged data */
                         len = bacapp_decode_application_data(&apdu[apdu_len], max_apdu_len - apdu_len, &application_value);
                        }

               apdu_len += len;

               if (opening_tag_number_counter)
                  {
                   if (len > 0)
                      {
                       total_len += len;
                      } else {
                              /* error: len is not incrementing */
                              total_len = -1;
                              break;
                             }
                 }

               if ((unsigned) apdu_len > max_apdu_len)
                  {
                   /* error: exceeding our buffer limit */
                   total_len = -1;
                   break;
                  }
              }// END WHILE
    }// END IF decode_is_opening_tag()

 return total_len;
}



/* from clause 20.1.2.4 max-segments-accepted */
/* and clause 20.1.2.5 max-APDU-length-accepted */
/* returns the encoded octet */
int I_cBACdec::decode_max_segs(uint8_t octet)
{
    int max_segs = 0;

    switch (octet & 0xF0) {
        case 0:
            max_segs = 0;
            break;
        case 0x10:
            max_segs = 2;
            break;
        case 0x20:
            max_segs = 4;
            break;
        case 0x30:
            max_segs = 8;
            break;
        case 0x40:
            max_segs = 16;
            break;
        case 0x50:
            max_segs = 32;
            break;
        case 0x60:
            max_segs = 64;
            break;
        case 0x70:
            max_segs = 65;
            break;
        default:
            break;
    }

    return max_segs;
}



int I_cBACdec::decode_max_apdu( uint8_t octet)
{
    int max_apdu = 0;

    switch (octet & 0x0F) {
        case 0:
            max_apdu = 50;
            break;
        case 1:
            max_apdu = 128;
            break;
        case 2:
            max_apdu = 206;
            break;
        case 3:
            max_apdu = 480;
            break;
        case 4:
            max_apdu = 1024;
            break;
        case 5:
            max_apdu = 1476;
            break;
        default:
            break;
    }

    return max_apdu;
}


/* decode the object portion of the service request only */
int I_cBACdec::rpm_decode_object_id(uint8_t * apdu,unsigned apdu_len,BACNET_OBJECT_TYPE * object_type,uint32_t * object_instance)
{
    unsigned len = 0;
    uint16_t type = 0;  /* for decoding */

    /* check for value pointers */
    if (apdu && apdu_len && object_type && object_instance) {
        /* Tag 0: Object ID */
        if (!decode_is_context_tag(&apdu[len++], 0))
            return -1;
        len += decode_object_id(&apdu[len], &type, object_instance);
        if (object_type)
            *object_type = (BACNET_OBJECT_TYPE) type;
        /* Tag 1: sequence of ReadAccessSpecification */
        if (!decode_is_opening_tag_number(&apdu[len], 1))
            return -1;
        len++;  /* opening tag is only one octet */
    }

    return (int) len;
}




bool I_cBACdec::decode_context_boolean(uint8_t * apdu)
{
    bool boolean_value = false;

    if (apdu[0]) {
        boolean_value = true;
    }

    return boolean_value;
}





/* decode the application class and code */
int I_cBACdec::bacerror_create_object_decode_error_class_and_code( uint8_t * apdu,unsigned apdu_len,BACNET_ERROR_CLASS * error_class,BACNET_ERROR_CODE * error_code)
{
    int len = 0;
    uint8_t tag_number = 0;
    uint32_t len_value_type = 0;
    uint32_t decoded_value = 0;

    if (apdu_len) {
        if (decode_is_opening_tag_number(&apdu[len],0))
           {
            len++;  // opening

			/* error class */
			len += decode_tag_number_and_value(&apdu[len], &tag_number,&len_value_type);


			if (tag_number != BACNET_APPLICATION_TAG_ENUM)
				return 0;
			len += decode_enumerated(&apdu[len], len_value_type, &decoded_value);
			if (error_class)
				*error_class = (BACNET_ERROR_CLASS) decoded_value;
			/* error code */
			len +=
				decode_tag_number_and_value(&apdu[len], &tag_number,
				&len_value_type);
			if (tag_number != BACNET_APPLICATION_TAG_ENUM)
				return 0;
			len += decode_enumerated(&apdu[len], len_value_type, &decoded_value);
			if (error_code)
				*error_code = (BACNET_ERROR_CODE) decoded_value;

			len++;  // preskoci closing

			len+=2; // first failed element number (uint16_t)
		   }else {
		          return -1;
		         }

    }

    return len;
}





/* decode the application class and code */
int I_cBACdec::bacerror_decode_error_class_and_code( uint8_t * apdu,unsigned apdu_len,BACNET_ERROR_CLASS * error_class,BACNET_ERROR_CODE * error_code)
{
    int len = 0;
    uint8_t tag_number = 0;
    uint32_t len_value_type = 0;
    uint32_t decoded_value = 0;

    if (apdu_len) {



        /* error class */
        len += decode_tag_number_and_value(&apdu[len], &tag_number,&len_value_type);


        if (tag_number != BACNET_APPLICATION_TAG_ENUM)
            return 0;
        len += decode_enumerated(&apdu[len], len_value_type, &decoded_value);
        if (error_class)
            *error_class = (BACNET_ERROR_CLASS) decoded_value;
        /* error code */
        len +=
            decode_tag_number_and_value(&apdu[len], &tag_number,
            &len_value_type);
        if (tag_number != BACNET_APPLICATION_TAG_ENUM)
            return 0;
        len += decode_enumerated(&apdu[len], len_value_type, &decoded_value);
        if (error_code)
            *error_code = (BACNET_ERROR_CODE) decoded_value;
    }

    return len;
}





/* from clause 20.2.12 Encoding of a Date Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_application_date( uint8_t * apdu, BACNET_DATE * bdate)
{
    int len = 0;

    /* assumes that the tag only consumes 1 octet */
    len = encode_bacnet_date(&apdu[1], bdate);
    len +=
        encode_tag(&apdu[0], BACNET_APPLICATION_TAG_DATE, false,
        (uint32_t) len);

    return len;

}




/* from clause 20.2.13 Encoding of a Time Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_application_time(uint8_t * apdu,BACNET_TIME * btime)
{
    int len = 0;

    /* assumes that the tag only consumes 1 octet */
    len = encode_bacnet_time(&apdu[1], btime);
    len +=
        encode_tag(&apdu[0], BACNET_APPLICATION_TAG_TIME, false,
        (uint32_t) len);

    return len;
}




/* from clause 20.2.13 Encoding of a Time Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_bacnet_time(uint8_t * apdu, BACNET_TIME * btime)
{
 apdu[0] = btime->hour;
 apdu[1] = btime->minute;
 apdu[2] = btime->sec;
 apdu[3] = btime->hundredths;

 return 4;
}




/* BACnet Date */
/* year = years since 1900 */
/* month 1=Jan */
/* day = day of month */
/* wday 1=Monday...7=Sunday */

/* from clause 20.2.12 Encoding of a Date Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::encode_bacnet_date( uint8_t * apdu,BACNET_DATE * bdate)
{
    /* allow 2 digit years */
    if (bdate->year >= 1900) {
        apdu[0] = (uint8_t) (bdate->year - 1900);
    } else if (bdate->year < 0x100) {
        apdu[0] = (uint8_t) bdate->year;

    } else {
        /*
         ** Don't try and guess what the user meant here. Just fail
         */
        return -1;
    }


    apdu[1] = bdate->month;
    apdu[2] = bdate->day;
    apdu[3] = bdate->wday;

    return 4;
}


int I_cBACdec::decode_date(uint8_t * apdu, BACNET_DATE * bdate)
{
    bdate->year = (uint16_t) (apdu[0] + 1900);
    bdate->month = apdu[1];
    bdate->day = apdu[2];
    bdate->wday = apdu[3];

    return 4;
}




/* from clause 20.2.13 Encoding of a Time Value */
/* and 20.2.1 General Rules for Encoding BACnet Tags */
/* returns the number of apdu bytes consumed */
int I_cBACdec::decode_bacnet_time( uint8_t * apdu,BACNET_TIME * btime)
{
    btime->hour = apdu[0];
    btime->minute = apdu[1];
    btime->sec = apdu[2];
    btime->hundredths = apdu[3];

    return 4;
}



int I_cBACdec::decode_context_bitstring( uint8_t * apdu, uint8_t tag_number, BACNET_BIT_STRING * bit_string)
{
    uint32_t len_value;
    int len = 0;

    if (decode_is_context_tag(&apdu[len], tag_number)) {
        len +=
            decode_tag_number_and_value(&apdu[len], &tag_number, &len_value);
        len += decode_bitstring(&apdu[len], len_value, bit_string);
    } else {
        len = -1;
    }
    return len;
}




int I_cBACdec::bacapp_decode_context_property_state( uint8_t * apdu, uint8_t tag_number,BACNET_PROPERTY_STATE * value)
{
    int len = 0;
    int section_length;

    if (decode_is_opening_tag_number(&apdu[len], tag_number)) {
        len++;
        section_length = bacapp_decode_property_state(&apdu[len], value);

        if (section_length == -1) {
            len = -1;
        } else {
            len += section_length;
            if (decode_is_closing_tag_number(&apdu[len], tag_number)) {
                len++;
            } else {
                len = -1;
            }
        }
    } else {
        len = -1;
    }
    return len;
}




int I_cBACdec::bacapp_decode_property_state( uint8_t * apdu,BACNET_PROPERTY_STATE * value)
{
    int len = 0;
    uint32_t len_value_type;
    int section_length;
    uint32_t enumValue;
    uint8_t tag = 0;

    section_length =decode_tag_number_and_value(&apdu[len], &tag,&len_value_type);
    value->tag = (BACNET_PROPERTY_STATE_TYPE)tag;

    if (-1 == section_length) {
        return -1;
    }

    len += section_length;
    switch (value->tag) {
        case BOOLEAN_VALUE:
            value->state.booleanValue = decode_boolean(len_value_type);
            break;

        case BINARY_VALUE:
            if (-1 == (section_length =
                    decode_enumerated(&apdu[len], len_value_type,
                        &enumValue))) {
                return -1;
            }
            value->state.binaryValue = (BACNET_BINARY_PV) enumValue;
            break;

        case EVENT_TYPE:
            if (-1 == (section_length =
                    decode_enumerated(&apdu[len], len_value_type,
                        &enumValue))) {
                return -1;
            }
            value->state.eventType = (BACNET_EVENT_TYPE) enumValue;
            break;

        case POLARITY:
            if (-1 == (section_length =
                    decode_enumerated(&apdu[len], len_value_type,
                        &enumValue))) {
                return -1;
            }
            value->state.polarity = (BACNET_POLARITY) enumValue;
            break;

        case PROGRAM_CHANGE:
            if (-1 == (section_length =
                    decode_enumerated(&apdu[len], len_value_type,
                        &enumValue))) {
                return -1;
            }
            value->state.programChange = (BACNET_PROGRAM_REQUEST) enumValue;
            break;

        case PROGRAM_STATE:
            if (-1 == (section_length =
                    decode_enumerated(&apdu[len], len_value_type,
                        &enumValue))) {
                return -1;
            }
            value->state.programState = (BACNET_PROGRAM_STATE) enumValue;
            break;

        case REASON_FOR_HALT:
            if (-1 == (section_length =
                    decode_enumerated(&apdu[len], len_value_type,
                        &enumValue))) {
                return -1;
            }
            value->state.programError = (BACNET_PROGRAM_ERROR) enumValue;
            break;

        case RELIABILITY:
            if (-1 == (section_length =
                    decode_enumerated(&apdu[len], len_value_type,
                        &enumValue))) {
                return -1;
            }
            value->state.reliability = (BACNET_RELIABILITY) enumValue;
            break;

        case STATE:
            if (-1 == (section_length =
                    decode_enumerated(&apdu[len], len_value_type,
                        &enumValue))) {
                return -1;
            }
            value->state.state = (BACNET_EVENT_STATE) enumValue;
            break;

        case SYSTEM_STATUS:
            if (-1 == (section_length =
                    decode_enumerated(&apdu[len], len_value_type,
                        &enumValue))) {
                return -1;
            }
            value->state.systemStatus = (BACNET_DEVICE_STATUS) enumValue;
            break;

        case UNITS:
            if (-1 == (section_length =
                    decode_enumerated(&apdu[len], len_value_type,
                        &enumValue))) {
                return -1;
            }
            value->state.units = (BACNET_ENGINEERING_UNITS) enumValue;
            break;

        case UNSIGNED_VALUE:
            if (-1 == (section_length =
                    decode_unsigned(&apdu[len], len_value_type,
                        &value->state.unsignedValue))) {
                return -1;
            }
            break;

        case LIFE_SAFETY_MODE:
            if (-1 == (section_length =
                    decode_enumerated(&apdu[len], len_value_type,
                        &enumValue))) {
                return -1;
            }
            value->state.lifeSafetyMode = (BACNET_LIFE_SAFETY_MODE) enumValue;
            break;

        case LIFE_SAFETY_STATE:
            if (-1 == (section_length =
                    decode_enumerated(&apdu[len], len_value_type,
                        &enumValue))) {
                return -1;
            }
            value->state.lifeSafetyState =
                (BACNET_LIFE_SAFETY_STATE) enumValue;
            break;

        default:
            return -1;
    }
    len += section_length;

    return len;
}



int I_cBACdec::decode_context_real( uint8_t * apdu,uint8_t tag_number,float *real_value)
{
    uint32_t len_value;
    int len = 0;

    if (decode_is_context_tag(&apdu[len], tag_number)) {
        len +=
            decode_tag_number_and_value(&apdu[len], &tag_number, &len_value);
        len += decode_real_safe(&apdu[len], len_value, real_value);
    } else {
        len = -1;
    }
    float f = *real_value;
    return len;
}




int I_cBACdec::bacapp_decode_context_device_obj_property_ref( uint8_t * apdu, uint8_t tag_number,BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE * value)
{
    int len = 0;
    int section_length;

    if (decode_is_opening_tag_number(&apdu[len], tag_number)) {
        len++;
        section_length =
            bacapp_decode_device_obj_property_ref(&apdu[len], value);

        if (section_length == -1) {
            len = -1;
        } else {
            len += section_length;
            if (decode_is_closing_tag_number(&apdu[len], tag_number)) {
                len++;
            } else {
                len = -1;
            }
        }
    } else {
        len = -1;
    }
    return len;
}




int I_cBACdec::decode_context_enumerated(uint8_t * apdu, uint8_t tag_value,uint32_t * value)
{
    int len = 0;
    uint8_t tag_number;
    uint32_t len_value;

    if (decode_is_context_tag(&apdu[len], tag_value)) {
        len +=
            decode_tag_number_and_value(&apdu[len], &tag_number, &len_value);
        len += decode_enumerated(&apdu[len], len_value, value);
    } else {
        len = -1;
    }
    return len;
}





int I_cBACdec::decode_context_unsigned(uint8_t * apdu,uint8_t tag_number,uint32_t * value)
{
    uint32_t len_value;
    int len = 0;

    if (decode_is_context_tag(&apdu[len], tag_number)) {
        len +=
            decode_tag_number_and_value(&apdu[len], &tag_number, &len_value);
        len += decode_unsigned(&apdu[len], len_value, value);
    } else {
        len = -1;
    }
    return len;
}


int I_cBACdec::decode_context_signed( uint8_t * apdu,uint8_t tag_number, int32_t *value)
{
    uint32_t len_value;
    int len = 0;

    if (decode_is_context_tag(&apdu[len], tag_number))
       {
        len +=decode_tag_number_and_value(&apdu[len], &tag_number, &len_value);
        len += decode_signed(&apdu[len], len_value, value);
       } else {
               len = -1;
              }
    return len;
}



int I_cBACdec::encode_context_date( uint8_t * apdu, uint8_t tag_number,BACNET_DATE * bdate)
{
    int len = 0;        /* return value */

    /* length of date is 4 octets, as per 20.2.12 */
    len = encode_tag(&apdu[0], tag_number, true, 4);
    len += encode_bacnet_date(&apdu[len], bdate);

    return len;
}


int I_cBACdec::encode_context_time(uint8_t * apdu, uint8_t tag_number, BACNET_TIME * btime)
{
    int len = 0;        /* return value */

    /* length of time is 4 octets, as per 20.2.13 */
    len = encode_tag(&apdu[0], tag_number, true, 4);
    len += encode_bacnet_time(&apdu[len], btime);

    return len;
}



int I_cBACdec::bacapp_encode_device_obj_property_ref(uint8_t * apdu,BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE * value)
{
 int len = 0;

 if (value)
    {
     len += encode_context_object_id(&apdu[len],0,value->objectIdentifier.type,value->objectIdentifier.instance);
     len += encode_context_enumerated(&apdu[len],1,value->propertyIdentifier);
     if (value->arrayIndex != -1)
        {
         len += encode_context_unsigned(&apdu[len],2,value->arrayIndex);
        }
     len += encode_context_object_id(&apdu[len],3,value->deviceIndentifier.type,value->deviceIndentifier.instance);
    }

 return len;
}


int I_cBACdec::bacapp_decode_device_obj_property_ref(uint8_t * apdu,BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE * value)
{
    int len = 0;
    int apdu_len = 0;
    uint32_t enumValue;
    int ret = 0;

    ret = decode_context_object_id(&apdu[len], 0, &value->objectIdentifier.type,&value->objectIdentifier.instance);
    if (ret < 0)
        return -1;

    len += ret;

    ret = decode_context_enumerated(&apdu[len], 1, &enumValue);
    if (ret < 0)
        return -1;

    value->propertyIdentifier = (BACNET_PROPERTY_ID) enumValue;
    len += ret;

    if (decode_is_context_tag(&apdu[len], 2))
       {
        ret = decode_context_unsigned(&apdu[len], 2,&value->arrayIndex);
        if (ret < 0)
            return -1;

        len += ret;
       }else value->arrayIndex = 0;

    uint8_t *tmp = &apdu[len];

    if (this->decode_is_closing_tag(&apdu[len]))
       {
        value->deviceIndentifier.instance = 0;
        value->deviceIndentifier.type = 0;
        return len;
       } else {
			   if (decode_is_context_tag(&apdu[len], 3))
				  {
				   ret = decode_context_object_id(&apdu[len], 3,  &value->deviceIndentifier.type, &value->deviceIndentifier.instance);
				   if (ret < 0)
					   return -1;

				   len += ret;
				  } else {
						  value->deviceIndentifier.instance = 0;
						  value->deviceIndentifier.type = 0;
						 }
			  }
    return len;
}



int I_cBACdec::decode_context_object_id( uint8_t * apdu,uint8_t tag_number,uint16_t * object_type,uint32_t * instance)
{
 int len = 0;

 if (decode_is_context_tag_with_length(&apdu[len], tag_number, &len))
    {
     len += decode_object_id(&apdu[len], object_type, instance);
    } else {
            len = -1;
           }
 return len;
}




int I_cBACdec::decode_context_character_string(uint8_t * apdu,uint8_t tag_number, BACNET_CHARACTER_STRING *txt)
{
 int len = 0;
 uint32_t len_value= 0;
 uint8_t tag = 0;

 len += decode_tag_number_and_value(&apdu[len],&tag,&len_value);
 if (tag != tag_number)
    {
     return -1;
    }

 int encoding = apdu[len++];
 txt->encoding = (BACNET_TEXT_ENCODING)encoding;

 switch (txt->encoding)
		{
		 case CHAR_SET_UCS2:
		 {
		  len = decode_standard_character_string(&apdu[len], len_value,  &txt->value16);
		  //int ln = BigToLittleEndian((char*)txt->value16.c_str(),len);
		  //txt->value16[ln] = 0;
		  //txt->length = len+1;
		  break;
		 }
		 case CHAR_SET_ISO8859:
		 case CHAR_SET_ANSI_X34:
		 case CHAR_SET_JISC_6226:
		 {
		  //value->type.character_String.value8 = new char[len_value_type];
		  //len = decode_standard_character_string(&apdu[len], len_value,  &txt->value8);
		  len = decode_standard_character_string(&apdu[len], len_value,  txt->value8);
		  //txt->length = len;
		  break;
		 }
		 default:
		 return -1;  // ne podrzavamp ostale za sada
		 break;
		}


 return len;
}





bool I_cBACdec::decode_is_context_tag_with_length(uint8_t * apdu,uint8_t tag_number,int *tag_length)
{
    uint8_t my_tag_number = 0;

    *tag_length = decode_tag_number(apdu, &my_tag_number);

    return (bool) (IS_CONTEXT_SPECIFIC(*apdu) &&
        (my_tag_number == tag_number));
}





int I_cBACdec::decode_context_bacnet_time(uint8_t * apdu,uint8_t tag_number,BACNET_TIME * btime)
{
    int len = 0;

    if (decode_is_context_tag_with_length(&apdu[len], tag_number, &len)) {
        len += decode_bacnet_time(&apdu[len], btime);
    } else {
        len = -1;
    }
    return len;
}



int I_cBACdec::bacapp_encode_device_obj_ref(uint8_t * apdu, BACNET_DEVICE_OBJECT_REFERENCE * value)
{
 int len = 0;

 len += encode_context_object_id(&apdu[len],0,value->deviceIndentifier.type,value->deviceIndentifier.instance);
 len += encode_context_object_id(&apdu[len],1,value->objectIdentifier.type,value->objectIdentifier.instance);

 return len;
}


/* simple decoding of a BACnetDeviceObjectReference */
int I_cBACdec::bacapp_decode_device_obj_ref(uint8_t * apdu, BACNET_DEVICE_OBJECT_REFERENCE * value)
{
    int len;
    int apdu_len = 0;

    /* [0] deviceIdentifier OPTIONAL */
    if (decode_is_context_tag(&apdu[apdu_len], 0) &&
        !decode_is_closing_tag(&apdu[apdu_len])) {
        if (-1 == (len =
                decode_context_object_id(&apdu[apdu_len], 0,
                    &value->deviceIndentifier.type,
                    &value->deviceIndentifier.instance))) {
            return -1;
        }
        apdu_len += len;
    } else {
        value->deviceIndentifier.type = 0;
        value->deviceIndentifier.instance = 0;
    }

    /* [1] objectIdentifier */
    if (-1 == (len =
            decode_context_object_id(&apdu[apdu_len], 1,
                &value->objectIdentifier.type,
                &value->objectIdentifier.instance))) {
        return -1;
    }
    apdu_len += len;
    return apdu_len;
}




/* decode a simple BACnetDateTime value */
int I_cBACdec::decode_application_datetime( uint8_t * apdu,BACNET_DATE_TIME * datetime)
{
    int len = 0;
    int apdu_len = 0;

    len = decode_application_date(&apdu[apdu_len], &datetime->date);
    if (len < 0)
        return len;
    apdu_len += len;
    len = decode_application_time(&apdu[apdu_len], &datetime->time);
    if (len < 0)
        return len;
    apdu_len += len;
    return apdu_len;
}


int I_cBACdec::encode_application_datetime(uint8_t * apdu,BACNET_DATE_TIME * datetime)
{
 int len = 0;

 len += encode_application_date(&apdu[len], &datetime->date);
 len += encode_application_time(&apdu[len], &datetime->time);

 return len;
}


int I_cBACdec::decode_application_date(uint8_t * apdu, BACNET_DATE * bdate)
{
    int len = 0;
    uint8_t tag_number;
    decode_tag_number(&apdu[len], &tag_number);

    if (tag_number == BACNET_APPLICATION_TAG_DATE) {
        len++;
        len += decode_date(&apdu[len], bdate);
    } else {
        len = -1;
    }
    return len;
}



int I_cBACdec::decode_application_time(uint8_t * apdu, BACNET_TIME * btime)
{
    int len = 0;
    uint8_t tag_number;
    decode_tag_number(&apdu[len], &tag_number);

    if (tag_number == BACNET_APPLICATION_TAG_TIME) {
        len++;
        len += decode_bacnet_time(&apdu[len], btime);
    } else {
        len = -1;
    }
    return len;
}



int I_cBACdec::bacapp_encode_obj_property_ref( uint8_t * apdu,BACNET_OBJECT_PROPERTY_REFERENCE * value)
{
 int len = 0;

 len += this->encode_context_object_id(&apdu[len], 0, value->oid.type,value->oid.instance);
 len += this->encode_context_enumerated(&apdu[len],1,value->propertyId);
 len += this->encode_context_unsigned(&apdu[len],2,value->propertyArrayIndex);

 return len;
}



int I_cBACdec::bacapp_decode_obj_property_ref( uint8_t * apdu,BACNET_OBJECT_PROPERTY_REFERENCE * value)
{
    int len;
    int apdu_len = 0;
    uint32_t enumValue;
    if (-1 == (len =decode_context_object_id(&apdu[apdu_len], 0, &value->oid.type, &value->oid.instance)))
       {
        return -1;
       }

    apdu_len += len;

    if (-1 == (len =decode_context_enumerated(&apdu[apdu_len], 1, &enumValue)))
       {
        return -1;
       }

    value->propertyId= (BACNET_PROPERTY_ID) enumValue;
    apdu_len += len;

    if (decode_is_context_tag(&apdu[apdu_len], 2) && !decode_is_closing_tag(&apdu[apdu_len]))
       {
        if (-1 == (len = decode_context_unsigned(&apdu[apdu_len], 2, &value->propertyArrayIndex)))
           {
            return -1;
            }

        apdu_len += len;
       } else {
               value->propertyArrayIndex = 0;
              }
    return apdu_len;
}



int I_cBACdec::encode_recipient(uint8_t * apdu,BACNET_RECIPIENT * destination)
{
 int len = 0;
 if (destination->TagRecipientType == BACNET_RECIPIENT_TYPE_OID)
    {
     len += this->encode_context_object_id(&apdu[len],BACNET_RECIPIENT_TYPE_OID,destination->RecipientType.Device.type,destination->RecipientType.Device.instance);
    }else if (destination->TagRecipientType == BACNET_RECIPIENT_TYPE_ADR)
             {
              len += encode_context_bacnet_address(&apdu[len],1,&destination->RecipientType.Address);
             }
 return len;
}



/* recipient address */
int I_cBACdec::decode_recipient(uint8_t * apdu,BACNET_RECIPIENT * destination)
{

    /* Object-Id */
    if (decode_is_context_tag(apdu, 0) && !decode_is_closing_tag(apdu))
       {
        destination->TagRecipientType = 0;
        return decode_context_object_id(apdu, 0, &destination->RecipientType.Device.type,&destination->RecipientType.Device.instance);
       } else /* BACnetAddress */
              if (decode_is_opening_tag_number(apdu, 1))
                 {
                  destination->TagRecipientType = 1;
                  return decode_context_bacnet_address(apdu, 1,&destination->RecipientType.Address);
                 } else return -1;
}




int I_cBACdec::encode_context_bacnet_address(uint8_t * apdu,uint8_t tag_number,BACNET_ADDRESS * destination)
{
 int len = 0;

 len += encode_opening_tag(&apdu[len],tag_number);
 len += this->encode_application_unsigned(&apdu[len],destination->networkNumber);

 BACNET_OCTET_STRING *ostr = new BACNET_OCTET_STRING;


 octetstring_init(ostr,destination->adr,destination->len);
 len += encode_application_octet_string(&apdu[len],ostr);


 len += encode_closing_tag(&apdu[len],1);

 delete ostr;
 return len;
}


/* BACnetAddress */
int I_cBACdec::decode_context_bacnet_address(uint8_t * apdu,uint8_t tag_number,BACNET_ADDRESS * destination)
{
    int len = 0;
    int section_length;

    if (decode_is_opening_tag_number(&apdu[len], tag_number)) {
        len++;
        section_length = decode_bacnet_address(&apdu[len], destination);

        if (section_length == -1) {
            len = -1;
        } else {
            len += section_length;
            if (decode_is_closing_tag_number(&apdu[len], tag_number)) {
                len++;
            } else {
                len = -1;
            }
        }
    } else {
        len = -1;
    }
    return len;
}



/* BACnetAddress */
int I_cBACdec::decode_bacnet_address(uint8_t * apdu, BACNET_ADDRESS * destination)
{
    int len = 0;
    int tag_len = 0;
    uint32_t len_value_type = 0;
    int i = 0;
    uint32_t data_unsigned;
    uint8_t tag_number = 0;
    BACNET_OCTET_STRING mac_addr;
    /* network number */
    tag_len =
        decode_tag_number_and_value(&apdu[len], &tag_number, &len_value_type);
    len += tag_len;
    if (tag_number != BACNET_APPLICATION_TAG_UINT)
        return -1;
    len += decode_unsigned(&apdu[len], len_value_type, &data_unsigned);
    destination->networkNumber = data_unsigned;
    /* encode mac address as an octet-string */
    tag_len =
        decode_tag_number_and_value(&apdu[len], &tag_number, &len_value_type);
    len += tag_len;
    if (tag_number != BACNET_APPLICATION_TAG_OSTRING)
        return -1;
    len += decode_octet_string(&apdu[len], len_value_type, &mac_addr);
    destination->len = mac_addr.vValue.size();
    /* paranoia : test too big strings */
    if (destination->len > sizeof(destination->adr))
        destination->len = sizeof(destination->adr);
    /* copy address */
    for (i = 0; i < destination->len; i++)
        destination->adr[i] = mac_addr.vValue[i];
    return len;
}


int I_cBACdec::encode_destination(uint8_t * apdu, BACNET_DESTINATION * destination)
{
 int len = 0;

    //BACNET_BIT_STRING ValidDays;
    ///*/ Begin of the time window (inclusive) during which the destination is viable */
    //BACNET_TIME FromTime;
    ///*/ End of the time window (inclusive) during which the destination is viable */
    //BACNET_TIME ToTime;
    ///*/ The destination device(s) to receive notifications */
    //BACNET_RECIPIENT Recipient;
    ///*/ Handle of a process within the recipient device */
    //uint32_t ProcessIdentifier;
    ///*/ True if confirmed notifications are to be sent, false if unconfirmed notifications */
    //bool IssueConfirmedNotifications;
    ///*/ BACnetEventTransitionBits : 3 flags that indicate the transitions  */
    ///*/ {to-offnormal, to-fault, to-normal } for which this recipient is suitable */
    //BACNET_BIT_STRING Transitions;


 len += encode_application_bitstring(&apdu[len],&destination->ValidDays);
 len += encode_application_time(&apdu[len],&destination->FromTime);
 len += encode_application_time(&apdu[len],&destination->ToTime);
 len += encode_recipient(&apdu[len],&destination->Recipient);
 len += encode_application_unsigned(&apdu[len],destination->ProcessIdentifier);
 len += encode_application_boolean(&apdu[len],destination->IssueConfirmedNotifications);
 len += encode_application_bitstring(&apdu[len],&destination->Transitions);


 return len;
}

/* BACnetDestination : one item of a recipient_list property */
int I_cBACdec::decode_destination(uint8_t * apdu,int max_apdu_len, BACNET_DESTINATION * destination)
{
    int len = 0;
    int tag_len;
    uint32_t len_value_type = 0;
    uint8_t tag_number = 0;

    /* validDays */
    tag_len =decode_tag_number_and_value(&apdu[len], &tag_number, &len_value_type);

    len += tag_len;
    if (tag_number != BACNET_APPLICATION_TAG_BSTRING)
        return -1;
    len +=
        decode_bitstring(&apdu[len], len_value_type, &destination->ValidDays);
    /* From time */
    len_value_type =
        decode_application_time(&apdu[len], &destination->FromTime);
    if (len_value_type < 0)
        return -1;
    len += len_value_type;
    /* From time */
    len_value_type = decode_application_time(&apdu[len], &destination->ToTime);
    if (len_value_type < 0)
        return -1;
    len += len_value_type;
    /* Recipient */
    len_value_type = decode_recipient(&apdu[len], &destination->Recipient);
    if (len_value_type < 0)
        return -1;
    len += len_value_type;
    /* Pid */
    tag_len =
        decode_tag_number_and_value(&apdu[len], &tag_number, &len_value_type);
    len += tag_len;
    if (tag_number != BACNET_APPLICATION_TAG_UINT)
        return -1;
    len +=
        decode_unsigned(&apdu[len], len_value_type,
        &destination->ProcessIdentifier);
    /* Confirmed */
    tag_len =
        decode_tag_number_and_value(&apdu[len], &tag_number, &len_value_type);
    len += tag_len;
    if (tag_number != BACNET_APPLICATION_TAG_BOOL)
        return -1;
    destination->IssueConfirmedNotifications = decode_boolean(len_value_type);
    /* Transitions */
    tag_len =
        decode_tag_number_and_value(&apdu[len], &tag_number, &len_value_type);
    len += tag_len;
    if (tag_number != BACNET_APPLICATION_TAG_BSTRING)
        return -1;
    len +=
        decode_bitstring(&apdu[len], len_value_type,
        &destination->Transitions);
    return len;
}



int I_cBACdec::encode_daterange(uint8_t * apdu,BACNET_DATE_RANGE * bdaterange)
{
 int len = 0;

 len += encode_application_date(&apdu[len],&bdaterange->startdate);
 len += encode_application_date(&apdu[len],&bdaterange->enddate);

 return len;
}




int I_cBACdec::decode_daterange(uint8_t * apdu,BACNET_DATE_RANGE * bdaterange)
{
    int apdu_len = 0;
    int len = 0;
    len = decode_application_date(&apdu[apdu_len], &bdaterange->startdate);
    if (len < 0)
       {
        // date range nije specificiran
        bdaterange->startdate.day = bdaterange->startdate.month = bdaterange->startdate.wday = bdaterange->startdate.year = 0;
        return -1;
       }
    apdu_len += len;
    len = decode_application_date(&apdu[apdu_len], &bdaterange->enddate);
    if (len < 0)
       {
         // date range nije specificiran
        bdaterange->enddate.day = bdaterange->enddate.month = bdaterange->enddate.wday = bdaterange->enddate.year = 0;
        return -1;
       }


    apdu_len += len;
    return apdu_len;
}


int I_cBACdec::encode_context_timestamp(uint8_t * apdu, uint8_t tag_number, BACNET_TIMESTAMP *timestamp)
{
 int len = 0;

 encode_opening_tag(&apdu[len],3);
 len++;


 switch(timestamp->tag)
       {
        case 1:
         len += encode_context_unsigned(&apdu[len],1,timestamp->value.sequenceNum);
         break;
        case 2:
         //len += encode_context_unsigned(&apdu[len],1,timestamp->value);
         break;
        case 3:
         //len += encode_context_unsigned(&apdu[len],1,timestamp->value);
         break;
        default:
         return -1;
       }

 encode_closing_tag(&apdu[len],3);
 len++;
 return len;
}


int I_cBACdec::bacapp_decode_timestamp( uint8_t * apdu, BACNET_TIMESTAMP * value, BACNET_PROPERTY_ID prop_id)
{
    int len = 0;
    int section_len;
    uint32_t len_value_type;
    uint32_t sequenceNum;

    if (apdu)
       {
        uint8_t tmp_tag = 0;
        section_len =decode_tag_number_and_value(&apdu[len], &tmp_tag,&len_value_type);
        value->tag = (BACNET_TIMESTAMP_TAG)tmp_tag;



        if (-1 == section_len) {
            return -1;
        }
        switch (value->tag) {
            case TIME_STAMP_TIME:
                if ((section_len =
                        decode_context_bacnet_time(&apdu[len], TIME_STAMP_TIME,
                            &value->value.time)) == -1) {
                    return -1;
                } else {
                    len += section_len;
                }
                break;

            case TIME_STAMP_SEQUENCE:
                if ((section_len =
                        decode_context_unsigned(&apdu[len],
                            TIME_STAMP_SEQUENCE, &sequenceNum)) == -1) {
                    return -1;
                } else {
                    if (sequenceNum <= 0xffff) {
                        len += section_len;
                        value->value.sequenceNum = (uint16_t) sequenceNum;
                    } else {
                        return -1;
                    }
                }
                break;

            case TIME_STAMP_DATETIME:
                if ((section_len =  bacapp_decode_context_datetime(&apdu[len],  TIME_STAMP_DATETIME, &value->value.dateTime)) == -1)
                   {
                    return -1;
                   } else {
                           len += section_len;
                          }
                break;

            default:
                char msg[256],desc[256];
                sprintf(msg,"Unknown context tag while decoding time stamp value of property: %s",BACAPI_BACnetPropertyName(prop_id));
                sprintf(desc,"Unknown context tag while decoding time stamp value property: %s. Expected TIME, SEQUENCE or DATETIME. Received: Unknown [%d].",BACAPI_BACnetPropertyName(prop_id),value->tag);
                BACAPI_Message(msg,desc,2);
                len += len_value_type += section_len;
                return len;
        }
    }

    return len;
}




int I_cBACdec::bacapp_decode_context_datetime(uint8_t * apdu,uint8_t tag_number,BACNET_DATE_TIME * value)
{
    int apdu_len = 0;
    int len;

    if (decode_is_opening_tag_number(&apdu[apdu_len], tag_number)) {
        apdu_len++;
    } else {
        return -1;
    }

    if (-1 == (len = bacapp_decode_datetime(&apdu[apdu_len], value))) {
        return -1;
    } else {
        apdu_len += len;
    }

    if (decode_is_closing_tag_number(&apdu[apdu_len], tag_number)) {
        apdu_len++;
    } else {
        return -1;
    }
    return apdu_len;
}




int I_cBACdec::bacapp_decode_datetime(uint8_t * apdu,BACNET_DATE_TIME * value)
{
    int len = 0;
    int section_len;

    if (-1 == (section_len =
            decode_application_date(&apdu[len], &value->date))) {
        return -1;
    }
    len += section_len;

    if (-1 == (section_len =
            decode_application_time(&apdu[len], &value->time))) {
        return -1;
    }

    len += section_len;

    return len;
}



/* BACnetCOVSubscription */
/*
BACnetCOVSubscription::= SEQUENCE 
{
 recipient                     [0] BACnetRecipientProcess,
 monitored-property-reference  [1] BACnetObjectPropertyReference,
 issue-confirmed-notifications [2] BOOLEAN,
 time-remaining                [3] Unsigned,
 cov-increment                 [4] REAL OPTIONAL -- used only with monitored properties with a numeric datatype
}
*/

int I_cBACdec::decode_cov_subscription(uint8_t * apdu,int max_apdu_len,BACNET_COV_SUBSCRIPTION * covs)
{
    int apdu_len = 0;
    int len = 0;
    int tag_len;
    uint32_t len_value_type = 0;
    uint8_t tag_number = 0;

    /* recipient [0] BACnetRecipientProcess,  */
    len = decode_context_recipient_process(&apdu[apdu_len], 0, &covs->recipientProcess);
    if (len < 0)
        return -1;
    apdu_len += len;
    /* monitoredPropertyReference [1] BACnetObjectPropertyReference,  */
    len = bacapp_decode_context_obj_property_ref(&apdu[apdu_len], 1, &covs->monitoredProperty);

    if (len < 0)
        return -1;
    apdu_len += len;
    /* issueConfirmedNotifications [2] BOOLEAN,  */
    tag_len = decode_context_boolean2(&apdu[apdu_len], 2, &covs->confirmed);

    if (tag_len < 0)
        return -1;
    apdu_len += tag_len;
    /* timeRemaining [3] Unsigned,  */
    tag_len = decode_tag_number_and_value(&apdu[apdu_len], &tag_number,  &len_value_type);

    apdu_len += tag_len;
    if (tag_number != 3)
        return -1;
        
    apdu_len += decode_unsigned(&apdu[apdu_len], len_value_type, &covs->timeRemaining);
    
    /* covIncrement [4] REAL OPTIONAL */
    if (decode_is_context_tag(&apdu[apdu_len], 4) && !decode_is_closing_tag(&apdu[apdu_len])) 
       {
        tag_len = decode_context_real(&apdu[apdu_len], 4, &covs->covIncrement);
        if (tag_len < 0)
            return -1;
        apdu_len += tag_len;
       } else covs->covIncrement = 0.0f;
 
 return apdu_len;
}



int I_cBACdec::decode_device_address_binding(uint8_t *apdu, BACNET_ADDRESS_BINDING *addressBind)
{
 int len = 0;
 uint16_t type = 0;
 uint32_t instance = 0;
 uint32_t network_num = 0;
 uint8_t tag = 0;
 uint32_t lenval = 0;

 len += decode_tag_number_and_value(&apdu[len],&tag,&lenval);

 int ret = decode_object_id_safe(&apdu[len], lenval, &type, &instance);
 if (ret == 0) // NULL vrijednost
     return len;
 else len+=ret;


 len += decode_tag_number_and_value(&apdu[len], &tag, &lenval);


 len += decode_enumerated(&apdu[len], lenval, &network_num);


 len += decode_tag_number_and_value(&apdu[len], &tag, &lenval);
 BACNET_OCTET_STRING ostr;
 len += decode_octet_string(&apdu[len],lenval,&ostr);


 addressBind->deviceOid.instance = instance;
 addressBind->deviceOid.type = type;
 addressBind->deviceAddress.len = ostr.vValue.size();
 addressBind->deviceAddress.networkNumber = network_num;


 for (int c=0;c<ostr.vValue.size();c++)
     {
	  addressBind->deviceAddress.adr[c] = ostr.vValue[c];
	 }

 return len;
}




/* BACnetRecipientProcess */
int I_cBACdec::decode_context_recipient_process(uint8_t * apdu,uint8_t tag_number,BACNET_RECIPIENT_PROCESS * destination)
{
    int len = 0;
    int section_length;

    if (decode_is_opening_tag_number(&apdu[len], tag_number)) {
        len++;
        section_length = decode_recipient_process(&apdu[len], destination);

        if (section_length == -1) {
            len = -1;
        } else {
            len += section_length;
            if (decode_is_closing_tag_number(&apdu[len], tag_number)) {
                len++;
            } else {
                len = -1;
            }
        }
    } else {
        len = -1;
    }
    return len;
}





int I_cBACdec::bacapp_decode_context_obj_property_ref(uint8_t * apdu,uint8_t tag_number,BACNET_OBJECT_PROPERTY_REFERENCE * value)
{
    int len = 0;
    int section_length;

    if (decode_is_opening_tag_number(&apdu[len], tag_number)) {
        len++;
        section_length = bacapp_decode_obj_property_ref(&apdu[len], value);

        if (section_length == -1) {
            len = -1;
        } else {
            len += section_length;
            if (decode_is_closing_tag_number(&apdu[len], tag_number)) {
                len++;
            } else {
                len = -1;
            }
        }
    } else {
        len = -1;
    }
    return len;
}


int I_cBACdec::decode_context_boolean2(uint8_t * apdu,uint8_t tag_number,bool * boolean_value)
{
    int len = 0;
    if (decode_is_context_tag_with_length(&apdu[len], tag_number, &len)) {
        if (apdu[len]) {
            *boolean_value = true;
        } else {
            *boolean_value = false;
        }
        len++;
    } else {
        len = -1;
    }
    return len;
}



/* recipient address */
int I_cBACdec::decode_recipient_process(uint8_t * apdu,BACNET_RECIPIENT_PROCESS * destination)
{
    /* BACnetRecipientProcess   */
    /*    recipient  [0] BACnetRecipient,  */
    /*    processIdentifier [1] Unsigned32  */
    int apdu_len = 0;
    int len = 0;
    len =
        decode_context_recipient(&apdu[apdu_len], 0, &destination->recipient);
    if (len < 0)
        return -1;
    apdu_len += len;
    len =
        decode_context_unsigned(&apdu[apdu_len], 1,
        &destination->recipientProcessId);
    if (len < 0)
        return -1;
    apdu_len += len;
    return apdu_len;
}





/* BACnetRecipient */
int I_cBACdec::decode_context_recipient( uint8_t * apdu,uint8_t tag_number,BACNET_RECIPIENT * destination)
{
    int len = 0;
    int section_length;

    if (decode_is_opening_tag_number(&apdu[len], tag_number)) {
        len++;
        section_length = decode_recipient(&apdu[len], destination);

        if (section_length == -1) {
            len = -1;
        } else {
            len += section_length;
            if (decode_is_closing_tag_number(&apdu[len], tag_number)) {
                len++;
            } else {
                len = -1;
            }
        }
    } else {
        len = -1;
    }
    return len;
}



int I_cBACdec::encode_calendar_entry(uint8_t * apdu, int max_apdu_len, BACNET_CALENDAR_ENTRY * entry)
{
 int len = 0;

 switch(entry->TagEntryType)
       {
        case 0:

         len += encode_context_date(&apdu[len],0,&entry->Entry.date);
         break;

        case 1:
         len += encode_context_daterange(&apdu[len],1,&entry->Entry.dateRange);
        break;
        case 2:
         len += encode_context_weenkday(&apdu[len],2,&entry->Entry.weekNDay);
        break;
       }
 return len;
}


int I_cBACdec::decode_calendar_entry(uint8_t * apdu, int max_apdu_len, BACNET_CALENDAR_ENTRY * entry)
{
    /* Date */
    if (decode_is_context_tag(apdu, 0) && !decode_is_closing_tag(apdu))
       {
        entry->TagEntryType = 0;
        return decode_context_date(apdu, 0, &entry->Entry.date);
       } else if (decode_is_context_tag(apdu, 1) && !decode_is_closing_tag(apdu))
                 {
                  entry->TagEntryType = 1;
                  return decode_context_daterange(apdu, 1, &entry->Entry.dateRange);
                 }
         else if (decode_is_context_tag(apdu, 2) && !decode_is_closing_tag(apdu))
                 {
                  entry->TagEntryType = 2;
                  return decode_context_weeknday(apdu, 2, &entry->Entry.weekNDay);
                 }
    return -1;
}




int I_cBACdec::decode_context_date(uint8_t * apdu, uint8_t tag_number, BACNET_DATE * bdate)
{
    int len = 0;

    if (decode_is_context_tag_with_length(&apdu[len], tag_number, &len)) {
        len += decode_date(&apdu[len], bdate);
    } else {
        len = -1;
    }
    return len;
}


int I_cBACdec::encode_context_daterange( uint8_t * apdu,uint8_t tag_number,BACNET_DATE_RANGE * bdaterange)
{
 int len = 0;

 len += encode_opening_tag(&apdu[len],tag_number);
 len += encode_daterange(&apdu[len], bdaterange);
 len += encode_closing_tag(&apdu[len],tag_number);

 return len;
}


int I_cBACdec::decode_context_daterange( uint8_t * apdu,uint8_t tag_number,BACNET_DATE_RANGE * bdaterange)
{
    int len = 0;
    int section_length;

    if (decode_is_opening_tag_number(&apdu[len], tag_number)) {
        len++;
        section_length = decode_daterange(&apdu[len], bdaterange);

        if (section_length == -1) {
            len = -1;
        } else {
            len += section_length;
            if (decode_is_closing_tag_number(&apdu[len], tag_number)) {
                len++;
            } else {
                len = -1;
            }
        }
    } else {
        len = -1;
    }
    return len;
}



int I_cBACdec::encode_context_weenkday(uint8_t * apdu,uint8_t tag_number,BACNET_WEEKNDAY * bweeknday)
{
 int len = 0;

 len += encode_tag(&apdu[len],2,true,3);
 len += encode_weeknday(&apdu[len],bweeknday);


 return len;
}


int I_cBACdec::decode_context_weeknday(uint8_t * apdu,uint8_t tag_number,BACNET_WEEKNDAY * bweeknday)
{
    int len = 0;

    if (decode_is_context_tag_with_length(&apdu[len], tag_number, &len)) {
        len += decode_weeknday(&apdu[len], bweeknday);
    } else {
        len = -1;
    }
    return len;
}



int I_cBACdec::encode_weeknday(uint8_t * apdu,BACNET_WEEKNDAY * bweeknday)
{
 int len = 0;
 apdu[len++] = bweeknday->month;
 apdu[len++] = bweeknday->weekofmonth;
 apdu[len++] = bweeknday->dayofweek;

 return len;
}


int I_cBACdec::decode_weeknday(uint8_t * apdu,BACNET_WEEKNDAY * bweeknday)
{
    bweeknday->month = apdu[0];
    bweeknday->weekofmonth = apdu[1];
    bweeknday->dayofweek = apdu[2];
    return 3;
}



/* decode one value of a priority array */
int I_cBACdec::decode_priority_value( uint8_t * apdu,unsigned max_apdu_len, BACNET_APPLICATION_DATA_VALUE * value, BACNET_PROPERTY_ID prop)
{
    int val_len = 0;
    uint32_t len_value_type = 0;
    int len = 0;
    bool is_opening_tag;
    uint8_t tag_number;

    if (decode_is_context_tag(apdu, 0) && !decode_is_closing_tag(apdu)) {
        /* Contextual Abstract-syntax & type */
        val_len =
            decode_tag_number_and_value(apdu, &tag_number, &len_value_type);
        is_opening_tag = decode_is_opening_tag(apdu);
        len += val_len;
        val_len =
            bacapp_decode_generic_property(&apdu[len], max_apdu_len - len,
            value, prop);
        if (val_len < 0)
            return -1;
        len += val_len;
        if (is_opening_tag) {
            if (!decode_is_closing_tag_number(apdu, 0))
                return -1;
            len++;
        }
        return len;
    } /* else generic decode */
    else
        return bacapp_decode_generic_property(apdu, max_apdu_len, value, prop);
}






/*/ Generic property decoding */
int I_cBACdec::bacapp_decode_generic_property( uint8_t * apdu,int max_apdu_len,BACNET_APPLICATION_DATA_VALUE * value,BACNET_PROPERTY_ID prop)
{
    int len = 0;
    if (IS_CONTEXT_SPECIFIC(*apdu)) {
        len = bacapp_decode_context_data(apdu, max_apdu_len, value, prop);
    } else {
        len = bacapp_decode_application_data(apdu, max_apdu_len, value);
    }
    return len;
}







int I_cBACdec::encode_weekly_schedule(uint8_t * apdu, BACNET_WEEKLY_SCHEDULE * week)
{
 int len = 0;

 for (int c=0;c<7;c++)
     {
      len += encode_daily_schedule(&apdu[len],BACNET_MAX_APDU_IP,&week->weeklySchedule[c]);
     }


  return len;

}

/*/ BACnetDailySchedule x7 / weekly-schedule */
int I_cBACdec::decode_weekly_schedule(uint8_t * apdu,int max_apdu_len, BACNET_WEEKLY_SCHEDULE * week)
{
    int j;
    int len = 0;
    int apdu_len = 0;

    for (j = 0; j < 7; j++)
        {
         len = decode_daily_schedule(&apdu[apdu_len], max_apdu_len - apdu_len, &week->weeklySchedule[j]);


         if (len < 0)
            {
             return -1;
            }

         apdu_len += len;
         uint8_t *tmp = &apdu[apdu_len];
        }
    return apdu_len;
}




int I_cBACdec::encode_daily_schedule(uint8_t * apdu,int max_apdu_len,BACNET_DAILY_SCHEDULE *day)
{
 return encode_context_time_values(apdu,BACNET_MAX_APDU_IP,0,&day->vDaySchedule);
}



/*/ BACnetDailySchedule */
int I_cBACdec::decode_daily_schedule(uint8_t * apdu,int max_apdu_len,BACNET_DAILY_SCHEDULE * day)
{
 return decode_context_time_values(apdu, max_apdu_len, 0, &day->vDaySchedule);
}


int I_cBACdec::encode_context_time_values( uint8_t * apdu,int max_apdu_len,uint8_t tag_number,vector<BACNET_TIME_VALUE>*daySchedule)
{
 int len = 0;
 //BACNET_APPLICATION_TAG_NULL

 len += encode_opening_tag(&apdu[len],0);

 int count = (int)daySchedule->size();
 for (int c=0;c<count;c++)
     {
      BACNET_TIME_VALUE *time_value = &daySchedule->at(c);
      //if (time_value->Value.tag != BACNET_APPLICATION_TAG_NULL)
         {
          len += encode_time_and_value(&apdu[len],max_apdu_len,time_value);
         }
     }
 len += encode_closing_tag(&apdu[len],0);

 return len;
}


/*/ Decodes a : [x] SEQUENCE OF BACnetTimeValue into a fixed-size buffer */
int I_cBACdec::decode_context_time_values( uint8_t * apdu,int max_apdu_len,uint8_t tag_number,vector<BACNET_TIME_VALUE>*daySchedule)
{
    unsigned int j;
    int len = 0;
    int apdu_len = 0;
    unsigned int count_values = 0;
    BACNET_TIME_VALUE dummy;
    BACNET_TIME_VALUE *tval = NULL;

    /* day-schedule [0] SEQUENCE OF BACnetTimeValue */
    if (decode_is_opening_tag_number(&apdu[apdu_len], tag_number))
       {
        apdu_len++;
        while ((apdu_len < max_apdu_len) && !decode_is_closing_tag_number(&apdu[apdu_len], tag_number))
              {
               BACNET_TIME_VALUE new_value;
               len = decode_time_and_value(&apdu[apdu_len], max_apdu_len - apdu_len,&new_value);
               daySchedule->push_back(new_value);

               if (len < 0)
                  {
                   return -1;
                  }
               apdu_len += len;
              }


        /* overflow ! */
        if (apdu_len >= max_apdu_len)
           {
            return -1;
           }

        apdu_len++;     /* closing tag */
        return apdu_len;
    }
    return -1;
}


int I_cBACdec::encode_time_and_value( uint8_t * apdu, int max_apdu_len, BACNET_TIME_VALUE * timevalue)
{
 int len = 0;

 len += encode_application_time(&apdu[len],&timevalue->Time);
 switch (timevalue->Value.tag)
        {
         case BACNET_APPLICATION_TAG_NULL:
          apdu[len] = 0;
          return len+1;
         case BACNET_APPLICATION_TAG_BOOL:
         {
          len += encode_application_boolean(&apdu[len],timevalue->Value.type.Boolean);
          break;
         }
         case BACNET_APPLICATION_TAG_UINT:
         {
          len += encode_application_unsigned(&apdu[len],timevalue->Value.type.Unsigned_Int);
          break;
         }
         case BACNET_APPLICATION_TAG_SINT:
         {
          len += encode_application_signed(&apdu[len],timevalue->Value.type.
          Signed_Int);
          break;
         }
         case BACNET_APPLICATION_TAG_REAL:
         {
          len += encode_application_real(&apdu[len],timevalue->Value.type.Real);
          break;
         }
         case BACNET_APPLICATION_TAG_ENUM:
         {
          len += encode_application_enumerated(&apdu[len],timevalue->Value.type.Enumerated);
          break;
         }
         case BACNET_APPLICATION_TAG_CSTRING:
         {
          len += encode_application_character_string(&apdu[len],&timevalue->Value.type.character_string);
          break;
         }
         default:
            return -1;
        }

 return len;
}


/*/ BACnetTimeValue */
int I_cBACdec::decode_time_and_value( uint8_t * apdu, int max_apdu_len, BACNET_TIME_VALUE * timevalue)
{
    int len = 0;
    int apdu_len = 0;
    BACNET_APPLICATION_DATA_VALUE tmpvalue = { 0 };

    if (max_apdu_len <= 0)
        return -1;

    len = decode_application_time(&apdu[apdu_len], &timevalue->Time);
    if (len < 0)
        return -1;

    apdu_len += len;

    len =bacapp_decode_application_data(&apdu[apdu_len],max_apdu_len - apdu_len, &tmpvalue);

    uint8_t *tmp = &apdu[apdu_len];

    if (len < 0)
        return -1;
    /* Copy plain value to bacnet-short-app-value type */
    timevalue->Value.tag = tmpvalue.application_tag;
    switch (tmpvalue.application_tag) {
        case BACNET_APPLICATION_TAG_NULL:
            break;
        case BACNET_APPLICATION_TAG_BOOL:
            timevalue->Value.type.Boolean = tmpvalue.type.Boolean;
            break;
        case BACNET_APPLICATION_TAG_UINT:
            timevalue->Value.type.Unsigned_Int = tmpvalue.type.Unsigned_Int;
            break;
        case BACNET_APPLICATION_TAG_SINT:
            timevalue->Value.type.Signed_Int = tmpvalue.type.Signed_Int;
            break;
        case BACNET_APPLICATION_TAG_REAL:
            timevalue->Value.type.Real = tmpvalue.type.Real;
            break;
        case BACNET_APPLICATION_TAG_ENUM:
            timevalue->Value.type.Enumerated = tmpvalue.type.Enumerated;
            break;
        case BACNET_APPLICATION_TAG_CSTRING:
            timevalue->Value.type.character_string.encoding = tmpvalue.type.character_String.encoding;
            timevalue->Value.type.character_string.value8 = tmpvalue.type.character_String.value8;
            break;

        default:
            return -1;
    }
    apdu_len += len;
    return apdu_len;
}





/*/ BACnetSpecialEvent */
/*
int I_cBACdec::decode_special_event(uint8_t * apdu,int max_apdu_len, BACNET_SPECIAL_EVENT * special)
{
    int len;
    int apdu_len = 0;
    uint32_t priority;

    if (apdu_len >= max_apdu_len)
        return -1;
    if (decode_is_context_tag(apdu, 0) && !decode_is_closing_tag(apdu)) {       // [0] calendar entry
        special->TagSpecialEventType = 0;
        len =
            decode_context_calendar_entry(apdu, max_apdu_len, 0,
            &special->period.calendarEntry);
        if (len < 0)
            return -1;
        apdu_len += len;
    } else if (decode_is_context_tag(apdu, 1) && !decode_is_closing_tag(apdu)) {        // [1] object_id
        special->TagSpecialEventType = 1;
        len =
            decode_context_object_id(apdu, 1,
            &special->period.calendarReference.type,
            &special->period.calendarReference.instance);
        if (len < 0)
            return -1;
        apdu_len += len;
    }
    if (apdu_len >= max_apdu_len)
        return -1;
    if (decode_is_opening_tag_number(&apdu[apdu_len], 2)) {     // [2] listOfTimeValues
        len =
            decode_context_time_values(&apdu[apdu_len],max_apdu_len - apdu_len, 2, &special->listOfTimeValues[0],MAX_SPECIAL_EVENT_VALUES);

        if (len < 0)
            return -1;
        apdu_len += len;
    } else
        return -1;
    if (apdu_len >= max_apdu_len)
        return -1;
    if (decode_is_context_tag(&apdu[apdu_len], 3) && !decode_is_closing_tag(&apdu[apdu_len])) { // [3] eventPriority
        len = decode_context_unsigned(&apdu[apdu_len], 3, &priority);
        if (len < 0)
            return -1;
        apdu_len += len;
        special->eventPriority = (uint8_t) priority;
    } else
        return -1;
    return apdu_len;
}
*/





/*/ BACnetCalendarEntry */
int I_cBACdec::decode_context_calendar_entry(uint8_t * apdu,int max_apdu_len,uint8_t tag_number,BACNET_CALENDAR_ENTRY * entry)
{
    int len = 0;
    int section_length;
    if (max_apdu_len <= 0)
        return -1;
    if (decode_is_opening_tag_number(&apdu[len], tag_number)) {
        len++;
        section_length =
            decode_calendar_entry(&apdu[len], max_apdu_len - len, entry);

        if (section_length == -1) {
            len = -1;
        } else {
            len += section_length;
            if (decode_is_closing_tag_number(&apdu[len], tag_number)) {
                len++;
            } else {
                len = -1;
            }
        }
    } else {
        len = -1;
    }
    return len;
}






/* ReadAccessSpecification */
int I_cBACdec::decode_read_access_specification(uint8_t * apdu,int max_apdu_len,BACNET_READ_ACCESS_SPECIFICATION * entry)
{
    int apdu_len = 0;
    int len;
    int item_number = 0;
    BACNET_PROPERTY_REF dummy_ref;

    /* objectIdentifier  [0] BACnetObjectIdentifier */
    len =
        decode_context_object_id(&apdu[apdu_len], 0,
        &entry->objectIdentifier.type, &entry->objectIdentifier.instance);
    if (len < 0)
        return -1;
    apdu_len += len;


    /* listOfPropertyReferences [1] SEQUENCE */
    if (decode_is_context_tag_with_length(&apdu[apdu_len], 1, &len)&& !decode_is_closing_tag(&apdu[apdu_len]))
       {
        apdu_len += len;

        /* Decode all property reference values seen */
        while (apdu_len < max_apdu_len)
              {
               /* Mark item as "empty" */
               if (item_number < MAX_LIST_OF_PROPERTY_REFERENCES)
                   entry->listOfPropertyReferences[item_number]. propertyIdentifier = EMPTY_PROPERTY_REFERENCE_ID;

               /* check closing tag */
               if (decode_is_closing_tag_number(&apdu[apdu_len], 1))
                   break;

               /* read sequence item */
               len = bacapp_decode_property_ref(&apdu[apdu_len],(item_number <MAX_LIST_OF_PROPERTY_REFERENCES) ? &entry-> listOfPropertyReferences[item_number] : &dummy_ref);

               if (len < 0)
                   return -1;

               apdu_len += len;
               item_number++;
              }
    } else
        return -1;
    if ((apdu_len < max_apdu_len) &&
        decode_is_closing_tag_number(&apdu[apdu_len], 1)) {
        apdu_len++;
    } else
        return -1;
    return apdu_len;
}




int I_cBACdec::bacapp_decode_property_ref(uint8_t * apdu,BACNET_PROPERTY_REF * value)
{
    int len;
    int apdu_len = 0;
    uint32_t enumValue;

    if (-1 == (len =
            decode_context_enumerated(&apdu[apdu_len], 0, &enumValue))) {
        return -1;
    }
    value->propertyIdentifier = (BACNET_PROPERTY_ID) enumValue;
    apdu_len += len;

    if (decode_is_context_tag(&apdu[apdu_len], 1) &&
        !decode_is_closing_tag(&apdu[apdu_len])) {
        if (-1 == (len =
                decode_context_unsigned(&apdu[apdu_len], 1,
                    &value->arrayIndex))) {
            return -1;
        }
        apdu_len += len;
    } else {
        value->arrayIndex = 0;
    }
    return apdu_len;
}




int I_cBACdec::decode_action_command_list(uint8_t *apdu, BACNET_ACTION_LIST *actionCommandList)
{
 int len = 0;
 uint8_t tagVal = 0;
 uint32_t lenVal = 0;
 uint32_t val = 0;

 if (decode_is_context_tag(&apdu[len],0))
    {
     len++;
     while (!decode_is_closing_tag(&apdu[len]))
           {
            BACNET_ACTION_COMMAND new_command;
			new_command.property = new BACNET_ACTION_PROPERTY;
			new_command.property->index = 255, new_command.property->priority = 255;



			decode_tag_number_and_value(&apdu[len],&tagVal,&lenVal);
			if (tagVal == 0)   // OPTIONAL device
			   {
				// dekodiraj device instance i type
			    len++;
				len += decode_object_id_safe(&apdu[len],lenVal,&new_command.device.type,&new_command.device.instance);
			   }else {
			          new_command.device.type = -1;
			         }


			decode_tag_number_and_value(&apdu[len],&tagVal,&lenVal);
			if (tagVal == 1)  // object id
			   {
			    len++;
			    len += decode_object_id_safe(&apdu[len],lenVal,&new_command.object.type,&new_command.object.instance);
			   }else {
				      return -1;
				     }

			// property Id
			decode_tag_number_and_value(&apdu[len],&tagVal,&lenVal);
			if (tagVal == 2)
			   {
			    len++;
				len += decode_enumerated(&apdu[len],lenVal, &val);
				new_command.property->property = (BACNET_PROPERTY_ID)val;
			   }else {
			          delete new_command.property;
					  return -1;
					 }

			// OPTIONAL: prop array index
			decode_tag_number_and_value(&apdu[len],&tagVal,&lenVal);
			if (tagVal == 3) // OPTIONAL array index
			   {
			    len ++;
				len += this->decode_unsigned(&apdu[len],lenVal,&val);
				new_command.property->index = (uint8_t)val;
			   }else {
			          new_command.property->index = -1;
					 }


			// stvori application_value na temelju app taga od propertija
			// stvori value
		    BACNET_APPLICATION_TAG tag = BACAPI_GetApplicationTag(OBJECT_ANALOG_INPUT,new_command.property->property);
		    new_command.property->value = BACAPI_CreateApplicationDataValue(tag);
			decode_tag_number_and_value(&apdu[len],&tagVal,&lenVal);
			if (tagVal == 4)
			   {
			    len++;
				len += bacapp_decode_application_data(&apdu[len],1476,new_command.property->value);
   				bool closing = decode_is_closing_tag_number(&apdu[len],4);  // ovdje se vrijednosni tag mora zatvoriti

   				if (!closing)
   				   {
   				    delete new_command.property->value;
   				    delete new_command.property;
   					return -1;
   				   }
   				len++;
			   }else {
					  return -1;
					 }

			// OPTIONAL: priority and postDelay
			decode_tag_number_and_value(&apdu[len],&tagVal,&lenVal);
			if (tagVal == 5)   // priority
			   {
			    len ++;
				len += this->decode_unsigned(&apdu[len],lenVal,&val);
				new_command.property->priority = (uint8_t)val;
			   }else {
			          new_command.property->priority = -1;
					 }

			decode_tag_number_and_value(&apdu[len],&tagVal,&lenVal);
		    if (tagVal == 6)  // post delay
			   {
			    len++;
				len += this->decode_unsigned(&apdu[len],lenVal,&val);
				new_command.postDelay = (uint8_t)val;
			   }else {
			          new_command.postDelay = -1;
			         }


			decode_tag_number_and_value(&apdu[len],&tagVal,&lenVal);
		    if (tagVal == 7)
			   {
			    len++;
				new_command.quitOnFail = decode_context_boolean(&apdu[len]);
				len++;
			   }else {
			          delete new_command.property->value;
					  return -1;
					 }

			// write ok
			decode_tag_number_and_value(&apdu[len],&tagVal,&lenVal);
			if (tagVal == 8)  // writeSuccessful
			   {
			    len++;
				new_command.writeSuccessful = decode_context_boolean(&apdu[len]);
				len++;
			   }

            actionCommandList->vBACnetActionCommand.push_back(new_command);
		   }
     // zatvarajuci context tag
     len++;
    }


 uint8_t *tmp = &apdu[len];
 return len;
}




int I_cBACdec::decode_scale(uint8_t *apdu, BACNET_SCALE *scale)
{
 int len = 0;
 uint8_t tagnum = 0;
 uint32_t lenVal = 0;
 uint32_t val = 0;
 uint8_t tagVal = 0;
 float real;

 decode_tag_number(&apdu[len],&tagnum);
 //len++;


 switch (tagnum)
        {
         case 0:   // floatScale  [0]
         {
          len += this->decode_context_real(&apdu[len],tagnum,&real);
		  scale->floatScale = real;
          break;
         }

         case 1:   // integerScale  [1]
         {
          len += this->decode_context_unsigned(&apdu[len],tagnum,&val);
          scale->intScale = (uint16_t)val;
          break;
         }

         default: return -1;
        }

 scale->type = tagnum;
 return len;
}


int I_cBACdec::decode_pre_scale(uint8_t *apdu, BACNET_PRESCALE *preScale)
{
 int len = 0;
 uint32_t val = 0;


 len += this->decode_context_unsigned(&apdu[len],0,&val);
 preScale->multiplier = (uint16_t)val;

 len += this->decode_context_unsigned(&apdu[len],1,&val);
 preScale->moduloDivide = (uint16_t)val;

 return len;
}



int I_cBACdec::decode_shed_level(uint8_t *apdu, BACNET_SHED_LEVEL *shedLevel)
{
 int len = 0;
 uint8_t tagnum = 0;
 uint32_t lenVal = 0;
 uint32_t val = 0;
 uint8_t tagVal = 0;
 float real = 0;

 decode_tag_number(&apdu[len],&tagnum);


 switch (tagnum)
        {
         case 0:
         {
          uint8_t *tmp =  &apdu[len];
          len += decode_tag_number_and_value(&apdu[len],&tagVal,&lenVal);
          len += decode_enumerated(&apdu[len],lenVal,&val);
          shedLevel->type = 0;
          shedLevel->value.percent = (uint16_t)val;
          break;
         }
         case 1:
         {
          uint8_t *tmp =  &apdu[len];
          len += decode_tag_number_and_value(&apdu[len],&tagVal,&lenVal);
          len += decode_unsigned(&apdu[len],lenVal,&val);
          shedLevel->type = 1;
          shedLevel->value.percent = (uint16_t)val;
          break;
         }

         case 2:
         {
          len++;
          uint8_t *tmp =  &apdu[len];
          len += decode_real(&apdu[len],&real);
          shedLevel->type = 2;
          shedLevel->value.amount = (uint16_t)val;
          break;
         }

         default:
         break;
        }

 return len;
}





// TODO:
int I_cBACdec::decode_action_text(uint8_t *apdu, BACNET_ACTION_TEXT *actionText)
{
 return 0;
}



/*
BACnetEventParameter ::= CHOICE {

-- These choices have a one-to-one correspondence with the Event_Type enum eration with the  exception of the
-- complex-event-type, which is used for proprietary event types.

  change-of-bitstring  [0] SEQUENCE {
									 time-delay  [0] Unsigned,
								     bitmask   [1] BIT STRING,
                                     list-of-bitstring-values  [2] SEQUENCE OF BIT STRING
                                    },

  change-of-state  [1] SEQUENCE {
                                 time-delay [0] Unsigned,
                                 list-of-values  [1] SEQUENCE OF BACnetPropertyStates
                                },

  change-of-value  [2] SEQUENCE {
                                 time-delay [0] Unsigned,
                                 cov-criteria [1] CHOICE {
                                 bitmask    [0] BIT STRING,
                                 referenced-property-increment [1] REAL
                                }
    },
  command-failure  [3] SEQUENCE {
    time-delay   [0] Unsigned,
    feedback-property-reference [1] BACnetDeviceObjectPropertyReference
    },
 floating-limit  [4] SEQUENCE {
    time-delay  [0] Unsigned,
    setpoint-reference [1] BACnetDeviceObjectPropertyReference,
    low-diff-limit  [2] REAL,
    high-diff-limit  [3] REAL,
    deadband  [4] REAL
    },
 out-of-range  [5] SEQUENCE {
    time-delay [0] Unsigned,
    low-limit [1] REAL,
    high-limit [2] REAL,
    deadband [3] REAL
    },
 -- context tag 7 is deprecated
 change-of-life-safety  [8] SEQUENCE {
 time-delay [0] Unsigned,    list-of-life-safety-alarm-values  [1] SEQUENCE OF BACnetLifeSafetyState,
 list-of-alarm-values  [2] SEQUENCE OF BACnetLifeSafetyState,
 mode-property-reference [3] BACnetDeviceObjectPropertyReference       },
 extended  [9] SEQUENCE {
    vendorId  [0] Unsigned,
    extendedEventType [1] Unsigned,
     parameters  [2] SEQUENCE OF CHOICE {
     null NULL,
     real REAL,
     integer Unsigned,
     boolean BOOLEAN,
     double DOUBLE,
     octet OCTET STRING,
     bitstrin BIT STRING,
     enum ENUMERATED,
     reference [0] BACnetDeviceObjectProperty
     }
    },
 buffer-ready  [10] SEQUENCE {
 notification-threshold [0] Unsigned,   previous-notification-count [1] Unsigned32       },
 unsigned-range  [11] SEQUENCE {
     time-delay [0] Unsigned,
     low-limit [1] Unsigned,
     high-limit [2] Unsigned
     }
 }
 */

int I_cBACdec::decode_event_parameters(uint8_t *apdu, BACNET_EVENT_PAREMETER *eventParam)
{
 int len = 0;
 uint8_t tagnum = 0;
 uint32_t lenVal = 0;
 uint32_t val = 0;
 uint8_t tagVal = 0;

 len += decode_tag_number(&apdu[len],&tagnum);


 switch (tagnum)
        {
         case 0:   // change-of-bitstring  [0]
         {
          //uint8_t *tmp =  &apdu[len];
          //len += decode_tag_number_and_value(&apdu[len],&tagVal,&lenVal);
          //len += decode_unsigned(&apdu[len],lenVal,&val);
          while (!decode_is_closing_tag_number(&apdu[len],0))
                {
                 len++;
                }
          len++;
          break;
         }

         case 1:   //change-of-state  [1] SEQUENCE
         {
          while (!decode_is_closing_tag_number(&apdu[len],1))
                {
                 len++;
                }
          len++;
          break;
         }

         case 2:   //change-of-state  [1] SEQUENCE
         {
          while (!decode_is_closing_tag_number(&apdu[len],2))
                {
                 len++;
                }
          len++;
          break;
         }

         case 3:   //change-of-state  [1] SEQUENCE
         {
          while (!decode_is_closing_tag_number(&apdu[len],3))
                {
                 len++;
                }
          len++;
          break;
         }

         case 4:   //change-of-state  [1] SEQUENCE
         {
          while (!decode_is_closing_tag_number(&apdu[len],4))
                {
                 len++;
                }
          len++;
          break;
         }

         case 5:   //change-of-state  [1] SEQUENCE
         {
          while (!decode_is_closing_tag_number(&apdu[len],5))
                {
                 len++;
                }
          len++;
          break;
         }

         case 6:   //change-of-state  [1] SEQUENCE
         {
          while (!decode_is_closing_tag_number(&apdu[len],6))
                {
                 len++;
                }
          len++;
          break;
         }

         case 7:   //change-of-state  [1] SEQUENCE
         {
          while (!decode_is_closing_tag_number(&apdu[len],7))
                {
                 len++;
                }
          len++;
          break;
         }

         case 8:   //change-of-state  [1] SEQUENCE
         {
          while (!decode_is_closing_tag_number(&apdu[len],8))
                {
                 len++;
                }
          len++;
          break;
         }

         case 9:   //change-of-state  [1] SEQUENCE
         {
          while (!decode_is_closing_tag_number(&apdu[len],9))
                {
                 len++;
                }
          len++;
          break;
         }

         case 10:   //change-of-state  [1] SEQUENCE
         {
          while (!decode_is_closing_tag_number(&apdu[len],10))
                {
                 len++;
                }
          len++;
          break;
         }



         default:
         break;
        }


 uint8_t *tmp =  &apdu[len];
 return len;
}





int I_cBACdec::BigToLittleEndian(char *str, int size)
{
 int len = 0;
 for (int c=0;c<size;c+=2,len++)
     {
      char tmp1,tmp2;
      tmp1 = str[c];
      tmp2 = str[c+1];

      str[c] = tmp2;
      str[c+1] = tmp1;
     }
 return len-1;
}



int I_cBACdec::encode_exception_schedule(uint8_t *apdu, BACNET_SPECIAL_EVENT *special_event)
{
 int len = 0;

 return len;
}



void I_cBACdec::bitstring_set_bit(BACNET_BIT_STRING * bit_string,uint8_t bit_number,bool value)
{
    uint8_t byte_number = bit_number / 8;
    uint8_t bit_mask = 1;

    if (byte_number < MAX_BITSTRING_BYTES) {
        /* set max bits used */
        if (bit_string->bits_used < (bit_number + 1)) {
            bit_string->bits_used = bit_number + 1;
        }
        bit_mask = bit_mask << (bit_number - (byte_number * 8));
        if (value) {
            bit_string->value[byte_number] |= bit_mask;
        } else {
            bit_string->value[byte_number] &= (~(bit_mask));
        }
    }
}


int I_cBACdec::encode_bacnet_host_port(uint8_t *apdu, BACNET_HOST_NPORT *port)
{
 int len = 0;
 // host address

  switch (port->address.tag)
        {
         case HOST_ADDRESS_TAG_NULL:  // [0]
          len += encode_context_null(&apdu[len],0);
          break;
          
         case HOST_ADDRESS_TAG_IP_ADDRESS:  // ostr
          len += encode_context_octet_string(&apdu[len],HOST_ADDRESS_TAG_IP_ADDRESS,&port->address.type.ip_address);
          break;
         case HOST_ADDRESS_TAG_NAME:   // cstr          
          len += encode_context_character_string(&apdu[len],HOST_ADDRESS_TAG_NAME,&port->address.type.name);
          break;
         default:
          len += encode_context_null(&apdu[len],0);
          break;
        }
        
 return len;        
}




int I_cBACdec::decode_bacnet_host_port(uint8_t *apdu, BACNET_HOST_NPORT *port)
{
 int len = 0;
 uint8_t tag = 0;
 uint32_t value =0;
 int tag_len = 0;
 // host address
 len += decode_tag_number_and_value(apdu,&tag,&value);
 port->address.tag = (BACNET_HOST_ADDRESS_TAG)tag;

 switch (port->address.tag)
        {
         case HOST_ADDRESS_TAG_NULL:  // [0]
          return len;

         case HOST_ADDRESS_TAG_IP_ADDRESS:  // ostr
         {
              for (int c=0;c<value;c++)
                   port->address.type.ip_address.vValue.push_back(apdu[len++]);

          uint8_t *tmp = &apdu[len];                   
          return len;
          break;
         }

         case HOST_ADDRESS_TAG_NAME:   // cstr          
          len += gDec->decode_context_character_string(&apdu[len],HOST_ADDRESS_TAG_NAME,&port->address.type.name);
          break;
          
         default:
          len += gDec->encode_context_null(&apdu[len],0);
          break;
        }
        
 return len;        
}



int I_cBACdec::decode_context_octet_string(uint8_t *apdu, uint8_t tag, BACNET_OCTET_STRING *ostr)
{
 int len = 0;
 uint8_t dec_tag;
 uint32_t value =0;
 len += decode_tag_number_and_value(apdu,&dec_tag,&value);

 if (dec_tag != tag) return -1;

 for (int c=0;c<value;c++)
      ostr->vValue.push_back(apdu[len++]);
 
 return len;
}




int I_cBACdec::decode_value_source(uint8_t *apdu, BACNET_VALUE_SOURCE *valsrc)
{
 int len = 0;
 uint8_t dec_tag;
 uint32_t value =0;
 
 len += decode_tag_number_and_value(apdu,&dec_tag,&value);
 // TODO:
 // staviti vrijednosti u valsrc
 
 return len;
 
}




int I_cBACdec::decode_notif_sub(uint8_t *apdu, BACNET_EVENT_NOTIFICATION_SUBSCRIPTION *sub)
{
 int len = 0;
 uint8_t dec_tag;
 uint32_t value =0;
 
 BACNET_RECIPIENT rec;
 len += this->decode_recipient(apdu,&rec);
 
 
 return len;
}









