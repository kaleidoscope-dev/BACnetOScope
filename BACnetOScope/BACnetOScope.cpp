#include "BACnetOScope.h"
#include <qpropertyanimation.h>
#include <qlabel.h>
#include <qscreen.h>
#include <qscroller.h>
#include <QTapAndHoldGesture>
#include <QGestureEvent>
#include <qtimer.h>
#include <QGraphicsOpacityEffect>
#include <qcombobox.h>
#include <qlistview.h>
#include <qstandardpaths.h>
#include <qiodevice.h>
#include <qmessagebox.h>
#include <qscrollbar.h>
#include <qmetatype.h>


#define USE_ITEMS  // da li je bolje kotistiti QWidgetItem ili QLabel za presentaciju podataka?

// moramo deklarirati ako zelimo prenositi podatke preko QVariant objekta
Q_DECLARE_METATYPE(BACNET_DEVICE*)
Q_DECLARE_METATYPE(BACNET_OBJECT*)
Q_DECLARE_METATYPE(BACNET_APPLICATION_DATA_VALUE*)
Q_DECLARE_METATYPE(BACNET_OBJECT_PROPERTY*)


#ifdef __ANDROID__
BACnetOScope::~BACnetOScope()
{
    delete ui;
}
#endif

#ifdef __ANDROID__
BACnetOScope::BACnetOScope(QWidget *parent) : QMainWindow(parent), ui(new Ui::BACnetOScope)
#else
BACnetOScope::BACnetOScope(QWidget *parent) : QMainWindow(parent)
#endif
{

// Qt za Android koristi objekt za UI, ali ne za Windows. 
// Ujedinimo sve pointere na UI podatke pod jedinstveno ptr_ varijable da nemoramo 
// konstantno pisati #ifdef
#ifdef __ANDROID__
    ui->setupUi(this);

    ptr_ui = ui;

    ptr_deviceTree = ui->deviceTree;					  // dev tree
    ptr_objectTree = ui->objectTree;            // obj
    ptr_propertyTree = ui->propertyTree;        // prop tree
    ptr_valueTree = ui->valueTree;              // val tree
    ptr_buttonLeft = ui->buttonLeft;            // butt left
    ptr_buttonRight = ui->buttonRight;          // butt right
    ptr_pbObjectRead = ui->pbObjectRead;        // progress obj
    ptr_pbPropRead = ui->pbPropRead;            // progress prop



    ptr_deviceTree = ui->deviceTree;
    ptr_objectTree = ui->objectTree;
    ptr_propertyTree = ui->propertyTree;
    ptr_valueTree = ui->valueTree;
    ptr_infoTree = ui->deviceInfoTree;
    ptr_buttonLeft = ui->buttonLeft;
    ptr_buttonRight = ui->buttonRight;
    ptr_pbObjectRead = ui->pbObjectRead;
    ptr_pbPropRead = ui->pbPropRead;
    ptr_comboRegTime = ui->comboRegisterTime;

    ptr_cbWhoIsOnStart = ui->chWhoIsOnStart;
    ptr_regAddress = ui->editRegisterAddress;
    ptr_buttonRegister = ui->buttonRegister;
    ptr_editPort = ui->editPort;
    ptr_cbObjectNames = ui->cbShowObjectNames;
    ptr_cbWireshark = ui->cbWireshark;
    ptr_cbPriority = ui->comboWritePriority;
    ptr_labelSplash = ui->labelSplash;
#else
    ui.setupUi(this);

    ptr_ui = &ui;

    //ptr_deviceTree = ui.;
    ptr_deviceTree = ui.deviceTree;
    ptr_objectTree = ui.objectTree;
    ptr_propertyTree = ui.propertyTree;
    ptr_valueTree = ui.valueTree;
    ptr_infoTree = ui.deviceInfoTree;
    ptr_buttonLeft = ui.buttonLeft;
    ptr_buttonRight = ui.buttonRight;
    ptr_pbObjectRead = ui.pbObjectRead;
    ptr_pbPropRead = ui.pbPropRead;
    ptr_comboRegTime = ui.comboRegisterTime;


    ptr_cbWhoIsOnStart = ui.chWhoIsOnStart;
    ptr_regAddress = ui.editRegisterAddress;
    ptr_buttonRegister = ui.buttonRegister;
    ptr_editPort = ui.editPort;
    ptr_cbObjectNames = ui.cbShowObjectNames;
    ptr_cbWireshark = ui.cbWireshark;
    ptr_cbPriority = ui.comboWritePriority;
    ptr_labelReturnValue = ui.labelReturnValue;
    ptr_labelSplash = ui.labelSplash;

    ptr_ui->mainToolBar->hide();
#endif

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint); // onemoguci resize window na Windows
        
    vValuesWidgetsList.push_back(ptr_ui->labelType);
    vValuesWidgetsList.push_back(ptr_ui->comboWritePriority);
    vValuesWidgetsList.push_back(ptr_ui->editNewValue);
    vSettingsWidgetsList.push_back(ptr_ui->chWhoIsOnStart);
    vSettingsWidgetsList.push_back(ptr_ui->cbShowObjectNames);
    vSettingsWidgetsList.push_back(ptr_ui->labelPort);
    vSettingsWidgetsList.push_back(ptr_ui->editPort);
    vSettingsWidgetsList.push_back(ptr_ui->comboRegisterTime);
    vSettingsWidgetsList.push_back(ptr_ui->editRegisterAddress);
    vSettingsWidgetsList.push_back(ptr_ui->buttonRegister);
    vSettingsWidgetsList.push_back(ptr_cbWireshark);


    QString wiresharkFilePath;
    QString logFilePath;

#ifdef __ANDROID__
    wiresharkFilePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    logFilePath = wiresharkFilePath + "/baclog.log";
    wiresharkFilePath += "/output.pcap";
#else
    logFilePath = "baclog.log";
    wiresharkFilePath = "output.pcap";
#endif

    settings_.lastadr = "127.0.0.1";
    settings_.whois = false;
    settings_.names = false;
    settings_.port = 47808;
    settings_.wire = false;


    QScrollBar *sb = ptr_objectTree->horizontalScrollBar();
    sb->hide();

    directionStack_ = 0; // 0 == SELECTED_VIEW_DEVICE


    // Init bacnet stvari
    BACAPI_Init();
    gDeviceList = new I_cBACDeviceList();
    I_cBACnet *bacnet = new I_cBACnet();
    bacnet->Init(false);

    gLog = new I_cBACLog();
    gLog->SetDebugMode(FILE_MODE);
    gLog->SetDebugFile(logFilePath.toStdString());
    gLog->ResetLog();

    // port samo od 0-65535
    ptr_editPort->setValidator(new QIntValidator(0, 65535, this));

    LoadSettings();

    // zatrazi qt bacnet client
    client = bacnet->CreateQtIPClient(settings_.port, settings_.wire);
    if (client == NULL)
       {
        QMessageBox mb;
        mb.setText("Failed to create BACnet Client");
        mb.exec();
        return;
       }

    eventThread = new I_cEventThread(this);
    readThread = new I_cObjectRead(this);


    eventThread->start();



    // postavi konekcije
    showSettingsConnection_ = connect(ptr_buttonLeft, SIGNAL(clicked()), this, SLOT(OnClick_ShowSettings()));
    whoIsConnection_ = connect(ptr_buttonRight, SIGNAL(clicked()), this, SLOT(OnClick_WhoIs()));

    connect(ptr_buttonRegister, SIGNAL(clicked()), this, SLOT(OnClick_RegisterFD()));

    connect(eventThread, &I_cEventThread::handleIAM, this, &BACnetOScope::HandleEvent_I_AM);
    connect(eventThread, &I_cEventThread::handleRP_ACK_ArrayElement, this, &BACnetOScope::HandleEvent_RP_ACK_ArrayElement);
    connect(eventThread, &I_cEventThread::handleRP_ACK, this, &BACnetOScope::HandleEvent_RP_ACK);
    connect(eventThread, &I_cEventThread::handle_Error, this, &BACnetOScope::HandleEvent_Error);
    connect(eventThread, &I_cEventThread::handle_Success, this, &BACnetOScope::HandleEvent_Success);
    connect(eventThread, &I_cEventThread::handle_RFD_Success, this, &BACnetOScope::HandleEvent_RFD_Success);
    connect(readThread, &I_cObjectRead::Handle_ObjectRead_End, this, &BACnetOScope::Handle_ObjectRead_End);
    connect(readThread, &I_cObjectRead::Handle_PropertyRead_End, this, &BACnetOScope::Handle_PropertyRead_End);
    connect(eventThread, &I_cEventThread::finished, eventThread, &QObject::deleteLater);


    connect(ptr_ui->deviceTree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(OnClick_DeviceTreeItem(QTreeWidgetItem *, int)));
    connect(ptr_ui->objectTree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(OnClick_ObjectTreeItem(QTreeWidgetItem *, int)));
    connect(ptr_ui->propertyTree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(OnClick_PropertyTreeItem(QTreeWidgetItem *, int)));
    connect(ptr_ui->valueTree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(OnClick_ValueTreeItem(QTreeWidgetItem *, int)));


    // Load assets
    QString str_asset_prefix;
    QRect rect;
 #ifdef __ANDROID__
    QScreen *screen = QApplication::screens().at(0);
    rect = screen->geometry();
    str_asset_prefix = "assets:/";
 #else
    rect = this->geometry();
    str_asset_prefix = ":/BACnetOScope/";
 #endif

    bool ret1 = splash_pixmap.load(str_asset_prefix + "data/bacnet_splash.png");
    dev_pixmap.load(str_asset_prefix + "data/dev_icon.png");
    obj_pixmap.load(str_asset_prefix + "data/obj_icon.png");
    prop_pixmap.load(str_asset_prefix + "data/prop_icon.png");
    val_pixmap.load(str_asset_prefix + "data/val_icon.png");
    devinfo_pixmap.load(str_asset_prefix + "data/dev_info.png");
    backtodev_pixmap.load(str_asset_prefix + "data/back_to_dev.png");
    backtoobj_pixmap.load(str_asset_prefix + "data/back_to_obj.png");
    backtoprop_pixmap.load(str_asset_prefix + "data/back_to_prop.png");
    backtoval_pixmap.load(str_asset_prefix + "data/back_to_val.png");
    write_pixmap.load(str_asset_prefix + "data/write.png");

    dev_icon.addPixmap(dev_pixmap);
    obj_icon.addPixmap(obj_pixmap);
    prop_icon.addPixmap(prop_pixmap);
    val_icon.addPixmap(val_pixmap);
    devinfo_icon.addPixmap(devinfo_pixmap);
    backtodev_icon.addPixmap(backtodev_pixmap);
    backtoobj_icon.addPixmap(backtoobj_pixmap);
    backtoprop_icon.addPixmap(backtoprop_pixmap);
    backtoval_icon.addPixmap(backtoval_pixmap);
    write_icon.addPixmap(write_pixmap);

    refresh_pixmap.load(str_asset_prefix + "data/refresh.png");
    refresh_icon.addPixmap(refresh_pixmap);

    sett_pixmap.load(str_asset_prefix + "data/settings.png");
    sett_icon.addPixmap(sett_pixmap);


    // set behaviour data
    window_w_ = rect.width();
    window_h_ = rect.height();

    ptr_ui->labelSplash->setGeometry(0, 0, window_w_, window_h_);
    ptr_ui->labelSplash->setPixmap(splash_pixmap.scaled(window_w_, window_h_));

    int button_size_h = GetPercentage(window_h_, 12);

    ptr_deviceTree->setGeometry(0, 0, window_w_, window_h_ - button_size_h);
    ptr_objectTree->setGeometry(window_w_ * 1, 0, window_w_, window_h_ - button_size_h);
    ptr_propertyTree->setGeometry(window_w_ * 2, 0, window_w_, window_h_ - button_size_h);
    ptr_valueTree->setGeometry(window_w_ * 3, 0, window_w_, window_h_ - button_size_h);

    ptr_infoTree->setGeometry(0, window_h_, window_w_, window_h_ - button_size_h);


    ptr_propertyTree->setColumnWidth(0, window_w_ / 2);
    ptr_ui->deviceInfoTree->setColumnWidth(0, window_w_ / 2);


    ptr_deviceTree->setColumnWidth(0,window_w_-50);


    int header_h = GetPercentage(window_h_, 7);

    ptr_deviceTree->header()->setFixedHeight(header_h);
    ptr_objectTree->header()->setFixedHeight(header_h);
    ptr_propertyTree->header()->setFixedHeight(header_h);

    //header_item.
    ptr_pbObjectRead->setGeometry(window_w_ * 1, 0, window_w_, header_h);
    ptr_pbPropRead->setGeometry(window_w_ * 2, 0, window_w_, header_h);


    int size_h = GetPercentage(window_h_, 10);


    QWidget *ptr_widget_label_type = vValuesWidgetsList[0];
    QWidget *ptr_widget_combo_writePriority = vValuesWidgetsList[1];
    QWidget *ptr_widget_edit_newValue = vValuesWidgetsList[2];


    ptr_widget_label_type->setGeometry(window_w_ * 4, 0, window_w_, size_h);
    ptr_widget_edit_newValue->setGeometry(window_w_ * 4, size_h * 1 + 10, window_w_, size_h);
    ptr_widget_combo_writePriority->setGeometry(window_w_ * 4, size_h * 2 + 20, window_w_, size_h);

    ptr_ui->comboRegisterTime->setView(new QListView);


    QWidget *ptr_widget_whois = vSettingsWidgetsList[0];
    QWidget *ptr_widget_names = vSettingsWidgetsList[1];
    QWidget *ptr_widget_label_port = vSettingsWidgetsList[2];
    QWidget *ptr_widget_edit_port = vSettingsWidgetsList[3];
    QWidget *ptr_widget_reg_time = vSettingsWidgetsList[4];
    QWidget *ptr_widget_reg_adr = vSettingsWidgetsList[5];
    QWidget *ptr_widget_butt_reg = vSettingsWidgetsList[6];




    ptr_widget_whois->setGeometry(window_w_ * -1, 0, window_w_, size_h);
    ptr_widget_names->setGeometry(window_w_ * -1, size_h * 1, window_w_, size_h);
    ptr_cbWireshark->setGeometry(window_w_ * -1, size_h * 2, window_w_, size_h);
    ptr_widget_label_port->setGeometry(window_w_ * -1, size_h * 3 + 30, window_w_, size_h);
    ptr_widget_edit_port->setGeometry(window_w_ * -1, size_h * 4 + 20, window_w_, size_h);
    ptr_widget_reg_time->setGeometry(window_w_ * -1, size_h * 5 + 20, window_w_, size_h);
    ptr_widget_reg_adr->setGeometry(window_w_ * -1, size_h * 6 + 10, window_w_, size_h);
    ptr_widget_butt_reg->setGeometry(window_w_ * -1, size_h * 7 + 20, window_w_, size_h);

    ptr_buttonLeft->setGeometry(0, window_h_ - button_size_h, window_w_ / 2, button_size_h);
    ptr_buttonRight->setGeometry(window_w_ / 2, window_h_ - button_size_h, window_w_ / 2, button_size_h);

    ptr_buttonRight->setIcon(refresh_icon);
    ptr_buttonRight->setIconSize(QSize(64, 64));
    ptr_buttonLeft->setIcon(sett_icon);
    ptr_buttonLeft->setIconSize(QSize(64, 64));

    // sakri dok se ne zavrsi splash screen
    ptr_buttonLeft->hide();
    ptr_buttonRight->hide();


        // TODO: pogledati ovdje za touch and hold mozda staviti viewPort umjesto samo deviceTree
    QScroller::grabGesture(ptr_deviceTree->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller::grabGesture(ptr_objectTree->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller::grabGesture(ptr_propertyTree->viewport(), QScroller::LeftMouseButtonGesture);


    // touch skrolanje
    ptr_deviceTree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ptr_objectTree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ptr_propertyTree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);


    currentView_ = SELECTED_VIEW_DEVICE;

    QTimer::singleShot(1500, this, SLOT(OnTimer_FadeIntro()));

    bool ret;

    ptr_cbObjectNames->setChecked(settings_.names);
    ptr_cbWhoIsOnStart->setChecked(settings_.whois);
    ptr_editPort->setText(QString::number(settings_.port));
    ptr_regAddress->setText(settings_.lastadr);

    if (settings_.wire)
       {
        wireshark_ = new I_cWiresharkIO();
        std::string wire_path = wiresharkFilePath.toStdString();
        wireshark_->open_file(wire_path);
       }

    QTreeWidgetItem *header_item = ptr_deviceTree->headerItem();
    header_item->setIcon(0, devinfo_icon);

    if (settings_.whois)
       {
        OnClick_WhoIs();
       }

    int per = GetPercentage(window_w_,75);
    ptr_deviceTree->setColumnWidth(0,per);
    ptr_objectTree->setColumnWidth(0, per);
        
}






//=========================================================================================================
//=====================[ DEVICE ITEM CLICKED ]========================================================
// TODO: ako nema objekata? Message Error?
// Long touch - rescan
//=========================================================================================================
void BACnetOScope::OnClick_DeviceTreeItem(QTreeWidgetItem *item, int column)
{
    // onemoguci citanje dok ne dobijemo sve objekte
    //  TODO: omoguci ako isgubimo vezu?
    ptr_deviceTree->setEnabled(false); // nesmijemo dozvoliti double-click


    QVariant data = item->data(0, Qt::UserRole);
    BACNET_DEVICE *device = data.value<BACNET_DEVICE*>();

    // samo ako device nije procitan
    if (device->vObjects.empty())
        readingObjectFlag_ = true;

    // ako nismo skenirali device
    if (device->vObjects.empty())
       {
        firstReadArrayFlag_ = true;
        BACAPI_INVOKE_ID *invoke = BACAPI_ReserveInvokeId(device, NULL, NULL);

	    std::string str_adr = BACAPI_GetAddressStringIP(device->npduData->remote->adr);
	    QHostAddress dest_adr(str_adr.c_str());

        client->ReadProperty(&device->oid, PROP_OBJECT_LIST, 0, invoke->invokeId, NULL, device->npduData->sadr, dest_adr);
       }else {
	          RefreshObjectsTree(device);
		     }



    currentDevice_ = device;
	qDebug() << "Current Device: " << currentDevice_->oid.instance;


    ptr_buttonLeft->setIcon(backtodev_icon);
    ptr_buttonRight->setIcon(devinfo_icon);
    ptr_buttonRight->show();


    disconnect(showSettingsConnection_);
    disconnect(whoIsConnection_);

    // connect left BackToDeviceList
    backToDeviceListConnection_ = connect(ptr_buttonLeft, SIGNAL(clicked()), this, SLOT(OnClick_BackToDeviceList()));
    // connect right device info
    deviceInfoConnection_ = connect(ptr_buttonRight, SIGNAL(clicked()), this, SLOT(OnClick_ShowDeviceInfo()));

    MoveControls(DIRECTION_LEFT);
}


void BACnetOScope::RefreshObjectsTree(BACNET_DEVICE *device)
{
	ptr_objectTree->clear();
	ptr_objectTree->setHeaderLabel(QString::asprintf("%d (%d)", device->oid.instance, device->vObjects.size()));
 

	for (int c=0;c<device->vObjects.size();c++)
	    {
		 BACNET_OBJECT *obj = device->vObjects[c];
		 
         // set item data
		 QTreeWidgetItem *obj_item = new QTreeWidgetItem();
		 QPushButton *refresh_button = new QPushButton();
		 refresh_button->setIcon(refresh_icon);
		 refresh_button->setIconSize(QSize(50, 50));

		 QVariant data = QVariant::fromValue(obj);
		 obj_item->setData(0, Qt::UserRole, data);

		 obj_item->setIcon(0, obj_icon);

		 // set item string name
		 QString str_type = BACAPI_BACnetObjectName(obj->oid.type);
		 QString str_obj = QString::asprintf("%s, %d", str_type.toStdString().c_str(), obj->oid.instance);
 
		 QLabel *label = new QLabel(str_obj);
		 label->setWordWrap(true);

		 if (obj->vProperties.size() > 0)
			{
		     label->setStyleSheet("QLabel{color: rgb(0, 255, 0);""font: 75 22pt \"Square721 BT\"""}");
			}else {
			       label->setStyleSheet("QLabel{color: white;""font: 75 22pt \"Square721 BT\"""}");
				  }

		// insert into object tree
		ptr_objectTree->insertTopLevelItem(ptr_objectTree->topLevelItemCount(), obj_item);
		ptr_objectTree->setItemWidget(obj_item, 0, label);
		ptr_objectTree->setItemWidget(obj_item, 1, refresh_button);

		connect(refresh_button, SIGNAL(clicked()), this, SLOT(TapAndHoldTriggered()));
	   }
}




//=========================================================================================================
//=====================[ OBJECT TREE ITEM CLICKED ]========================================================
// Discover Properties
//=========================================================================================================
void BACnetOScope::OnClick_ObjectTreeItem(QTreeWidgetItem *item, int column)
{
    // nesmijemo nista dok ne zavrsimo citanje
    if (readingObjectFlag_ == true)
        return;

    ptr_objectTree->setEnabled(false); // nesmijemo dozvoliti double-click


    MoveControls(DIRECTION_LEFT);

    // disconnect device info
    //printf("OnClick_ObjectTreeItem: disconnect deviceinfo\n");
    disconnect(deviceInfoConnection_);
    disconnect(backToDeviceListConnection_);

    ptr_buttonLeft->setIcon(backtoobj_icon);
    ptr_buttonRight->hide();

    // Deconnect device, connect object
    backToObjectListConnection_ = connect(ptr_buttonLeft, SIGNAL(clicked()), this, SLOT(OnClick_BackToObjectList()));



    QVariant data = item->data(0, Qt::UserRole);
    BACNET_OBJECT *object = data.value<BACNET_OBJECT*>();
    currentObject_ = object;

    QString str_type = BACAPI_BACnetObjectName(object->oid.type);
    ptr_propertyTree->setHeaderLabel(QString::asprintf("%s\n%d", str_type.toStdString().c_str(), object->oid.instance));


    if (object)
       {  
        // ako propertiji nisu otkriveni
        if (object->vProperties.empty())
           {
            ptr_propertyTree->clear();
            StartPropertyRead(object);
           }else RefreshPropertyTree(object);
    }
}



//=========================================================================================================
//=====================[ PROPERTY CLICKED ]================================================================
// Prikazi value vrijednosti
//=========================================================================================================
void BACnetOScope::OnClick_PropertyTreeItem(QTreeWidgetItem *item, int column)
{
    ptr_propertyTree->setEnabled(false); // nesmijemo dozvoliti double-click


    QVariant data = item->data(0, Qt::UserRole);
    BACNET_OBJECT_PROPERTY *prop = data.value<BACNET_OBJECT_PROPERTY*>();
    currentProperty_ = prop;

    ptr_valueTree->setHeaderLabel(BACAPI_BACnetPropertyName(prop->property));


    if (BACAPI_IsComplexProperty(prop->property))
       {
        RefreshPropertyValues(prop);
       } else {
               if (prop->vValueList.size() > 0)
                  {
                   ptr_valueTree->clear();
                   //ui->valueTree->setWordWrap(true);
                   BACNET_APPLICATION_DATA_VALUE *val = prop->vValueList[0];
                   std::string str = BACAPI_GetStringValue(val);

                   QLabel *label = new QLabel(str.c_str());
                   label->setWordWrap(true);
                   label->setStyleSheet("QLabel{color: white;""font: 75 28pt \"Square721 BT\"" "}");
                                                 
                   QTreeWidgetItem *value_item = new QTreeWidgetItem();
                   QVariant data = QVariant::fromValue(val);
                   value_item->setData(0, Qt::UserRole, data);
                   value_item->setIcon(0, val_icon);

                   ptr_valueTree->insertTopLevelItem(0, value_item);
                   ptr_valueTree->setItemWidget(value_item, 0, label);
                  }
               }

    MoveControls(DIRECTION_LEFT);

    ptr_buttonLeft->setIcon(backtoprop_icon);
    ptr_buttonRight->hide();

    // Disconect Object, connect Property
    disconnect(backToObjectListConnection_);
    backToPropertyListConnection_ = connect(ptr_buttonLeft, SIGNAL(clicked()), this, SLOT(OnClick_BackToPropertyList()));


}






//=========================================================================================================
//=====================[ VALUE CLICKED ]================================================================
//=========================================================================================================
void BACnetOScope::OnClick_ValueTreeItem(QTreeWidgetItem *item, int column)
{

    ptr_valueTree->setEnabled(false); // nesmijemo dozvoliti double-click


    QVariant data = item->data(0, Qt::UserRole);
    BACNET_APPLICATION_DATA_VALUE *ptr_val = data.value<BACNET_APPLICATION_DATA_VALUE*>();


    // prikazi write kontrole
    if (ptr_val);
       {
        std::string str_curr_value = BACAPI_GetStringValue(ptr_val);

        BACNET_APPLICATION_TAG tag = BACAPI_GetApplicationTag((BACNET_OBJECT_TYPE)currentObject_->oid.type, currentProperty_->property);
        std::string str_tag = BACAPI_BACnetApplicationTagName(tag);


        ptr_ui->labelType->setText(str_tag.c_str());
        ptr_ui->editNewValue->setText(str_curr_value.c_str());

        // disconnect who Is , connect Write
        disconnect(whoIsConnection_);
        writeConnection_ = connect(ptr_buttonRight, SIGNAL(clicked()), this, SLOT(OnClick_WriteValue()));

       }


    MoveControls(DIRECTION_LEFT);
    ptr_buttonLeft->setIcon(backtoval_icon);
    ptr_buttonRight->setIcon(write_icon);
    ptr_buttonRight->show();

    // Disconect property, connect value
    disconnect(backToPropertyListConnection_);
    backToValueListConnection_ = connect(ptr_buttonLeft, SIGNAL(clicked()), this, SLOT(OnClick_BackToValueList()));

    currentView_ = SELECTED_VIEW_WRITE;
}






//=========================================================================================================
//=====================[ REFRESH PROPERTY LIST ]===========================================================
// Ovisno o odabranom objektu
// Resetirat ce se LABEL
//=========================================================================================================
void BACnetOScope::RefreshPropertyTree(BACNET_OBJECT *object)
{
    ptr_propertyTree->clear();
    for (int c = 0; c < object->vProperties.size(); c++)
        {
         BACNET_OBJECT_PROPERTY *prop = object->vProperties[c];
         // set item data
         QTreeWidgetItem *prop_item = new QTreeWidgetItem();
         QVariant data = QVariant::fromValue(prop);
         prop_item->setData(0, Qt::UserRole, data);

         // set item string name
         QString str_type = BACAPI_BACnetPropertyName(prop->property);
         prop_item->setText(0, str_type);


         QString str_prop_value;
         if (BACAPI_IsComplexProperty(prop->property))
            str_prop_value = "...";
            else str_prop_value = BACAPI_GetStringValue(prop).c_str();

        prop_item->setText(1, str_prop_value);
        // insert into property tree
        ptr_propertyTree->insertTopLevelItem(ptr_propertyTree->topLevelItemCount(), prop_item);
       }
}




//=========================================================================================================
//=====================[ WHO IS BACENT SERVICE REQUEST ]===================================================
//=========================================================================================================
void BACnetOScope::OnClick_WhoIs()
{
    BACNET_ADDRESS *adr = new BACNET_ADDRESS;
    adr->len = 0;
    adr->networkNumber = 65535;
    client->WhoIs(0, 100, adr, false);
}


//=========================================================================================================
//=====================[ REGISTER FOREIGN DEVICE SERVICE REQUEST ]=========================================
//=========================================================================================================
void BACnetOScope::OnClick_RegisterFD(void)
{

    QString str_adr = ptr_regAddress->text();
    QHostAddress adr(str_adr);

    int inx = ptr_comboRegTime->currentIndex();

    // prvi unos je text za reg combo
    if (inx > 0)
       {
        QString str_time = ptr_comboRegTime->itemText(inx);
        int itime = str_time.toInt();
        itime *= 60;

        client->RegisterFD(adr, itime);
       }

}




//=========================================================================================================
//=====================[ I - AM RECEIVED ]================================================================
//=========================================================================================================
void BACnetOScope::HandleEvent_I_AM(BACNET_DEVICE *dev)
{
    QString str = QString::asprintf("Main::HandleEvent_I_AM: Device Received. Instance %d", dev->oid.instance);
    qDebug() << str;

    // no duplicates
    if (!gDeviceList->DeviceExists(dev))
        {
         QTreeWidgetItem *item = new QTreeWidgetItem();
		 QPushButton *butt_refresh = new QPushButton();

		 butt_refresh->setIcon(refresh_icon);
		 butt_refresh->setIconSize(QSize(50, 50));
				
		 QVariant data = QVariant::fromValue(dev);
         item->setData(0, Qt::UserRole, data);
         item->setText(0, QString::asprintf("%d", dev->oid.instance));
         item->setIcon(0, dev_icon);
         item->setForeground(0, QBrush(QColor(Qt::white)));

         ptr_deviceTree->insertTopLevelItem(0, item);
         ptr_deviceTree->setItemWidget(item, 1, butt_refresh);

         gDeviceList->AddDevice(dev);

         QTreeWidgetItem *header_item = ptr_deviceTree->headerItem();
         QString str_header = QString::asprintf("Device List (%d)", gDeviceList->GetDeviceCount());
         header_item->setText(0, str_header);

	     connect(butt_refresh, SIGNAL(clicked()), this, SLOT(TapAndHoldTriggered()));
        }

}


void BACnetOScope::HandleEvent_Error(QString err_str)
{
    readingObjectFlag_ = false;
    readThread->waitcondition.wakeAll();
    rp_property_count_++;
}



void BACnetOScope::HandleEvent_Success()
{
	QMessageBox box;
	box.setText("Write Property: Success");
	box.exec();
}

void BACnetOScope::HandleEvent_RFD_Success(void)
{
 QMessageBox box;
 box.setText("Register Foreign Device: Success");
 box.exec();
}



//=========================================================================================================
//=====================[ HANDLE EVENT READ PROPERTY - ACK ]================================================
// Dobio property
//=========================================================================================================
void BACnetOScope::HandleEvent_RP_ACK(BACNET_OBJECT_PROPERTY *prop, int invokeId)
{
    BACAPI_INVOKE_ID *invoke = BACAPI_GetInvoke(invokeId);
    if (invoke == NULL)
       {
        BACAPI_ReleaseInvokeIdAll();
        return;
       }


    // provjeri postoji li dobiveni property na listi.
    // ako postoji, zamijeni ga sa dobivenim, ako ne postoji napravi novi
    BACNET_OBJECT_PROPERTY *old_prop = BACAPI_GetObjectProperty(prop->property, invoke->obj);
    if (!old_prop)
       {
        invoke->obj->vProperties.push_back(prop);
       }
    else {
          // Izbrisi staru vrijednost i kopiraj novu
          BACAPI_DeletePropertyValues(old_prop);
          BACAPI_CopyPropertyValue(old_prop, prop);
         }


#ifdef USE_ITEMS   // ili labe?
    // set item string name
    // set item data
    QTreeWidgetItem *prop_item = new QTreeWidgetItem();
    QVariant data = QVariant::fromValue(prop);
    prop_item->setData(0, Qt::UserRole, data);
    prop_item->setIcon(0, prop_icon);

    QString str_prop_value;
    if (BACAPI_IsComplexProperty(prop->property))
       {
        str_prop_value = "...";
       }else str_prop_value = BACAPI_GetStringValue(prop).c_str();

    // ako imamo ukljucena imena u settings, onda postavi ime objekta koje smo dobili za taj item u obj list
    if (settings_.names)
       {
        if (prop->property == PROP_OBJECT_NAME)
           {
            QTreeWidgetItem *obj_item = ptr_objectTree->currentItem();
            QLabel *obj_label_item = (QLabel*)ptr_objectTree->itemWidget(obj_item, 0);
            obj_label_item->setText(str_prop_value);
           }
       }

    // set item string name
    QString str_name = BACAPI_BACnetPropertyName(prop->property);
    prop_item->setText(0, str_name);
    prop_item->setText(1, str_prop_value);

    // insert into object tree
    ptr_propertyTree->insertTopLevelItem(ptr_propertyTree->topLevelItemCount(), prop_item);

#else

    // set item data
    QTreeWidgetItem *prop_item = new QTreeWidgetItem();
    QVariant data = QVariant::fromValue(prop);
    prop_item->setData(0, Qt::UserRole, data);

    // set item property name
    QString str_prop_name = BACAPI_BACnetPropertyName(prop->property);
    QLabel *label_prop_name = new QLabel(str_prop_name);
    label_prop_name->setWordWrap(true);
    label_prop_name->setStyleSheet("QLabel{color: white;   border-left: 3px solid rgb(255, 170, 0);}");


    // set item property value
    QString str_prop_value;
    if (BACAPI_IsComplexProperty(prop->property))
    {
        str_prop_value = "...";
    }
    else str_prop_value = BACAPI_GetStringValue(prop).c_str();
    QLabel *label_prop_value = new QLabel(str_prop_value);
    label_prop_value->setWordWrap(false);
    label_prop_value->setStyleSheet("QLabel{color: white;}");


    // insert into property tree
    ptr_propertyTree->insertTopLevelItem(ptr_propertyTree->topLevelItemCount(), prop_item);
    ptr_propertyTree->setItemWidget(prop_item, 0, label_prop_name);
    ptr_propertyTree->setItemWidget(prop_item, 1, label_prop_value);
#endif

    ptr_pbPropRead->setValue(rp_property_count_++);
    ptr_pbPropRead->setFormat(str_name);
    readThread->waitcondition.wakeAll();
    BACAPI_ReleaseInvokeId(invoke);
}






//==================================================================================================================
//=====================[ HANDLE EVENT RP ACK ARRAY ELEMENT ]========================================================
// Dobio objekt
//==================================================================================================================
void BACnetOScope::HandleEvent_RP_ACK_ArrayElement(BACNET_OBJECT_PROPERTY *prop, int invokeId)
{

    static int obj_count;
    BACAPI_INVOKE_ID *invoke = BACAPI_GetInvoke(invokeId);
    if (invoke == NULL)
       {
        BACAPI_ReleaseInvokeIdAll();
        return;
       }

    // ako je prop index 0, znaci da smo dobili broj objekata u devicu, pokreni thread za citanje tih objekata
    if (prop->index == 0)  // dobili property?
       {
        if (!prop->vValueList.empty()) // imamo vrijednost?
           {
            firstReadArrayFlag_ = false;
            BACNET_APPLICATION_DATA_VALUE *val = prop->vValueList[0];
            obj_count = val->type.Unsigned_Int;

            BACNET_DEVICE *parent_device = GetSelectedDevice();

            // isprazni sve objekte
            gDeviceList->DeleteAllObjects(parent_device);
            ptr_objectTree->clear();


            ptr_objectTree->setHeaderLabel(QString::asprintf("%d", parent_device->oid.instance));
            QTreeWidgetItem *header_item = ptr_objectTree->headerItem();
            header_item->setIcon(0, dev_icon);

            // postavi min/max na progress bar i pocni citanje
            ptr_pbObjectRead->setMinimum(0);
            ptr_pbObjectRead->setMaximum(obj_count);
            ptr_pbObjectRead->show();

            StartObjectRead(obj_count);
            return;
           }
        }

    // na ovom mjestu znamo da vValueList[0] postoji
    BACNET_APPLICATION_DATA_VALUE *val = prop->vValueList[0];

    // Stvori upis u Object List
    BACNET_OBJECT *obj = BACAPI_CreateBACnetObject(val->type.Object_Id.type, val->type.Object_Id.instance);
    if (obj)
       {
        // add object to device
        BACNET_DEVICE *parent_device = GetSelectedDevice();
        parent_device->vObjects.push_back(obj);

        // set item data
        QTreeWidgetItem *obj_item = new QTreeWidgetItem();
		QPushButton *refresh_button = new QPushButton();
        //refresh_button->setGeometry(0,0,50,50);
		refresh_button->setIcon(refresh_icon);
		refresh_button->setIconSize(QSize(50, 50));

        QVariant data = QVariant::fromValue(obj);
        obj_item->setData(0, Qt::UserRole, data);

        obj_item->setIcon(0, obj_icon);

        // set item string name
        QString str_type = BACAPI_BACnetObjectName(val->type.Object_Id.type);
        QString str_obj = QString::asprintf("%s, %d", str_type.toStdString().c_str(), val->type.Object_Id.instance);

        QLabel *label = new QLabel(str_obj);
        label->setWordWrap(true);
        label->setStyleSheet("QLabel{color: white;""font: 75 22pt \"Square721 BT\""  "}");

        // insert into object tree
        ptr_objectTree->insertTopLevelItem(ptr_objectTree->topLevelItemCount(), obj_item);
        ptr_objectTree->setItemWidget(obj_item, 0, label);
        
        ptr_objectTree->setItemWidget(obj_item, 1, refresh_button);

		connect(refresh_button, SIGNAL(clicked()), this, SLOT(TapAndHoldTriggered()));

        ptr_pbObjectRead->setValue(prop->index);
        ptr_pbObjectRead->setFormat(str_type);
       }

 
    readThread->waitcondition.wakeAll();

}



void BACnetOScope::StartObjectRead(int count)
{

    BACNET_DEVICE *dev = GetSelectedDevice();
    readThread->Init(client, dev, NULL, count);
    readThread->SetReadOperation(READ_ARRAY_INDEX);
    readThread->start();

}



void BACnetOScope::StartPropertyRead(BACNET_OBJECT *obj)
{
    BACNET_OBJECT *object = GetSelectedObject();
    BACNET_DEVICE *device = gDeviceList->GetParentDevice(object);

    // postavi progress bar za citanje propertyja
    rp_property_count_ = 0;
    ptr_pbPropRead->reset();
    ptr_pbPropRead->setMinimum(0);
    int max = BACAPI_GetPropertyCount((BACNET_OBJECT_TYPE)obj->oid.type);
    ptr_pbPropRead->setMaximum(max);
    ptr_pbPropRead->show();

    readThread->Init(client, device, obj, 0);
    readThread->SetReadOperation(READ_PROPERTY);

    readThread->start();

}



BACNET_DEVICE *BACnetOScope::GetSelectedDevice(void)
{
    QTreeWidgetItem *item = ptr_deviceTree->currentItem();

    if (!item)
        return NULL;

    QVariant data = item->data(0, Qt::UserRole);
    BACNET_DEVICE *device = data.value<BACNET_DEVICE*>();

    return device;
}



BACNET_OBJECT *BACnetOScope::GetSelectedObject(void)
{
    QTreeWidgetItem *item = ptr_objectTree->currentItem();

    if (!item)
        return NULL;

    QVariant data = item->data(0, Qt::UserRole);
    BACNET_OBJECT *object = data.value<BACNET_OBJECT*>();

    return object;
}



// FROM OBJECT LIST
void BACnetOScope::OnClick_BackToDeviceList()
{
    //qDebug() << "OnClick_BackToDeviceList():";
    ptr_deviceTree->setEnabled(true);

    MoveControls(DIRECTION_RIGHT);

    //ptr_buttonLeft->setText("Settings");
    ptr_buttonLeft->setIcon(sett_icon);

    //ptr_buttonRight->setText("Scan");
    ptr_buttonRight->setIcon(refresh_icon);
    ptr_buttonRight->show();

    currentView_ = SELECTED_VIEW_DEVICE;

    disconnect(backToDeviceListConnection_);

    printf("OnClick_BackToDeviceList: disconnect deviceinfo\n");
    disconnect(deviceInfoConnection_);


    whoIsConnection_ = connect(ptr_buttonRight, SIGNAL(clicked()), this, SLOT(OnClick_WhoIs()));
    showSettingsConnection_ = connect(ptr_buttonLeft, SIGNAL(clicked()), this, SLOT(OnClick_ShowSettings()));


}



// FROM PROPERTY LIST
void BACnetOScope::OnClick_BackToObjectList()
{
    ptr_objectTree->setEnabled(true);

    MoveControls(DIRECTION_RIGHT);

    // connect device info
    ptr_buttonRight->setIcon(devinfo_icon);
    ptr_buttonRight->show();
    printf("OnClick_BackToObjectList: connect deviceinfo\n");
    deviceInfoConnection_ = connect(ptr_buttonRight, SIGNAL(clicked()), this, SLOT(OnClick_ShowDeviceInfo()));


    // disconnect bacto to object and connect backt to device list
    ptr_buttonLeft->setIcon(backtodev_icon);
    disconnect(backToObjectListConnection_);
    backToDeviceListConnection_ = connect(ptr_buttonLeft, SIGNAL(clicked()), this, SLOT(OnClick_BackToDeviceList()));

    currentView_ = SELECTED_VIEW_OBJECT;
}


// FROM VALUE LIST
void BACnetOScope::OnClick_BackToPropertyList()
{
    ptr_propertyTree->setEnabled(true);

    MoveControls(DIRECTION_RIGHT);

    //ptr_buttonLeft->setText("Object List");
    ptr_buttonRight->hide();
    ptr_buttonLeft->setIcon(backtoobj_icon);


    currentView_ = SELECTED_VIEW_PROPERTY;


    disconnect(backToPropertyListConnection_);
    backToObjectListConnection_ = connect(ptr_buttonLeft, SIGNAL(clicked()), this, SLOT(OnClick_BackToObjectList()));

}


// FROM WRITE VALUE
void BACnetOScope::OnClick_BackToValueList()
{
    ptr_valueTree->setEnabled(true);

    MoveControls(DIRECTION_RIGHT);

    //ptr_buttonLeft->setText("Value List");
    ptr_buttonLeft->setIcon(backtoprop_icon);
    ptr_buttonRight->hide();

    disconnect(backToValueListConnection_);
    backToPropertyListConnection_ = connect(ptr_buttonLeft, SIGNAL(clicked()), this, SLOT(OnClick_BackToPropertyList()));


    currentView_ = SELECTED_VIEW_VALUE;


    // diskonektaj i write i povezi desni button natrag na who is
    disconnect(writeConnection_);
    whoIsConnection_ = connect(ptr_buttonRight, SIGNAL(clicked()), this, SLOT(OnClick_WhoIs()));

}



void BACnetOScope::OnClick_BackFromDeviceInfo(void)
{
    MoveDeviceInfo(DIRECTION_DOWN);

    disconnect(backFromdeviceInfoConnection_);

    backToDeviceListConnection_ = connect(ptr_buttonLeft, SIGNAL(clicked()), this, SLOT(OnClick_BackToDeviceList()));

    ptr_buttonRight->show();
    //ptr_buttonLeft->setText("Device List");
    ptr_buttonLeft->setIcon(backtodev_icon);


}


void BACnetOScope::OnClick_ShowDeviceInfo(void)
{
    MoveDeviceInfo(DIRECTION_UP);

    //ptr_buttonLeft->setText("Object List");
    ptr_buttonLeft->setIcon(backtoobj_icon);
    ptr_buttonRight->hide();

    // Deconnect left
    disconnect(backToDeviceListConnection_);


    backFromdeviceInfoConnection_ = connect(ptr_buttonLeft, SIGNAL(clicked()), this, SLOT(OnClick_BackFromDeviceInfo()));



    BACNET_DEVICE *dev = GetSelectedDevice();
    RefreshDeviceInfo(dev);
}



void BACnetOScope::OnClick_ShowSettings(void)
{
    MoveControls(DIRECTION_RIGHT);
    MoveSettinsControls(DIRECTION_RIGHT);

    disconnect(showSettingsConnection_);
    ptr_buttonRight->hide();

    //ptr_buttonLeft->setText("Save and Back");
    ptr_buttonLeft->setIcon(backtodev_icon);
    backFromSettingsConnection_ = connect(ptr_buttonLeft, SIGNAL(clicked()), this, SLOT(OnClick_BackFromSettings()));


}



void BACnetOScope::OnClick_BackFromSettings(void)
{
    SaveSettings();

    MoveControls(DIRECTION_LEFT);
    MoveSettinsControls(DIRECTION_LEFT);

    disconnect(backFromSettingsConnection_);
    showSettingsConnection_ = connect(ptr_buttonLeft, SIGNAL(clicked()), this, SLOT(OnClick_ShowSettings()));

    ptr_buttonLeft->setIcon(sett_icon);
    ptr_buttonRight->show();
    //ptr_buttonLeft->setText("Settings");

}



//==================================================================================================================
//=====================[ REFRESH DEVICE INFO ]======================================================================
//==================================================================================================================
void BACnetOScope::RefreshDeviceInfo(BACNET_DEVICE *dev)
{
    ptr_infoTree->clear();

    QTreeWidgetItem *item_instance = new QTreeWidgetItem();


    item_instance->setText(0, "Instance");
    item_instance->setText(1, QString::asprintf("%d", dev->oid.instance));



    QTreeWidgetItem *item_maxapdu = new QTreeWidgetItem();
    item_maxapdu->setText(0, "Max APDU");
    item_maxapdu->setText(1, QString::asprintf("%d", dev->maximumAPDU));


    QTreeWidgetItem *item_seg = new QTreeWidgetItem();
    item_seg->setText(0, "Segmentation");
    QString s = QString::asprintf("%s", BACAPI_BACnetSegmentationName(dev->segmentation));

    item_seg->setText(1, s);


    QTreeWidgetItem *item_vendor = new QTreeWidgetItem();
    item_vendor->setText(0, "Vendor");
    item_vendor->setText(1, QString::asprintf("%s", BACAPI_BACnetVendorName(dev->vendorId)));



    QTreeWidgetItem *item_addr = new QTreeWidgetItem();

    string adr = BACAPI_GetAddressStringIP(dev->npduData->remote);
    string mac = BACAPI_GetAddressString(dev->npduData->remote);
    QString str_adr = QString::asprintf("%s, (%s)", adr.c_str(), mac.c_str());

    item_addr->setText(0, "Address");
    item_addr->setText(1, str_adr);



    QTreeWidgetItem *item_hop = new QTreeWidgetItem();
    item_hop->setText(0, "Hop Count");
    item_hop->setText(1, QString::asprintf("%d", dev->npduData->hopCount));


    QTreeWidgetItem *item_npdu = NULL;

    if (dev->npduData->sadr)
    {
        item_npdu = new QTreeWidgetItem();
        item_npdu->setText(0, "NPDU Data");


        QTreeWidgetItem *item_nn = new QTreeWidgetItem();
        item_nn->setText(0, "Net. Num.");
        item_nn->setText(1, QString::asprintf("%d", dev->npduData->sadr->networkNumber));



        QTreeWidgetItem *item_sadr = new QTreeWidgetItem();
        QString str_adr = BACAPI_GetAddressString(dev->npduData->sadr, dev->npduData->sadr->len).c_str();
        item_sadr->setText(0, "Src. Adr.");
        item_sadr->setText(1, str_adr);


        item_npdu->insertChild(0, item_nn);
        item_npdu->insertChild(0, item_sadr);

        item_npdu->setExpanded(true);
    }



    ptr_infoTree->insertTopLevelItem(0, item_instance);
    ptr_infoTree->insertTopLevelItem(1, item_maxapdu);
    ptr_infoTree->insertTopLevelItem(2, item_seg);
    ptr_infoTree->insertTopLevelItem(3, item_vendor);
    ptr_infoTree->insertTopLevelItem(4, item_addr);

    if (item_npdu)
    {
        ptr_infoTree->insertTopLevelItem(5, item_npdu);

    }

}




//==================================================================================================================
//=====================[ MOVE CONTROLS ]============================================================================
// Pomakni sve poglede i sve kontrole za pogled ljevo ili desno.
//==================================================================================================================
void BACnetOScope::MoveControls(int where)
{

    directionStack_ += where;



    QPropertyAnimation* device_anima = new QPropertyAnimation(ptr_deviceTree, "geometry");
    QPropertyAnimation* object_anima = new QPropertyAnimation(ptr_objectTree, "geometry");
    QPropertyAnimation* property_anima = new QPropertyAnimation(ptr_propertyTree, "geometry");
    QPropertyAnimation* value_anima = new QPropertyAnimation(ptr_valueTree, "geometry");

    QPropertyAnimation* pb_object_anima = new QPropertyAnimation(ptr_pbObjectRead, "geometry");
    QPropertyAnimation* pb_prop_anima = new QPropertyAnimation(ptr_pbPropRead, "geometry");


    device_anima->setDuration(DIRECTION_SPEED);
    object_anima->setDuration(DIRECTION_SPEED);
    property_anima->setDuration(DIRECTION_SPEED);
    value_anima->setDuration(DIRECTION_SPEED);

    pb_object_anima->setDuration(DIRECTION_SPEED);
    pb_prop_anima->setDuration(DIRECTION_SPEED);




    device_anima->setEasingCurve(QEasingCurve::InOutQuad);
    object_anima->setEasingCurve(QEasingCurve::InOutQuad);
    property_anima->setEasingCurve(QEasingCurve::InOutQuad);
    value_anima->setEasingCurve(QEasingCurve::InOutQuad);

    pb_object_anima->setEasingCurve(QEasingCurve::InOutQuad);
    pb_prop_anima->setEasingCurve(QEasingCurve::InOutQuad);


    QRect device_rect = ptr_deviceTree->geometry();
    QRect object_rect = ptr_objectTree->geometry();
    QRect prop_rect = ptr_propertyTree->geometry();
    QRect value_rect = ptr_valueTree->geometry();

    QRect pbobj_rect = ptr_pbObjectRead->geometry();
    QRect pbprop_rect = ptr_pbPropRead->geometry();



    device_anima->setStartValue(device_rect);
    object_anima->setStartValue(object_rect);
    property_anima->setStartValue(prop_rect);
    value_anima->setStartValue(value_rect);

    pb_object_anima->setStartValue(pbobj_rect);
    pb_prop_anima->setStartValue(pbprop_rect);


    QRect end_device_rect = device_rect;
    QRect end_object_rect = object_rect;
    QRect end_prop_rect = prop_rect;
    QRect end_value_rect = value_rect;

    QRect end_pbobj_rect = pbobj_rect;
    QRect end_pbprop_rect = pbprop_rect;



    printf("dir stack %d\n", directionStack_);


    int dev_end_x = directionStack_ * device_rect.width();  //  0,   -400
    int obj_end_x = (directionStack_ + 1) * object_rect.width();  // -400,  0
    int pbobj_end_x = (directionStack_ + 1) * pbobj_rect.width();  // -400,  0

    int prop_end_x = (directionStack_ + 2) * prop_rect.width();
    int pbprop_end_x = (directionStack_ + 2) * pbprop_rect.width();


    int value_end_x = (directionStack_ + 3) * value_rect.width();



    end_device_rect.moveLeft(dev_end_x);
    end_object_rect.moveLeft(obj_end_x);
    end_prop_rect.moveLeft(prop_end_x);
    end_value_rect.moveLeft(value_end_x);

    end_pbobj_rect.moveLeft(pbobj_end_x);
    end_pbprop_rect.moveLeft(pbprop_end_x);


    device_anima->setEndValue(end_device_rect);
    object_anima->setEndValue(end_object_rect);
    property_anima->setEndValue(end_prop_rect);
    value_anima->setEndValue(end_value_rect);

    pb_object_anima->setEndValue(end_pbobj_rect);
    pb_prop_anima->setEndValue(end_pbprop_rect);


    device_anima->start();
    object_anima->start();
    property_anima->start();
    value_anima->start();

    pb_object_anima->start();
    pb_prop_anima->start();

    MoveValueControls();
}


void BACnetOScope::MoveSettinsControls(int direction)
{
    for (int c = 0; c < vSettingsWidgetsList.size(); c++)
        {
         QWidget *ptr_widget = vSettingsWidgetsList[c];

         QPropertyAnimation  *widget_anima = new QPropertyAnimation(ptr_widget, "geometry");
         widget_anima->setDuration(DIRECTION_SPEED);
         widget_anima->setEasingCurve(QEasingCurve::InOutQuad);
         QRect widget_rect = ptr_widget->geometry();
         widget_anima->setStartValue(widget_rect);
         QRect end_widget_rect = widget_rect;

         int widget_end_x = 0;
         if (direction == DIRECTION_RIGHT)
            widget_end_x = 0;//directionStack_ * widget_rect.width();
         else widget_end_x = window_w_ * -1;

         end_widget_rect.moveLeft(widget_end_x);
         widget_anima->setEndValue(end_widget_rect);
         widget_anima->start();
       }

}

void BACnetOScope::MoveValueControls()
{

    for (int c = 0; c < vValuesWidgetsList.size(); c++)
        {
         QWidget *ptr_widget = vValuesWidgetsList[c];

         QPropertyAnimation  *widget_anima = new QPropertyAnimation(ptr_widget, "geometry");
         widget_anima->setDuration(DIRECTION_SPEED);
         widget_anima->setEasingCurve(QEasingCurve::InOutQuad);
         QRect widget_rect = ptr_widget->geometry();
         widget_anima->setStartValue(widget_rect);
         QRect end_widget_rect = widget_rect;
         int widget_end_x = (directionStack_ + 4) * widget_rect.width();
         end_widget_rect.moveLeft(widget_end_x);
         widget_anima->setEndValue(end_widget_rect);
         widget_anima->start();
        }      

}


void BACnetOScope::MoveDeviceInfo(int direction)
{

    QPropertyAnimation  *info_anima = new QPropertyAnimation(this->ptr_infoTree, "geometry");
    QPropertyAnimation  *object_anima = new QPropertyAnimation(ptr_objectTree, "geometry");

    info_anima->setDuration(DIRECTION_SPEED);
    object_anima->setDuration(DIRECTION_SPEED);



    info_anima->setEasingCurve(QEasingCurve::InOutQuad);
    object_anima->setEasingCurve(QEasingCurve::InOutQuad);

    QRect info_rect = ptr_infoTree->geometry();
    QRect object_rect = ptr_objectTree->geometry();



    info_anima->setStartValue(info_rect);
    object_anima->setStartValue(object_rect);

    QRect end_info_rect = info_rect;
    QRect end_object_rect = object_rect;


    int info_end_y = 0;
    int obj_end_y = 0;
    if (direction < 0)
    {
        info_end_y = 0;  //  0,   -400
        obj_end_y = -object_rect.height();  // -400,  0
    }
    else {
        info_end_y = info_rect.height() + 80;
        obj_end_y = 0;
    }

    end_info_rect.moveTop(info_end_y);
    end_object_rect.moveTop(obj_end_y);


    info_anima->setEndValue(end_info_rect);
    object_anima->setEndValue(end_object_rect);

    info_anima->start();
    object_anima->start();

}





void BACnetOScope::OnClick_WriteValue(void)
{

    QString str_value = ptr_ui->editNewValue->text();

    bool ret = BACAPI_SetApplicationDataValue(str_value.toStdString(), currentProperty_->vValueList[0]);
    if (ret == false)
       {
        printf("Unsupported Application value");
        return;
       }


    int index = ptr_cbPriority->currentIndex();
    if (index > 0)
       {
        currentProperty_->priority = index;
       }
    std::string str_adr = BACAPI_GetAddressStringIP(currentDevice_->npduData->remote->adr);
	QHostAddress dest_adr(str_adr.c_str());

    client->WriteProperty(&currentObject_->oid, currentProperty_, 1, NULL, currentDevice_->npduData->sadr, dest_adr);
    
}




// TODO: BACNET_DEVICE PARAMETER
void BACnetOScope::Handle_ObjectRead_End(void)
{
    QTreeWidgetItem *sel_item = ptr_deviceTree->currentItem();

    QString new_text = QString::asprintf("%d (%d)", currentDevice_->oid.instance, currentDevice_->vObjects.size());
    sel_item->setText(0, new_text);


    sel_item->setForeground(0, QBrush(QColor(Qt::green)));


    ptr_pbObjectRead->hide();

    // postavi flag da smo prestali citati
    readingObjectFlag_ = false;
}




// TODO: BACNET_OBJECT PARAMETER
void BACnetOScope::Handle_PropertyRead_End(void)
{
    QTreeWidgetItem *sel_item = ptr_objectTree->currentItem();
    QLabel *ptr_label = (QLabel*)ptr_objectTree->itemWidget(sel_item, 0);

    ptr_label->setStyleSheet("QLabel{color: rgb(0, 255, 0);" "font: 75 22pt \"Square721 BT\"""}");
                            
                             

    ptr_pbPropRead->hide();

}



//==================================================================================================================
//====================================[ REFRESH FUNKCIJE ]==========================================================
//==================================================================================================================




void BACnetOScope::RefreshPropertyValues(BACNET_OBJECT_PROPERTY *prop)
{

    ptr_valueTree->clear();
    BACNET_OBJECT_PROPERTY *property = prop;
    switch (prop->property)
    {
    case PROP_ACCESS_DOORS:
    case PROP_CREDENTIALS_IN_ZONE:
    case PROP_MEMBERS:
    case PROP_CREDENTIALS:
    case PROP_LOG_DEVICE_OBJECT_PROPERTY:
    case PROP_GROUP_MEMBERS:
    {
        RefreshDeviceObjectPropertyRef(prop);
        break;
    }

    case PROP_TIME_OF_ACTIVE_TIME_RESET:
    case PROP_TIME_OF_STATE_COUNT_RESET:
    case PROP_CHANGE_OF_STATE_TIME:
    case PROP_MAXIMUM_VALUE_TIMESTAMP:
    case PROP_MINIMUM_VALUE_TIMESTAMP:
    case PROP_VALUE_CHANGE_TIME:
    case PROP_START_TIME:
    case PROP_STOP_TIME:
    case PROP_MODIFICATION_DATE:
    case PROP_UPDATE_TIME:
    case PROP_COUNT_CHANGE_TIME:
    case PROP_LAST_CREDENTIAL_ADDED_TIME:
    case PROP_LAST_CREDENTIAL_REMOVED_TIME:
    case PROP_ACTIVATION_TIME:
    case PROP_EXPIRATION_TIME:
    case PROP_LAST_USE_TIME:
    {
        RefreshDateTime(prop);
        break;
    }

    case PROP_SUBORDINATE_LIST:
    case PROP_DOOR_MEMBERS:
    case PROP_ZONE_MEMBERS:
    case PROP_MEMBER_OF:
    case PROP_ENTRY_POINTS:
    case PROP_EXIT_POINTS:
    case PROP_ACCOMPANIMENT:
    case PROP_BELONGS_TO:
    case PROP_LAST_ACCESS_POINT:
    case PROP_ACCESS_EVENT_CREDENTIAL:
    case PROP_ZONE_FROM:
    case PROP_ZONE_TO:
    case PROP_LAST_CREDENTIAL_ADDED:
    case PROP_LAST_CREDENTIAL_REMOVED:
    case PROP_ENERGY_METER_REF:
    {
        RefreshDeviceObjectReference(prop);
        break;
    }

    case PROP_MANIPULATED_VARIABLE_REFERENCE:
    case PROP_CONTROLLED_VARIABLE_REFERENCE:
    case PROP_INPUT_REFERENCE:
    case PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES:
    case PROP_OBJECT_PROPERTY_REFERENCE:
    case PROP_EVENT_ALGORITHM_INHIBIT_REF:
    {
        RefreshObjectPropertyReference(prop);
        break;
    }


    case PROP_TIME_SYNCHRONIZATION_RECIPIENTS:
    case PROP_RESTART_NOTIFICATION_RECIPIENTS:
    case PROP_UTC_TIME_SYNCHRONIZATION_RECIPIENTS:
    {
        RefreshRecipientList(property);  // *
        break;
    }
    break;



    case PROP_RECIPIENT_LIST:
    {
        RefreshBACDestination(property);  // *
        break;
    }
    break;


    case PROP_EFFECTIVE_PERIOD:  // *
    {
        RefreshDateRange(property);
        break;
    }


    case PROP_EVENT_TIME_STAMPS:
    case PROP_LAST_RESTORE_TIME:
    case PROP_TIME_OF_DEVICE_RESTART:
    case PROP_ACCESS_EVENT_TIME:
    case PROP_COMMAND_TIME_ARRAY:
    {
        RefreshEventTimeStamps(property);  //*
        break;
    }
    break;

    case PROP_ACTIVE_COV_SUBSCRIPTIONS:
    {
        RefreshCOVSubscriptions(property);  //*
        break;
    }
    break;



    case PROP_DATE_LIST:
    {
        RefreshCalendarDateList(property); // *
        break;
    }
    break;

    case PROP_PRIORITY_ARRAY:
    {
        RefreshPriorityArray(property);  // -
        break;
    }
    break;


    case PROP_WEEKLY_SCHEDULE:
    {
        RefreshWeeklySchedule(property);  // TODO
        break;
    }
    break;

    case PROP_EXCEPTION_SCHEDULE:
    {
        RefreshSpecialEvent(property); //  TODO
        break;
    }
    break;


    case PROP_LIST_OF_GROUP_MEMBERS:
    {
        RefreshReadAccessSpecification(property);  // TODO
        break;
    }

    case PROP_DEVICE_ADDRESS_BINDING:
    case PROP_SLAVE_ADDRESS_BINDING:
    case PROP_MANUAL_SLAVE_ADDRESS_BINDING:
    {
        RefreshDeviceAddressBinding(property);   // --
        break;
    }

    case PROP_ACTION:
    {
     // ne handlamo u BACnetOScope
        break;
    }


    case PROP_ACTION_TEXT:
    {
        RefreshActionText(property);   // --
        break;
    }


    case PROP_ACCEPTED_MODES:
    case PROP_LIFE_SAFETY_ALARM_VALUES:
    case PROP_ALARM_VALUES:
    case PROP_FAULT_VALUES:
    {
        RefreshEnumList(property);  
        break;
    }

    case PROP_EVENT_PARAMETERS:
    {
        //gLog->AppendLog("RefreshComplexProperties: Missing RefreshEventParameters();\n");
        break;
    }

    case PROP_LOG_BUFFER:
    {
        //gLog->AppendLog("RefreshComplexProperties: Missing RefreshLogBuffer();\n");
        break;
    }

    case PROP_SCALE:
    {
        RefreshScale(property);  // *
        break;
    }

    case PROP_PRESCALE:
    {
        RefreshPreScale(property);  //-
        break;
    }
    case PROP_LOGGING_RECORD:
    {
        RefreshLoggingRecord(property);  // Nemogu naci objekt gdje se pojavljuje???!
        break;
    }
    case PROP_FAULT_PARAMETERS:
    {
        RefreshFaultParameters(property);  //  TODO
        break;
    }

    case PROP_FD_BBMD_ADDRESS:
    case PROP_BACNET_IP_GLOBAL_ADDRESS:
    {
        RefreshHostNPort(property);   //  TODO
        break;
    }

    case PROP_BBMD_BROADCAST_DISTRIBUTION_TABLE:
        RefreshBACnetBDTEntry(property);  // *
        break;

        // nista od ovog ako se ne spomene
    case PROP_BBMD_FOREIGN_DEVICE_TABLE:
        RefreshBACnetFDTEntry(property);
        break;

    case PROP_VIRTUAL_MAC_ADDRESS_TABLE:
        RefreshBACnetVMACEntry(property);
        break;

    case PROP_ROUTING_TABLE:
        RefreshBACnetRouterEntry(property);
        break;

    case PROP_ASSIGNED_ACCESS_RIGHTS:
        RefreshBACnetAssignedAccessRights(property);
        break;

    case PROP_AUTHENTICATION_FACTORS:
        RefreshCredentialAuthenticationFactors(property);
        break;

    case PROP_KEY_SETS:
        RefreshSecurityKeySets(property);
        break;

    case PROP_NETWORK_ACCESS_SECURITY_POLICIES:
        RefreshNetworkAccessSecurityPolicies(property);
        break;

    case PROP_VALUE_SOURCE:
    case PROP_VALUE_SOURCE_ARRAY:
        RefreshValueSource(property);
        break;



    default: break;
    }

}


void BACnetOScope::RefreshDeviceObjectPropertyRef(BACNET_OBJECT_PROPERTY *prop)
{


    int count = (int)prop->complexValue.vDeviceObjectPropertyReference.size();
    for (int c = 0; c < count; c++)
        {
        QTreeWidgetItem *item_entry = new QTreeWidgetItem();
        QString str_entry = QString::asprintf("Entry %d", c + 1);
        item_entry->setText(0, str_entry);



        BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE *ptr_ref = prop->complexValue.vDeviceObjectPropertyReference[c];

        QTreeWidgetItem *item_oid = new QTreeWidgetItem();
        QTreeWidgetItem *item_propid = new QTreeWidgetItem();
        QTreeWidgetItem *item_devoid = new QTreeWidgetItem();
        QTreeWidgetItem *item_index = new QTreeWidgetItem();

        QString str_oid = QString::asprintf("%s: %d", BACAPI_BACnetObjectName(ptr_ref->objectIdentifier.type), ptr_ref->objectIdentifier.instance);
        item_oid->setText(0, str_oid);

        QString str_dev_oid = QString::asprintf("%s: %d", BACAPI_BACnetObjectName(ptr_ref->deviceIndentifier.type), ptr_ref->deviceIndentifier.instance);
        item_devoid->setText(0, str_dev_oid);

        item_propid->setText(0, BACAPI_BACnetPropertyName(ptr_ref->propertyIdentifier));

        item_index->setText(0, BACAPI_ToString(ptr_ref->arrayIndex).c_str());


        item_entry->insertChild(0, item_index);
        item_entry->insertChild(0, item_propid);
        item_entry->insertChild(0, item_devoid);
        item_entry->insertChild(0, item_oid);


        // insert into object tree
        ptr_valueTree->insertTopLevelItem(ptr_valueTree->topLevelItemCount(), item_entry);
        //item_entry->setExpanded();
       }

}




void BACnetOScope::RefreshDateTime(BACNET_OBJECT_PROPERTY *prop)
{

    QDate d;
    QTime t;

    d.setDate(prop->complexValue.dateTime.date.year, prop->complexValue.dateTime.date.month, prop->complexValue.dateTime.date.day);
    t.setHMS(prop->complexValue.dateTime.time.hour, prop->complexValue.dateTime.time.minute, prop->complexValue.dateTime.time.sec);

    QDateTime dt(d, t);

    QString str_dt = dt.toString();

    QTreeWidgetItem *item_dt = new QTreeWidgetItem();
    item_dt->setText(0, str_dt);

    ptr_valueTree->insertTopLevelItem(ptr_valueTree->topLevelItemCount(), item_dt);
}





void BACnetOScope::RefreshDeviceObjectReference(BACNET_OBJECT_PROPERTY *prop)
{

    int count = (int)prop->complexValue.vDeviceObjectReference.size();
    int cnt = 1;
    for (int c = 0; c < count; c++)
    {
        BACNET_DEVICE_OBJECT_REFERENCE *value = prop->complexValue.vDeviceObjectReference[c];

        QTreeWidgetItem *item_entry = new QTreeWidgetItem();
        QString str_entry = QString::asprintf("Entry %d", c + 1);
        item_entry->setText(0, str_entry);



        BACNET_DEVICE_OBJECT_REFERENCE *ptr_ref = prop->complexValue.vDeviceObjectReference[c];

        QTreeWidgetItem *item_oid = new QTreeWidgetItem();
        QTreeWidgetItem *item_devoid = new QTreeWidgetItem();

        QString str_oid = QString::asprintf("%s: %d", BACAPI_BACnetObjectName(ptr_ref->objectIdentifier.type), ptr_ref->objectIdentifier.instance);
        item_oid->setText(0, str_oid);

        QString str_dev_oid = QString::asprintf("%s: %d", BACAPI_BACnetObjectName(ptr_ref->deviceIndentifier.type), ptr_ref->deviceIndentifier.instance);
        item_devoid->setText(0, str_dev_oid);



        item_entry->insertChild(0, item_devoid);
        item_entry->insertChild(0, item_oid);


        // insert into object tree
        ptr_valueTree->insertTopLevelItem(ptr_valueTree->topLevelItemCount(), item_entry);
        //item_entry->setExpanded();
    }


}





void BACnetOScope::RefreshObjectPropertyReference(BACNET_OBJECT_PROPERTY *prop)
{

    int count = (int)prop->complexValue.vObjectPropertyReference.size();

    for (int c = 0; c < count; c++)
    {

        QTreeWidgetItem *item_entry = new QTreeWidgetItem();
        QString str_entry = QString::asprintf("Entry %d", c + 1);
        item_entry->setText(0, str_entry);



        BACNET_OBJECT_PROPERTY_REFERENCE *ptr_ref = prop->complexValue.vObjectPropertyReference[c];

        QTreeWidgetItem *item_oid = new QTreeWidgetItem();
        QTreeWidgetItem *item_prop = new QTreeWidgetItem();
        QTreeWidgetItem *item_index = new QTreeWidgetItem();

        QString str_oid = QString::asprintf("%s: %d", BACAPI_BACnetObjectName(ptr_ref->oid.type), ptr_ref->oid.instance);
        item_oid->setText(0, str_oid);

        QString str_prop = BACAPI_BACnetPropertyName(ptr_ref->propertyId);
        item_prop->setText(0, str_prop);


        item_index->setText(0, BACAPI_ToString(ptr_ref->propertyArrayIndex).c_str());


        item_entry->insertChild(0, item_index);
        item_entry->insertChild(0, item_prop);
        item_entry->insertChild(0, item_oid);


        // insert into object tree
        ptr_valueTree->insertTopLevelItem(ptr_valueTree->topLevelItemCount(), item_entry);



    }
}



void BACnetOScope::RefreshRecipientList(BACNET_OBJECT_PROPERTY *property)
{
    size_t count = property->complexValue.vRecipientList.size();

    for (int c = 0; c < (int)count; c++)
    {
        BACNET_RECIPIENT *ptr_recipient = property->complexValue.vRecipientList[c];

        QTreeWidgetItem *item_recipient = new QTreeWidgetItem();
        QString str_recipient = QString::asprintf("Recipient %d", c + 1);
        item_recipient->setText(0, str_recipient);


        if (ptr_recipient->TagRecipientType == 0) // OID
        {
            QString str_oid = QString::asprintf("%s, %d", BACAPI_BACnetObjectName(ptr_recipient->RecipientType.Device.type), ptr_recipient->RecipientType.Device.instance);

            QTreeWidgetItem *item_oid = new QTreeWidgetItem();
            item_oid->setText(0, str_oid);
            item_recipient->insertChild(0, item_oid);
        }
        else if (ptr_recipient->TagRecipientType == 1) // address
        {

            QString str_oid = QString::asprintf("%X %X %X %X - NetNum: %d", ptr_recipient->RecipientType.Address.adr[0],
                ptr_recipient->RecipientType.Address.adr[1],
                ptr_recipient->RecipientType.Address.adr[2],
                ptr_recipient->RecipientType.Address.adr[3],
                ptr_recipient->RecipientType.Address.networkNumber);


            QTreeWidgetItem *item_oid = new QTreeWidgetItem();
            item_oid->setText(0, str_oid);
            item_recipient->insertChild(0, item_oid);
        }
        else {}

        ptr_valueTree->insertTopLevelItem(ptr_valueTree->topLevelItemCount(), item_recipient);
    }


}



void BACnetOScope::RefreshBACDestination(BACNET_OBJECT_PROPERTY * property)
{
    int cnt = (int)property->complexValue.vDestinationList.size();


    int c = 1;
    for (int c = 0; c < cnt; c++)
    {
        BACNET_DESTINATION *ptr_dest = property->complexValue.vDestinationList[c];


    }
}


void BACnetOScope::RefreshDateRange(BACNET_OBJECT_PROPERTY * property)
{
    QDate sd, ed;

    sd.setDate(property->complexValue.dateRange.startdate.year, property->complexValue.dateRange.startdate.month, property->complexValue.dateRange.startdate.day);
    ed.setDate(property->complexValue.dateRange.enddate.year, property->complexValue.dateRange.enddate.month, property->complexValue.dateRange.enddate.day);


    QString str_sd = QString::asprintf("Start Date: %s", sd.toString().toStdString().c_str());
    QString str_ed = QString::asprintf("End Date: %s", ed.toString().toStdString().c_str());

    QTreeWidgetItem *item_sd = new QTreeWidgetItem();
    QTreeWidgetItem *item_ed = new QTreeWidgetItem();

    item_sd->setText(0, str_sd);
    item_ed->setText(0, str_ed);


    ptr_valueTree->insertTopLevelItem(ptr_valueTree->topLevelItemCount(), item_sd);
    ptr_valueTree->insertTopLevelItem(ptr_valueTree->topLevelItemCount(), item_ed);

}


void BACnetOScope::RefreshEventTimeStamps(BACNET_OBJECT_PROPERTY *property)
{
    QTreeWidgetItem *item_parent = new QTreeWidgetItem();

    size_t count = property->complexValue.vTimeStamps.size();

    for (size_t c = 0; c < count; c++)
    {
        BACNET_TIMESTAMP *ptr_stamp = property->complexValue.vTimeStamps[c];
        switch (ptr_stamp->tag)
        {
        case TIME_STAMP_TIME:
        {
            item_parent->setText(0, "Value: Time");
            QTime t;
            t.setHMS(ptr_stamp->value.time.hour, ptr_stamp->value.time.minute, ptr_stamp->value.time.sec);
            QTreeWidgetItem *item_time = new QTreeWidgetItem();
            item_time->setText(0, t.toString());
            item_parent->insertChild(0, item_time);
            break;
        }
        case TIME_STAMP_SEQUENCE:
        {
            item_parent->setText(0, "Value: Sequence");
            QTreeWidgetItem *item_seq = new QTreeWidgetItem();
            item_seq->setText(0, BACAPI_ToString(ptr_stamp->value.sequenceNum).c_str());
            item_parent->insertChild(0, item_seq);
            break;

        }
        case TIME_STAMP_DATETIME:
        {
            item_parent->setText(0, "Value: Date Time");
            QDate d;
            QTime t;

            d.setDate(ptr_stamp->value.dateTime.date.year, ptr_stamp->value.dateTime.date.month, ptr_stamp->value.dateTime.date.day);
            t.setHMS(ptr_stamp->value.dateTime.time.hour, ptr_stamp->value.dateTime.time.minute, ptr_stamp->value.dateTime.time.sec);

            QDateTime dt(d, t);

            QString str_dt = dt.toString();

            QTreeWidgetItem *item_dt = new QTreeWidgetItem();
            item_dt->setText(0, str_dt);
            item_parent->insertChild(0, item_dt);


            break;
        }
        default: break;
        }

        ptr_valueTree->insertTopLevelItem(0, item_parent);
    }
}



void BACnetOScope::RefreshCOVSubscriptions(BACNET_OBJECT_PROPERTY * property)
{

    size_t count = property->complexValue.vActiveCOVSubs.size();

    for (size_t c = 0; c < count; c++)
    {
        BACNET_COV_SUBSCRIPTION *ptr_sub = property->complexValue.vActiveCOVSubs[c];

        QTreeWidgetItem *item_sub = new QTreeWidgetItem();
        QString str_sub = QString::asprintf("Subscription: %d", c + 1);
        item_sub->setText(0, str_sub);


        // prikazi primatelje
        if (ptr_sub->recipientProcess.recipient.TagRecipientType == 0)  // OID
        {
            QTreeWidgetItem *item_rec = new QTreeWidgetItem();
            QString str_rec = QString::asprintf("Device, %d", ptr_sub->recipientProcess.recipient.RecipientType.Device.instance);
            item_rec->setText(0, str_rec);
            item_sub->insertChild(0, item_rec);
        }
        else {
            // MAC ADDRESS
            QTreeWidgetItem *item_adr = new QTreeWidgetItem();
            QString str_adr = QString::asprintf("%X %X %X %X: %X%X", ptr_sub->recipientProcess.recipient.RecipientType.Address.adr[0],
                ptr_sub->recipientProcess.recipient.RecipientType.Address.adr[1],
                ptr_sub->recipientProcess.recipient.RecipientType.Address.adr[2],
                ptr_sub->recipientProcess.recipient.RecipientType.Address.adr[3],
                ptr_sub->recipientProcess.recipient.RecipientType.Address.adr[4],
                ptr_sub->recipientProcess.recipient.RecipientType.Address.adr[5]);

            item_adr->setText(0, str_adr);
            item_sub->insertChild(0, item_adr);
        }


        QTreeWidgetItem *item_ref = new QTreeWidgetItem();
        QTreeWidgetItem *item_obj = new QTreeWidgetItem();
        QTreeWidgetItem *item_prop = new QTreeWidgetItem();

        QString str_obj = QString::asprintf("%s, %d", BACAPI_BACnetObjectName(ptr_sub->monitoredProperty.oid.type), ptr_sub->monitoredProperty.oid.instance);
        QString str_prop = QString::asprintf(BACAPI_BACnetPropertyName(ptr_sub->monitoredProperty.propertyId));

        item_obj->setText(0, str_obj);
        item_prop->setText(0, str_prop);

        item_ref->insertChild(0, item_prop);
        item_ref->insertChild(0, item_obj);
        item_sub->insertChild(0, item_ref);

        QTreeWidgetItem *item_conf = new QTreeWidgetItem();
        QTreeWidgetItem *item_inc = new QTreeWidgetItem();
        QTreeWidgetItem *item_time = new QTreeWidgetItem();
        QTreeWidgetItem *item_recid = new QTreeWidgetItem();

        if (ptr_sub->confirmed)
        {
            item_conf->setText(0, "TRUE");
        }
        else item_conf->setText(0, "FALSE");

        item_inc->setText(0, BACAPI_ToString((int)ptr_sub->covIncrement).c_str());
        item_time->setText(0, BACAPI_ToString(ptr_sub->timeRemaining).c_str());
        item_recid->setText(0, BACAPI_ToString(ptr_sub->recipientProcess.recipientProcessId).c_str());

        item_ref->insertChild(0, item_conf);
        item_ref->insertChild(0, item_inc);
        item_sub->insertChild(0, item_time);
        item_sub->insertChild(0, item_recid);
    }


}



void BACnetOScope::RefreshCalendarDateList(BACNET_OBJECT_PROPERTY * property)
{


    size_t count = property->complexValue.vCalendarEntryList.size();

    for (size_t c = 0; c < count; c++)
    {
        QTreeWidgetItem *item_entry = new QTreeWidgetItem();
        QString str_entry = QString::asprintf("Date Entry: %d", c + 1);
        item_entry->setText(0, str_entry);

        BACNET_CALENDAR_ENTRY *ptr_entry = property->complexValue.vCalendarEntryList[c];


        // date / daterange / weeknday
        switch (ptr_entry->TagEntryType)
        {
        case 0:  // [0] Date
        {
            QTreeWidgetItem *item_date = new QTreeWidgetItem();
            QDate date;

            date.setDate(ptr_entry->Entry.date.year, ptr_entry->Entry.date.month, ptr_entry->Entry.date.day);
            QString str_date = date.toString();
            item_date->setText(0, str_date);

            item_entry->insertChild(0, item_date);
            break;
        }
        case 1:  // [1] BACnetDateRange,
        {
            QDate sd, ed;

            sd.setDate(ptr_entry->Entry.dateRange.startdate.year, ptr_entry->Entry.dateRange.startdate.month, ptr_entry->Entry.dateRange.startdate.day);
            ed.setDate(ptr_entry->Entry.dateRange.enddate.year, ptr_entry->Entry.dateRange.enddate.month, ptr_entry->Entry.dateRange.enddate.day);


            QString str_sd = QString::asprintf("Start Date: %s", sd.toString().toStdString().c_str());
            QString str_ed = QString::asprintf("End Date: %s", ed.toString().toStdString().c_str());

            QTreeWidgetItem *item_sd = new QTreeWidgetItem();
            QTreeWidgetItem *item_ed = new QTreeWidgetItem();

            item_sd->setText(0, str_sd);
            item_ed->setText(0, str_ed);

            item_entry->insertChild(0, item_sd);
            item_entry->insertChild(0, item_ed);

            break;
        }
        case 2:   // [2] BACnetWeekNDay
        {
            QTreeWidgetItem *item_wnd = new QTreeWidgetItem();
            QString str_wnd = QString::asprintf("Month: %d, WoM: %d, DoW: %d", ptr_entry->Entry.weekNDay.month,
                ptr_entry->Entry.weekNDay.weekofmonth,
                ptr_entry->Entry.weekNDay.dayofweek);

            item_entry->insertChild(0, item_wnd);
            break;
        }

        default:
            break;

        }

        ptr_valueTree->insertTopLevelItem(0, item_entry);
    }
}



void BACnetOScope::RefreshPriorityArray(BACNET_OBJECT_PROPERTY *property)
{

    int cnt = 16;


    int count = (int)property->vValueList.size();

    for (int c = 0; c < count; c++)
    {
        BACNET_APPLICATION_DATA_VALUE *ptr_value = property->vValueList[c];

        QTreeWidgetItem *item_value = new QTreeWidgetItem();
        QString str_value = BACAPI_GetStringValue(ptr_value).c_str();
        item_value->setText(0, str_value);

        ptr_valueTree->insertTopLevelItem(0, item_value);
    }

}


// TODO:
void BACnetOScope::RefreshReadAccessSpecification(BACNET_OBJECT_PROPERTY *property)
{
/*

    BACNET_READ_ACCESS_SPECIFICATION *linkedValue = property->complexValue.readAccessSpec;

    while (linkedValue != NULL)
    {
            char oidtxt[256];
            sprintf(oidtxt, "%s, %d", BACAPI_BACnetObjectName(linkedValue->objectIdentifier.type), linkedValue->objectIdentifier.instance);

            COptionTreeItem *otiOID = ptrOptionTree_->InsertItem(new COptionTreeItem(), otiReadAccess);
            otiOID->SetLabelText(oidtxt);
            otiOID->SetInfoText("Click '+' to expand");


            // ispisi sve prop ref za ovaj objekt EMPTY_PROPERTY_REFERENCE_ID
            for (int c = 0; c < MAX_LIST_OF_PROPERTY_REFERENCES; c++)
            {
                    if (linkedValue->listOfPropertyReferences[c].propertyIdentifier != EMPTY_PROPERTY_REFERENCE_ID)
                    {
                            COptionTreeItemEdit *editprop = (COptionTreeItemEdit*)ptrOptionTree_->InsertItem(new COptionTreeItemEdit(), otiOID);
                            editprop->SetLabelText("Property");
                            editprop->CreateEditItem(NULL, NULL);
                            editprop->SetWindowText(BACAPI_BACnetPropertyName(linkedValue->listOfPropertyReferences[c].propertyIdentifier));
                    }

            }

            linkedValue = linkedValue->next;
    }
        */
}






void BACnetOScope::RefreshDeviceAddressBinding(BACNET_OBJECT_PROPERTY * property)
{

    int count = (int)property->complexValue.vBACnetAddressBindingList.size();


    for (int c = 0; c < count; c++)
    {
        BACNET_ADDRESS_BINDING *value = property->complexValue.vBACnetAddressBindingList[c];

        QTreeWidgetItem *item_entry = new QTreeWidgetItem();
        QString str_entry = QString::asprintf("Entry %d", c + 1);
        item_entry->setText(0, str_entry);

        QTreeWidgetItem *item_oid = new QTreeWidgetItem();
        QTreeWidgetItem *item_nn = new QTreeWidgetItem();
        QTreeWidgetItem *item_adr = new QTreeWidgetItem();

        QString str_oid = QString::asprintf("%s, Instance : %d", BACAPI_BACnetObjectName(value->deviceOid.type), value->deviceOid.instance);
        QString str_nn = QString::asprintf("Network Number: %d", value->deviceAddress.networkNumber);
        QString str_adr = QString::asprintf("Address %s", BACAPI_GetAddressString(&value->deviceAddress, value->deviceAddress.len).c_str());

        item_oid->setText(0, str_oid);
        item_nn->setText(0, str_nn);
        item_adr->setText(0, str_adr);

        item_entry->insertChild(0, item_adr);
        item_entry->insertChild(0, item_nn);
        item_entry->insertChild(0, item_oid);

        ptr_valueTree->insertTopLevelItem(0, item_entry);
    }

}



void BACnetOScope::RefreshActionText(BACNET_OBJECT_PROPERTY * property)
{

    BACNET_ACTION_TEXT *linkedValue = property->complexValue.actionText;

    int c = 1;
    while (linkedValue != NULL)
    {
        QTreeWidgetItem *item_value = new QTreeWidgetItem();
        QString str_value = QString::asprintf("Action Text %d: %s", c++, linkedValue->value.type.character_String.value8.c_str());
        item_value->setText(0, str_value);

        ptr_valueTree->insertTopLevelItem(0, item_value);
        linkedValue = linkedValue->next;
    }
}


void BACnetOScope::RefreshEnumList(BACNET_OBJECT_PROPERTY *property)
{
    int count = (int)property->vValueList.size();
    for (int c = 0; c < count; c++)
    {
        BACNET_APPLICATION_DATA_VALUE *ptr_value = property->vValueList[c];

        QTreeWidgetItem *item_value = new QTreeWidgetItem();
        QString str_value = QString::asprintf("Item %d:  Value %d", c++, ptr_value->type.Enumerated);
        item_value->setText(0, str_value);

        ptr_valueTree->insertTopLevelItem(0, item_value);
    }
}



void BACnetOScope::RefreshScale(BACNET_OBJECT_PROPERTY *property)
{

    if (property->complexValue.scale.type == 0)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        QString str = QString::asprintf("Float: %f", property->complexValue.scale.floatScale);
        item->setText(0, str);
        ptr_valueTree->insertTopLevelItem(0, item);

    }
    else {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        QString str = QString::asprintf("Integer: %d", property->complexValue.scale.floatScale);
        item->setText(0, str);
        ptr_valueTree->insertTopLevelItem(0, item);
    }

}


void BACnetOScope::RefreshPreScale(BACNET_OBJECT_PROPERTY *property)
{

    QTreeWidgetItem *item_multi = new QTreeWidgetItem();
    QTreeWidgetItem *item_modulo = new QTreeWidgetItem();


    QString str_multi = QString::asprintf("Multipler: %d", property->complexValue.preScale.multiplier);
    QString str_modulo = QString::asprintf("Modulo Devide: %d", property->complexValue.preScale.moduloDivide);

    item_multi->setText(0, str_multi);
    item_modulo->setText(0, str_modulo);

    ptr_valueTree->insertTopLevelItem(0, item_multi);
    ptr_valueTree->insertTopLevelItem(0, item_modulo);

    ptr_valueTree->insertTopLevelItem(0, item_multi);
    ptr_valueTree->insertTopLevelItem(0, item_modulo);

}



void BACnetOScope::RefreshHostNPort(BACNET_OBJECT_PROPERTY *property)
{

    QTreeWidgetItem *item = new QTreeWidgetItem();
    switch (property->complexValue.hostnPort->address.tag)
    {
    case 0:
        item->setText(0, "Host Port: N/A");
        break;

    case 1:
    {
        uint8_t *val = BACAPI_GetOctetStringValue(&property->complexValue.hostnPort->address.type.ip_address);
        string s = BACAPI_GetOctetData(val, property->complexValue.hostnPort->address.type.ip_address.vValue.size());

        QString str_val = QString::asprintf("Host Port - OCT: %s", s.c_str());
        item->setText(0, str_val);


        break;
    }

    case 2:
    {
        QString str_val = QString::asprintf("Host Port: %s", property->complexValue.hostnPort->address.type.name.value8.c_str());
        item->setText(0, str_val);
        break;
    }
    }


    ptr_valueTree->insertTopLevelItem(0, item);

}



void BACnetOScope::RefreshBACnetBDTEntry(BACNET_OBJECT_PROPERTY *property)
{
    size_t count = property->complexValue.vBDTEntry.size();

    for (size_t c = 0; c < count; c++)
    {
        BACNET_BDT_ENTRY *entry = property->complexValue.vBDTEntry[c];

        QTreeWidgetItem *item_entry = new QTreeWidgetItem();
        QTreeWidgetItem *item_adr = new QTreeWidgetItem();

        switch (property->complexValue.hostnPort->address.tag)
        {
        case 0:
            item_adr->setText(0, "Host Port: N/A");
            break;

        case 1:
        {
            uint8_t *val = BACAPI_GetOctetStringValue(&property->complexValue.hostnPort->address.type.ip_address);
            string s = BACAPI_GetOctetData(val, property->complexValue.hostnPort->address.type.ip_address.vValue.size());

            QString str_val = QString::asprintf("Host Port - OCT: %s", s.c_str());
            item_adr->setText(0, str_val);


            break;
        }

        case 2:
        {
            QString str_val = QString::asprintf("Host Port: %s", property->complexValue.hostnPort->address.type.name.value8.c_str());
            item_adr->setText(0, str_val);
            break;
        }
        default: break;
        }

        item_entry->insertChild(0, item_adr);

        QTreeWidgetItem *item_port = new QTreeWidgetItem();
        QTreeWidgetItem *item_mask = new QTreeWidgetItem();


        // port
        QString str_port = QString::asprintf("Port: %d", BACAPI_ToString(entry->bbmd_address.port).c_str());
        item_port->setText(0, str_port);


        uint8_t *val = BACAPI_GetOctetStringValue(&entry->broadcast_mask);
        QString str_mask = BACAPI_GetOctetData(val, entry->broadcast_mask.vValue.size()).c_str();
        item_mask->setText(0, str_mask);

        item_entry->insertChild(0, item_port);
        item_entry->insertChild(0, item_mask);

        ptr_valueTree->insertTopLevelItem(0, item_entry);
    }

}


void BACnetOScope::RefreshPropertyList(uint32_t property)
{
}
void BACnetOScope::RefreshProtocolServicesSupported(BACNET_OBJECT_PROPERTY *property)
{
}
void BACnetOScope::RefreshProtocolTypesSupported(BACNET_OBJECT_PROPERTY *property)
{
}
void BACnetOScope::RefreshBitString(BACNET_BIT_STRING *bstr)
{
}
void BACnetOScope::RefreshStatusFlags(BACNET_BIT_STRING *bstr)
{
}
void BACnetOScope::RefreshStatusFlags(BACNET_OBJECT_PROPERTY *property)
{
}
void BACnetOScope::RefreshWeeklySchedule(BACNET_OBJECT_PROPERTY *property)
{
}
void BACnetOScope::RefreshPriority(BACNET_OBJECT_PROPERTY *property)
{
}
void BACnetOScope::RefreshUnsignedValuesList(BACNET_OBJECT_PROPERTY *property)
{
}
void BACnetOScope::RefreshCommandAction(BACNET_OBJECT_PROPERTY *property)
{
}

void BACnetOScope::RefreshSpecialEvent(BACNET_OBJECT_PROPERTY *property)
{
}

void BACnetOScope::RefreshLoggingRecord(BACNET_OBJECT_PROPERTY *property)
{
}


void BACnetOScope::RefreshBACnetFDTEntry(BACNET_OBJECT_PROPERTY *property)
{
}
void BACnetOScope::RefreshBACnetVMACEntry(BACNET_OBJECT_PROPERTY *property)
{
}
void BACnetOScope::RefreshBACnetRouterEntry(BACNET_OBJECT_PROPERTY *property)
{
}
void BACnetOScope::RefreshBACnetAssignedAccessRights(BACNET_OBJECT_PROPERTY *property)
{
}
void BACnetOScope::RefreshCredentialAuthenticationFactors(BACNET_OBJECT_PROPERTY *property)
{
}
void BACnetOScope::RefreshSecurityKeySets(BACNET_OBJECT_PROPERTY *property)
{
}
void BACnetOScope::RefreshNetworkAccessSecurityPolicies(BACNET_OBJECT_PROPERTY *property)
{
}
void BACnetOScope::RefreshValueSource(BACNET_OBJECT_PROPERTY *property)
{
}

void BACnetOScope::RefreshAckRequired(BACNET_OBJECT_PROPERTY * property)
{
    // PRAZNA - Nije Complex?
}



void BACnetOScope::RefreshConfigurationFilesList(BACNET_OBJECT_PROPERTY * property)
{
    // prazna
}




void BACnetOScope::RefreshDeviceObjectList(BACNET_OBJECT_PROPERTY * property)
{
    // prazna
}


void BACnetOScope::RefreshLoopObjectActionProperty(BACNET_OBJECT_PROPERTY *property)
{
    // razlicite vrijednosti u odnosu na objekt,
    // ne izvrasuje se
}


void BACnetOScope::RefreshFaultParameters(BACNET_OBJECT_PROPERTY *property)
{
    // TODO:
}



int BACnetOScope::GetPercentage(int num, int p)
{
    int result = num * p / 100;
    return result;
}



void BACnetOScope::LoadSettings(void)
{
    QFile file;
#ifdef __ANDROID__

    bool ret = false;
    QString str_ret;
    QString DatabaseDataStoragePath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppDataLocation);
    //QString DatabaseDataStoragePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    DatabaseDataStoragePath += "/bacdata.sys";
/*


    // ako je ovo prvo pokretanje
    QFile bac_file(DatabaseDataStoragePath);
    if (bac_file.exists() == false)
    {
     SaveSettins();
      ret = bac_file.open(QIODevice::WriteOnly);
         str_ret = file.errorString();


      bac_file.close();

      ret = bac_file.exists();
     //   file.setFileName("assets:/bacdata.sys");
     //   ret = file.copy(DatabaseDataStoragePath);
      //   str_ret = file.errorString();

        QFile::setPermissions("DatabaseDataStoragePath",QFile::WriteOwner | QFile::ReadOwner);
        printf("");
    }
*/
  file.setFileName(DatabaseDataStoragePath);
#else

 file.setFileName("data/bacdata.sys");
#endif

 if (!file.open(QIODevice::ReadOnly))
    {
     SaveSettings();
     if (!file.open(QIODevice::ReadOnly))
        {
         return;
        }
    }


 QByteArray line = file.readLine();




 uint8_t *data = new uint8_t[line.count()];
 int size = line.size();
 for (int c = 0; c < size; c++)
 {
     data[c] = (uint8_t)line.at(c);
 }



 settings_.whois = data[0];
 settings_.names = data[1];
 settings_.wire = data[2];

 gDec->decode_unsigned16(&data[3], &settings_.port);


 QString str = QString::asprintf("%d.%d.%d.%d",data[5],data[6],data[7],data[8]);

 ptr_ui->chWhoIsOnStart->setChecked(settings_.whois);
 ptr_ui->cbShowObjectNames->setChecked(settings_.names);
 ptr_ui->cbWireshark->setChecked(settings_.wire);

 ptr_ui->editPort->setText(QString(settings_.port));
 ptr_ui->editRegisterAddress->setText(str);

 settings_.lastadr = str;
}


void BACnetOScope::SaveSettings(void)
{

    settings_.whois = ptr_ui->chWhoIsOnStart->isChecked();
    settings_.names = ptr_ui->cbShowObjectNames->isChecked();
    settings_.wire = ptr_ui->cbWireshark->isChecked();
    settings_.port = ptr_ui->editPort->text().toInt();
    settings_.lastadr = ptr_ui->editRegisterAddress->text();

    QString DatabaseDataStoragePath;

#ifdef __ANDROID__
    bool ret = false;
    DatabaseDataStoragePath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppDataLocation);
    DatabaseDataStoragePath += "/bacdata.sys";
#else
    DatabaseDataStoragePath = "data/bacdata.sys";
#endif

    QFile file(DatabaseDataStoragePath);
    file.setPermissions(DatabaseDataStoragePath,QFile::WriteOwner | QFile::ReadOwner);

    if (!file.open(QIODevice::WriteOnly))
    {
      QString str = file.errorString();
      printf("");
        return;
    }

    int len = 0;
    uint8_t buffer[32];

    buffer[len++] = settings_.whois;
    buffer[len++] = settings_.names;
    buffer[len++] = settings_.wire;

    len += gDec->encode_unsigned16(&buffer[len],settings_.port);


    QHostAddress addr(settings_.lastadr);
    qint32 int_src_adr = addr.toIPv4Address();

    BACNET_ADDRESS *bacadr = BACAPI_CreateBACnetAddress(int_src_adr, 0, 0);
    buffer[len++] = bacadr->adr[0];
    buffer[len++] = bacadr->adr[1];
    buffer[len++] = bacadr->adr[2];
    buffer[len++] = bacadr->adr[3];

    QByteArray data((char*)buffer, len);

    file.write(data);
    file.close();

}



void BACnetOScope::OnTimer_FadeIntro(void)
{


    // w is your widget
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    ptr_ui->labelSplash->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
    a->setDuration(1000);
    a->setStartValue(1);
    a->setEndValue(0);
    a->setEasingCurve(QEasingCurve::OutBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);
    connect(a, SIGNAL(finished()), this, SLOT(OnTimer_FinishedFadeout()));

}


void BACnetOScope::OnTimer_FinishedFadeout(void)
{
	ptr_ui->labelSplash->hide();
    ptr_buttonLeft->show();
    ptr_buttonRight->show();

}



QTimer *timer = NULL;





void BACnetOScope::OnTimer_TapAndHold(void)
{
    qDebug() << "BACnetOScope::OnTimer_TapAndHold()";

    delete timer;
    timer = NULL;
  TapAndHoldTriggered();
}


bool BACnetOScope::eventFilter(QObject *obj, QEvent *event) 
{
 //qDebug() << "BACnetOScope::eventFilter";
 /*
 QString name = obj->objectName();
 printf("name = %s\n",name.toStdString().c_str());

 if (object == target && event->type() == QEvent::KeyPress) {
	 QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
	 if (keyEvent->key() == Qt::Key_Tab) {
		 // Special tab handling
		 return true;
	 }
	 else
		 return false;
 }
 return false;
 */
 return false;
}



bool BACnetOScope::event(QEvent *event)
{


    return QWidget::event(event);
}


void BACnetOScope::TapAndHoldTriggered(void)
{
    switch (currentView_)
    {
    case SELECTED_VIEW_DEVICE:
    {
        BACNET_DEVICE *dev = GetSelectedDevice();
        if (dev == NULL)
            return;

        BACAPI_DeleteObjectList(dev);
        ptr_objectTree->clear();
        QTreeWidgetItem *sel_item = ptr_deviceTree->currentItem();
        OnClick_DeviceTreeItem(sel_item, 0);
        break;
    }

    case SELECTED_VIEW_OBJECT:
    {
        BACNET_OBJECT *obj = GetSelectedObject();
        if (obj == NULL)
            return;

        BACAPI_DeleteProperties(obj);
        ptr_propertyTree->clear();
        QTreeWidgetItem *sel_item = ptr_objectTree->currentItem();
        OnClick_ObjectTreeItem(sel_item, 0);
        break;
    }

    case SELECTED_VIEW_PROPERTY:
    case SELECTED_VIEW_VALUE:
    case SELECTED_VIEW_WRITE:
    default: break;
    }
}
