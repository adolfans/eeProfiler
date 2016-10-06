#include "monitor.h"
#include <QScrollArea>
#include <QGroupBox>
#include "curverenderer.h"

//-----------------------------------------------------------------------
Monitor::Monitor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags),
	connectedReceiver( NULL ),
	frameCurveRenderer( NULL )
{
	ui.setupUi(this);
	verticalLayout = ui.verticalLayout;
	scrollAreaWidgetContents = ui.scrollAreaWidgetContents;

	frameGroupBox = new QGroupBox(scrollAreaWidgetContents);
	frameGroupBox->setTitle("Frame");
	verticalLayout->addWidget(frameGroupBox);

}

//-----------------------------------------------------------------------
Monitor::~Monitor()
{
	if( connectedReceiver )
	{
		connectedReceiver->CloseConnection();
	}
	receiveThread.quit();
	receiveThread.wait();
}

//-----------------------------------------------------------------------
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
	emit startAsyncWork();
}

//-----------------------------------------------------------------------
void Monitor::handleResults( sFrameResult* rs )
{
	double _stamp = rs->GetTimeStamp();
	double _length = rs->GetLength();
	
	if( !frameCurveRenderer )
	{
		QVBoxLayout* layoutInGroupBox = new QVBoxLayout(frameGroupBox);
		layoutInGroupBox->setSpacing(6);
		layoutInGroupBox->setContentsMargins(11, 11, 11, 11);
		frameCurveRenderer = new CurveRenderer( this );
		layoutInGroupBox->addWidget( frameCurveRenderer );
	}

	frameCurveRenderer->AddValue( _stamp, _length );

	for( sProfileEntry* entry = rs->GetFirstProfileEntry();
		entry;
		entry = entry->Next() )
	{
		long entryId = entry->GetId();
		QMap<long,CurveRenderer*>::iterator
			itr = idToCurveRenderer.find( entryId );
		if( itr == idToCurveRenderer.end() )
		{
			QVBoxLayout* layoutInGroupBox = new QVBoxLayout(frameGroupBox);
			layoutInGroupBox->setSpacing(6);
			layoutInGroupBox->setContentsMargins(11, 11, 11, 11);
			CurveRenderer* entryCurveRenderer = new CurveRenderer( this );
			layoutInGroupBox->addWidget( entryCurveRenderer );
			itr = idToCurveRenderer.insert( entryId, entryCurveRenderer ); 
		}
		itr.value()->AddValue( _stamp, entry->GetTime() );
	}


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