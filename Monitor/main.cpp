#include "monitor.h"
#include <QtGui/QApplication>
#include "connectdialog.h"
#include "ProfilerNetwork.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	InitializeSocket();
	ConnectDialog dlg;
	dlg.show();
	if( dlg.result() == QDialog::Accepted )
	{
		Monitor w;
		w.Connected( dlg.GetConnectedReceiver() );
		w.show();
	}
	int returnValue = a.exec();
	UninitializeSocket();
	return returnValue;
}
