#ifndef MONITOR_H
#define MONITOR_H

#include <QtGui/QMainWindow>
#include "ui_monitor.h"
#include <QThread>
class Receiver;
class sFrameResult;
#include "ProfilerClient.h"
//Q_DECLARE_METATYPE ( sFrameResult* )
/*
===========================================
class Monitor
main window
===========================================
*/
class Monitor : public QMainWindow
{
	Q_OBJECT

public:
	Monitor(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Monitor();
	void Connected( Receiver* );

public slots:
	void handleResults( sFrameResult* );
signals:
	void startAsyncWork();
private:
	QThread		receiveThread;
	Ui::MonitorClass ui;
	Receiver* connectedReceiver;
};


/*
===========================================
class ResultReceiver
Receive frame results in async thread
===========================================
*/
class ResultReceiver : public QObject
{
	Q_OBJECT
public:
	ResultReceiver();
	~ResultReceiver();
	void SetReceiver( Receiver* );
public slots:
	void onStart();
signals:
	void onReceived( sFrameResult* );

private:
	Receiver* receiver;
};

#endif // MONITOR_H
