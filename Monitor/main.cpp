#include "monitor.h"
#include <QtGui/QApplication>
#include "connectdialog.h"
#include "ProfilerNetwork.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	InitializeSocket();
	Monitor w;
	//w.show();
	ConnectDialog dlg;
	dlg.show();
	int returnValue = a.exec();
	UninitializeSocket();
	return returnValue;
}
