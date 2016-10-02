#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QWidget>
#include <QDialog>
class QLabel;
class QLineEdit;
class QDialogButtonBox;
class Receiver;
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
	Receiver* GetConnectedReceiver();
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
};

#endif // CONNECTDIALOG_H
