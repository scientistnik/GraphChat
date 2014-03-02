#include "FieldDraw.h"
#include <QMouseEvent>
#include <QPainter>

FieldDraw::FieldDraw(QWidget* parent) : QWidget(parent)
{
	push_mouse = false;
}

void FieldDraw::paintEvent(QPaintEvent*)
{
	QPainter paint(this);
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
	if( push_mouse ) points << QPoint(pe->x(), pe->y());
	points << QPoint(pe->x(), pe->y());
	update();
}
