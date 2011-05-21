/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <QHash>
#include <QObject>
#include <QPointer>
#include <QSettings>
#include <QStandardItemModel>
#include <QTextCursor>

#include "abstractsettings.h"
#include "emoticons/emoticons.h"
#include "network.h"
#include "update/update.h"

class AbstractProfile;

#define SimpleSettings (static_cast<Settings *>(AbstractSettings::instance()))

namespace schat {
  #ifndef Q_OS_WINCE
  static const QStringList DefaultToolBarLayout = QStringList() << "bold" << "italic" << "underline"
      << "separator" << "emoticons" << "stretch" << "log" << "separator" << "send";
  #else
  static const QStringList DefaultToolBarLayout = QStringList() << "bold" << "italic" << "underline"
      << "separator" << "settings" << "emoticons" << "stretch" << "sound" << "log" << "separator" << "send";
  #endif
}

/*!
 * \brief Класс читает и записывает настройки клиента.
 */
class Settings : public AbstractSettings {
  Q_OBJECT

public:
  /// Уведомления об изменении настроек.
  enum {
    NetworkSettingsChanged,
    NetworksModelIndexChanged,
    ProfileSettingsChanged,
    AwaySettingsChanged,
    ServerChanged,
    UpdateSettingsChanged,
    EmoticonsChanged,
    SoundChanged,
    NotificationChanged,
    MiscSettingsChanged,
    ByeMsgChanged,
    UpdateError,
    UpdateAvailable,
    UpdateNoAvailable,
    UpdateAvailableForce,

    #ifndef SCHAT_NO_UPDATE
      UpdateReady,
      UpdateGetting,
    #endif
    #ifndef SCHAT_NO_WEBKIT
      InterfaceSettingsChanged,
    #endif

    HideWelcomeChanged, ///< Состояние чекбокса "Всегда использовать это подключение".
  };

  /// Пути.
  enum Paths {
    EmoticonsPath,   ///< Пути к смайликам.
    NetworksPath,    ///< Путь к файлам сети.
    SoundsPath,      ///< Пути к звукам.
    StylesPath,      ///< Пути к стилям текста.
    LogPath,         ///< Путь к журналу.
    TranslationsPath ///< Путь к переводам.
  };

  Settings(bool unixLike, QObject *parent = 0);
  const QString& richTextCSS();
  inline AbstractProfile* profile()                    { return m_profile; }
  inline bool unixLike() const { return m_unixLike; }
  inline Emoticons* emoticons() const                  { return m_emoticons; }
  inline QPoint pos() const                            { return m_pos; }
  inline QSize size() const                            { return m_size; }
  inline Update::State updateState() const             { if (m_update) return m_update->state(); else return Update::Unknown; }
  inline void setPos(const QPoint &pos)                { m_pos = pos; }
  inline void setSize(const QSize &size)               { m_size = size; }
  inline void setUnixLike(bool unixLike) { m_unixLike = unixLike; }
  int save(const QString &key, bool value);
  int save(const QString &key, const QString &value);
  int save(const QString &key, int value);
  QStringList path(Paths type) const;
  static QStandardItem* findItem(const QStandardItemModel *model, const QString &text, Qt::MatchFlags flags = Qt::MatchExactly, int column = 0);
  void notify(int notify);
  void notify(int notify, int index);
  void read();
  void setApplicationProxy() const;
  void write();

  #ifndef SCHAT_NO_UPDATE
    static bool install();
  #endif

  Network network;
  QStandardItemModel networksModel;

signals:
  void changed(int notify);
  void networksModelIndexChanged(int index);

public slots:
  inline bool updatesCheck() { return update(); }
  #ifndef SCHAT_NO_UPDATE
    inline bool updatesGet() { return update(true); }
  #endif

private:
  bool update(bool get = false);
  QStringList path(const QString &base) const;
  void createServerList();
  void normalizeInterval();
  void saveRecentServers();

  AbstractProfile *m_profile;
  bool m_initRichTextCSS;
  bool m_unixLike;
  QPoint m_pos;
  QPointer<Emoticons> m_emoticons;
  QPointer<Update> m_update;
  QSettings *m_default;
  QSize m_size;
  QString m_richTextCSS;
  QTimer *m_updateTimer;
};

#endif /*SETTINGS_H_*/
