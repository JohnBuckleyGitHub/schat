/*******************************************************************************
SingleApplication provides a way to detect a running instance.
Copyright (C) 2008  Ritt K.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
********************************************************************************/

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
