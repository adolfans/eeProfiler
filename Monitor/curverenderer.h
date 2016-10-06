#ifndef CURVERENDERER_H
#define CURVERENDERER_H

#include <QWidget>

/*
===========================================
class curveRenderer
===========================================
*/
class CurveRenderer 
	: public QWidget
{
	Q_OBJECT

public:
	CurveRenderer(QWidget *parent = 0);
	~CurveRenderer();

	void paintEvent( QPaintEvent *event );

	void AddValue( float time, float value );
	void SetRange( int seconds );
	int range;

	float wireGridTimeOffset;

	float yCenter;

	float yValueMax;
	float yValueMin;

	QPolygonF values;

	float lastTime;

	float lastFixRangeTime;
	bool rangeChanged;

	float yMinForRenderRange;
	float yMaxForRenderRange; 
private:
};

#endif // CURVERENDERER_H
