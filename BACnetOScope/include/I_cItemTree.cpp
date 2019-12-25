#include "I_cItemTree.h"
#include "qscroller.h"
#include "qpainter.h"
#include <qdebug.h>

I_cItemTree::I_cItemTree(QWidget *parent): QTreeWidget(parent)
{
     printf("Creating tree\n");
		 //QScroller::grabGesture(viewport(), QScroller::LeftMouseButtonGesture);
         //setAttribute(Qt::WA_AcceptTouchEvents);
         grabGesture(Qt::TapAndHoldGesture);


         timer = NULL;

         //this->installEventFilter(this);
}



bool I_cItemTree::eventFilter(QObject *obj, QEvent *e)
{

    /*
    if( obj == window )
        return true; //should discard the signal (?)
    else
        return false; // I tried setting this to 'true' also without success
        */
    //qDebug() << "I_cItemTree::eventFilter: " << e->type();
    return false;
}


void I_cItemTree::OnTimer_TapAndHold(void)
{
    //qDebug() << "BACnetOScope::OnTimer_TapAndHold()";


    if (timer)
       {
        delete timer;
        timer = NULL;
       }
     //emit Handle_TapAndHoldTriggered();
}

void I_cItemTree::inputMethodQuery(Qt::InputMethodQuery,QVariant)
{
 qDebug() << "I_cItemTree::inputMethodQuery";
}


bool I_cItemTree::event(QEvent *event)
{

 switch (event->type())
 {

  case QEvent::TouchBegin:
      {
       qDebug() << "BEGIN!";

       //timer = new QTimer(this);
       //connect(timer, SIGNAL(timeout()), this, SLOT(OnTimer_TapAndHold()));
       //timer->start(500);


       //event->ignore();
       event->accept();
       //bool ret = QWidget::event(event);
       return true;
      }

  case QEvent::TouchEnd:
    qDebug() << "END!";

    if (timer)
    {
     delete timer;
     timer = NULL;
    }
    event->ignore();

    return QWidget::event(event);

case QEvent::TouchCancel:
     qDebug() << "CANCEL!";
   break;
    //event->accept();
    //return true;
    //returnQWidget::event(event);
    //return true;



 default:
          //qDebug() << event->type();
    break;
  }


    return QWidget::event(event);

}

//
//
//void I_cItemTree::paintEvent(QPaintEvent *event)
//{
//
//	QPainter p(viewport());
//
//    //QPainter p()
//
//    /*
//    const qreal iw = currentImage.width();
//    const qreal ih = currentImage.height();
//    const qreal wh = height();
//    const qreal ww = width();
//
//    p.translate(ww/2, wh/2);
//    p.translate(horizontalOffset, verticalOffset);
//    p.rotate(rotationAngle);
//    p.scale(currentStepScaleFactor * scaleFactor, currentStepScaleFactor * scaleFactor);
//    p.translate(-iw/2, -ih/2);
//    p.drawImage(0, 0, currentImage);
//    */
//}
