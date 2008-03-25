#ifndef SCHATWINDOW_H_
#define SCHATWINDOW_H_

#include <QMainWindow>
#include <QTcpSocket>
#include <QStandardItemModel>

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

private:
  ClientSocket *clientSocket;
  void scroll();
  QString currentTime();
  QString defaultNick();
  QStandardItemModel model;

};

#endif /*SCHATWINDOW_H_*/
