#include "curverenderer.h"
#include <QQueue>
#include <QGraphicsPolygonItem>
#include <QPainter>

#define PIXELS_PER_SECOND	15		//To calculate x position
#define PXIELS_PER_TEN_MILLIONSECONDS 15	//To calcualte y position

CurveRenderer::CurveRenderer(QWidget *parent)
	: QWidget(parent),
	range( 30 )
{
	this->setMinimumHeight( 200 );
}

CurveRenderer::~CurveRenderer()
{

}

//-----------------------------------------------------------------------
void CurveRenderer::AddValue( float time, float value )
{
	//convert from millisecond to second
	time = time * 0.001;
	value = value * 0.001;

	static int _testIIII = 0;

	wireGridTimeOffset = fmod( time, 1.0f );

	value = PXIELS_PER_TEN_MILLIONSECONDS * value * 0.01;

	if( values.size() )
	{
		float moveLeftOffsetOfTime = time - lastTime;
		for( int i = 0; i != values.size(); ++ i )
		{
			values[i].setX( values[i].x() - moveLeftOffsetOfTime * PIXELS_PER_SECOND );
		}
		float _range = - values[0].x();
		if( _range > range * PIXELS_PER_SECOND )
		{
			values.remove( 0 );
		}
		values.push_back( QPointF( 0.0f, value * PIXELS_PER_SECOND * 500 ) );
		_testIIII += 1 ;

		if( value > yValueMax )	yValueMax = value;
		if( value < yValueMin ) yValueMin = value;

		lastTime = time;
	}else
	{
		yValueMin = yValueMax = value;
		values.push_back( QPointF( 0.0f, value * PIXELS_PER_SECOND * 500 ) );
		lastTime = time;
		_testIIII += 1 ;
	}
	update();
}

//-----------------------------------------------------------------------
void CurveRenderer::SetRange( int seconds )
{
	//this->scale( 1.0f, -1.0f );
	//xCenter = ( 0 + seconds ) * 0.5; 
	//this->centerOn( xCenter, oldCenterScenePoint.y() );
}

//------------------------------------------------------------------------
void CurveRenderer::paintEvent( QPaintEvent *event )
{
	QRect rect = this->rect();

	QPainter painter;
	painter.begin(this);
	//painter.setRenderHint(QPainter::Antialiasing);
	painter.translate( rect.width(), 0 );
	
	if( values.size() )
	{
		for( int i = 0; i != values.size(); ++ i )
		{
			painter.drawPolyline( values );
		}
	}

	//draw wiregrid
	float offset = wireGridTimeOffset * PIXELS_PER_SECOND;
	for( int xPos = 0; xPos > -rect.width(); xPos -= PIXELS_PER_SECOND )
	{
		float xPosToDraw = xPos - offset;
		QPointF startPos( xPosToDraw, 0 );
		QPointF endPos( xPosToDraw, rect.height() );
		painter.drawLine( startPos, endPos );
	}
	
	painter.end();
}