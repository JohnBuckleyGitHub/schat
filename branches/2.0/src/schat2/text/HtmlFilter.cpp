/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#include <QRegExp>
#include <QStringList>
#include <QDebug>

#include "text/HtmlFilter.h"
#include "text/PlainTextFilter.h"

HtmlFilter::HtmlFilter()
  : AbstractFilter(QLatin1String("Html"))
{
}


QString HtmlFilter::filter(const QString &text, QVariantHash options) const
{
  Q_UNUSED(options)

  QString out = text.simplified();
  m_optimize = false;

  PlainTextFilter::removeTag(out, QLatin1String("head"));
  PlainTextFilter::removeTag(out, QLatin1String("style"));
  PlainTextFilter::removeTag(out, QLatin1String("script"));

  QList<HtmlToken> tokens;
  tokenize(out, tokens);
  out.clear();

  if (m_optimize)
    optimize(tokens);

  for (int i = 0; i < tokens.size(); ++i) {
    out += tokens.at(i).text;
  }

//  removeTags(out, safe);

  /// Очищает html документ от неотображаемой информации.
//  QRegExp badStuff(QLatin1String("<![^<>]*>|<head[^<>]*>.*</head[^<>]*>|</?html[^<>]*>|</?body[^<>]*>|</?p[^<>]*>"));
//  badStuff.setCaseSensitivity(Qt::CaseInsensitive);
//  out.remove(badStuff);
//  out = out.trimmed();

//  if (out.isEmpty())
//    return "";

//  out.remove('\r');

  /// Обрезает до максимальной длинны.
//  if (left)
//    out = out.left(left);
//
//  if (!strict)
//    return out;

  /// Удаляет пустые ссылки.
//  badStuff.setPattern(QLatin1String("<a[^<]*>[\\s]*</a>"));
//  out.remove(badStuff);

//  out.replace("  ", "&nbsp;&nbsp;");

  /// Заменяет перенос строк на соответствующий html код.
//  out.replace(QLatin1String("\n"), QLatin1String("<br />"));

  /// Заменяет двойные переносы строк на одинарные.
//  while (out.contains(QLatin1String("<br /><br /><br />")))
//    out.replace(QLatin1String("<br /><br /><br />"), QLatin1String("<br /><br />"));

  /// Удаляет код переноса строки если тот находится в конце сообщения.
//  out.replace(QLatin1String("<br /><br /></span>"), QLatin1String("<br /></span>"));
//  out.replace(QLatin1String("<br /></span>"), QLatin1String("</span>"));
//
//  if (out.endsWith(QLatin1String("<br /><br />")))
//    out = out.left(out.size() - 12);
//  if (out.endsWith(QLatin1String("<br />")))
//    out = out.left(out.size() - 6);

  /// Удаляет запрещённые css стили.
  /// \todo Эта функция также удалит заданные селекторы из текста, что не допустимо.
//  badStuff.setPattern("\\s?font-size:[^;]*;|\\s?background-color:[^;]*;|\\s?font-family:[^;]*;");
//  out.remove(badStuff);

//  if (toPlainText(out).isEmpty()) /// FIXME ! toPlainText
//    return "";

  return out;
}


/*!
 * Удаляет все html теги из текста, кроме разрешённых.
 */
void HtmlFilter::removeTags(QString &text, const QStringList &exclude)
{
  int lt = -1;
  int gt = 0;
  forever {
    lt = text.indexOf(QLatin1Char('<'), lt + 1);
    if (lt == -1)
      break;

    gt = text.indexOf(QLatin1Char('>'), lt);
    if (gt == -1)
      gt = text.size() - lt;

    QString tag = text.mid(lt + 1, gt - lt - 1);
    if (tag.startsWith(QLatin1Char('/')))
      tag.remove(0, 1);

    int space = tag.indexOf(QLatin1Char(' '));
    if (space != -1)
      tag.remove(space, tag.size() - space);

    bool safe = false;
    for (int i = 0; i < exclude.size(); ++i) {
      if (exclude.at(i) == tag) {
        safe = true;
        break;
      }
    }

    if (!safe)
      text.remove(lt, gt - lt + 1);
  }
}


void HtmlFilter::optimize(QList<HtmlToken> &tokens) const
{
  int index = -1;
  for (int i = 0; i < tokens.size(); ++i) {
    if (!tokens.at(i).simple) {
      index = i;
      break;
    }
  }

  if (index == -1)
    return;

  HtmlToken token = tokens.at(index);

  if (token.tag == QLatin1String("a")) {
    tokens[index].simple = true;
    return optimize(tokens);
  }
  else if (token.tag == QLatin1String("span")) {
    QList<HtmlToken> tags;

    if (token.text.contains(QLatin1String("font-weight:600;")))
      tags.append(HtmlToken(HtmlToken::Tag, QLatin1String("<b>")));

    if (token.text.contains(QLatin1String("font-style:italic;")))
      tags.append(HtmlToken(HtmlToken::Tag, QLatin1String("<i>")));

    if (token.text.contains(QLatin1String("underline")))
      tags.append(HtmlToken(HtmlToken::Tag, QLatin1String("<u>")));

    if (token.text.contains(QLatin1String("line-through")))
      tags.append(HtmlToken(HtmlToken::Tag, QLatin1String("<s>")));

    if (token.text.contains(QLatin1String("color:"))) {
      int at = token.text.indexOf(QLatin1String("color:"));
      if (at != -1) {
        QString color = token.text.mid(at + 6, 7);
        if (color.startsWith(QLatin1Char('#'))) {
          HtmlToken token(HtmlToken::Tag, QLatin1String("<font color=\"") + color + QLatin1String("\">"));
          token.simple = true;
          tags.append(token);
        }
      }
    }

    tokens.removeAt(index);
    int gt = -1;

    for (int i = index; i < tokens.size(); ++i) {
      if (tokens.at(i).type == HtmlToken::EndTag && tokens.at(i).tag == token.tag) {
        gt = i;
        break;
      }
    }

    if (gt == -1)
      return optimize(tokens);

    tokens.removeAt(gt);
    if (index == gt)
      return optimize(tokens);

    gt++;

    foreach (HtmlToken tag, tags) {
      tokens.insert(index, tag);
      tokens.insert(gt, tag.toEndTag());
      index++;
      gt++;
    }

    return optimize(tokens);
  }
  else if (token.tag == QLatin1String("font")) {
    tokens[index].simple = true;
    return optimize(tokens);
  }
}


/*!
 * Разбивает текст на токены, удаляет все не допустимые теги.
 */
void HtmlFilter::tokenize(const QString &text, QList<HtmlToken> &tokens) const
{
  QStringList simple; // Список простых тегов.
  simple.append(QLatin1String("br"));
  simple.append(QLatin1String("b"));
  simple.append(QLatin1String("i"));
  simple.append(QLatin1String("u"));
  simple.append(QLatin1String("s"));

  QStringList safe;
  safe.append(QLatin1String("span"));
  safe.append(QLatin1String("a"));
  safe.append(QLatin1String("font"));

  int lt = 0;
  int gt = 0;
  int pos = 0;

  for (int i = 0; i < text.size(); ++i) {
    lt = text.indexOf(QLatin1Char('<'), pos);
    if (lt == -1) {
      HtmlToken token(text.mid(pos));
      tokens.append(token);
      break;
    }

    if (lt != pos) {
      HtmlToken token(text.mid(pos, lt - pos));
      tokens.append(token);
    }

    gt = text.indexOf(QLatin1Char('>'), lt);
    pos = gt + 1;
    HtmlToken token(HtmlToken::Tag, text.mid(lt, pos - lt));

    if (token.type == HtmlToken::EndTag && token.tag == QLatin1String("p")) {
      token.simple = true;
      token.type = HtmlToken::StartTag;
      token.tag = QLatin1String("br");
      token.text = QLatin1String("<br>");
      tokens.append(token);
      continue;
    }

    if (simple.contains(token.tag)) {
      if (token.type == HtmlToken::StartTag && !token.simple) {
        token.simple = true;
        token.text = QLatin1Char('<') + token.tag + QLatin1Char('>');
      }

      tokens.append(token);
    }

    if (safe.contains(token.tag)) {
      m_optimize = true;
      tokens.append(token);
    }
  }
}
