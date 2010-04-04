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

import java.util.Calendar;
import java.util.Date;
import java.util.Hashtable;
import java.util.Vector;
import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.AlertType;
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.List;
import javax.microedition.lcdui.TextBox;
import javax.microedition.lcdui.TextField;


/**
 *
 * @author IMPOMEZIA
 */
public class Window extends Canvas implements CommandListener {
  public static final int TYPE_MAIN = 0;
  public static final int TYPE_PRIVATE = 2;

  public static final int STATE_NONE = 0;
  public static final int STATE_INFO = 1;
  public static final int STATE_MSG = 2;
  public static final int STATE_HILIGHT = 3;
  public static final int STATE_SELECTED = 4;
  private static boolean m_initialized = false;

  private boolean m_showHeader;
  private boolean m_useColors;
  private final static int MAX_LIST_PERSONS = 20;
  private Font m_headerFont;
  private Hashtable m_whois;
  private int m_personPosition = 0;
  private int m_type;
  private int m_state;
  private List m_usersList;
  private List userCmdList;
  private long m_keyLockTime;
  private SimpleUI m_ui;
  private static Command c_close;
  private static Command c_closeConsole;
  private static Command c_closeUsers;
  private static Command c_disconnect;
  private static Command c_msg;
  private static Command c_ok;
  private static Command c_send, c_cancel;
  private static Command c_traffic;
  private static Command c_users;
  private static Command c_whois;
  private String m_name;
  private TextArea m_textArea;
  private TextBox m_textBox;
  private Vector m_users;

  /**
   * Конструктор класса Window.
   * @param ui
   * @param name
   * @param type
   * @param showheader
   * @param usecol
   * @param fontsize
   * @param buflines
   */
  public Window(SimpleUI ui, String name, int type, boolean showheader, boolean usecol, int fontsize, int buflines) {
    super();

    m_ui = ui;
    m_name = name;
    m_type = type;
    m_showHeader = showheader;
    m_useColors = usecol;

    m_state = STATE_NONE;
    int textfontsize = 0;
    if (fontsize == 0)
      textfontsize = Font.SIZE_SMALL;
    if (fontsize == 2)
      textfontsize = Font.SIZE_LARGE;

    m_headerFont = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD, textfontsize);
    m_textArea = new TextArea(0, 0, getWidth(), getHeight(), Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, textfontsize), buflines, true);
    m_users = new Vector();
    m_whois = new Hashtable();
    setHeaderVisible(showheader);

    if (!m_initialized) {
      c_ok = new Command("Ok", Command.OK, 10);

      c_send = new Command("Отправить", Command.OK, 10);
      c_cancel = new Command("Отмена", Command.CANCEL, 20);

      c_msg = new Command("Написать", Command.OK, 10);
      c_traffic = new Command("Трафик", Command.SCREEN, 50);
      c_disconnect = new Command("Отключиться", Command.SCREEN, 70);

      c_close = new Command("Закрыть", Command.SCREEN, 65);
      c_whois = new Command("Информация", Command.SCREEN, 35);
      c_users = new Command("Список", Command.SCREEN, 35);

      c_closeConsole = new Command("Закрыть", Command.CANCEL, 10);
      c_closeUsers = new Command("Закрыть", Command.CANCEL, 90);

      m_initialized = true;
    }

    addMenu();
    setCommandListener(this);
  }


  /**
   * Добавление меню.
   */
  private void addMenu() {
    addCommand(c_msg);

    if (m_type == Window.TYPE_PRIVATE) {
      addCommand(c_whois);
      addCommand(c_close);
    }
    else {
      addCommand(c_traffic);
      addCommand(c_users);
    }

    addCommand(c_disconnect);
  }


  /**
   * Добавление нового пользователя в список.
   * @param user
   */
  public void addUser(User user) {
    if (m_whois.get(user.nick()) != null)
      return;

    int i, size = m_users.size();
    String upnick = user.nick().toUpperCase();

    for (i = 0; i < size; i++) {
      String n = (String) m_users.elementAt(i);

      if (upnick.compareTo(n.toUpperCase()) < 1) {
        m_users.insertElementAt(user.nick(), i);
        break;
      }
    }

    if (i == size)
      m_users.addElement(user.nick());

    m_whois.put(user.nick(), user);

    updateHeader();
  }


  /**
   * Очистка окна.
   */
  public void clear() {
    m_textArea.clear();
  }


  /**
   * Закрытие окна.
   */
  public void close() {
    m_ui.deleteWindow(this);
  }


  /**
   * Обработка событий от команд.
   * @param command
   * @param displayable
   */
  public void commandAction(Command command, Displayable displayable) {
    if (command == c_msg) {
      m_textBox = new TextBox("Написать","",1000,TextField.ANY);
      m_textBox.addCommand(c_send);
      m_textBox.addCommand(c_cancel);
      m_textBox.setCommandListener(new TextboxListener());
      m_ui.setWinlock(true);
      m_ui.setDisplay(m_textBox);
    }
    // Отключение от сервера.
    else if (command == c_disconnect) {
      Simple.client().exit();
      m_ui.clearChanPriv();
      m_ui.cleanup();
    }
    else if (command == c_closeConsole) {
      m_ui.cleanup();
    }
    // Отображение переданного/полученного трафика.
    else if (command == c_traffic) {
      trafficAlert();
    }
    // Отображение списка пользователей.
    else if (command == c_users) {
      usersList();
    }
    else if (command == c_whois) {
      whoisAlert(m_name);
    }
    // Закрытие привата.
    else if (command == c_close && m_type == TYPE_PRIVATE) {
      close();
    }
    else if (command == c_closeUsers) {
      m_usersList = null;
      m_ui.setWinlock(false);
      show();
    }
    else if (m_usersList != null && command == List.SELECT_COMMAND) {
      String name = m_usersList.getString(m_usersList.getSelectedIndex());
      if (name.equals("[Следующие]")) {
        m_personPosition++;
        usersList();
      }
      else if (name.equals("[Предыдущие]")) {
        m_personPosition--;
        usersList();
      }
      else {
        userCmdList = new List(name, List.IMPLICIT);
        userCmdList.append("[Назад]", null);
        userCmdList.append("Приват", null);
        userCmdList.append("Информация", null);

        userCmdList.addCommand(c_closeUsers);
        userCmdList.setCommandListener(this);
        m_ui.setDisplay(userCmdList);
        m_usersList = null;
      }
    }
    else if (userCmdList != null && command == List.SELECT_COMMAND) {
      String name = userCmdList.getString(userCmdList.getSelectedIndex());
      String nick = userCmdList.getTitle();

      if (name.equals("[Назад]")) {
        usersList();
        userCmdList = null;
      }
      else if (name.equals("Приват")) {
        Window p = m_ui.privateWindow(nick, true);
        m_state = STATE_NONE;
        p.show();
        userCmdList = null;
      }
      else if (name.equals("Информация")) {
        whoisAlert(nick);
      }
      else {
        show();
        userCmdList = null;
      }
    }
  }


  /**
   * Удаление меню.
   */
  private void deleteMenu() {
    removeCommand(c_msg);

    if (m_type == Window.TYPE_PRIVATE) {
      removeCommand(c_whois);
      removeCommand(c_close);
    }
    else {
      removeCommand(c_traffic);
      removeCommand(c_users);
    }

    removeCommand(c_disconnect);
  }


  /**
   * Удаление пользователя.
   * @param nick
   */
  public void deleteUser(String nick) {
    m_users.removeElement(nick);
    m_whois.remove(nick);
  }


  public void enterExitMode() {
    this.addCommand(c_closeConsole);
    deleteMenu();
  }


  /**
   * Sending data from m_textBox.
   * @param str
   */
  private void handleMsg(String str) {
    if (str == null || str.length() == 0)
      return;

    boolean echo = true;
    boolean me = false;

    if (str.startsWith("/")) {
      String lower = str.toLowerCase();
      if (lower.startsWith("/server") || lower.startsWith("/motd")) {
        echo = false;
      }
      else if (lower.startsWith("/me ")) {
        me = true;
      }
      else if (lower.startsWith("/ping")) {
        echo = false;
        SimpleClient.ping = new Date().getTime();
      }
      else if (lower.startsWith("/away") || lower.startsWith("/dnd")) {
        int status = lower.startsWith("/away") ? User.STATUS_AWAY : User.STATUS_DND;
        User user = m_ui.mainChat().user(SimpleClient.user().nick());

        if (user != null) {
          if (user.status() == status)
            status = User.STATUS_NORMAL;

          Simple.client().sendPacket(Protocol.statusPacket(status));
        }
        return;
      }
      else if (lower.startsWith("/clear")) {
        m_textArea.clear();
        return;
      }
    }

    if (Simple.client().state() == SimpleClient.STATE_CONNECTED) {
      if (m_type == TYPE_MAIN) {
        Simple.client().sendPacket(Protocol.messagePacket(null, str));
      } else if (m_ui.mainChat().user(m_name) != null) {
        Simple.client().sendPacket(Protocol.messagePacket(m_name, str));
      }
      else {
        echo = false;
      }

      if (echo) {
        write(SimpleClient.user().nick(), str, me);
      }
    }
  }


  /**
   *
   * @param keyCode
   */
  protected void keyPressed(int keyCode) {
    if (keyCode == KEY_NUM5) {
      m_keyLockTime = System.currentTimeMillis();

      return; // no other function for 5
    }
    else if (keyCode == KEY_POUND) {
      if (System.currentTimeMillis()-m_keyLockTime < 1000) {
        if (!m_ui.keylock) {
          Alert a = new Alert("Keylock", "Keypad locked!", null, AlertType.INFO);
          a.setTimeout(1000);
          m_ui.setDisplay(a);
          m_ui.setWinlock(true);
          deleteMenu();
        }
        else {
          Alert a = new Alert("Keylock", "Keylock removed!", null, AlertType.INFO);
          a.setTimeout(1000);
          m_ui.setDisplay(a);
          m_ui.setWinlock(false);
          addMenu();
        }

        m_ui.keylock = !m_ui.keylock;
        m_keyLockTime = 0;
        repaint();
        return;
      }
    }
    else m_keyLockTime = 0;

    if (m_ui.keylock) {
    }
    else if ((keyCode >= 97 && keyCode <= 122) || (keyCode >= 65 && keyCode <= 90)) { // BlackBerry
      m_textBox = new TextBox("Написать", null, 512, TextField.ANY);
      m_textBox.insert("" + (char) keyCode, 0);
      m_textBox.setCommandListener(new TextboxListener());
      m_textBox.addCommand(c_send);
      m_textBox.addCommand(c_cancel);
      m_ui.setWinlock(true);
      m_ui.setDisplay(m_textBox);
    }
    else if (keyCode == 137) { // another BlackBerry
      this.commandAction(c_msg, null);
    }
    else if (keyCode == KEY_NUM2 || getGameAction(keyCode) == UP) {
      if (m_textArea.updatePosition(-1)) repaint();
    }
    else if (keyCode == KEY_NUM8 || getGameAction(keyCode) == DOWN) {
      if (m_textArea.updatePosition(1)) repaint();
    }
    else if (keyCode == KEY_NUM4 || getGameAction(keyCode) == LEFT) {
      m_ui.displayPreviousWindow();
    }
    else if (keyCode == KEY_NUM6 || getGameAction(keyCode) == RIGHT) {
      m_ui.displayNextWindow();
    }
    else if (keyCode == KEY_NUM1) {
      if (m_textArea.setPosition(0)) repaint();
    }
    else if (keyCode == KEY_NUM7) {
      if (m_textArea.setPosition(-1)) repaint();
    }
    else if (keyCode == KEY_NUM3) {
      if (m_textArea.setPosition(-2)) repaint();
    }
    else if (keyCode == KEY_NUM9) {
      if (m_textArea.setPosition(-3)) repaint();
    }
    else if (keyCode == KEY_NUM0) {
//      this.commandAction(cmd_favourites, null);
    }
    else if (keyCode == KEY_POUND) {
      m_ui.setHeader(!m_showHeader);
      repaint();
    }
    else if (getGameAction(keyCode) == FIRE) {
      this.commandAction(c_msg, null);
    }
  }


  /**
   *
   * @param keyCode
   */
  protected void keyReleased(int keyCode) {
    if (!m_ui.keylock && keyCode == KEY_STAR) {
      this.commandAction(c_msg, null);
    }
  }


  /**
   *
   * @param keyCode
   */
  protected void keyRepeated(int keyCode) {
    keyPressed(keyCode);
  }


  /**
   * @return Возвращает имя.
   */
  public String name() {
    return m_name;
  }


  /**
   * Обработка разрыва соединения.
   */
  public void offline() {
    m_users.removeAllElements();
    m_whois.clear();
  }


  /**
   * Отрисовка окна.
   * @param g
   */
  public void paint(Graphics g) {
    g.setColor(0xffffff);
    g.fillRect(0, 0, getWidth(), getHeight());

    if (m_showHeader) {
      int i;
      int headerHeight =  m_headerFont.getHeight();

      g.setColor(0x638093);
      g.fillRect(0, 0, getWidth(), headerHeight);

      // indicators drawn here
      int[] wins = m_ui.indicators();
      for (i = 0; i < wins.length; i++) {
        switch (wins[i]) {
          case STATE_NONE:
            g.setColor(0xffffff);
            break;
          case STATE_INFO:
            g.setColor(0xaaaaaa);
            break;
          case STATE_MSG:
            g.setColor(0xaa00aa);
            break;
          case STATE_HILIGHT:
            g.setColor(0xaa0000);
            break;
          case STATE_SELECTED:
            g.setColor(0x000000);
            break;
        }
        g.fillRect(3 + (i * 5), 1, 3, headerHeight - 4);
      }

      // draw m_header text
      g.setFont(m_headerFont);
      g.setColor(0x000000);

      String chantext = m_name;
      String onlineCount = "";
      if (m_users.size() > 0) {
        onlineCount = " (" + m_users.size() + ")";
      }

      if (m_headerFont.stringWidth(m_name + onlineCount) >= getWidth() - 5 - i * 5) {
        // not enough space, we need to cut
        int textwidth = getWidth() - 5 - i * 5 - m_headerFont.stringWidth(onlineCount);
        textwidth -= m_headerFont.stringWidth(".." + m_name.substring(m_name.length() - 2));
        for (i = m_name.length() - 3; i >= 0 && m_headerFont.substringWidth(m_name, 0, i) > textwidth; i--) {}
        chantext = m_name.substring(0, i) + ".." + m_name.substring(m_name.length() - 2);
      }
      g.drawString(onlineCount, getWidth() - 2, 0, Graphics.RIGHT | Graphics.TOP);
      g.drawString(chantext, getWidth() - 2 - m_headerFont.stringWidth(onlineCount), 0, Graphics.RIGHT | Graphics.TOP);

      if (m_ui.keylock) {
        int keyx = 3, keyy = 5;

        g.setColor(0, 0, 0);
        g.drawLine(keyx, keyy + 1, keyx, keyy + 3);
        g.drawLine(keyx + 2, keyy + 1, keyx + 2, keyy + 3);
        g.drawLine(keyx + 4, keyy + 1, keyx + 4, keyy + 2);
        g.drawLine(keyx + 6, keyy + 1, keyx + 6, keyy + 2);
        g.drawLine(keyx, keyy + 1, keyx + 3, keyy + 1);
        g.drawLine(keyx + 5, keyy, keyx + 5, keyy);
        g.drawLine(keyx + 5, keyy + 3, keyx + 5, keyy + 3);
      }
    }
    m_textArea.draw(g);
  }


  /**
   * Установка текста заголовка.
   * @param header
   */
  public void setHeader(String header) {
    m_name = header;
    repaint();
  }


  /**
   * Управление видимостью заголовка.
   * @param visible
   */
  public void setHeaderVisible(boolean visible) {
    if (visible) {
      int hHeight = m_headerFont.getHeight();
      m_textArea.setSize(hHeight + 1, getHeight() - hHeight);
    } else {
      m_textArea.setSize(0, getHeight());
    }

    m_showHeader = visible;
  }


  public void setState(int newstate) {
    m_state = newstate;
  }


  /**
   * Показ окна.
   */
  private void show() {
    m_ui.setDisplay(this);
  }


  /**
   * @return Состояние окна.
   */
  public int state() {
    return m_state;
  }


  /**
   * Возвращает строку содержащию время.
   * @return
   */
  private String time() {
    Calendar calendar = Calendar.getInstance();
    return (calendar.get(Calendar.HOUR_OF_DAY) < 10 ? "0" : "") + calendar.get(Calendar.HOUR_OF_DAY) + ":"
            + (calendar.get(Calendar.MINUTE) < 10 ? "0" : "") + calendar.get(Calendar.MINUTE) + ":"
            + (calendar.get(Calendar.SECOND) < 10 ? "0" : "") + calendar.get(Calendar.SECOND) + " | ";
  }


  /**
   * Alert Счётчик трафика.
   */
  private void trafficAlert() {
    String str = "Получено байт: " + Simple.client().rx() + "\n";
    str += "Отправлено байт: " + Simple.client().tx() + "\n";
    str += "Всего: " + (Simple.client().rx() + Simple.client().tx()) ;
    Alert alert = new Alert("Счётчик трафика", str, null, AlertType.INFO);
    alert.setTimeout(Alert.FOREVER);
    m_ui.setDisplay(alert);
  }


  /**
   * @return Тип окна.
   */
  public int type() {
    return m_type;
  }


  /**
   * Перерисовка заголовка.
   */
  private void updateHeader() {
    if (m_showHeader) {
      repaint(0, 0, getWidth(), m_headerFont.getHeight());
    }
  }


  /**
   * Возвращает объект пользователя.
   * @param nick
   * @return null в случае если пользователь не найден.
   */
  public User user(String nick) {
    return (User) m_whois.get(nick);
  }


  /**
   *
   * @param command
   * @param displayable
   */
  private void usersList() {
    m_usersList = new List("Список", List.IMPLICIT);
    int mp = (m_personPosition + 1) * MAX_LIST_PERSONS;

    if (m_personPosition > 0) {
      m_usersList.append("[Предыдущие]", null);
    }

    if (m_users.size() > mp) {
      m_usersList.append("[Следующие]", null);
    }

    for (int i = (m_personPosition * MAX_LIST_PERSONS); i < mp && i < m_users.size(); i++) {
      String n = (String) m_users.elementAt(i);
      m_usersList.append(n, null);
    }

    m_usersList.addCommand(c_closeUsers);
    m_usersList.setCommandListener(this);
    m_ui.setDisplay(m_usersList);
  }


  /**
   * Alert Информация о пользователе.
   */
  private void whoisAlert(String nick) {
    User u = m_ui.mainChat().user(nick);

    StringBuffer str = new StringBuffer();
    if (u != null) {
      str.append("Ник: " + u.nick());

      if (u.fullName() != null)
        str.append("\nФИО: " + u.fullName());

      str.append("\nПол: " + (u.gender() == User.GENDER_MALE ? "Мужской" : "Женский"));
      str.append("\nАдрес: " + u.host() + "\nСтатус: " + u.statusDesc());
      str.append("\nКлиент: " + u.userAgent().replace('/', ' '));
    }
    else
      str.append("Статус: Не в сети\n");

    Alert alert = new Alert("Информация", str.toString(), null, AlertType.INFO);
    alert.setTimeout(Alert.FOREVER);
    m_ui.setDisplay(alert);
  }



  /**
   * Запись сообщения от пользователя.
   * @param nick
   * @param text
   */
  public void write(String nick, String text, boolean me) {
    if (m_ui.keylock)
      m_ui.playAlarm(false);

    if (me)
      write(new String[] {time(), nick, text.substring(3)}, new char[] {0xf7, 0x1f5, 0xf5});
    else
      write(new String[] {time(), nick + ": ", text}, new char[] {0xf7, 0x1f4, 0xf0});

    if (m_state < STATE_MSG)
      m_state = STATE_MSG;
  }


  /**
   * Базовая функция записи текста.
   * @param strings Массив строк составляющих текст.
   * @param colours Цвета массива строк.
   */
  public void write(String[] strings, char[] colours) {
    String[] rets;
    boolean end = m_textArea.isAtEndPos();

    if (m_useColors) {
      rets = new String[strings.length];
      for (int i = 0; i < strings.length; i++)
        rets[i] = colours[i] + strings[i];
    }
    else {
      rets = new String[1];
      StringBuffer combined = new StringBuffer();
      for (int i = 0; i < strings.length; i++)
        combined.append(strings[i]);
      rets[0] = ((char) 0xf0) + combined.toString();
    }

    m_textArea.addText(rets);
    if (end)
      m_textArea.setPosition(-1);

    m_ui.repaint();
  }


  /**
   * Запись информационного сообщения.
   * @param str
   */
  public void writeInfo(String str) {
    writeInfo(str, 0xf7);
  }


  /**
   * Запись информационного сообщения.
   * @param str
   * @param color
   */
  public void writeInfo(String str, int color) {
    write(new String[] {time(), str}, new char[] {0xf7, (char) color});
    if (m_state < STATE_INFO)
      m_state = STATE_INFO;
  }


  /**
   *
   */
  private class TextboxListener implements CommandListener {
    public void commandAction(Command command, Displayable displayable) {
      m_ui.setWinlock(false);
      if (m_textBox == null)
        return;

      String text = m_textBox.getString();

      // Отправка текста.
      if (command == c_send) {
        if (text.trim().equals(""))
          return;

        handleMsg(text);
        m_textBox = null;
        show();
      }
      // Отмена
      else if (command == c_cancel) {
        m_textBox = null;
        show();
      }
      else if (command == c_ok) {
        if (text.trim().equals(""))
          return;

        m_textBox = null;
      }
    }
  }
}
