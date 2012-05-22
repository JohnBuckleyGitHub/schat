/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include <QStringList>

#include "sglobal.h"
#include "text/HtmlFilter.h"
#include "text/PlainTextFilter.h"

HtmlFilter::HtmlFilter(const QVariantHash &options)
  : m_options(NoOptions)
{
  m_sizeLimit = options.value(LS("size"), 8000).toInt();
  m_breaksLimit = options.value(LS("breaks"), 20).toInt();

  if (options.value(LS("nbsp"), false) == true)
    m_options |= ConvertSpacesToNbsp;

  if (options.value(LS("span"), false) == true)
    m_options |= AllowSpanTag;
}


HtmlFilter::HtmlFilter(int options, int sizeLimit, int breaksLimit)
  : m_breaksLimit(breaksLimit)
  , m_options(options)
  , m_sizeLimit(sizeLimit)
{
}


QList<HtmlToken> HtmlFilter::tokenize(const QString &text) const
{
  QString out = prepare(text);

  QList<HtmlToken> tokens;
  tokenize(out, tokens);
  out.clear();

  if (m_optimize)
    optimize(tokens);

  for (int i = tokens.size() - 1; i >= 0; --i) {
    if ((tokens.at(i).type == HtmlToken::EndTag || tokens.at(i).type == HtmlToken::StartTag) && tokens.at(i).tag == LS("span")) {
      tokens.removeAt(i);
    }
  }

  if (m_sizeLimit > 0) {
    for (int i = 0; i < tokens.size(); ++i) {
      m_size += tokens.at(i).text.size();
      if (m_size > m_sizeLimit) {
        truncate(tokens, i);
        break;
      }
    }
  }

  trimmed(tokens);
  return tokens;
}


QString HtmlFilter::filter(const QString &text) const
{
  return build(tokenize(text));
}


QString HtmlFilter::build(const QList<HtmlToken> &tokens)
{
  QString out;

  for (int i = 0; i < tokens.size(); ++i) {
    out += tokens.at(i).text;
  }

  QString nbsp = out;
  nbsp.remove(LS("&nbsp;"));
  if (nbsp.simplified().isEmpty())
    return QString();

  return out;
}


bool HtmlFilter::isLastIsBreak(const QList<HtmlToken> &tokens) const
{
  if (m_breaksLimit == 0)
    return true;

  if (tokens.isEmpty())
    return false;

  if (m_breaksLimit > 0 && m_breaksLimit == m_breaks)
    return true;

  if (m_br > 1)
    return true;

  if (tokens.last().text.contains(QString("&nbsp;"))) {
    QString text = tokens.last().text;
    text.remove(QString("&nbsp;"));
    if (text.simplified().isEmpty())
      return true;
  }

  return false;
}


bool HtmlFilter::isSpace(const HtmlToken &token) const
{
  if (token.text.isEmpty())
    return true;

  if (token.text == LS(" "))
    return true;

  if (token.tag == LS("br"))
    return true;

  return false;
}


/*!
 * Поиск закрывающего тега для элемента.
 */
int HtmlFilter::endTag(const QString &tag, QList<HtmlToken> &tokens, int pos) const
{
  int gt = -1;
  for (int i = pos; i < tokens.size(); ++i) {
    if (tokens.at(i).type == HtmlToken::EndTag && tokens.at(i).tag == tag) {
      gt = i;
      break;
    }
  }

  return gt;
}


QString HtmlFilter::prepare(const QString &text) const
{
  QString out;
  if (m_options & ConvertSpacesToNbsp) {
    out = text;
//    out.replace(QLatin1String("  "), QLatin1String("&nbsp; "));
    out = out.simplified();
  }
  else
    out = text.simplified();

  m_optimize = false;
  m_breaks = 0;
  m_size = 0;
  m_br = 0;

  PlainTextFilter::removeTag(out, LS("head"));
  PlainTextFilter::removeTag(out, LS("style"));
  PlainTextFilter::removeTag(out, LS("script"));

  return out;
}


void HtmlFilter::makeTextToken(QList<HtmlToken> &tokens, const QString &text) const
{
  HtmlToken token(text);

  if (tokens.isEmpty()) {
    tokens.append(token);
    m_br = 0;
    return;
  }

  if (tokens.last().type == HtmlToken::Text) {
    if (text == LS(" ") && tokens.last().text == text)
      return;

    tokens.last().text.append(text);
    return;
  }

  if (tokens.last().type == HtmlToken::StartTag && text == LS(" ") && tokens.last().tag == LS("br"))
    return;

  tokens.append(token);
  m_br = 0;
}


/*!
 * Оптимизация и дополнительная фильтрация токенов.
 */
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

  if (token.tag == LS("a") || token.tag == LS("font")) {
    tokens[index].simple = true;
    int gt = endTag(token.tag, tokens, index);

    // Если закрывающий тег не найден, удаляем тег.
    if (gt == -1) {
      tokens.removeAt(index);
      return optimize(tokens);
    }

    // Если закрывающий следует сразу после открывающего, удаляем тег и закрывающий тег.
    if (gt - index == 1) {
      tokens.removeAt(index);
      if (index < tokens.size())
        tokens.removeAt(index);

      return optimize(tokens);
    }

    // Удаляем все не текстовые токены внутри тега.
    for (int i = gt - 1; i > index; --i) {
      if (tokens.at(i).type != HtmlToken::Text) {
        tokens.removeAt(i);
        gt--;
      }
      else {
        tokens[i].parent = token.tag;
      }
    }

    AbstractTag *tag = 0;
    if (token.tag == LS("a"))
      tag = new HtmlATag(token);
    else if (token.tag == LS("font"))
      tag = new HtmlFontTag(token);

    if (!tag->valid || gt - index == 1) {
      tokens.removeAt(gt);
      tokens.removeAt(index);
    }
    else
      tokens[index].text = tag->toText();

    delete tag;
    return optimize(tokens);
  }
  /// Тег span в зависимости от css стилей преобразуется в теги b, i, u, s и font
  /// и полностью удаляется из текста. Тег font используется для установки цвета элемента.
  else if (token.tag == LS("span")) {
    QList<HtmlToken> tags;

    if (token.text.contains(LS("font-weight:600;")))
      tags.append(HtmlToken(HtmlToken::Tag, LS("<b>")));

    if (token.text.contains(LS("font-style:italic;")))
      tags.append(HtmlToken(HtmlToken::Tag, LS("<i>")));

    if (token.text.contains(LS("underline")))
      tags.append(HtmlToken(HtmlToken::Tag, LS("<u>")));

    if (token.text.contains(LS("line-through")))
      tags.append(HtmlToken(HtmlToken::Tag, LS("<s>")));

    if (token.text.contains(LS("color:"))) {
      int at = token.text.indexOf(LS("color:"));
      if (at != -1) {
        QString color = token.text.mid(at + 6, 7);
        if (color.startsWith(LC('#'))) {
          HtmlToken token(HtmlToken::Tag, LS("<font color=\"") + color + LS("\">"));
          token.simple = true;
          tags.append(token);
        }
      }
    }

    tokens.removeAt(index);
    int gt = endTag(token.tag, tokens, index);

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
}


/*!
 * Разбивает текст на токены, удаляет все не допустимые теги.
 */
void HtmlFilter::tokenize(const QString &text, QList<HtmlToken> &tokens) const
{
  QStringList simple; // Список простых тегов.
  simple.append(LS("br"));
  simple.append(LS("b"));
  simple.append(LS("i"));
  simple.append(LS("u"));
  simple.append(LS("s"));

  QStringList safe;
  if (m_options & AllowSpanTag)
    safe.append(LS("span"));

  safe.append(LS("a"));
  safe.append(LS("font"));

  int lt = 0;
  int gt = 0;
  int pos = 0;

  for (int i = 0; i < text.size(); ++i) {
    lt = text.indexOf(LC('<'), pos);
    if (lt == -1) {
      HtmlToken token(text.mid(pos));
      tokens.append(token);
      m_br = 0;
      break;
    }

    if (lt != pos) {
      makeTextToken(tokens, text.mid(pos, lt - pos));
    }

    gt = text.indexOf(LC('>'), lt);
    pos = gt + 1;
    HtmlToken token(HtmlToken::Tag, text.mid(lt, pos - lt));

    // Закрывающий тег p преобразуется в тег br.
    if (token.type == HtmlToken::EndTag && (token.tag == LS("p") || token.tag == LS("div"))) {
      if (!tokens.isEmpty()) {
        if (token.tag == LS("div"))
          m_br++;

        if (isLastIsBreak(tokens))
          continue;

        HtmlToken token(HtmlToken::Tag, LS("<br>"));
        tokens.append(token);
        m_br++;
        m_breaks++;
      }
      continue;
    }

    // Обработка простых тегов, начальный тег принудительно приводится к простому виду.
    if (simple.contains(token.tag)) {
      if (token.type == HtmlToken::StartTag && !token.simple) {
        token.simple = true;
        token.text = LC('<') + token.tag + LC('>');
      }

      if (token.tag == LS("br")) {
        if (isLastIsBreak(tokens))
          continue;

        m_br++;
        m_breaks++;
      }

      tokens.append(token);
    }

    if (safe.contains(token.tag)) {
      m_optimize = true;
      tokens.append(token);
      m_br = 0;
    }
  }

  trimmed(tokens);
}



void HtmlFilter::trimmed(QList<HtmlToken> &tokens) const
{
  forever {
    if (!tokens.isEmpty() && isSpace(tokens.first()))
      tokens.removeFirst();
    else
      break;
  }

  forever {
    if (!tokens.isEmpty() && isSpace(tokens.last()))
      tokens.removeLast();
    else
      break;
  }
}


/*!
 * Ограничение максимальной длины текста.
 * В зависимости от html разметки результирующий размер может быть немного меньше или больше лимита.
 *
 * \param tokens Токены.
 * \param pos    Индекс токена на котором было обнаружено превышение размера.
 */
void HtmlFilter::truncate(QList<HtmlToken> &tokens, int pos) const
{
  if (tokens.at(pos).type == HtmlToken::Text) {
    tokens[pos].text = tokens.at(pos).text.left(tokens.at(pos).text.size() - (m_size - m_sizeLimit));
    if (tokens.at(pos).text.isEmpty())
       return truncate(tokens, --pos);

    ++pos;

    for (int i = pos; i < tokens.size(); ++i) {
      if (tokens.at(i).type != HtmlToken::EndTag) {
        pos = i;
        break;
      }
    }
  }
  else if (tokens.at(pos).type == HtmlToken::StartTag) {
    for (int i = pos; i >= 0; --i) {
      if (tokens.at(i).type != HtmlToken::StartTag) {
        pos = ++i;
        break;
      }
    }
  }
  else if (tokens.at(pos).type == HtmlToken::EndTag) {
    ++pos;

    for (int i = pos; i < tokens.size(); ++i) {
      if (tokens.at(i).type != HtmlToken::EndTag) {
        pos = i;
        break;
      }
    }
  }

  for (int i = tokens.size(); i >= pos; --i) {
    tokens.removeAt(i);
  }
}
