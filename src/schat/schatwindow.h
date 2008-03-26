/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef SCHATWINDOW_H_
#define SCHATWINDOW_H_

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTcpSocket>

class QAction;
class QHBoxLayout;
class QLineEdit;
class QListView;
class QSplitter;
class QStatusBar;
class QTabWidget;
class QVBoxLayout;
class Tab;

#include "clientsocket.h"

class SChatWindow : public QMainWindow
{
  Q_OBJECT

public:
  SChatWindow(QWidget *parent = 0);
  
public slots:
  void newMessage(const QString &nick, const QString &message);
  void newParticipant(const QString &p, bool echo = true);
  void participantLeft(const QString &nick);
  
private slots:
  void addTab();
  void addTab(const QModelIndex &index);
  void closeTab();
  void connectionError(QAbstractSocket::SocketError socketError);
  void disconnected();
  void newConnection();
  void returnPressed();

private:
  int tabIndex(const QString &s, int start = 1);
  QString currentTime();
  void createActions();
  void removeConnection(ClientSocket *socket);
  
  Tab *mainChannel;
  ClientSocket *clientSocket;
  QAction *addTabAction;
  QAction *closeTabAction;
  QLineEdit *lineEdit;  
  QListView *listView;
  QSplitter *splitter;
  QStandardItemModel model;
  QStatusBar *statusbar;
  QString nick;
  QTabWidget *tabWidget;
  QVBoxLayout *mainLayout;
  QVBoxLayout *rightLayout;
  QWidget *centralWidget;
  QWidget *rightWidget;
};

#endif /*SCHATWINDOW_H_*/
