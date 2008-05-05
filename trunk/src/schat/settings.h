/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <QObject>
#include <QStandardItemModel>

#include "network.h"

class SChatWindow;
class Profile;

class Settings : public QObject {
  Q_OBJECT
  
public:
  Settings(Profile *p, QObject *parent);
  void read();
  void write();
  
  bool firstRun;
  bool hideWelcome;
  bool needInitNetworkList;  
  Network network;
  QStandardItemModel networksModel;
  QString style;
  
private:
  Profile *profile;
  SChatWindow *chat;
};

#endif /*SETTINGS_H_*/
