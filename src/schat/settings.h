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
    NetworkSettingsChanged,
    NetworksModelIndexChanged,
    ProfileSettingsChanged,
    ServerChanged,
    UpdateSettingsChanged
  };
  
  Settings(Profile *p, QObject *parent);
  inline void notify(int notify) { emit changed(notify); }
  void notify(int notify, int index);
  void read();
  void write();
  
  bool firstRun;
  bool hideWelcome;
  bool needCreateNetworkList;
  bool updateAutoClean;
  bool updateAutoDownload;
  int updateCheckInterval;
  Network network;
  QStandardItemModel networksModel;
  QString style;
  QString updateUrl;

signals:
  void changed(int notify);
  void networksModelIndexChanged(int index);

private:
  void createServerList(QSettings &s);
  void saveRecentServers(QSettings &s);
  
  Profile *profile;
  SChatWindow *chat;
};

#endif /*SETTINGS_H_*/
