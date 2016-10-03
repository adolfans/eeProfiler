#include "curverenderer.h"
#include <QQueue>
#include <QGraphicsPolygonItem>
#include <QPainter>

#define PIXELS_PER_SECOND	15		//To calculate x position
#define PXIELS_PER_TEN_MILLIONSECONDS 15	//To calcualte y position

curveRenderer::curveRenderer(QWidget *parent)
	: QWidget(parent),
	range( 30 )
{
}

curveRenderer::~curveRenderer()
{

}

//-----------------------------------------------------------------------
void curveRenderer::AddValue( float time, float value )
{
	wireGridTimeOffset = fmod( time, 1.0f );

	value = PXIELS_PER_TEN_MILLIONSECONDS * value;

	if( values.size() )
	{
		float moveLeftOffsetOfTime = time - values[ values.size() - 1 ].x();
		float pixelsToMoveLeft = PIXELS_PER_SECOND * moveLeftOffsetOfTime;

		float _range = time - values[0].x();
		if( _range > range )
		{
			values.remove( 0 );
		}

		for( int i = 0; i != values.size(); ++ i )
		{
			values[i].setX( values[i].x() - pixelsToMoveLeft );
		}
		values.push_back( QPointF( 0.0f,value ) );

		if( value > yValueMax )	yValueMax = time;
		if( value < yValueMin ) yValueMin = time;

	}else
	{
		yValueMin = yValueMax = value;
		values.push_back( QPointF( 0.0f, value ) );
	}
}

//-----------------------------------------------------------------------
void curveRenderer::SetRange( int seconds )
{
	//this->scale( 1.0f, -1.0f );
	//xCenter = ( 0 + seconds ) * 0.5; 
	//this->centerOn( xCenter, oldCenterScenePoint.y() );
}

//------------------------------------------------------------------------
void curveRenderer::paintEvent( QPaintEvent *event )
{
	QRect rect = this->rect();

	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);
	//helper->paint(&painter, event, elapsed);
	
	if( values.size() )
	{
		painter.translate( -rect.width(), yCenter );
	}

	//draw wiregrid
	float offset = wireGridTimeOffset * PIXELS_PER_SECOND;
	for( int xPos = rect.x(); xPos > 0; xPos -= PIXELS_PER_SECOND )
	{
		float xPosToDraw = xPos - offset;
		QPointF startPos( xPosToDraw, rect.height() );
		QPointF endPos( xPosToDraw, -rect.height() );
		painter.drawLine( startPos, endPos );
	}
	
	painter.end();
}