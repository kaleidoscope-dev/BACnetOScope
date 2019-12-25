#pragma once
#include <qthread.h>
#include "bacapiQt/bacnet.h"
#include "bacapiQt/I_cBACClientQt.h"
#include "qmutex.h"
#include "qwaitcondition.h"


typedef enum READ_OPERATION
{
 READ_ARRAY_INDEX = 0,
 READ_PROPERTY = 1,
 READ_COUNT
}READ_OPERATION;

class I_cObjectRead : public QThread
{
	Q_OBJECT

public:
	explicit I_cObjectRead(QObject *parent = 0);


	void Init(I_cBACClientQt *client, BACNET_DEVICE *dev, BACNET_OBJECT *obj, int count);
	void SetReadOperation(READ_OPERATION ro);

	QWaitCondition waitcondition;


	int count_;
	READ_OPERATION operation_;

	BACNET_DEVICE *device_;
	BACNET_OBJECT *object_;
	I_cBACClientQt *client_;
	//QMutex sleepMutex;

		void run(); 


signals:
	    void Handle_ObjectRead_End(void);
			void Handle_PropertyRead_End(void);
};
