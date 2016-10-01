#ifndef MONITOR_H
#define MONITOR_H

#include <QtGui/QMainWindow>
#include "ui_monitor.h"

class Monitor : public QMainWindow
{
	Q_OBJECT

public:
	Monitor(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Monitor();

private:
	Ui::MonitorClass ui;
};

#endif // MONITOR_H
