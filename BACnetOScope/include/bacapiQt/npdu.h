#ifndef NPDU_H
#define NPDU_H



#define BVLC_RESULT                                   0
#define BVLC_WRITE_BROADCAST_DISTRIBUTION_TABLE       0x01
#define BVLC_READ_BROADCAST_DISTRIBUTION_TABLE        0x02
#define BVLC_READ_BROADCAST_DISTRIBUTION_TABLE_ACK    0x03
#define BVLC_FORWARDED_NPDU                           0x04
#define BVLC_REGISTER_FOREIGN_DEVICE                  0x05
#define BVLC_READ_FOREIGN_DEVICE_TABLE                0x06
#define BVLC_READ_FOREIGN_DEVICE_TABLE_ACK            0x07
#define BVLC_DELETE_FOREIGN_DEVICE_TABLE_ENTRY        0x08
#define BVLC_DISTRIBUTE_BROADCAST_TO_NETWORK          0x09
#define BVLC_ORIGINAL_UNICAST_NPDU                    0x0A
#define BVLC_ORIGINAL_BROADCAST_NPDU                  0x0B     // samo ovo za sada

// BACNET VIRTUAL LAYER LINK Annex J  - Bacnet/IP
// TODO: podrzati i ostale protokole
#define BACNET_BVLC_TYPE_BACNET_IP 0x81    // kako je navedeno u Annex J za prijenos pdua kroz IP mreze



// network interface tip
typedef enum BACNET_DATALINK
{
 BACNET_DATALINK_UNKNOWN = 0,
 BACNET_DATALINK_BACIP,
 BACNET_DATALINK_ETH802,
 BACNET_DATALINK_MSTP
}BACNET_DATALINK;



typedef enum BVLCResultCode
{
 BVLC_RESULT_SUCCESS = 0x0000,
 BVLC_RESULT_WRITE_BDT_NAK = 0x0010,
 BVLC_RESULT_READ_BDT_NAK = 0x0020,
 BVLC_RESULT_REGISTER_FOREIGN_DEVICE_NAK = 0x0030,
 BVLC_RESULT_READ_FOREIGN_DEVICE_TABLE_NAK = 0x0040,
 BVLC_RESULT_DELETE_FOREIGN_DEVICE_TABLE_ENTRY_NAK = 0x0050,
 BVLC_RESULT_DISTRIBUTE_BROADCAST_TO_NETWORK_NAK = 0x0060,
 BVLC_RESULT_CONTAINS_NPDU = 0x1000
}BVLC_RESULT_CODE;

// bacnet NPDU tip poruke za BACNET_NPDU_DATA
typedef enum BACnetNPDUDataType{
              NETWORK_MESSAGE_WHO_IS_ROUTER_TO_NETWORK = 0,
              NETWORK_MESSAGE_I_AM_ROUTER_TO_NETWORK = 1,
              NETWORK_MESSAGE_I_COULD_BE_ROUTER_TO_NETWORK = 2,
              NETWORK_MESSAGE_REJECT_MESSAGE_TO_NETWORK = 3,
              NETWORK_MESSAGE_ROUTER_BUSY_TO_NETWORK = 4,
              NETWORK_MESSAGE_ROUTER_AVAILABLE_TO_NETWORK = 5,
              NETWORK_MESSAGE_INIT_RT_TABLE = 6,
              NETWORK_MESSAGE_INIT_RT_TABLE_ACK = 7,
              NETWORK_MESSAGE_ESTABLISH_CONNECTION_TO_NETWORK = 8,
              NETWORK_MESSAGE_DISCONNECT_CONNECTION_TO_NETWORK = 9,
              /* X'0A' to X'7F': Reserved for use by ASHRAE, */
              /* X'80' to X'FF': Available for vendor proprietary messages */
              NETWORK_MESSAGE_INVALID = 0x100
} BACNET_NPDU_MESSAGE_TYPE;


typedef struct BACNET_NPDU_DATA
{
 uint8_t protocolVersionNumber;
 uint8_t hopCount;
 bool isExpectingReply;
 bool network_layer_message;
 //bool dpresent;   // DNET,DLEN,DADR i hopcount
 //bool spresent;   // SNET,SLEN,SADR
 int  priority;
 BACNET_NPDU_MESSAGE_TYPE npduMessageType;    // kako je opisano u poglavlju 6.4. (opcionalno polje)

 BACNET_ADDRESS *dadr;   // 6.2.2 Network Layer Protocol Control Information
 BACNET_ADDRESS *sadr;   // 6.2.2 Network Layer Protocol Control Information

 uint16_t dnet;   // HACK: upotrebljava se samo kod routera dok ne promjenim *dadr u dadr gdje se provjerava dard.net == 0 ili 65535
 uint16_t snet;   // HACK: upotrebljava se samo kod routera dok ne promjenim *sadr u sdadr gdje se provjerava sard.net == 0 ili 65535

 BACNET_ADDRESS *remote;

 BACNET_DATALINK dataLink;  // BAC/IP ili ETH802 - BAC/IP = true, EHT802 = false
 uint16_t vendor_id; // ovo se ispunjava samo ako je ovaj npdu network_layer_message == true
 bool mstp_slave;  // da li je ovo slave device

}BACNET_NPDU_DATA;





typedef struct BACnet_BVLC_BDT_Entry
{
 uint8_t address[4];
 uint16_t port;
 uint8_t mask[4];
}BACNET_BVLC_BDT_ENTRY;



typedef struct BACnet_BVLC_BDT
{
 BACNET_ADDRESS *source;
 vector<BACNET_BVLC_BDT_ENTRY>vTableEntryList;
}BACNET_BVLC_BDT;



typedef struct BACnet_BVLC_FDT_Entry
{
 uint8_t address[4];
 uint16_t port;
 uint16_t ttl;
 uint16_t time_remaining;
}BACNET_BVLC_FDT_ENTRY;

typedef struct BACnet_BVLC_FDT
{
 vector<BACnet_BVLC_FDT_Entry*>vTableEntryList;
}BACNET_BVLC_FDT;



typedef struct BACNET_BVLC
{
 uint8_t  bacnet_vlc_type;       // samo BACNET_BVLC_TYPE_BACNET_IP za sada
 uint8_t  bacnet_vlc_function;   // sto ce se izvrsiti. Jedna od gore navedenih
 uint16_t bacnet_vlc_len;
}BACNET_BVLC;


typedef struct BACNET_LLC  // Logical-Link Control. -koristi se kod BACNET-Ethernet protokola
{
 uint8_t  bacnet_llc_type;       // samo BACNET_BVLC_TYPE_BACNET_IP za sada
 uint8_t  bacnet_llc_function;   // sto ce se izvrsiti. Jedna od gore navedenih
 uint8_t  bacnet_llc_control_field;
}BACNET_LLC;



#endif
