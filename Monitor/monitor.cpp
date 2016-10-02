#include "monitor.h"


Monitor::Monitor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags),
	connectedReceiver( NULL )
{
	ui.setupUi(this);
}

Monitor::~Monitor()
{
	if( connectedReceiver )
	{
		connectedReceiver->CloseConnection();
	}
	receiveThread.quit();
	receiveThread.wait();
}

void Monitor::Connected( Receiver* receiver )
{
	connectedReceiver = receiver;

	ResultReceiver *worker = new ResultReceiver;
	worker->SetReceiver( receiver );
	worker->moveToThread(&receiveThread);
	connect(&receiveThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
	connect(this, SIGNAL(startAsyncWork()), worker, SLOT(onStart()));
	connect(worker, SIGNAL(onReceived(sFrameResult*)), this, SLOT(handleResults(sFrameResult*)));
	receiveThread.start();
	
}

//-----------------------------------------------------------------------
void Monitor::handleResults( sFrameResult* rs )
{
	
	delete rs;
}

//-----------------------------------------------------------------------
ResultReceiver::ResultReceiver()
{

}

//-----------------------------------------------------------------------
ResultReceiver::~ResultReceiver()
{

}

//-----------------------------------------------------------------------
void ResultReceiver::SetReceiver( Receiver* rec )
{
	receiver = rec;
}

//-----------------------------------------------------------------------
void ResultReceiver::onStart()
{
	for(;;)
	{
		sFrameResult* result = new sFrameResult;
		bool rs = receiver->WaitToReceivePacket( *result );
		if( !rs )
		{
			delete result;
			return;
		}else
		{
			emit onReceived( result );
		}

	}
}