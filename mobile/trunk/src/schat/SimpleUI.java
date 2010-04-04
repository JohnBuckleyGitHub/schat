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

import java.util.Hashtable;
import java.util.Vector;
import javax.microedition.lcdui.AlertType;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;


/**
 *
 * @author IMPOMEZIA
 */
public class SimpleUI {
  private boolean m_header;
  private boolean m_useColor;
  private boolean m_vibrate;
  private boolean m_winLock;
  private Display m_display;
  private Hashtable m_privates;
  private int m_bufLines;
  private int m_currentWin;
  private int m_fontSize;
  private Vector m_windows; // excludes m_main
  private Window m_main;
  public boolean keylock;


  /**
   * Конструктор класса SimpleUI.
   * @param storage
   * @param disp
   */
  public SimpleUI(Storage storage, Display disp) {
    keylock = false;

    m_header = storage.header;
    m_fontSize = storage.fontSize;
    m_bufLines = storage.bufLines;
    m_useColor = storage.useColor;
    m_vibrate = storage.vibrate;
    m_display = disp;

    m_winLock = false;
    m_privates = new Hashtable();
    m_windows = new Vector();
    m_currentWin = 0;
    m_main = new Window(this, "Simple Chat", Window.TYPE_MAIN, m_header, m_useColor, m_fontSize, m_bufLines);
    addWindow(m_main);
  }


  /**
   * Добавление окна.
   * @param win
   */
  public void addWindow(Window win) {
    m_windows.addElement(win);
    displayWindow(-1);
  }


  /**
   * Очистка.
   */
  public void cleanup() {
    m_windows.removeAllElements();
    m_privates.clear();
    m_currentWin = -1;

    System.gc();
    setDisplay(Simple.m_mainForm);
  }


  /**
   *
   */
  public void clearChanPriv() {
    m_main.enterExitMode();

    if (m_currentWin >= 0) {
      setDisplay(m_main);
      m_currentWin = 0;
    }

    for (int i = m_windows.size() - 1; i >= 1; i--) {
      // we don't clear the m_windows, just put to closing mode
      ((Window) m_windows.elementAt(i)).enterExitMode();
    }
    m_main.repaint();
  }


  /**
   * Удаление окна.
   * @param win
   */
  public void deleteWindow(Window win) {
    m_privates.remove(win.name().toUpperCase());

    if (m_windows.indexOf(win) <= m_currentWin) {
      m_currentWin--;
      setDisplay((Window) m_windows.elementAt(m_currentWin));
      m_windows.removeElement(win);
    }
    else {
      m_windows.removeElement(win);
      repaint();
    }
  }


  /**
   * Показ следующего окна.
   */
  public void displayNextWindow() {
    displayWindow(m_currentWin + 1);
  }


  /**
   * Показ предидущего окна.
   */
  public void displayPreviousWindow() {
    displayWindow(m_currentWin - 1);
  }


  /**
   * Показ окна.
   * @param num
   */
  public void displayWindow(int num) {
    if (m_winLock) return; // no window changing on m_winLock

    if (num >= m_windows.size()) num = 0;
    if (num < 0) num = m_windows.size()-1;

    if (num != m_currentWin) {
      ((Window) m_windows.elementAt(m_currentWin)).setState(Window.STATE_NONE);
      setDisplay((Window) m_windows.elementAt(num));
      m_currentWin = num;
    }
    System.gc();
  }


  /**
   * @return Индикаторы состояний окон.
   */
  public int[] indicators() {
    int[] ret = new int[m_windows.size()];

    for (int i = 0; i < m_windows.size(); i++) {
      if (i == m_currentWin) {
        ret[i] = Window.STATE_SELECTED;
      } else {
        ret[i] = ((Window) m_windows.elementAt(i)).state();
      }
    }
    return ret;
  }


  /**
   * @return Возвращает окно основного канала.
   */
  public Window mainChat() {
    return m_main;
  }


  /**
   *
   * @param louder
   * @return true if the user was alerted, false otherwise.
   */
  public boolean playAlarm(boolean louder) {
    if (louder)
      return AlertType.ALARM.playSound(m_display);
    else
      return AlertType.INFO.playSound(m_display);
  }


  /**
   * @param priv
   * @param create
   * @return Возращает приват.
   */
  public Window privateWindow(String priv, boolean create) {
    priv = priv.trim();
    Window win = (Window) m_privates.get(priv.toUpperCase());

    if (win == null && create) {
      win = new Window(this, priv, Window.TYPE_PRIVATE, m_header, m_useColor, m_fontSize, m_bufLines);
      m_privates.put(priv.toUpperCase(), win);
      addWindow(win);

      if (keylock)
        playAlarm(true);
    }
    return win;
  }


  /**
   * Переименование окна.
   * @param oldNick
   * @param window
   */
  void renameWindow(String oldNick, Window window) {
    m_privates.remove(oldNick.toUpperCase());
    m_privates.put(window.name().toUpperCase(), window);
  }


  /**
   * Перерисовка.
   */
  public void repaint() {
    if (m_windows.size() > 0)
      ((Window) m_windows.elementAt(m_currentWin)).repaint();
  }


  /**
   *
   * @param disp
   */
  public void setDisplay(Displayable disp) {
    m_display.setCurrent(disp);
  }


  /**
   *
   * @param visible
   */
  public void setHeader(boolean visible) {
    m_header = visible;

    for (int i = 0; i < m_windows.size(); i++) {
      ((Window) m_windows.elementAt(i)).setHeaderVisible(m_header);
    }
  }


  /**
   *
   * @param lock
   */
  public void setWinlock(boolean lock) {
    m_winLock = lock;
  }


  /**
   * Вибрация.
   */
  public void vibrate() {
    if (m_vibrate)
      m_display.vibrate(500);
  }
}
