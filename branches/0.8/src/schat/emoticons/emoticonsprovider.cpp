/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
 *
 * Base class EmoticonsProvider
 * Copyright © 2008 by Carlo Segato <brandon.ml@gmail.com>
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

#include <QFileInfo>
#include <QTextDocument>
#include <QDir>
#include <QPixmap>
#include <QUrl>

#include "emoticonsprovider.h"
#include "emoticons.h"

class EmoticonsProviderPrivate
{
public:
    EmoticonsProviderPrivate();
    QString m_themeName;
    QString m_fileName;
    QString m_themePath;
    QMap<QString, QStringList> m_emoticonsMap;
    QHash<QChar, QList<EmoticonsProvider::Emoticon> > m_emoticonsIndex;
};

EmoticonsProviderPrivate::EmoticonsProviderPrivate()
{
}


/*!
 * Default constructor.
 */
EmoticonsProvider::EmoticonsProvider(QObject *parent)
  : QObject(parent), d(new EmoticonsProviderPrivate)
{
}


/*!
 * Destructor.
 */
EmoticonsProvider::~EmoticonsProvider()
{
  delete d;
}


/*!
 * Returns a QHash that contains emoticons indexed by the first char.
 */
QHash<QChar, QList<EmoticonsProvider::Emoticon> > EmoticonsProvider::emoticonsIndex() const
{
  return d->m_emoticonsIndex;
}


/*!
 * Returns a QHash that contains the emoticons path as keys and the text as values.
 */
QMap<QString, QStringList> EmoticonsProvider::emoticonsMap() const
{
  return d->m_emoticonsMap;
}


/*!
 * Returns the file name of the theme.
 */
QString EmoticonsProvider::fileName() const
{
  return d->m_fileName;
}


/*!
 * Returns the theme name.
 */
QString EmoticonsProvider::themeName() const
{
  return d->m_themeName;
}


/*!
 * Returns the theme path.
 */
QString EmoticonsProvider::themePath() const
{
  return d->m_themePath;
}


/*!
 * Add the emoticon \p emo with text \p text.
 *
 * \param emo    path to the emoticon image.
 * \param text   the text of the emoticon separated by space for multiple text.
 * \param option whether or not copy \p emo into the theme directory.
 * \return \c true if it can add the emoticon.
 */
bool EmoticonsProvider::addEmoticon(const QString &emo, const QString &text, AddEmoticonOption option)
{
//    if (option == Copy) {
//        KIO::NetAccess::dircopy(KUrl(emo), KUrl(d->m_themePath));
//    }

  Q_UNUSED(emo);
  Q_UNUSED(text);
  Q_UNUSED(option)
  return false;
}


/*!
 * Load the theme inside the directory \p path.
 *
 * \param path path to the directory.
 */
bool EmoticonsProvider::loadTheme(const QString &path)
{
  QFileInfo info(path);
  d->m_fileName = info.fileName();
  d->m_themeName = info.dir().dirName();
  d->m_themePath = info.absolutePath();
  return true;
}


/*!
 * Remove the emoticon \p emo, this will not delete the image file too.
 *
 * \param emo the emoticon text to remove.
 * \return \c true if it can delete the emoticon.
 */
bool EmoticonsProvider::removeEmoticon(const QString &emo)
{
  Q_UNUSED(emo);
  return false;
}


/*!
 * Create a new theme.
 */
void EmoticonsProvider::createNew()
{
}


/*!
 * Save the emoticon theme.
 */
void EmoticonsProvider::save()
{
}


/*!
 * Set the theme name.
 *
 * \param name name of the theme.
 */
void EmoticonsProvider::setThemeName(const QString &name)
{
  d->m_themeName = name;
}


/*!
 * Add an emoticon to the index.
 *
 * \param path    path to the emoticon.
 * \param emoList list of text associated with this emoticon.
 */
void EmoticonsProvider::addEmoticonIndex(const QString &path, const QStringList &emoList)
{
  foreach(const QString &s, emoList) {
    EmoticonsProvider::Emoticon e;
    QPixmap p;

    QString escaped = Qt::escape(s);
    e.picPath = path;
    p.load(path);

    e.picHTMLCode = QString("<a href='smile:%3'><img class=\"smile\" title=\"%1\" alt=\"%1\" src=\"%2\" width=\"%4\" height=\"%5\"  /></a>")
                            .arg(escaped)
                            .arg(QUrl::fromLocalFile(path).toString())
                            .arg(QString(s.toUtf8().toHex()))
                            .arg(p.width()).arg(p.height());

    e.matchTextEscaped = escaped;
    e.matchText = s;

    if (!s.isEmpty() && !escaped.isEmpty()) {
      d->m_emoticonsIndex[escaped[0]].append(e);
      d->m_emoticonsIndex[s[0]].append(e);
    }
  }
}


/*!
 * Insert a new item in the emoticons map.
 */
void EmoticonsProvider::addEmoticonsMap(const QString &key, QStringList value)
{
  if (!value.isEmpty())
    d->m_emoticonsMap.insert(QFileInfo(key).fileName(), value);
}


/*!
 * Clears the emoticons map.
 */
void EmoticonsProvider::clearEmoticonsMap()
{
  d->m_emoticonsMap.clear();
}


/*!
 * Remove an emoticon from the index.
 * \param path path to the emoticon.
 * \param emoList list of text associated with this emoticon.
 */
void EmoticonsProvider::removeEmoticonIndex(const QString &path, const QStringList &emoList)
{
  foreach(const QString &s, emoList) {
    QString escaped = Qt::escape(s);

    if (s.isEmpty() || escaped.isEmpty()) {
      continue;
    }

    QList<Emoticon> ls = d->m_emoticonsIndex.value(escaped[0]);

    for (int i = 0; i < ls.size(); ++i) {
      if (ls.at(i).picPath == path) {
        ls.removeAt(i);
      }
    }

    ls = d->m_emoticonsIndex.value(s[0]);

    for (int i = 0; i < ls.size(); ++i) {
      if (ls.at(i).picPath == path) {
        ls.removeAt(i);
      }
    }
  }
}


/*!
 * Remove an item from the emoticons map.
 */
void EmoticonsProvider::removeEmoticonsMap(QString key)
{
  d->m_emoticonsMap.remove(key);
}
