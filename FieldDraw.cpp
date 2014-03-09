#include "FieldDraw.h"
#include <QMouseEvent>
#include <QPainter>

FieldDraw::FieldDraw(QWidget* parent) : QWidget(parent)
{
	push_mouse = false;

	QPalette pall;
	pall.setColor(this->backgroundRole(),Qt::white);
	setPalette(pall);
}

void FieldDraw::paintEvent(QPaintEvent*)
{
	QPainter paint(this);
	paint.setPen(Qt::blue);
	paint.drawLines(points);
	paint.setPen(Qt::red);
	paint.drawLines(net_points);
}

void FieldDraw::mousePressEvent(QMouseEvent* pe)
{
	push_mouse = true;
	points << QPoint(pe->x(), pe->y());
}

void FieldDraw::mouseReleaseEvent(QMouseEvent*)
{
	push_mouse = false;
	points.remove(points.size()-1);
}

void FieldDraw::mouseMoveEvent(QMouseEvent* pe)
{
	QPoint p(pe->x(), pe->y());
	if( push_mouse ) 
	{
		points << p;
		emit sendPoints(p);
	}
	points << p;
	emit sendPoints(p);
	update();
}

void FieldDraw::slotAddPoint(QPoint& p)
{
	if (!net_points.empty()) net_points << p;
	net_points << p;
	update();
}
