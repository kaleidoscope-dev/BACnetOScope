#include "I_cObjectRead.h"
#include <include/bacapiQt/bacapi.h>
#include <include/bacapiQt/bacevent.h>


I_cObjectRead::I_cObjectRead(QObject *parent)
{
}


void I_cObjectRead::Init(I_cBACClientQt *client, BACNET_DEVICE *dev, BACNET_OBJECT *obj, int count)
{
 count_ = count;
 device_ = dev;
 client_ = client;
 object_ = obj;
}

void I_cObjectRead::SetReadOperation(READ_OPERATION ro)
{
 operation_ = ro;
}


void I_cObjectRead::run()
{




 switch(operation_)
       {
			  case READ_ARRAY_INDEX:
				{
					for (int c = 1; c < count_; c++)
					{
						QMutex sleepMutex;

						sleepMutex.lock();
						//QMutexLocker locker(&sleepMutex);

again:
						BACAPI_INVOKE_ID *invoke = BACAPI_ReserveInvokeId(device_, NULL, NULL);
						// TODO: negdje se ne otpustaju invoci, pokusaj sada ovaj hack napravit
						if (invoke == NULL)
						   {
							  BACAPI_ReleaseInvokeIdAll();
								goto again;
							 }
						std::string str_adr  = BACAPI_GetAddressStringIP(device_->npduData->remote->adr);
						QHostAddress dest_adr(str_adr.c_str());
						client_->ReadProperty(&device_->oid, PROP_OBJECT_LIST, c, invoke->invokeId, NULL, device_->npduData->sadr, dest_adr);

						if (waitcondition.wait(&sleepMutex, 5000) == false)
						{
						  // Izgubljena veza sa deviceom, unlock mutex i prekini radnju
							qDebug() << "TIMEOUT";
							sleepMutex.unlock();
							return;
						}

						sleepMutex.unlock();
					}
					// zavrsili smo citanje, posalji poruku da smo gotovi
					emit Handle_ObjectRead_End();
					break;
				 }

				case READ_PROPERTY:
				{
				 BACNET_PROPERTY_ID *list = BACAPI_GetPropertyList((BACNET_OBJECT_TYPE)object_->oid.type);
					int propCount = BACAPI_GetPropertyCount((BACNET_OBJECT_TYPE)object_->oid.type);

					
					for (int c = 0; c < propCount; c++)
					{
						QMutex sleepMutex;
						sleepMutex.lock();

						BACNET_PROPERTY_ID property = list[c];
						BACAPI_INVOKE_ID *invoke = BACAPI_ReserveInvokeId(device_, object_, NULL);
						std::string str_adr = BACAPI_GetAddressStringIP(device_->npduData->remote->adr);
						QHostAddress dest_adr(str_adr.c_str());
						client_->ReadProperty(&object_->oid, property, -1, invoke->invokeId, NULL, device_->npduData->sadr, dest_adr);
						if (waitcondition.wait(&sleepMutex, 5000) == false)
						{
							// Izgubljena veza sa deviceom, unlock mutex i prekini radnju
							qDebug() << "TIMEOUT";
							sleepMutex.unlock();
							return;
						}
						sleepMutex.unlock();
					}

					// zavrsili smo citanje, posalji poruku da smo gotovi
					emit Handle_PropertyRead_End();
				 break;	
				}
			 }

			 
 
}


