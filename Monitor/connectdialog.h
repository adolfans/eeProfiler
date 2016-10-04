#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QWidget>
#include <QDialog>
class QLabel;
class QLineEdit;
class QDialogButtonBox;
class Receiver;
class Monitor;
/*
===========================================
class ConnectDialog
===========================================
*/
class ConnectDialog : public QDialog
{
	Q_OBJECT

public:
	ConnectDialog(QWidget *parent = 0);
	~ConnectDialog();
	void setMonitor( Monitor* );
public slots:
	void onConnect();
private:
	bool TryToConnect( const char* address, int port );

	QLabel *hostLabel;
	QLabel *portLabel;
	QLineEdit *hostLineEdit;
	QLineEdit *portLineEdit;

	QLabel *statusLabel;

	QPushButton *connectButton;
	QPushButton *quitButton;
	QDialogButtonBox *buttonBox;

	Receiver* clientReceiver;
	Monitor* monitor;
};

#endif // CONNECTDIALOG_H
