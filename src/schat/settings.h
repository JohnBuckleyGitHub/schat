/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <QObject>
#include <QSettings>
#include <QStandardItemModel>

#include "network.h"

class SChatWindow;
class Profile;

class Settings : public QObject {
  Q_OBJECT
  
public:
  enum {
    NetworksModelIndexChanged,
    ServerChanged
  };
  
  Settings(Profile *p, QObject *parent);
  void notify(int notify);
  void notify(int notify, int index);
  void read();
  void write();
  
  bool firstRun;
  bool hideWelcome;
  bool needCreateNetworkList;
  Network network;
  QStandardItemModel networksModel;
  QString style;

signals:
  void networksModelIndexChanged(int index);
  void serverChanged();
 
private:
  void createServerList(QSettings &s);
  void saveRecentServers(QSettings &s);
  
  Profile *profile;
  SChatWindow *chat;
};

#endif /*SETTINGS_H_*/
