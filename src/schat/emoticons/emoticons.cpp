/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
 *
 * Base class Emoticons
 * Copyright © 2008 by Carlo Segato <brandon.ml@gmail.com>
 * Copyright © 2008 Montel Laurent <montel@kde.org>
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

#include <QDir>

#include "emoticons.h"
#include "emoticonsprovider.h"
#include "providers/adium/adium_emoticons.h"
#include "providers/kde/kde_emoticons.h"
#include "providers/pidgin/pidgin_emoticons.h"
#include "providers/xmpp/xmpp_emoticons.h"
#include "settings.h"

class EmoticonsPrivate
{
public:
  EmoticonsPrivate(Emoticons *parent);
  ~EmoticonsPrivate();
  QHash<QString, EmoticonsTheme> m_themes;
  Emoticons *q;
};


EmoticonsPrivate::EmoticonsPrivate(Emoticons *parent)
  : q(parent)
{
}


EmoticonsPrivate::~EmoticonsPrivate()
{
}


/*!
 * Default constructor.
 */
Emoticons::Emoticons(QObject *parent)
  : QObject(parent), d(new EmoticonsPrivate(this))
{
}


/*!
 * Destruct the object.
 */
Emoticons::~Emoticons()
{
  delete d;
}


/*!
 * Retrieve the current emoticons theme.
 *
 * \return the current EmoticonsTheme.
 */
EmoticonsTheme Emoticons::theme()
{
  return theme(currentThemeName());
}


/*!
 * Retrieve the theme with name \p name.
 *
 * \param name name of the theme.
 * \return the EmoticonsTheme \p name.
 */
EmoticonsTheme Emoticons::theme(const QString &name)
{
  if (d->m_themes.contains(name))
    return d->m_themes.value(name);

  QStringList emoticons = SimpleSettings->path(Settings::EmoticonsPath);
  QString path;
  EmoticonsProvider *provider = 0;

  for (int i = 0; i < emoticons.size(); ++i) {
    path = emoticons.at(i) + '/' + name + '/';

    if (QFile::exists(path + "icondef.xml")) {
      provider = new XmppEmoticons(this);
      path += "icondef.xml";
      break;
    }
    else if (QFile::exists(path + "emoticons.xml")) {
      provider = new KdeEmoticons(this);
      path += "emoticons.xml";
      break;
    }
    else if (QFile::exists(path + "theme")) {
      provider = new PidginEmoticons(this);
      path += "theme";
      break;
    }
    else if (QFile::exists(path + "Emoticons.plist")) {
      provider = new AdiumEmoticons(this);
      path += "Emoticons.plist";
      break;
    }
  }

  if (provider) {
    EmoticonsTheme theme(provider);
    theme.loadTheme(path);
    d->m_themes.insert(name, theme);
    return theme;
  }

  return EmoticonsTheme();
}


bool Emoticons::strictParse()
{
  return SimpleSettings->getBool("EmoticonsRequireSpaces");
}


/*!
 * Retrieve the current emoticon theme name.
 */
QString Emoticons::currentThemeName()
{
  return SimpleSettings->getString("EmoticonTheme");
}


/*!
* Returns a list of installed theme.
 *
 * \todo Добавить улучшенную проверку на корректность добавляемых тем.
 */
QStringList Emoticons::themeList()
{
  QStringList ls;
  QStringList themeDirs = SimpleSettings->path(Settings::EmoticonsPath);

  for (int i = 0; i < themeDirs.count(); ++i) {
    QDir themeQDir(themeDirs.at(i));
    themeQDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    themeQDir.setSorting(QDir::Name);
    QString dir = themeDirs.at(i) + "/";
    foreach (QString theme, themeQDir.entryList()) {
      if (QFile::exists(dir + theme + "/icondef.xml") || QFile::exists(dir + theme + "/emoticons.xml") || QFile::exists(dir + theme + "/theme") || QFile::exists(dir + theme + "/Emoticons.plist"))
        ls << theme;
    }
  }

  return ls;
}


/*!
 * Устанавливает режим парсинга.
 */
void Emoticons::setStrictParse(bool strict)
{
  SimpleSettings->setBool("EmoticonsRequireSpaces", strict);
}


/*!
 * Set \p theme as the current theme.
 *
 * \param theme a pointer to a EmoticonsTheme object.
 */
void Emoticons::setTheme(const EmoticonsTheme &theme)
{
  setTheme(theme.themeName());
}


/*!
 * Set \p theme as the current theme.
 *
 * \param theme the name of a theme.
 */
void Emoticons::setTheme(const QString &theme)
{
  SimpleSettings->setString("EmoticonTheme", theme);
}
