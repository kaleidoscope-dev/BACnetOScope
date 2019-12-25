#include "I_cBACClientQt.h"
#include "I_cWiresharkIO.h"
#include <QtNetwork/qnetworkinterface.h>
#include <QtNetwork/qhostinfo.h>


I_cBACClientQt::I_cBACClientQt(QObject *parent)
{
	port_ = 0;
	encoderNPDU_ = NULL;
	encoderAPDU_ = NULL;
	decoder_ = NULL;
}

/*
I_cBACClientQt::~I_cBACClientQt()
{
	//delete ace_;
}
*/
void I_cBACClientQt::local(void)
{


/*
	QString localhostname = QHostInfo::localHostName();
	QString localhostIP;
	QList<QHostAddress> hostList = QHostInfo::fromName(localhostname).addresses();
	foreach (const QHostAddress& address, hostList) 
	        {
					 QString str = address.toString();
		      if (address.protocol() == QAbstractSocket::IPv4Protocol && address.isLoopback() == false) 
					   {
			        localhostIP = address.toString();
			        local_adr_ = address;
		         }
	        }
*/



	QString localhostname = QHostInfo::localHostName();
	QString localhostIP;
	QString localMacAddress;
	QString localNetmask;
	QString localbadr;
	foreach(const QNetworkInterface& networkInterface, QNetworkInterface::allInterfaces()) 
         {
          foreach(const QNetworkAddressEntry& entry, networkInterface.addressEntries()) 
                 {
                  QHostAddress address = entry.ip();
                  
									if (address.protocol() == QAbstractSocket::IPv4Protocol && address.isLoopback() == false)
                     {
                      localhostIP = address.toString();
                      local_adr_ = address;
                      localMacAddress = networkInterface.hardwareAddress();
                      localNetmask = entry.netmask().toString();
                      localbadr = entry.broadcast().toString();
                      broadcast_adr_ = entry.broadcast();
                     }
                     
										/*
										QString str = entry.ip().toString();

										if (entry.ip().toString() == localhostIP) 
										{
										localMacAddress = networkInterface.hardwareAddress();
										localNetmask = entry.netmask().toString();
										localbadr = entry.broadcast().toString();
										broadcast_adr_ = entry.broadcast();
										break;
										}*/
                 }
          }

#ifdef WIN32
	localhostIP = "192.168.5.12";
	localbadr = "192.168.5.255";
	local_adr_.setAddress(localhostIP);
	broadcast_adr_.setAddress(localbadr);
#endif

	qDebug() << "Localhost name: " << localhostname;
	qDebug() << "IP = " << localhostIP;
	qDebug() << "Broadcast IP = " << localbadr;
	qDebug() << "MAC = " << localMacAddress;
	qDebug() << "Netmask = " << localNetmask;

}


BACAPI_ERROR I_cBACClientQt::Init(u_short port, I_cNPDUEncoder *npdu, I_cAPDUEncoder *apdu, I_cNPDUDecoder *decnpdu, I_cAPDUDecoder *decapdu, I_cDecoder *dec, bool wire)
{
 local();

 wire_ = wire;
	// create a QUDP socket
	socket = new QUdpSocket(this);
	port_ = port;

	// The most common way to use QUdpSocket class is 
	// to bind to an address and port using bind()
	// bool QAbstractSocket::bind(const QHostAddress & address, 
	//     quint16 port = 0, BindMode mode = DefaultForPlatform)


	bool ret = socket->bind(QHostAddress::Any, port_);
	 if (ret == false)
        {
			 printf("failed to bind socket\n");
			 return BACAPI_ERROR_INTERFACE_IP;
			}

	 connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));


	 encoderAPDU_ = apdu;
	 encoderNPDU_ = npdu;
	 decoderNPDU_ = decnpdu;
	 decoderAPDU_ = decapdu;
	 decoder_ = dec;

	return BACAPI_OK;
}



void I_cBACClientQt::WriteWire(uint8_t *buffer, QHostAddress src, QHostAddress dest, int size)
{
	//QHostAddress addr("192.168.5.255");
    if (wire_)
    {

	quint32 int_dest_adr = dest.toIPv4Address();
	BACNET_ADDRESS *bdest_adr = BACAPI_CreateBACnetAddress(int_dest_adr, 47808, 65535);

	qint32 int_src_adr = src.toIPv4Address();
	BACNET_ADDRESS *bsrc_adr = BACAPI_CreateBACnetAddress(int_src_adr, 47808, 65535);

     wireshark_->WritePDUBufferIP(buffer, bdest_adr->adr, bsrc_adr->adr, size);


	delete bdest_adr;
	delete bsrc_adr;
 }
}



BACAPI_ERROR I_cBACClientQt::WhoIs(uint32_t devInstanceFrom, uint32_t devInstanceTo, BACNET_ADDRESS *adr, bool params)
{

	int len = 4;   // NPDU kodiranje pocinje poslje BVLC-a
	uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
	BACNET_NPDU_DATA npduData = { 0 };


	encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL, 255);
	len += encoderNPDU_->EncodeNPDU(&buffer[len], &npduData, adr, NULL);
	len += encoderAPDU_->WhoIsAPDU(&buffer[len], devInstanceFrom, devInstanceTo, params);
	encoderNPDU_->EncodeBVLC(&buffer[0], len, BVLC_ORIGINAL_BROADCAST_NPDU);   // prva 4 byta su BVLC

 QByteArray data((char*)buffer,len);
 //QHostAddress addr("255.255.255.255");
 qint64 size = socket->writeDatagram(data, broadcast_adr_, port_);


  WriteWire(buffer,local_adr_, broadcast_adr_,size);





 if (size == 0 || size < 0)
	{
		return BACAPI_ERROR_CLIENT_SEND_FAILED;
	}
	
	return BACAPI_OK;
}





BACAPI_ERROR I_cBACClientQt::ReadProperty(BACNET_OBJECT_ID *oid, BACNET_PROPERTY_ID prop, int arrayIndex, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, QHostAddress dest)
{
	int len = 4;
	uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
	BACNET_NPDU_DATA npduData = { 0 };

	encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL, 255);
	len += encoderNPDU_->EncodeNPDU(&buffer[len], &npduData, npdu_dest, npdu_src);
	len += encoderAPDU_->ReadPropAPDU(&buffer[len], oid, prop, arrayIndex, invokeId, BACNET_MAX_APDU_IP);
	encoderNPDU_->EncodeBVLC(buffer, len, BVLC_ORIGINAL_UNICAST_NPDU);


	QByteArray data((char*)buffer, len);
	//QHostAddress addr("192.168.5.13");
	qint64 size = socket->writeDatagram(data, dest, port_);

	if (size == 0 || size < 0)
	{
		//delete sendToAddr;
		return BACAPI_ERROR_CLIENT_SEND_FAILED;

	}

	WriteWire(buffer, local_adr_, dest, size);




	//delete sendToAddr;
	return BACAPI_OK;

}




BACAPI_ERROR I_cBACClientQt::WriteProperty(BACNET_OBJECT_ID *oid, BACNET_OBJECT_PROPERTY *prop, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, QHostAddress dest)
{
	int len = 4;
	uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
	BACNET_NPDU_DATA npduData = { 0 };

	encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL, 255);
	len += encoderNPDU_->EncodeNPDU(&buffer[len], &npduData, npdu_dest, npdu_src);
	len += encoderAPDU_->EncodeWritePropAPDU(&buffer[len], oid, prop, invokeId, BACNET_MAX_APDU_IP);
	encoderNPDU_->EncodeBVLC(buffer, len, BVLC_ORIGINAL_UNICAST_NPDU);


	QByteArray data((char*)buffer, len);
	//QHostAddress addr("192.168.5.13");
	qint64 size = socket->writeDatagram(data, dest, port_);

	if (size == 0 || size < 0)
	{
		//delete sendToAddr;
		return BACAPI_ERROR_CLIENT_SEND_FAILED;

	}

	WriteWire(buffer, local_adr_, dest, size);


	return BACAPI_OK;

}



BACAPI_ERROR I_cBACClientQt::RegisterFD(QHostAddress dest, uint16_t ttl)
{
	int len = 0;
	uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };

	len += encoderNPDU_->EncodeBVLC_RegisterFD(&buffer[len], ttl);   // prva 4 byta su BVLC


	QByteArray data((char*)buffer, len);
	//QHostAddress addr("192.168.5.13");
	qint64 size = socket->writeDatagram(data, dest, port_);

	if (size == 0 || size < 0)
	{
		return BACAPI_ERROR_CLIENT_SEND_FAILED;
	}

	return BACAPI_OK;


}





void I_cBACClientQt::readyRead()
{

  while(socket->hasPendingDatagrams())
	     {
	      QByteArray buffer;
	      buffer.resize(socket->pendingDatagramSize());

	      QHostAddress sender;
	      quint16 senderPort;

	      socket->readDatagram(buffer.data(), buffer.size(),&sender , &senderPort);

				uint32_t loc = local_adr_.toIPv4Address();
				uint32_t from = sender.toIPv4Address();
				
				if (loc == from)
						return;

	      uint8_t *data = new uint8_t[buffer.count()];
				for (int c = 0; c < buffer.size(); c++)
				    {
				     data[c] = (uint8_t)buffer.at(c);
				    }

				WriteWire(data, sender, local_adr_,buffer.count());
				
				BACNET_ADDRESS *src_adr = BACAPI_CreateBACnetAddress(sender.toIPv4Address(), 65535, 0);
      	decoder_->HandleDataPacket(data, *src_adr, buffer.count(), BACNET_DATALINK_BACIP);


			}

}



