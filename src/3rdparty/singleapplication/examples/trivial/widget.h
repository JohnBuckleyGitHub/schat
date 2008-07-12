#ifndef WIDGET_H
#define WIDGET_H

#include <QtCore/QString>

#include <QtGui/QWidget>

class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget* parent = 0);
	~Widget();

public slots:
	void handleMessage(const QString& message);
};

#endif // WIDGET_H
