/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#ifndef EMOTICONSPROVIDER_H_
#define EMOTICONSPROVIDER_H_

#include <QObject>
#include <QPair>
#include <QStringList>
#include <QVariant>

class EmoticonsProviderPrivate;
class QString;
struct Emoticon;

/*!
 * This is the base class for the emoticons provider plugins.
 */
class EmoticonsProvider : public QObject
{
  Q_OBJECT

public:
  struct Emoticon
  {
    Emoticon() {}
    /* sort by longest to shortest matchText */
    bool operator < (const Emoticon &e) const { return matchText.length() > e.matchText.length(); }
    QString matchText;
    QString matchTextEscaped;
    QString picPath;
    QString picHTMLCode;
  };

  /*!
   * Options to pass to addEmoticon
   */
  enum AddEmoticonOption {
    DoNotCopy, /**<< Don't copy the emoticon file into the theme directory */
    Copy /**<< Copy the emoticon file into the theme directory */
  };

  explicit EmoticonsProvider(QObject *parent = 0);
  virtual ~EmoticonsProvider();
  QHash<QChar, QList<Emoticon> > emoticonsIndex() const;
  QMap<QString, QStringList> emoticonsMap() const;
  QString fileName() const;
  QString themeName() const;
  QString themePath() const;
  virtual bool addEmoticon(const QString &emo, const QString &text, AddEmoticonOption option = DoNotCopy);
  virtual bool loadTheme(const QString &path);
  virtual bool removeEmoticon(const QString &emo);
  virtual void createNew();
  virtual void save();
  void setThemeName(const QString &name);

protected:
  EmoticonsProviderPrivate * const d;
  void addEmoticonIndex(const QString &path, const QStringList &emoList);
  void addEmoticonsMap(const QString &key, QStringList value);
  void clearEmoticonsMap();
  void removeEmoticonIndex(const QString &path, const QStringList &emoList);
  void removeEmoticonsMap(QString key);
};

#endif /*EMOTICONSPROVIDER_H_*/
