/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#include <QtGui>

#include "emoticonstheme.h"
#include "emoticons.h"

class EmoticonsTheme::EmoticonsThemeData : public QSharedData
{
public:
  EmoticonsThemeData();
  ~EmoticonsThemeData();
  EmoticonsProvider *provider;
  int maxParse;
};

EmoticonsTheme::EmoticonsThemeData::EmoticonsThemeData()
{
  provider = 0;
  maxParse = 4;
}

EmoticonsTheme::EmoticonsThemeData::~EmoticonsThemeData()
{
  delete provider;
}


/*!
 * Default constructor, it creates a null emoticons theme
 * you should probably never use this, instead use Emoticons::theme()
 */
EmoticonsTheme::EmoticonsTheme()
{
  d = new EmoticonsThemeData;
}


/*!
 * Copy constructor
 */
EmoticonsTheme::EmoticonsTheme(const EmoticonsTheme &ket)
{
  d = ket.d;
}


/*!
 * Another constructor where you set the EmoticonsProvider \p p
 * you should probably never use this, instead use Emoticons::theme()
 */
EmoticonsTheme::EmoticonsTheme(EmoticonsProvider *p)
{
  d = new EmoticonsThemeData;
  d->provider = p;
}


/*!
 * Destructor.
 */
EmoticonsTheme::~EmoticonsTheme()
{
}


/*!
 * Add the emoticon \p emo with text \p text.
 *
 * \code
 * EmoticonsTheme theme = Emoticons().theme();
 * theme.addEmoticon("/path/to/smiley.png", ":) :-)");
 * \endcode
 *
 * \param emo    path to the emoticon image.
 * \param text   the text of the emoticon separated by space for multiple text.
 * \param option whether or not copy \p emo into the theme directory.
 * \return \c true if it can add the emoticon.
 */
bool EmoticonsTheme::addEmoticon(const QString &emo, const QString &text, EmoticonsProvider::AddEmoticonOption option)
{
  if (!d->provider)
    return false;

  return d->provider->addEmoticon(emo, text, option);
}


/*!
 * Check if the theme has a valid provider and it returns true if it can't find it.
 */
bool EmoticonsTheme::isNull() const
{
  return d->provider ? false : true;
}


/*!
 * Load the theme inside the directory \p path.
 *
 * \param path path to the directory.
 */
bool EmoticonsTheme::loadTheme(const QString &path)
{
  if (!d->provider) {
    return false;
  }

  return d->provider->loadTheme(path);
}


/*!
 * Remove the emoticon \p emo, this will not delete the image file too.
 *
 * \code
 * EmoticonsTheme theme = Emoticons().theme();
 * theme.removeEmoticon(":)");
 * \endcode
 *
 * \param emo the emoticon text to remove.
 * \return \c true if it can delete the emoticon.
 */
bool EmoticonsTheme::removeEmoticon(const QString &emo)
{
  if (!d->provider) {
    return false;
  }

  return d->provider->removeEmoticon(emo);
}


/*!
 * \internal
 */
EmoticonsTheme& EmoticonsTheme::operator=(const EmoticonsTheme &ket)
{
  if (d == ket.d) {
    return *this;
  }

  d = ket.d;
  return *this;
}


/*!
 * Tokenize the message \p message with ParseMode \p mode.
 *
 * \code
 * EmoticonsTheme theme = Emoticons().theme();
 * QString text = "hi :)";
 * QList<Token> tokens = theme.tokenize(text, EmoticonsTheme::DefaultParse);
 * // tokens[0].text = "hi "
 * // tokens[1].text = ":)"
 * // tokens[1].picPath = "/path/to/:).png"
 * // tokens[1].picHTMLCode = "<img align="center" title=":)" alt=":)" src="/path/to/:).png" width="24" height="24" />"
 * \endcode
 */
QList<EmoticonsTheme::Token> EmoticonsTheme::tokenize(const QString &message, ParseMode mode) const
{
  if (!d->provider) {
    return QList<EmoticonsTheme::Token> ();
  }

  if (!(mode & (StrictParse | RelaxedParse))) {
    //if none of theses two mode are selected, use the mode from the config
    mode |= StrictParse;
  }

  QList<Token> result;

  /* previous char, in the firs iteration assume that it is space since we want
   * to let emoticons at the beginning, the very first previous QChar must be a space. */
  QChar p = ' ';
  QChar c; /* current char */
  QChar n;

  /* This is the EmoticonNode container, it will represent each matched emoticon */
  typedef QPair<EmoticonsProvider::Emoticon, int> EmoticonNode;
  QList<EmoticonNode> foundEmoticons;
  /* First-pass, store the matched emoticon locations in foundEmoticons */
  QList<EmoticonsProvider::Emoticon> emoticonList;
  QList<EmoticonsProvider::Emoticon>::const_iterator it;
  int pos;

  bool inHTMLTag = false;
  bool inHTMLLink = false;
  bool inHTMLEntity = false;
  QString needle; // search for this

  for (pos = 0; pos < message.length(); ++pos) {
    c = message[pos];

    if (mode & SkipHTML) { // Shall we skip HTML ?
      if (!inHTMLTag) { // Are we already in an HTML tag ?
        if (c == '<') { // If not check if are going into one
          inHTMLTag = true; // If we are, change the state to inHTML
          p = c;
          continue;
        }
      }
      else { // We are already in a HTML tag
        if (c == '>') { // Check if it ends
          inHTMLTag = false; // If so, change the state

          if (p == 'a') {
            inHTMLLink = false;
          }
        }
        else if (c == 'a' && p == '<') { // check if we just entered an achor tag
          inHTMLLink = true; // don't put smileys in urls
        }
        p = c;
        continue;
      }

      if (!inHTMLEntity) { // are we
        if (c == '&') {
          inHTMLEntity = true;
        }
      }
    }

    if (inHTMLLink) { // i can't think of any situation where a link address might need emoticons
      p = c;
      continue;
    }

    if ((mode & StrictParse) && !p.isSpace() && p != '>') { // '>' may mark the end of an html tag
      p = c;
      continue;
    } /* strict requires space before the emoticon */

    if (d->provider->emoticonsIndex().contains(c)) {
      emoticonList = d->provider->emoticonsIndex().value(c);
      bool found = false;
      for (it = emoticonList.constBegin(); it != emoticonList.constEnd(); ++it) {
        // If this is an HTML, then search for the HTML form of the emoticon.
        // For instance <o) => &gt;o)
        needle = (mode & SkipHTML) ? (*it).matchTextEscaped : (*it).matchText;
        if ((pos == message.indexOf(needle, pos))) {
          if (mode & StrictParse) {
            /* check if the character after this match is space or end of string*/
            if (message.length() > pos + needle.length()) {
              n = message[pos + needle.length()];
              //<br/> marks the end of a line
              if (n != '<' && !n.isSpace() && !n.isNull() && n != '&') {
                break;
              }
            }
          }
          /* Perfect match */
          foundEmoticons.append(EmoticonNode((*it), pos));
          found = true;
          /* Skip the matched emoticon's matchText */
          pos += needle.length() - 1;
          break;
        }

        if (found) {
          break;
        }
      }

      if (!found) {
        if (inHTMLEntity) {
          // If we are in an HTML entitiy such as &gt;
          int htmlEnd = message.indexOf(';', pos);
          // Search for where it ends
          if (htmlEnd == -1) {
            // Apparently this HTML entity isn't ended, something is wrong, try skip the '&'
            // and continue
            inHTMLEntity = false;
            pos++;
          }
          else {
            pos = htmlEnd;
            inHTMLEntity = false;
          }
        }
      }
    } /* else no emoticons begin with this character, so don't do anything */
    p = c;
  }

  /* if no emoticons found just return the text */
  if (foundEmoticons.isEmpty()) {
    result.append(Token(Text, message));
    return result;
  }

  /* Second-pass, generate tokens based on the matches */
  pos = 0;
  int length;

  for (int i = 0; i < foundEmoticons.size(); ++i) {
    EmoticonNode itFound = foundEmoticons.at(i);
    needle = (mode & SkipHTML) ? itFound.first.matchTextEscaped
        : itFound.first.matchText;

    if ((length = (itFound.second - pos))) {
      result.append(Token(Text, message.mid(pos, length)));
      result.append(Token(Image, itFound.first.matchTextEscaped,
          itFound.first.picPath, itFound.first.picHTMLCode));
      pos += length + needle.length();
    }
    else {
      result.append(Token(Image, itFound.first.matchTextEscaped,
          itFound.first.picPath, itFound.first.picHTMLCode));
      pos += needle.length();
    }
  }

  if (message.length() - pos) { // if there is remaining regular text
    result.append(Token(Text, message.mid(pos)));
  }

  return result;
}


/*!
 * Returns a QHash that contains the emoticons path as keys and the text as values.
 */
QMap<QString, QStringList> EmoticonsTheme::emoticonsMap() const
{
  if (!d->provider) {
    return QMap<QString, QStringList> ();
  }

  return d->provider->emoticonsMap();
}


/*!
 * Returns the file name of the theme.
 */
QString EmoticonsTheme::fileName() const
{
  if (!d->provider) {
    return QString();
  }

  return d->provider->fileName();
}


/*!
 * Parse emoticons in text \p text with ParseMode \p mode and optionally excluding emoticons from \p exclude.
 *
 * \code
 * EmoticonsTheme theme = Emoticons().theme();
 * QString text = ":D hi :)";
 * QStringList exclude(":)");
 * QString parsed = theme.parseEmoticons(text, EmoticonsTheme::DefaultParse, exclude);
 * // parsed will be "<img align="center" title=":D" alt=":D" src="/path/to/:D.png" width="24" height="24" /> hi :)"
 * \endcode
 *
 * \param text the text to parse.
 * \param mode how to parse the text.
 * \param exclude a list of emoticons to exclude from the parsing.
 * \return the text with emoticons replaced by html images.
 * \note SkipHTML is forced when using this function.
 */
QString EmoticonsTheme::parseEmoticons(const QString &text, ParseMode mode, const QStringList &exclude) const
{
  QList<Token> tokens = tokenize(text, mode | SkipHTML);
  if (tokens.isEmpty() && !text.isEmpty())
    return text;

  QString result;

  int max = d->maxParse;
  int count = 0;

  foreach(const Token &token , tokens) {
    switch (token.type) {
      case Text:
        result += token.text;
        break;
      case Image:
        if (!exclude.contains(token.text)) {
          if (count < max) {
            #if !defined(SCHAT_NO_WEBKIT)
            result += "<span style='display:inline-block;width:1px;height:1px;overflow:hidden;'>" + token.text + "</span>";
            #endif
            result += token.picHTMLCode;
            count++;
          }
        }
        else {
          result += token.text;
        }
        break;

      default:
        break;
    }
  }
  return result;
}


/*!
 * Returns the theme name.
 */
QString EmoticonsTheme::themeName() const
{
  if (!d->provider) {
    return QString();
  }

  return d->provider->themeName();
}


/*!
 * Returns the theme path.
 */
QString EmoticonsTheme::themePath() const
{
  if (!d->provider) {
    return QString();
  }

  return d->provider->themePath();
}


/*!
 * Create a new theme.
 */
void EmoticonsTheme::createNew()
{
  if (!d->provider) {
    return;
  }

  d->provider->createNew();
}


/*!
 * Save the emoticon theme
 */
void EmoticonsTheme::save()
{
  if (!d->provider) {
    return;
  }

  d->provider->save();
}


/*!
 * Устанавливает ограничение на количество смайлов
 * которые будут вставлены.
 */
void EmoticonsTheme::setMaxParse(int max)
{
   if (max < 1)
     d->maxParse = 5;
   else
     d->maxParse = max;
}


/*!
 * Set the theme name.
 *
 * \param name name of the theme.
 */
void EmoticonsTheme::setThemeName(const QString &name)
{
  if (!d->provider) {
    return;
  }

  d->provider->setThemeName(name);
}
