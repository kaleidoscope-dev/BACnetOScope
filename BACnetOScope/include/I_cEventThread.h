#pragma once
#include <qthread.h>
#include "include/bacapiQt/bacnet.h"

class I_cEventThread : public QThread
{
	Q_OBJECT

public:
	explicit I_cEventThread(QObject *parent = 0);


		void run(); 
signals:
	void handleIAM(BACNET_DEVICE *dev);
	void handleRP_ACK(BACNET_OBJECT_PROPERTY *prop, int invokeId);
	void handleRP_ACK_ArrayElement(BACNET_OBJECT_PROPERTY *prop, int  invokeId);
	void handle_Error(QString err_str);
	void handle_Success(void);
	void handle_RFD_Success(void);
	//void handleRP_ACK_ArrayElement(BACNET_OBJECT_PROPERTY *prop);

};
