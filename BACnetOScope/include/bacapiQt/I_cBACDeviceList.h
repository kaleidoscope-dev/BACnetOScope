#pragma once
#include "bacnet.h"
//#include "I_cDeviceList.h"

#include <vector>


class I_cBACDeviceList//: public I_cDeviceList
{
 public:
        I_cBACDeviceList();
       ~I_cBACDeviceList();


        void AddDevice(BACNET_DEVICE *dev);
        void AddObject(BACNET_DEVICE *dev, BACNET_OBJECT *obj);
        void AddProperty(BACNET_DEVICE *dev, BACNET_OBJECT *obj, BACNET_OBJECT_PROPERTY *prop);
        bool DeviceExists(BACNET_DEVICE *dev);
        virtual int GetDeviceCount(void);
        int GetObjectCount(BACNET_DEVICE *dev);
        void DirtyClearDeviceList(void);
        void ClearDeviceList(void);
        virtual BACNET_DEVICE *GetDevice(int pos);
        BACNET_DEVICE *GetDevice(uint32_t instance, BACNET_ADDRESS *adr);
        BACNET_DEVICE *GetDevice(uint32_t instance, uint16_t nn);
        BACNET_DEVICE *GetParentDevice(BACNET_OBJECT_ID oid);
        BACNET_DEVICE *GetParentDevice(BACNET_OBJECT *obj);
        BACNET_OBJECT *GetParentObject(BACNET_OBJECT_PROPERTY *prop);

        void DeleteDevice(BACNET_DEVICE *dev);
        void DeleteObject(BACNET_OBJECT *obj);
				void DeleteAllObjects(BACNET_DEVICE *dev);
        void DeleteProperty(BACNET_OBJECT_PROPERTY *prop);

        // router
        void AddRouter(BACNET_ROUTER *router);
        BACNET_ROUTER *GetRouter(int num);
        int GetRouterCount(void);
        bool RouterExists(BACNET_ROUTER *router);
        void ClearRouterList(void);

 private:
         bool CompareNPDUData(BACNET_DEVICE *dev1, BACNET_DEVICE *dev2);
         bool CompareAddressData(BACNET_ADDRESS *adr1, BACNET_ADDRESS *adr2);


 private:
         vector<BACNET_DEVICE*>vDeviceList;
         vector<BACNET_ROUTER*>vRouterList;
};



extern I_cBACDeviceList *gDeviceList;  // device tree
extern I_cBACDeviceList *mDeviceList;
