#include "connectdialog.h"
#include <QLabel>
#include <QLineEdit>
#include <qvalidator.h>
#include <QList>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>
ConnectDialog::ConnectDialog(QWidget *parent)
	: QDialog(parent)
{

	hostLabel = new QLabel(tr("&Server name:"));
	portLabel = new QLabel(tr("S&erver port:"));

#if 0
	// find out which IP to connect to
	QString ipAddress;
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	// use the first non-localhost IPv4 address
	for (int i = 0; i < ipAddressesList.size(); ++i) {
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
			ipAddressesList.at(i).toIPv4Address()) {
				ipAddress = ipAddressesList.at(i).toString();
				break;
		}
	}
	// if we did not find one, use IPv4 localhost
	if (ipAddress.isEmpty())
		ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
#endif

	hostLineEdit = new QLineEdit;
	portLineEdit = new QLineEdit;
	portLineEdit->setValidator(new QIntValidator(1, 65535, this));

	statusLabel = new QLabel;
	statusLabel->setWordWrap(true);

	hostLabel->setBuddy(hostLineEdit);
	portLabel->setBuddy(portLineEdit);

	connectButton = new QPushButton(tr("Connect"));
	connectButton->setDefault(true);
	connectButton->setEnabled(false);

	quitButton = new QPushButton(tr("Quit"));

	buttonBox = new QDialogButtonBox;
	buttonBox->addButton(connectButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

	connect(connectButton, SIGNAL(clicked()), this, SLOT(onConnect()));
	connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

	connect(hostLineEdit, SIGNAL(textChanged(QString)),
		this, SLOT(enableGetFortuneButton()));
	connect(portLineEdit, SIGNAL(textChanged(QString)),
		this, SLOT(enableGetFortuneButton()));
	
	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addWidget(hostLabel, 0, 0);
	mainLayout->addWidget(hostLineEdit, 0, 1);
	mainLayout->addWidget(portLabel, 1, 0);
	mainLayout->addWidget(portLineEdit, 1, 1);
	mainLayout->addWidget(buttonBox, 3, 0, 1, 2);
	mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
	setLayout(mainLayout);

	setWindowTitle(tr("Connect to..."));
	portLineEdit->setFocus();

	setLayout(mainLayout);
}

ConnectDialog::~ConnectDialog()
{

}

int ConnectDialog::GetConnectedSocket()
{
	return clientFd;
}

//-----------------------------------------------------------------------
void ConnectDialog::onConnect()
{
	QString address = hostLineEdit->text();
	int port = 	portLineEdit->text().toInt();
	bool rs = TryToConnect( address.toAscii(), port );
	if( !rs )
	{
		statusLabel->setText(tr("Failed to connect to remote process..."));
	}else
	{
		accept();
	}
}

#include <WinSock2.h>

//-----------------------------------------------------------------------
bool ConnectDialog::TryToConnect( const char* address, int port )
{
	clientFd = ::socket( AF_INET, SOCK_STREAM, 0 );

	bool closeSocketAndReturnFalse = false;

	::sockaddr_in servaddr;
	memset( &servaddr, 0, sizeof(servaddr) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = ::inet_addr( address );
	servaddr.sin_port = ::htons( port );

	int bindResult = ::connect( clientFd, (::sockaddr*)&servaddr, sizeof(servaddr) );
	if( bindResult == SOCKET_ERROR )
	{
		int errorcode = errno;
		printf( "Failed to connect, errorcode: %d", (int)port, errorcode );
		closeSocketAndReturnFalse = true;
	}

	if( closeSocketAndReturnFalse )
	{
		::closesocket( clientFd );
		return false;
	}

	return true;
}