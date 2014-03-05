#include "FieldDraw.h"
#include <QMouseEvent>
#include <QPainter>

FieldDraw::FieldDraw(QWidget* parent) : QWidget(parent)
{
	push_mouse = false;

	QPalette pall;
	pall.setColor(this->backgroundRole(),Qt::white);
	setPalette(pall);
	//setAutoFillBackground(true);
}

void FieldDraw::paintEvent(QPaintEvent*)
{
	QPainter paint(this);
	//paint.drawText(10,10,"Hello");
	paint.setPen(Qt::blue);
	paint.drawLines(points);
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
	if( push_mouse ) points << p;//QPoint(pe->x(), pe->y());
	points << p;//QPoint(pe->x(), pe->y());
	emit sendPoints(p);
	update();
}

void FieldDraw::slotAddPoint(QPoint)
{
}
