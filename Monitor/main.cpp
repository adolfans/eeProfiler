#include "monitor.h"
#include <QtGui/QApplication>
#include "connectdialog.h"
#include "ProfilerNetwork.h"
#include "monitor.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	InitializeSocket();
	ConnectDialog dlg;
	dlg.show();
	Monitor _monitor;
	dlg.setMonitor( &_monitor );
	int returnValue = a.exec();
	UninitializeSocket();
	return returnValue;
}
