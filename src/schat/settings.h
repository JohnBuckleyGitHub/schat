/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <QObject>
#include <QSettings>
#include <QStandardItemModel>
#include <QTextCursor>

#include "abstractsettings.h"
#include "network.h"

class SChatWindow;
class AbstractProfile;

class Settings : public AbstractSettings {
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

  Settings(const QString &filename, AbstractProfile *profile, QObject *parent);
  inline QByteArray splitter() const                  { return m_splitter; }
  inline QPoint pos() const                           { return m_pos; }
  inline QSize size() const                           { return m_size; }
  inline void notify(int notify)                      { emit changed(notify); }
  inline void setPos(const QPoint &pos)               { m_pos = pos; }
  inline void setSize(const QSize &size)              { m_size = size; }
  inline void setSplitter(const QByteArray &splitter) { m_splitter = splitter; }
  QString smileFile(const QString &smile);
  QStringList smiles(const QString &text) const;
  void createEmoticonsMap();
  void notify(int notify, int index);
  void read();
  void write();

  bool needCreateNetworkList;
  Network network;
  QStandardItemModel networksModel;

signals:
  void changed(int notify);
  void networksModelIndexChanged(int index);

private:
  inline void writeBool(const QString &key)           { m_settings->setValue(key, m_bool.value(key)); }
  inline void writeInt(const QString &key)            { m_settings->setValue(key, m_int.value(key)); }
  inline void writeString(const QString &key)         { m_settings->setValue(key, m_string.value(key)); }
  void createServerList();
  void saveRecentServers();

  AbstractProfile *m_profile;
  QByteArray m_splitter;
  QMap<QString, int> m_emoticons;
  QPoint m_pos;
  QSize m_size;
  QString m_emoticonsPath;
  QStringList m_emoticonsFiles;
};

#endif /*SETTINGS_H_*/
