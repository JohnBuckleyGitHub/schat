/* $Id$
 * IMPOMEZIA Simple Chat Mobile
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
 * Copyright © 2004-2009 Juho Vähä-Herttua
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
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;


/**
 *
 * @author IMPOMEZIA
 */
public class TextArea {
  private boolean m_scrollBar;
  private Font m_font;
  private int m_bufIndex;
  private int m_fontHeight;
  private int m_height;
  private int m_left;
  private int m_position, m_emptyLines;
  private int m_top;
  private int m_width;
  private String[][] m_scrollBuffer;

  private final int MAX_LINES;;


  /**
   *
   * @param left
   * @param top
   * @param width
   * @param height
   * @param font
   * @param buflines
   * @param scrollbar
   */
  public TextArea(int left, int top, int width, int height, Font font, int buflines, boolean scrollbar) {
    m_position = 0;

    m_left = left;
    m_top = top;
    m_width = width;
    m_height = height;
    m_font = font;
    m_scrollBar = scrollbar;
    m_fontHeight = font.getHeight();

    MAX_LINES = buflines;
    m_scrollBuffer = new String[MAX_LINES][];
    m_emptyLines = MAX_LINES;
  }


  /**
   * Добавление строки.
   * @param strings
   */
  private void addLine(String[] strings) {
    boolean end = isAtEndPos();

    m_scrollBuffer[m_bufIndex] = strings;
    m_bufIndex = (m_bufIndex + 1) % MAX_LINES;

    if (!end && m_emptyLines == 0) {
      updatePosition(-1);
    }

    if (m_emptyLines > 0) {
      m_emptyLines--;
    }
  }


  /**
   * First character or String is the colour with following rules:
   *   lowest 4 bits indicate the foreground colour
   *   next 4 bits indicate the background colour
   *   next 4 bits indicate the m_font style as in Font.STYLE_* (only 3 in use)
   * @param strings
   */
  public synchronized void addText(String[] strings) {
    String tmpline;
    Font font;
    Vector rets, retc;
    int new_width;
    boolean endspace;

    rets = new Vector();
    retc = new Vector();
    font = this.m_font;
    new_width = 0;
    endspace = false;

    for (int i = 0; i < strings.length; i++) {
      String line;
      String[] s;
      char currentcol;

      line = null;
      tmpline = null;
      currentcol = strings[i].charAt(0);
      strings[i] = strings[i].substring(1);

      font = Font.getFont(font.getFace(), (currentcol >> 8)&0x0f, font.getSize());
      s = Utils.splitString(strings[i], " ");
      for (int j = 0; j < s.length; j++) {
        // Notice that m_width includes now m_scrollBar so it's decreased here in 3 places
        // using 2 pixels for m_scrollBar so we decrease 5 (1 pixel m_left, 1 right)
        if (tmpline == null) {
          // colour just changed, we need special handling
          tmpline = s[j];

          // also special linechange handling
          if (new_width + font.stringWidth(tmpline) > m_width-5) {
            if (endspace) {
              line = "";
              tmpline = " " + tmpline;
            }
            else {
              int k;
              for (k = 1; new_width + font.stringWidth(tmpline.substring(0, k)) < m_width - 5; k++) {}
              line = tmpline.substring(0, k-1);
              tmpline = " " + tmpline.substring(k-1);
            }
            new_width = 0;
          }
          new_width += font.stringWidth(tmpline);
        }
        // linechange handling
        else if (new_width + font.stringWidth(" " + s[j]) > m_width-5) {
          line = tmpline;
          tmpline = " " + s[j];
          new_width = font.stringWidth(tmpline);
        }
        // normal adding
        else {
          tmpline += " " + s[j];
          new_width += font.stringWidth(" " + s[j]);
        }

        while (line != null) {
          // we don't want to add an empty line
          if (!line.equals("")) {
            rets.addElement(line);
            retc.addElement(new Character(currentcol));
          }

          String[] sarray = new String[rets.size()];
          for (int k = 0; k < sarray.length; k++)
            sarray[k] = ((Character) retc.elementAt(k)).charValue() + (String) rets.elementAt(k);
          addLine(sarray);

          rets.removeAllElements();
          retc.removeAllElements();

          if (font.stringWidth(tmpline) > m_width-5) {
            int k;
            for (k = 1; font.stringWidth(tmpline.substring(0, k)) < m_width - 5; k++) {}
            line = tmpline.substring(0, k - 1);
            tmpline = " " + tmpline.substring(k - 1);
            new_width = font.stringWidth(tmpline);
          }
          else line = null;
        }
      }

      rets.addElement(tmpline);
      retc.addElement(new Character(currentcol));

      if (tmpline.length() > 0 && tmpline.charAt(tmpline.length()-1) == ' ') endspace = true;
      else endspace = false;
    }

    if (rets.size() > 0 && retc.size() > 0) {
      String[] sarray = new String[rets.size()];
      for (int i = 0; i < sarray.length; i++) {
        sarray[i] = ((Character) retc.elementAt(i)).charValue() + (String) rets.elementAt(i);
      }
      addLine(sarray);

      rets.removeAllElements();
      retc.removeAllElements();
    }
  }


  /**
   * Очистка.
   */
  public void clear() {
    m_scrollBuffer = new String[MAX_LINES][];
    m_emptyLines = MAX_LINES;
  }


  /**
   * Таблица цветов.
   * @param numb
   * @return
   */
  private int color(int numb) {
    numb &= 0x0f;

    switch (numb) {
      case 0:  return 0x00000000;
      case 1:  return 0x00aa0000;
      case 2:  return 0x006bb521; // Зелёный - успех.
      case 3:  return 0x00aa5522;
      case 4:  return 0x00185074; // Синий: ники.
      case 5:  return 0x00be00c4; // Фиолетовый
      case 6:  return 0x0000aaaa;
      case 7:  return 0x0090a4b3; // Серый: служебные сообщения.
      case 8:  return 0x00444444;
      case 9:  return 0x00da251d; // Красный - ошибки.
      case 10:  return 0x0044ff44;
      case 11:  return 0x00ff9900; // Жёлтый.
      case 12:  return 0x005096cf; // Информационный синий.
      case 13:  return 0x00ff44ff;
      case 14:  return 0x0044ffff;
      case 15:  return 0x00ffffff;
    }
    return 0x00FFFFFF;
  }


  /**
   * Отрисовка.
   * @param g
   */
  public void draw(Graphics g) {
    int mls = (m_height / m_fontHeight); // max lines in screen
    char lastcolour;

    g.setFont(m_font);
    g.setColor(0);

    // loops through every line on screen
    for (int i = 0; i < mls; i++) {
      int leftpixels = 1 + m_left;
      int idx = (m_bufIndex + m_emptyLines + m_position + i) % m_scrollBuffer.length;
      String[] strings = m_scrollBuffer[idx];

      lastcolour = 0;
      if (strings == null) {
        break; // we get null and stop iterating
      }
      if (strings[0].charAt(1) == ' ') {
        leftpixels += g.getFont().stringWidth(" ");
      }

      for (int j = 0; j < strings.length; j++) {
        char currentcol = strings[j].charAt(0);
        String currentstr = strings[j].substring(1);

        if (currentstr.charAt(0) == ' ' && j == 0) {
          currentstr = currentstr.substring(1);
        }
        if (currentcol != lastcolour) {
          // set new style
          g.setFont(Font.getFont(m_font.getFace(), (currentcol >> 8) & 0x0f, m_font.getSize()));
          // set background colour and paint it to screen
          g.setColor(color((currentcol >> 4) & 0x0f));
          g.fillRect(leftpixels, m_top + i * m_fontHeight, g.getFont().stringWidth(currentstr), m_fontHeight);
          // set m_font colour and update lastcolour
          g.setColor(color(currentcol & 0x0f));
          lastcolour = currentcol;
        }
        g.drawString(currentstr, leftpixels, m_top + i * m_fontHeight, Graphics.LEFT | Graphics.TOP);
        leftpixels += g.getFont().stringWidth(currentstr);
      }
    }

    if (m_scrollBar && MAX_LINES - m_emptyLines > mls) {
      int startpos = ((m_position * m_height) / (MAX_LINES - m_emptyLines - mls));

      g.setColor(200, 200, 200);
      g.fillRect(m_width - 3, m_top, 2, m_height);
      g.setColor(0, 0, 0);
      g.fillRect(m_width - 3, m_top + startpos, 2, 10);
    }
  }


  /**
   *
   * @return ??
   */
  public boolean isAtEndPos() {
    return (m_position >= MAX_LINES - m_emptyLines - (m_height / m_fontHeight));
  }


  /**
   * -1 is end, -2 is pageup, -3 is pagedown
   * @param pos
   * @return true if repaint needed
   */
  public boolean setPosition(int pos) {
    int screenlines = (m_height / m_fontHeight);
    int maxpos = MAX_LINES - m_emptyLines - screenlines;
    int oldpos = m_position;
    if (maxpos < 0) {
      maxpos = 0;
    }

    if (pos == -3) {
      updatePosition(screenlines);
    } else if (pos == -2) {
      updatePosition(0 - screenlines);
    } else if (pos < 0) {
      m_position = maxpos;
    } else if (pos > maxpos) {
      m_position = maxpos;
    } else {
      m_position = pos;
    }

    if (oldpos == m_position) {
      return false;
    } else {
      return true;
    }
  }


  /**
   * Установка размера.
   * @param newtop
   * @param newheight
   */
  public void setSize(int newtop, int newheight) {
    boolean end = isAtEndPos();
    this.m_top = newtop;
    this.m_height = newheight;
    if (end) {
      setPosition(-1);
    } else {
      setPosition(m_position);
    }
  }


  /**
   * Обновление позиции.
   * @param dpos
   * @return ??
   */
  public boolean updatePosition(int dpos) {
    if (m_position + dpos < 0) {
      return setPosition(0);
    } else {
      return setPosition(m_position + dpos);
    }
  }
}
