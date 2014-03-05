#ifndef __FIELD_DRAW__
#define __FIELD_DRAW__

#include <QWidget>
#include <QPolygon>

class FieldDraw : public QWidget
{
	Q_OBJECT

	signals:
		void sendPoints(QPoint&);
	public slots:
		void slotAddPoint(QPoint);
	public:
		FieldDraw(QWidget* parent = 0);
		void paintEvent(QPaintEvent*);
	protected:
		void mousePressEvent(QMouseEvent* pe);
		void mouseReleaseEvent(QMouseEvent* pe);
		void mouseMoveEvent(QMouseEvent* pe);
	private:
		QVector<QPoint> points;
		bool push_mouse;
};

#endif // __FIELD_DRAW__
