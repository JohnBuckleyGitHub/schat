/* $Id$
 * IMPOMEZIA Simple Chat Mobile
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

package schat;

import java.util.Vector;

/**
 * Служебные функции.
 */
public class Utils {

  /**
   * @param s
   * @return Строка с экранироваными HTML сущностями.
   */
  public static String escapeHTML(String s) {
    char[] ch = s.toCharArray();
    StringBuffer buf = new StringBuffer();

    for (int i = 0; i < ch.length; i++) {
      switch (ch[i]) {
        case '&':
          buf.append("&amp;");
          break;

        case '<':
          buf.append("&lt;");
          break;

        case '>':
          buf.append("&gt;");
          break;

        case '"':
          buf.append("&quot;");
          break;

        default:
          buf.append(ch[i]);
      }
    }

    return buf.toString();
  }


  /**
   * Выполянет замену.
   * @param source
   * @param from
   * @param to
   * @return Результат замены.
   */
  public static String replaceAll(String source, String from, String to) {
    int index = source.indexOf(from, 0);
    if (index == -1)
      return source;

    StringBuffer buf = new StringBuffer();
    buf.append(source.substring(0, index));
    buf.append(to);
    int length = from.length();
    int offset = index + length;

    while (true) {
      index = source.indexOf(from, offset);
      if (index != -1) {
        buf.append(source.substring(offset, index));
        buf.append(to);
        offset = index + length;
      }
      else {
        buf.append(source.substring(offset));
        break;
      }
    }

    return buf.toString();
  }


  /**
   * Разбивает строку.
   * @param str
   * @param delims
   * @return Результирующий массив.
   */
  public static String[] splitString(String str, String delims) {
    if (str == null) {
      return null;
    } else if (str.equals("") || delims == null || delims.length() == 0) {
      return new String[]{str};
    }

    String[] s;
    Vector v = new Vector();
    int pos, newpos;

    pos = 0;
    newpos = str.indexOf(delims, pos);

    while (newpos != -1) {
      v.addElement(str.substring(pos, newpos));
      pos = newpos + delims.length();
      newpos = str.indexOf(delims, pos);
    }
    v.addElement(str.substring(pos));

    s = new String[v.size()];
    for (int i = 0; i < s.length; i++) {
      s[i] = (String) v.elementAt(i);
    }
    return s;
  }


  /**
   * Удаляет HTML теги из строки.
   * @param s
   * @return Результирующая строка.
   */
  public static String stripHTMLTags(String s) {

    s = replaceAll(s, "<br />", " ");
    s = replaceAll(s, "<tr>", " ");

    char[] ch = s.toCharArray();
    boolean inElement = false;
    StringBuffer buf = new StringBuffer();

    for (int i = 0; i < ch.length; i++) {
      switch (ch[i]) {
        case '<':
          inElement = true;
          break;

        case '>':
          if (inElement) {
            inElement = false;
          } else {
            buf.append(ch[i]);
          }
          break;

        default:
          if (!inElement) {
            buf.append(ch[i]);
          }
          break;
      }
    }

    return unEscapeHTML(buf.toString());
  }


  /**
   * Удаляет все повторяющиеся пробелы в строке и обрезает до указанной длинны.
   * @param s
   * @param left
   * @return Результирующая строка.
   */
  public static String simplified(String s, int left) {
    if (s.length() == 0)
      return s;

    s = s.trim();

    if (s.indexOf(' ') != -1) {
      char[] ch = s.toCharArray();
      char prev = 0;
      StringBuffer buf = new StringBuffer();
      for (int i = 0; i < ch.length; i++) {
        if (ch[i] == ' ' && prev == ' ') {
          prev = ' ';
          continue;
        }

        prev = ch[i];
        buf.append(prev);
      }

      s = buf.toString();
    }

    if (left != 0 && s.length() > left)
      s = s.substring(0, left);

    return s;
  }


  /**
   * Преобразование HTML сущностей в текст.
   * @param s
   * @return Результирующая строка.
   */
  public static String unEscapeHTML(String s) {
    String out = replaceAll(s, "&gt;", ">");
    out = replaceAll(out, "&lt;", "<");
    out = replaceAll(out, "&quot;", "\"");
    out = replaceAll(out, "&nbsp;", " ");
    out = replaceAll(out, "&amp;", "&");

    return out;
  }
}
