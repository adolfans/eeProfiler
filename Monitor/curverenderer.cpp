#include "curverenderer.h"
#include <QQueue>
#include <QGraphicsPolygonItem>

class CurveGraphicsRenderer
	: public QGraphicsScene
{

public:
	CurveGraphicsRenderer(QObject *parent = 0);
	~CurveGraphicsRenderer();

	void AddValue( float time, float value );
	void SetRange( int seconds );

private:
	int range;	//Default value: 30 seconds
	QPolygonF	points;
	QGraphicsPolygonItem* curve;
};

CurveGraphicsRenderer::CurveGraphicsRenderer( QObject *parent )
	: range( 30 )
{
	curve = new QGraphicsPolygonItem();
}

CurveGraphicsRenderer::~CurveGraphicsRenderer()
{
	delete curve;
}

void CurveGraphicsRenderer::AddValue( float time, float value )
{
	points.push_back( QPointF( time, value ) );
	if( time - points[0].rx() > ( float )range )
	{
		points.remove( 0 );
	}
	curve->setPolygon( points );
	//curve->
}

curveRenderer::curveRenderer(QWidget *parent)
	: QGraphicsView(parent)
{
}

curveRenderer::~curveRenderer()
{

}

//-----------------------------------------------------------------------
void curveRenderer::AddValue( float time, float value )
{
	this->scale( 1.0f, -1.0f );
	//this->centerOn( ( end + start ) * 0.5, oldCenterScenePoint.y() );
}

//-----------------------------------------------------------------------
void curveRenderer::SetRange( int seconds )
{

}