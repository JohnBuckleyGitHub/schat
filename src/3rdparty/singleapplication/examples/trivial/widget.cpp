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

#include "widget.h"

#include <QtCore/QCoreApplication>

#include <QtGui/QLabel>
#include <QtGui/QMessageBox>
#include <QtGui/QVBoxLayout>

Widget::Widget(QWidget* parent) : QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(new QLabel(tr("Became to a Single Mode. start app once again...")));
	layout->addWidget(new QLabel(tr("PID: %1").arg(QCoreApplication::applicationPid())));
}

Widget::~Widget()
{
}

void Widget::handleMessage(const QString& message)
{
	QMessageBox::information(0, "", message, QMessageBox::Ok, QMessageBox::Ok);
}

#include "moc_widget.cpp"
