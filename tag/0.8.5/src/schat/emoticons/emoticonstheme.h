/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
 *
 * Base class EmoticonsTheme
 * Copyright © 2008      by Carlo Segato          <brandon.ml@gmail.com>
 * Copyright © 2002-2003 by Stefan Gehn           <metz@gehn.net>
 * Kopete    © 2002-2008 by the Kopete developers <kopete-devel@kde.org>
 * Copyright © 2005      by Engin AYDOGAN         <engin@bzzzt.biz>
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

#ifndef EMOTICONSTHEME_H_
#define EMOTICONSTHEME_H_

#include "emoticonsprovider.h"

#include <QtCore/QSharedDataPointer>

/**
 * This class contains the emoticons theme
 */
class EmoticonsTheme
{
public:

  /**
   * The possible parse modes
   */
  enum ParseModeEnum {
    DefaultParse = 0x0,      /**< Use strict or relaxed according to the config  */
    StrictParse = 0x1,       /**< Strict parsing requires a space between each emoticon */
    RelaxedParse = 0x2,      /**< Parse mode where all possible emoticon matches are allowed */
    SkipHTML = 0x4           /**< Skip emoticons within HTML */
  };

  Q_DECLARE_FLAGS(ParseMode, ParseModeEnum)

  /**
   * TokenType, a token might be an image ( emoticon ) or text.
   */
  enum TokenType {
    Undefined, /**< Undefined, for completeness only */
    Image,     /**< Token contains a path to an image */
    Text       /**< Token contains text */
  };

  /**
   * A token consists of a QString text which is either a regular text
   * or a path to image depending on the type.
   * If type is Image the text refers to an image path.
   * If type is Text the text refers to a regular text.
   */
  struct Token {
    Token() : type(Undefined) {}
    /**
     * Create a Token of type @p t, and text @p m
     */
    Token(TokenType t, const QString &m) : type(t), text(m) {}
    /**
     * Create a Token of type @p t, text @p m, image path @p p and html code @p html
     */
    Token(TokenType t, const QString &m, const QString &p, const QString &html)
            : type(t), text(m), picPath(p), picHTMLCode(html) {}
    TokenType   type; /**< type */
    QString     text; /**< text */
    QString     picPath; /**< path to the image */
    QString     picHTMLCode; /**< \<img> html code */
  };

  EmoticonsTheme();
  EmoticonsTheme(const EmoticonsTheme &ket);
  EmoticonsTheme(EmoticonsProvider *p);
  ~EmoticonsTheme();

  bool addEmoticon(const QString &emo, const QString &text, EmoticonsProvider::AddEmoticonOption option = EmoticonsProvider::DoNotCopy);
  bool isNull() const;
  bool loadTheme(const QString &path);
  bool removeEmoticon(const QString &emo);
  EmoticonsTheme& operator=(const EmoticonsTheme &ket);
  QList<Token> tokenize(const QString &message, ParseMode mode = DefaultParse) const;
  QMap<QString, QStringList> emoticonsMap() const;
  QString fileName() const;
  QString parseEmoticons(const QString &text, ParseMode mode = DefaultParse, const QStringList &exclude = QStringList()) const;
  QString themeName() const;
  QString themePath() const;
  void createNew();
  void save();
  void setMaxParse(int max);
  void setThemeName(const QString &name);

private:
  class EmoticonsThemeData;
  QSharedDataPointer<EmoticonsThemeData> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(EmoticonsTheme::ParseMode)

#endif /*EMOTICONSTHEME_H_*/
