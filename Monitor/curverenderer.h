#ifndef CURVERENDERER_H
#define CURVERENDERER_H

#include <QWidget>

/*
===========================================
class curveRenderer
===========================================
*/
class curveRenderer 
	: public QWidget
{
	Q_OBJECT

public:
	curveRenderer(QWidget *parent = 0);
	~curveRenderer();

	void paintEvent( QPaintEvent *event );

	void AddValue( float time, float value );
	void SetRange( int seconds );
	int range;

	float wireGridTimeOffset;

	float yCenter;

	float yValueMax;
	float yValueMin;

	QVector<QPointF> values;
private:
};

#endif // CURVERENDERER_H
