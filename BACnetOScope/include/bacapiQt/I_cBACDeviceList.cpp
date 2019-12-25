#include "I_cBACDeviceList.h"
#include "bacapi.h"
#include <string.h>

I_cBACDeviceList *gDeviceList = NULL;
I_cBACDeviceList *mDeviceList = NULL;



I_cBACDeviceList::I_cBACDeviceList()
{
}


I_cBACDeviceList::~I_cBACDeviceList()
{
}








void I_cBACDeviceList::AddDevice(BACNET_DEVICE *dev)
{
 this->vDeviceList.push_back(dev);
}


void I_cBACDeviceList::AddObject(BACNET_DEVICE *dev, BACNET_OBJECT *obj)
{
 dev->vObjects.push_back(obj);
}


void I_cBACDeviceList::AddProperty(BACNET_DEVICE *dev, BACNET_OBJECT *obj, BACNET_OBJECT_PROPERTY *prop)
{
}


bool I_cBACDeviceList::DeviceExists(BACNET_DEVICE *dev)
{
 bool ret = false;
 int cnt = (int)vDeviceList.size();

 for (int c=0;c<cnt;c++)
     {
      BACNET_DEVICE *ptrDevice = vDeviceList[c];

      // ako device imaju isti oid
      if (dev->oid.instance == ptrDevice->oid.instance)
         {
          // provjeri da li imaju istu source adresu
          //printf("I_cBACDeviceList::DeviceExists: instance %d == instance %d\n",dev->oid.instance,ptrDevice->oid.instance);

          ret = CompareAddressData(dev->npduData->remote,ptrDevice->npduData->remote);
          if (ret == true)
             {
               // ako imaju ustu adresu vrati da uredjaj postoji
              return true;
             }
         }
     }

 //printf("vracam false\n");
 return ret;
}



// ako su len i  mac_len == 0 onda se usporedjuju mstp uredjaji
bool I_cBACDeviceList::CompareAddressData(BACNET_ADDRESS *adr1, BACNET_ADDRESS *adr2)
{
 if (!adr1 || !adr2) return false;


 uint8_t *ptr_adr1 = (uint8_t*)adr1;
 uint8_t *ptr_adr2 = (uint8_t*)adr2;

 for (int c=0;c<sizeof(BACNET_ADDRESS);c++)
     {
      if ((*ptr_adr1++) != (*ptr_adr2++))
         {
          return false;
         }
     }


 return true;
}




bool I_cBACDeviceList::CompareNPDUData(BACNET_DEVICE *dev1, BACNET_DEVICE *dev2)
{
 BACNET_NPDU_DATA *data1 = dev1->npduData;
 BACNET_NPDU_DATA *data2 = dev2->npduData;


 if (data1 && data2)
     if (memcmp(data1->remote,data2->remote,sizeof(BACNET_NPDU_DATA)) != 0)
         return false;

     //if (data1->remote->len == data2->remote->len)
     //   {
     //    for (int c=0;c<data1->remote->len;c++)
     //        {
     //         if (data1->remote->adr[c] != data2->remote->adr[c])
     //            {
     //             return false;  // adrese nisu iste
     //            }
     //        }
     //   }


 return true;  // adrese su iste
}


BACNET_DEVICE *I_cBACDeviceList::GetDevice(uint32_t instance, BACNET_ADDRESS *adr)
{
 int cnt = vDeviceList.size();
 for (int c=0;c<cnt;c++)
     {
      BACNET_DEVICE *ptrDevice = vDeviceList[c];

      if (CompareAddressData(ptrDevice->npduData->remote,adr) == true)
         {
          return ptrDevice;
         }

     }
  return NULL;
}


BACNET_DEVICE *I_cBACDeviceList::GetDevice(uint32_t instance, uint16_t nn)
{
 int cnt = vDeviceList.size();
 for (int c=0;c<cnt;c++)
     {
      BACNET_DEVICE *ptrDevice = vDeviceList[c];
      if (ptrDevice->npduData->remote->networkNumber == nn)
         {
          if (ptrDevice->oid.instance == instance)
             {
              return ptrDevice;
             }
         }
     }
  return NULL;
}





int I_cBACDeviceList::GetDeviceCount(void)
{
 return (int)vDeviceList.size();
}

int I_cBACDeviceList::GetObjectCount(BACNET_DEVICE *dev)
{
 if (dev)
     return (int)dev->vObjects.size();

 return 0;
}


void I_cBACDeviceList::ClearDeviceList(void)
{
 int devCnt = (int)vDeviceList.size();

 for (int c=0;c<devCnt;c++)
     {
      BACNET_DEVICE *device = vDeviceList[c];
      BACAPI_DeleteDevice(device);
     }
 vDeviceList.clear();
}


void I_cBACDeviceList::ClearRouterList(void)
{
 int cnt = (int)vRouterList.size();

 for (int c=0;c<cnt;c++)
     {
      BACNET_ROUTER *r = vRouterList[c];
      delete r;
     }
 vRouterList.clear();
}



void I_cBACDeviceList::DirtyClearDeviceList(void)
{
 vDeviceList.clear();
}



BACNET_DEVICE *I_cBACDeviceList::GetDevice(int pos)
{
 if (GetDeviceCount() == 0) 
     return NULL;

 if (pos < 0 || pos > this->GetDeviceCount())
     return NULL;

 return vDeviceList[pos];
}


// NOTE: Sto ako postoje dva razlicita devica s istim objekt oidom?
// TODO: promjeniti u BACNET_OBJECT * - obavezno. i provjeriti sto ce se dogoditi.
BACNET_DEVICE *I_cBACDeviceList::GetParentDevice(BACNET_OBJECT_ID oid)
{
 int dev_count = GetDeviceCount();
 for (int c=0;c<dev_count;c++)
     {
      BACNET_DEVICE *dev = vDeviceList[c];
      int obj_count = (int)dev->vObjects.size();
      for (int i=0;i<obj_count;i++)
          {
           BACNET_OBJECT *object = dev->vObjects[i];
           if (object->oid.instance == oid.instance && object->oid.type == oid.type)
              {
               return dev;
              }
          }
     }
 return NULL;
}



BACNET_DEVICE *I_cBACDeviceList::GetParentDevice(BACNET_OBJECT *obj)
{
 int dev_count = GetDeviceCount();
 for (int c=0;c<dev_count;c++)
     {
      BACNET_DEVICE *dev = vDeviceList[c];
      int obj_count = (int)dev->vObjects.size();
      for (int i=0;i<obj_count;i++)
          {
           BACNET_OBJECT *object = dev->vObjects[i];
           if (obj == object)
              {
               return dev;
              }
          }
     }
 return NULL;
}



BACNET_OBJECT *I_cBACDeviceList::GetParentObject(BACNET_OBJECT_PROPERTY *prop)
{
 int dev_count = GetDeviceCount();
 for (int c=0;c<dev_count;c++)
     {
      BACNET_DEVICE *dev = vDeviceList[c];
      int obj_count = (int)dev->vObjects.size();
      for (int i=0;i<obj_count;i++)
          {
           BACNET_OBJECT *object = dev->vObjects[i];
           size_t prop_count = object->vProperties.size();
           for (int j=0;j<prop_count;j++)
               {
                BACNET_OBJECT_PROPERTY *ptr_prop = object->vProperties[j];
                if (ptr_prop == prop)
                   {
                    return object;
                   }
               }
          }
     }
}



void I_cBACDeviceList::DeleteDevice(BACNET_DEVICE *dev)
{
 int dev_count = GetDeviceCount();
 for (int c=0;c<dev_count;c++)
     {
      BACNET_DEVICE *ptr_dev = vDeviceList[c];
      if (ptr_dev == dev)
         {
          BACAPI_DeleteDevice(dev);
          vDeviceList.erase(vDeviceList.begin()+c);
         }
     }
}



void I_cBACDeviceList::DeleteObject(BACNET_OBJECT *obj)
{
 int dev_count = GetDeviceCount();
 for (int c=0;c<dev_count;c++)
     {
      BACNET_DEVICE *dev = GetDevice(c);
      int obj_count = GetObjectCount(dev);

      for (int i=0;i<obj_count;i++)
          {
           BACNET_OBJECT *ptr_obj = dev->vObjects[i];
           if (ptr_obj == obj)
              {
               BACAPI_DeleteObject(ptr_obj);
               dev->vObjects.erase(dev->vObjects.begin()+i);
              }
          }
     }
}


void I_cBACDeviceList::DeleteAllObjects(BACNET_DEVICE *dev)
{
	int obj_count = GetObjectCount(dev);

	for (int i = 0; i < obj_count; i++)
	{
		BACNET_OBJECT *ptr_obj = dev->vObjects[i];
		BACAPI_DeleteObject(ptr_obj);
	}

	dev->vObjects.clear();
}






void I_cBACDeviceList::DeleteProperty(BACNET_OBJECT_PROPERTY *prop)
{
 int dev_count = GetDeviceCount();
 for (int c=0;c<dev_count;c++)
     {
      BACNET_DEVICE *dev = GetDevice(c);
      int obj_count = GetObjectCount(dev);

      for (int i=0;i<obj_count;i++)
          {
           BACNET_OBJECT *obj = dev->vObjects[i];
           int prop_count = obj->vProperties.size();

           for (int j=0;j<prop_count;j++)
               {
                BACNET_OBJECT_PROPERTY *ptr_prop = obj->vProperties[j];

                if (ptr_prop == prop)
                   {
                    BACAPI_DeleteObjectProperty(prop);
                    obj->vProperties.erase(obj->vProperties.begin()+j);
                   }
               }
          }
     }
}


int I_cBACDeviceList::GetRouterCount(void)
{
 return (int)vRouterList.size();
}


void I_cBACDeviceList::AddRouter(BACNET_ROUTER *router)
{
 vRouterList.push_back(router);
}

BACNET_ROUTER *I_cBACDeviceList::GetRouter(int num)
{
 if (num < 0 || num > GetRouterCount())
     return NULL;

 return vRouterList[num];
}


bool I_cBACDeviceList::RouterExists(BACNET_ROUTER *router)
{
 int cnt = GetRouterCount();

 for (int c=0;c<cnt;c++)
     {
      BACNET_ROUTER *ptr = GetRouter(c);

      if (CompareAddressData(ptr->router_address,router->router_address))
         {
          return true;
         }
     }
 return false;
}







