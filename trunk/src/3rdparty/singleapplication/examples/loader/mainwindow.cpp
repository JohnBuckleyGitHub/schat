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

#include "mainwindow.h"

#include <QtCore/QFile>

#include <QtGui/QTextEdit>
#include <QtGui/QWorkspace>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	workspace = new QWorkspace(this);
	setCentralWidget(workspace);
}

MainWindow::~MainWindow()
{
}

void MainWindow::activate()
{
	setWindowState(windowState() & ~Qt::WindowMinimized);
	raise();
	activateWindow();
}

void MainWindow::handleMessage(const QString& message)
{
	QString filename(message);
	if(filename.isEmpty() || !QFile::exists(filename))
		return;

	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly))
		return;
	QString plainText = file.readAll();
	file.close();

	QTextEdit* textEdit = new QTextEdit(workspace);
	workspace->addWindow(textEdit);
	textEdit->setWindowTitle(filename);
	textEdit->setPlainText(plainText);
	textEdit->show();

	activate();
}

#include "moc_mainwindow.cpp"
