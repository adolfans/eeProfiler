#include "curverenderer.h"
#include <QQueue>
#include <QGraphicsPolygonItem>
#include <QPainter>

#define PIXELS_PER_SECOND	15		//To calculate x position
#define PXIELS_PER_TEN_MILLIONSECONDS 15	//To calcualte y position
#define MINIMUM_HEIGHT				150

CurveRenderer::CurveRenderer(QWidget *parent)
	: QWidget(parent),
	range( 30 )
{
	this->setMinimumHeight( 150 );
	lastFixRangeTime = 0.0f;
	rangeChanged = true;
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

	wireGridTimeOffset = fmod( time, 1.0f );

	value = PXIELS_PER_TEN_MILLIONSECONDS * value * 0.01;

	if( values.size() )
	{
		float moveLeftOffsetOfTime = time - lastTime;
		for( int i = 0; i != values.size(); ++ i )
		{
			values[i].setX( values[i].x() - moveLeftOffsetOfTime * PIXELS_PER_SECOND  );
		}
		float _range = - values[0].x();
		if( _range > range * PIXELS_PER_SECOND )
		{
			values.remove( 0 );
		}
		values.push_back( QPointF( 0.0f, value  ) );
		if( value > yValueMax )
		{
			yValueMax = value;
			rangeChanged = true;
		}
		if( value < yValueMin )
		{
			yValueMin = value;
			rangeChanged = true;
		}
		lastTime = time;
		if( rangeChanged && values.size() < range  )
		{
			float _yValueRangeFix = ( yValueMax - yValueMin ) * .3;
			yMinForRenderRange = yValueMin - _yValueRangeFix;
			yMaxForRenderRange = yValueMax + _yValueRangeFix;
			lastFixRangeTime = time;
			rangeChanged = false;
		}else if( rangeChanged && time - lastFixRangeTime > (float)range )
		{
			float _yValueRangeFix = ( yValueMax - yValueMin ) * .2;
			yMinForRenderRange = yValueMin - _yValueRangeFix;
			yMaxForRenderRange = yValueMax + _yValueRangeFix;
			lastFixRangeTime = time;
			rangeChanged = false;
		}
	}else
	{
		yValueMin = yValueMax = value;
		values.push_back( QPointF( 0.0f, value ) );
		lastTime = time;
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

#include "qmath.h"

//------------------------------------------------------------------------
void CurveRenderer::paintEvent( QPaintEvent *event )
{
	QRect rect = this->rect();

	QPainter painter;
	painter.begin(this);
	painter.translate( rect.width(), 0 );
	
	if( values.size() )
	{
		float _scale = (float)rect.height() / ( yMaxForRenderRange - yMinForRenderRange );
		float _offset = yMinForRenderRange;

		QPolygonF pts4Draw;
		for( int i = 0; i != values.size(); ++ i )
		{
			pts4Draw.push_back( QPointF( values[i].rx(), rect.height() - ( ( values[i].ry() - _offset ) * _scale ) ) );
		}
		painter.setRenderHint(QPainter::Antialiasing);
		painter.drawPolyline( pts4Draw );
		painter.setRenderHint(QPainter::Antialiasing, false);
		painter.resetTransform();
		painter.translate( rect.width(), 0 );
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