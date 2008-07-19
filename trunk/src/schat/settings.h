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
class AbstractProfile;

class Settings : public QObject {
  Q_OBJECT
  
public:
  enum {
    NetworkSettingsChanged,
    NetworksModelIndexChanged,
    ProfileSettingsChanged,
    ServerChanged,
    UpdateSettingsChanged,
    ByeMsgChanged
  };
  
  Settings(AbstractProfile *p, QObject *parent);
  inline void notify(int notify) { emit changed(notify); }
  void notify(int notify, int index);
  void read();
  void write();
  
  bool firstRun;
  bool hideWelcome;
  bool needCreateNetworkList;
  Network network;
  QStandardItemModel networksModel;
  QString style;
  
  #ifdef SCHAT_UPDATE
  bool updateAutoClean;
  bool updateAutoDownload;
  int updateCheckInterval;  
  QString updateUrl;
  #endif

signals:
  void changed(int notify);
  void networksModelIndexChanged(int index);

private:
  void createServerList(QSettings &s);
  void saveRecentServers(QSettings &s);
  
  AbstractProfile *m_profile;
  SChatWindow *chat;
};

#endif /*SETTINGS_H_*/
