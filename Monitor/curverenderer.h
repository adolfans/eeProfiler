#ifndef CURVERENDERER_H
#define CURVERENDERER_H

#include <QGraphicsView>

/*
===========================================
class curveRenderer
===========================================
*/
class curveRenderer 
	: public QGraphicsView
{
	Q_OBJECT

public:
	curveRenderer(QWidget *parent = 0);
	~curveRenderer();

	void AddValue( float time, float value );
	void SetRange( int seconds );
	int range;
private:
};

#endif // CURVERENDERER_H
