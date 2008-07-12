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
