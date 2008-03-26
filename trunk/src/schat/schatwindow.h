/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef SCHATWINDOW_H_
#define SCHATWINDOW_H_

#include <QMainWindow>
#include <QTcpSocket>
#include <QStandardItemModel>

class QAction;

#include "clientsocket.h"
#include "ui_schatwindow.h"

class SChatWindow : public QMainWindow, public Ui::SChatWindow
{
  Q_OBJECT

public:
  SChatWindow(QWidget *parent = 0);
  
public slots:
  void newParticipant(const QString &p, bool echo = true);
  void participantLeft(const QString &nick);
  void newMessage(const QString &nick, const QString &message);
  
private slots:
  void returnPressed();
  void newConnection();
  void connectionError(QAbstractSocket::SocketError socketError);
  void disconnected();
  void addTab();
  void addTab(const QModelIndex &index);
  void closeTab();

private:
  void removeConnection(ClientSocket *socket);
  void createActions();
  void scroll();
  int tabIndex(const QString &s, int start = 1);
  
  QString nick;
  ClientSocket *clientSocket;
  QString currentTime();
  QStandardItemModel model;
  QAction *addTabAction;
  QAction *closeTabAction;
};

#endif /*SCHATWINDOW_H_*/
