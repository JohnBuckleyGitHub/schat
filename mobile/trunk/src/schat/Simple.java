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

import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.TextField;
import javax.microedition.midlet.MIDlet;

/**
 * @author IMPOMEZIA
 */
public class Simple extends MIDlet implements CommandListener {

  public final static String VERSION = "0.1.0.1277";
  private final static int FORM_MAIN = 0;
  private final static int FORM_CONFIG = 1;
  private final static int FORM_ABOUT = 2;

  private boolean m_running;
  private ChoiceGroup m_fontSize;
  private ChoiceGroup m_gender;
  private ChoiceGroup m_notice;
  private Command c_about;
  private Command c_cancel;
  private Command c_config;
  private Command c_connect;
  private Command c_exit;
  private Command c_ok;
  private Display m_display;
  private int m_currentForm;
  private static SimpleClient m_client;
  private static SimpleUI m_ui;
  private static Storage m_storage;
  private TextField m_byeMsg;
  private TextField m_fullName;
  private TextField m_host;
  private TextField m_nick;
  protected static Form m_mainForm;


  /**
   * Конструктор мидлета.
   */
  public Simple() {
    m_display = Display.getDisplay(this);
    c_connect = new Command("Подключится", Command.OK, 1);
    c_config = new Command("Настройка", Command.SCREEN, 2);
    c_about = new Command("О Программе", Command.SCREEN, 3);
    c_exit = new Command("Выход", Command.EXIT, 6);

    c_ok = new Command("Ок", Command.OK, 1);
    c_cancel = new Command("Отмена", Command.EXIT, 10);

    m_mainForm = new Form("Simple Chat");
    m_mainForm.addCommand(c_connect);
    m_mainForm.addCommand(c_config);
    m_mainForm.addCommand(c_about);
    m_mainForm.addCommand(c_exit);
    m_mainForm.setCommandListener(this);

    m_storage = new Storage();
    m_running = false;
  }


  /**
   * @return Возвращает объект SimpleClient.
   */
  public static SimpleClient client() {
    return m_client;
  }


  /**
   *
   * @param command
   * @param displayable
   */
  public void commandAction(Command command, Displayable displayable) {
    System.out.println("commandAction");
    if (command == c_exit) {
      exitChat();
    }
    else if (command == c_connect) {
      connect();
    }
    else if (command == c_config) {
      m_fontSize = new ChoiceGroup("Размер шрифта", ChoiceGroup.EXCLUSIVE);
      m_fontSize.append("Маленький", null);
      m_fontSize.append("Средний", null);
      m_fontSize.append("Большой", null);
      m_fontSize.setSelectedIndex(m_storage.fontSize, true);

      m_notice = new ChoiceGroup("Уведомления", ChoiceGroup.MULTIPLE);
      m_notice.append("Вибрация", null);
      m_notice.setSelectedIndex(0, m_storage.vibrate);

      Form configForm = new Form("Настройка");
      configForm.append(m_fontSize);
      configForm.append(m_notice);
      configForm.addCommand(c_ok);
      configForm.addCommand(c_cancel);

      m_currentForm = FORM_CONFIG;
      configForm.setCommandListener(this);
      m_display.setCurrent(configForm);
    }
    else if (command == c_about) {
      Form aboutForm = new Form("О Программе");
      aboutForm.append("Simple Chat Mobile " + VERSION);
      aboutForm.append("Copyright © 2008-2010 IMPOMEZIA");
      aboutForm.append("http://impomezia.ru\nЛицензия: GNU GPL v3\n\n");
      aboutForm.append("Интерфейс базируется на коде jmIrc <http://jmirc.sf.net>");

      aboutForm.addCommand(c_ok);

      m_currentForm = FORM_ABOUT;
      aboutForm.setCommandListener(this);
      m_display.setCurrent(aboutForm);
    }
    else if ((command == c_ok || command == c_cancel) && m_currentForm != FORM_MAIN) {
      if (command == c_ok && m_currentForm == FORM_CONFIG) {
        m_storage.fontSize = m_fontSize.getSelectedIndex();
        m_storage.vibrate = m_notice.isSelected(0);
      }
      m_currentForm = FORM_MAIN;
      m_display.setCurrent(m_mainForm);
    }

  }


  /**
   * Подключение к серверу.
   */
  private void connect() {
    m_ui = new SimpleUI(m_storage, m_display);
    m_ui.setDisplay(m_ui.mainChat());

    User user = new User(m_nick.getString(), m_fullName.getString(), m_byeMsg.getString(), m_gender.getSelectedIndex(), User.STATUS_NORMAL);
    m_storage.nick = user.nick();
    m_storage.fullName = user.fullName();
    m_storage.gender = user.gender();
    m_storage.host = m_host.getString();
    m_storage.byeMsg = user.byeMsg();
    m_storage.save();

    m_client = new SimpleClient(user, m_storage.host);
    m_client.start();

    System.gc();
  }


  /**
   *
   * @param unconditional
   */
  public void destroyApp(boolean unconditional) {
    if (m_client != null)
      m_client.exit();
  }


  /**
   * Выход из чата.
   */
  private void exitChat() {
    destroyApp(true);
    notifyDestroyed();
  }


  /**
   *
   */
  public void pauseApp() {
  }


  /**
   *
   */
  public void startApp() {
    if (!m_running) {
      m_storage.load();
      updateMainForm();
      m_display.setCurrent(m_mainForm);
      m_currentForm = FORM_MAIN;
      m_running = true;
    }
  }


  /**
   * @return Возвращает объект хранилища.
   */
  public static Storage storage() {
    return m_storage;
  }


  /**
   * @return Возвращает объект SimpleUI.
   */
  public static SimpleUI ui() {
    return m_ui;
  }


  /**
   * Создаёт главную форму.
   */
  private void updateMainForm() {
    m_nick = new TextField("Ник:", m_storage.nick, User.MAX_NICK_LENGTH, TextField.ANY);
    m_fullName = new TextField("ФИО:", m_storage.fullName, User.MAX_NAME_LENGTH, TextField.ANY);

    m_gender = new ChoiceGroup("Пол:", ChoiceGroup.POPUP);
    m_gender.append("Мужской", null);
    m_gender.append("Женский", null);
    m_gender.setSelectedIndex(m_storage.gender, true);

    m_host = new TextField("Адрес сервера:", m_storage.host, 128, TextField.ANY);
    m_byeMsg = new TextField("Сообщение при выходе:", m_storage.byeMsg, User.MAX_BYE_MSG_LENGTH, TextField.ANY);

    m_mainForm.append(m_nick);
    m_mainForm.append(m_fullName);
    m_mainForm.append(m_gender);
    m_mainForm.append(m_host);
    m_mainForm.append(m_byeMsg);
  }
}
