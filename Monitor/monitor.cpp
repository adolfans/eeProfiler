#include "monitor.h"

Monitor::Monitor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
}

Monitor::~Monitor()
{

}

void Monitor::SocketConnnected( int )
{

}