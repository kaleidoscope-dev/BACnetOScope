#pragma once
#include <qthread.h>
#include "qtreewidget.h"
#include "qgesture.h"
#include <qtimer.h>
#include <QTouchEvent>

// DEPRECATED, Legacy include
// TODO: maknuti


class I_cItemTree : public QTreeWidget
{
    Q_OBJECT

public:
    explicit I_cItemTree(QWidget *parent = 0);


   // void grabGestures(const QList<Qt::GestureType> &gestures);

 protected:



    QTimer *timer;

 bool event(QEvent *event) override;
  bool eventFilter(QObject *, QEvent*) override;
 //void paintEvent(QPaintEvent *event) override;


    Q_INVOKABLE
   void inputMethodQuery(Qt::InputMethodQuery,QVariant);

signals:
 void Handle_TapAndHoldTriggered(void);


private:
/*
    bool gestureEvent(QGestureEvent *event);
    void panTriggered(QPanGesture*);
    void pinchTriggered(QPinchGesture*);
    void swipeTriggered(QSwipeGesture*);
		*/

private slots:
 void OnTimer_TapAndHold(void);


};
