#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_BACnetOScope.h"

#include <QElapsedTimer>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlerror.h>
#include <QTouchEvent>

#include "include/bacapiQt/I_cBACNet.h"
#include "include/bacapiQt/I_cWiresharkIO.h"
#include "include/bacapiQt/I_cBACLog.h"
#include "include/bacapiQt/I_cBACDeviceList.h"
#include "include/bacapiQt/I_cBACdec.h"

#include "include/I_cObjectRead.h"
#include "include/I_cEventThread.h"
#include "include/I_cObjectRead.h"


#define DIRECTION_LEFT -1
#define DIRECTION_RIGHT 1
#define DIRECTION_UP -1
#define DIRECTION_DOWN 1
#define DIRECTION_SPEED 500
#define DOUBLE_CLICK_TIMER 500

typedef enum
{
	SELECTED_VIEW_DEVICE,
	SELECTED_VIEW_OBJECT,
	SELECTED_VIEW_PROPERTY,
	SELECTED_VIEW_VALUE,
	SELECTED_VIEW_WRITE
}SELECTED_VIEW;

// Pamtimo podatke od proslog pokretanja
typedef struct Settings
{
	bool whois;
	bool names;
	uint16_t port;
	QString lastadr;
	bool wire;
}SETTINGS;


class BACnetOScope : public QMainWindow
{
    Q_OBJECT

	public:
		   BACnetOScope(QWidget *parent = Q_NULLPTR);

	private:	
			bool firstReadArrayFlag_;

			BACNET_DEVICE *GetSelectedDevice(void);
			BACNET_OBJECT *GetSelectedObject(void);
			void RefreshPropertyTree(BACNET_OBJECT *object);
			void RefreshPropertyValues(BACNET_OBJECT_PROPERTY *prop);
			void RefreshDeviceInfo(BACNET_DEVICE *dev);
			void StartObjectRead(int count);
			void StartPropertyRead(BACNET_OBJECT *obj);
			void MoveControls(int direction);
			void MoveValueControls();
			void MoveSettinsControls(int direction);
			void MoveDeviceInfo(int direction);
			void LoadSettings(void);
			int GetPercentage(int num, int p);
			void SaveSettings(void);

			bool event(QEvent *event) override;
			bool eventFilter(QObject *, QEvent*) override;


	private:
			Ui::BACnetOScopeClass ui;
			Ui::BACnetOScopeClass* ptr_ui;  

			I_cBACnet *bacnet;
			I_cBACClientQt *client;
			I_cObjectRead *readThread;
			I_cEventThread* eventThread;

			QTreeWidget *ptr_deviceTree;
			QTreeWidget *ptr_objectTree;
			QTreeWidget *ptr_propertyTree;
			QTreeWidget *ptr_valueTree;
			QTreeWidget *ptr_infoTree;
			QPushButton *ptr_buttonLeft;
			QPushButton *ptr_buttonRight;
			QProgressBar *ptr_pbObjectRead;
			QProgressBar *ptr_pbPropRead;
			QPushButton *ptr_buttonSettings;
			QLabel *ptr_labelReturnValue;
			QLabel *ptr_labelSplash;
			QCheckBox *ptr_cbWhoIsOnStart;
			QCheckBox *ptr_cbObjectNames;
			QCheckBox *ptr_cbWireshark;
			QLineEdit *ptr_editPort;
			QLineEdit *ptr_regAddress;
			QComboBox *ptr_comboRegTime;
			QPushButton *ptr_buttonRegister;
			QComboBox *ptr_cbPriority;

			QPixmap dev_pixmap;
			QPixmap obj_pixmap;
			QPixmap prop_pixmap;
			QPixmap val_pixmap;
			QPixmap refresh_pixmap;
			QPixmap sett_pixmap;
			QPixmap splash_pixmap;
			QPixmap devinfo_pixmap;
			QPixmap backtodev_pixmap;
			QPixmap backtoobj_pixmap;
			QPixmap backtoprop_pixmap;
			QPixmap backtoval_pixmap;
			QPixmap write_pixmap;

			QIcon dev_icon;
			QIcon obj_icon;
			QIcon prop_icon;
			QIcon val_icon;
			QIcon refresh_icon;
			QIcon sett_icon;
			QIcon devinfo_icon;
			QIcon backtodev_icon;
			QIcon backtoobj_icon;
			QIcon backtoprop_icon;
			QIcon backtoval_icon;
			QIcon write_icon;

			QSqlDatabase database_;

			QElapsedTimer elapsedTimer;

			SELECTED_VIEW currentView_;
			BACNET_DEVICE *currentDevice_;
			BACNET_OBJECT *currentObject_;
			BACNET_OBJECT_PROPERTY *currentProperty_;
			SETTINGS settings_;

			vector<QWidget*>vValuesWidgetsList;
			vector<QWidget*>vSettingsWidgetsList;


			bool readingObjectFlag_;
			bool readingPropertyFlag_;
			int doubleClickTimer_;


			int directionStack_;
			int window_w_;
			int window_h_;
			int rp_property_count_;   // kod progress bara

			// koristimo kod OnClick pointera za dvije navigacijske tipke.
			// U odnosu na view prikazan, remapirat cemo konekcije
			QMetaObject::Connection deviceInfoConnection_;   // kada je prikazan Object List
			QMetaObject::Connection backFromdeviceInfoConnection_;   // kada je prikazan Object List
			QMetaObject::Connection backToDeviceListConnection_;   // kada je prikazan Object List
			QMetaObject::Connection backToObjectListConnection_;   // kada je prikazan Property List
			QMetaObject::Connection backToPropertyListConnection_; // kada je prikazan value list
			QMetaObject::Connection backToValueListConnection_;    // kada je prikazan Write controls
			QMetaObject::Connection whoIsConnection_;   // Who iS  Right button
			QMetaObject::Connection writeConnection_;   // write
			QMetaObject::Connection showSettingsConnection_;   // Left button on device list
			QMetaObject::Connection backFromSettingsConnection_;   // Left button, iz settings na dvice list



	private:
			void RefreshObjectsTree(BACNET_DEVICE* device);
			void RefreshDeviceObjectPropertyRef(BACNET_OBJECT_PROPERTY *prop);
			void RefreshDateTime(BACNET_OBJECT_PROPERTY *prop);
			void RefreshDeviceObjectReference(BACNET_OBJECT_PROPERTY *prop);
			void RefreshObjectPropertyReference(BACNET_OBJECT_PROPERTY *prop);
			void RefreshAckRequired(BACNET_OBJECT_PROPERTY *property);
			void RefreshBACDestination(BACNET_OBJECT_PROPERTY *property);
			void RefreshActionText(BACNET_OBJECT_PROPERTY *property);
			void RefreshCalendarDateList(BACNET_OBJECT_PROPERTY *property);
			void RefreshConfigurationFilesList(BACNET_OBJECT_PROPERTY *property);
			void RefreshCOVSubscriptions(BACNET_OBJECT_PROPERTY *property);
			void RefreshDateRange(BACNET_OBJECT_PROPERTY *property);
			void RefreshDeviceAddressBinding(BACNET_OBJECT_PROPERTY *property);
			void RefreshDeviceObjectList(BACNET_OBJECT_PROPERTY *property);
			void RefreshLoopObjectActionProperty(BACNET_OBJECT_PROPERTY *property);
			void RefreshReadAccessSpecification(BACNET_OBJECT_PROPERTY *property);
			void RefreshPriorityArray(BACNET_OBJECT_PROPERTY *property);
			void RefreshPropertyList(uint32_t property);
			void RefreshProtocolServicesSupported(BACNET_OBJECT_PROPERTY *property);
			void RefreshProtocolTypesSupported(BACNET_OBJECT_PROPERTY *property);
			void RefreshBitString(BACNET_BIT_STRING *bstr);
			void RefreshStatusFlags(BACNET_BIT_STRING *bstr);
			void RefreshStatusFlags(BACNET_OBJECT_PROPERTY *property);
			void RefreshWeeklySchedule(BACNET_OBJECT_PROPERTY *property);
			void RefreshPriority(BACNET_OBJECT_PROPERTY *property);
			void RefreshUnsignedValuesList(BACNET_OBJECT_PROPERTY *property);
			void RefreshCommandAction(BACNET_OBJECT_PROPERTY *property);
			void RefreshFaultParameters(BACNET_OBJECT_PROPERTY *property);
			void RefreshSpecialEvent(BACNET_OBJECT_PROPERTY *property);
			void RefreshRecipientList(BACNET_OBJECT_PROPERTY *property);
			void RefreshPreScale(BACNET_OBJECT_PROPERTY *property);
			void RefreshLoggingRecord(BACNET_OBJECT_PROPERTY *property);
			void RefreshScale(BACNET_OBJECT_PROPERTY *property);
			void RefreshEventTimeStamps(BACNET_OBJECT_PROPERTY *property);
			void RefreshHostNPort(BACNET_OBJECT_PROPERTY *property);
			void RefreshBACnetBDTEntry(BACNET_OBJECT_PROPERTY *property);
			void RefreshBACnetFDTEntry(BACNET_OBJECT_PROPERTY *property);
			void RefreshBACnetVMACEntry(BACNET_OBJECT_PROPERTY *property);
			void RefreshBACnetRouterEntry(BACNET_OBJECT_PROPERTY *property);
			void RefreshBACnetAssignedAccessRights(BACNET_OBJECT_PROPERTY *property);
			void RefreshCredentialAuthenticationFactors(BACNET_OBJECT_PROPERTY *property);
			void RefreshSecurityKeySets(BACNET_OBJECT_PROPERTY *property);
			void RefreshNetworkAccessSecurityPolicies(BACNET_OBJECT_PROPERTY *property);
			void RefreshValueSource(BACNET_OBJECT_PROPERTY *property);
			void RefreshEnumList(BACNET_OBJECT_PROPERTY *property);

	// Handle eventi iz event threada
	private:
			void HandleEvent_I_AM(BACNET_DEVICE* dev);
			void HandleEvent_RP_ACK(BACNET_OBJECT_PROPERTY* prop, int invoke_id);
			void HandleEvent_RP_ACK_ArrayElement(BACNET_OBJECT_PROPERTY* prop, int invoke_id);
			void HandleEvent_Error(QString err_str);
			void HandleEvent_Success();
			void HandleEvent_RFD_Success(void);
			void Handle_ObjectRead_End(void);
			void Handle_PropertyRead_End(void);


	private slots:
			void OnClick_WhoIs();
			void OnClick_DeviceTreeItem(QTreeWidgetItem* item, int column);
			void OnClick_ObjectTreeItem(QTreeWidgetItem* item, int column);
			void OnClick_PropertyTreeItem(QTreeWidgetItem* item, int column);
			void OnClick_ValueTreeItem(QTreeWidgetItem* item, int column);
			void OnClick_WriteValue(void);
			void OnTimer_FadeIntro(void);
			void OnTimer_FinishedFadeout(void);
			void OnTimer_TapAndHold(void);
			void TapAndHoldTriggered(void);

			// button left
			void OnClick_BackToDeviceList();
			void OnClick_BackToObjectList();
			void OnClick_BackToPropertyList();
			void OnClick_BackToValueList();
			void OnClick_ShowDeviceInfo(void);
			void OnClick_ShowSettings(void);
			void OnClick_BackFromDeviceInfo(void);
			void OnClick_BackFromSettings(void);
			void OnClick_RegisterFD(void);
				
			
};
