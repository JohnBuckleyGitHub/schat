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

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.EOFException;
import java.io.IOException;
import java.util.Date;

/**
 * Реализует основную функциональность клиента.
 */
public class SimpleClient extends Thread {
  public static final int STATE_UNCONNECTED = 0;
  public static final int STATE_CONNECTING  = 1;
  public static final int STATE_CONNECTED   = 2;
  public static long ping;

  private Connection m_connection;   // Соединение с сервером.
  private int m_port;
  private String m_host;
  private SimpleUI m_ui;
  private volatile boolean m_active; // true если поток активен.
  private volatile int m_state;

	private static final Object m_wait = new Object(); // Wait object
  private static User m_user;        // Информация о пользователе.


 /**
  * Конструктор класса SimpleClient.
  * @param user
  * @param host
  */
  public SimpleClient(User user, String host) {
    m_user = user;

    int index = host.lastIndexOf(':');
    if (index == -1) {
      m_host = host;
      m_port = 7666;
    }
    else {
      m_host = host.substring(0, index);
      try {
        m_port = Integer.parseInt(host.substring(index + 1));
      } catch (Exception e) {
        m_port = 7666;
      }
    }

    m_connection = new Connection(m_user, m_host, m_port);
    m_active = false;
    m_ui = Simple.ui();
    m_state = STATE_UNCONNECTED;
  }


  /**
   * Завершение работы клиента.
   */
  public void exit() {
    if (m_connection.state())
      m_connection.close();

    m_connection.setFatal(true);
    m_active = false;

    synchronized (m_wait) {
      m_wait.notifyAll();
    }
  }


  /**
   * Разбор поступивыших пакетов.
   * @param packet
   */
  private void parse(RawPacket packet) {
    //#ifdef SCHAT_DEBUG
    System.out.println("PARSE PACKET: " + packet.opcode());
    //#endif

    if (packet.body() != null) {
      ByteArrayInputStream is = new ByteArrayInputStream(packet.body());
      DataInputStream dis = new DataInputStream(is);
//      try {
//        sleep(500);
//      } catch (Exception e) {
//      }

      try {
        if (packet.opcode() > 0) {
          switch (packet.opcode()) {
            case Protocol.OPCODE_MESSAGE:
            case Protocol.OPCODE_PRIVATE_MESSAGE:
              parseMessage(packet.opcode(), dis);
              break;

            case Protocol.OPCODE_NEW_USER:
              parseNewUser(dis);
              break;

            case Protocol.OPCODE_UNIVERSAL:
              parseUniversal(dis);
              break;

            case Protocol.OPCODE_USER_LEAVE:
              parseUserLeave(dis);
              break;

            case Protocol.OPCODE_SYNC_USERS_END:
              parseSyncUsersEnd();
              break;

            case Protocol.OPCODE_NEW_PROFILE:
            case Protocol.OPCODE_NEW_NICK:
              parseNewProfile(packet.opcode(), dis);
              break;

            case Protocol.OPCODE_SERVER_MESSAGE:
              parseServerMessage(dis);
              break;

            case Protocol.OPCODE_NEW_LINK:
            case Protocol.OPCODE_LINK_LEAVE:
              parseLink(packet.opcode(), dis);
              break;
          }
        }
        else {
          vCode(packet);
        }
      } catch (Exception e) {
      }
    }
  }


  /**
   * Разбор пакета Protocol.OPCODE_NEW_LINK и OPCODE_LINK_LEAVE.
   * @param opcode
   * @param dis
   * @throws IOException
   * @throws EOFException
   */
  private void parseLink(int opcode, DataInputStream dis) throws IOException, EOFException {
    dis.read();
    Protocol.readString(dis);
    String server = Protocol.readString(dis);
    m_ui.mainChat().writeInfo("Сервер " + server + (opcode == Protocol.OPCODE_NEW_LINK ? " подключился" : " отключился"));
  }


  /**
   * Разбор пакета Protocol.OPCODE_MESSAGE и OPCODE_PRIVATE_MESSAGE.
   * @param opcode
   * @param dis
   * @throws IOException
   * @throws EOFException
   */
  private void parseMessage(int opcode, DataInputStream dis) throws IOException, EOFException {
    int flag = 0;
    if (opcode == Protocol.OPCODE_PRIVATE_MESSAGE)
      flag = dis.read();

    String nick = Protocol.readString(dis);
    String message = Utils.stripHTMLTags(Protocol.readString(dis));
    boolean me = false;

    if (message.startsWith("/") && message.length() > 4 && message.substring(0, 4).equals("/me "))
      me = true;

    if (opcode == Protocol.OPCODE_MESSAGE) {
      if (!m_connection.nick().equals(nick)) {
        m_ui.mainChat().write(nick, message, me);
        m_ui.vibrate();
      }
    } else {
      Window window = m_ui.privateWindow(nick, true);
      if (flag != 1) {
        window.write(nick, message, me);
        m_ui.vibrate();
      }
    }
  }


  /**
   * Разбор пакетов Protocol.OPCODE_NEW_PROFILE и Protocol.OPCODE_NEW_NICK.
   * @param opcode
   * @param dis
   * @throws IOException
   * @throws EOFException
   */
  private void parseNewProfile(int opcode, DataInputStream dis) throws IOException, EOFException {
    int gender = dis.read();
    String nick = Protocol.readString(dis);
    String newNick = null;
    if (opcode == Protocol.OPCODE_NEW_NICK)
      newNick = Protocol.readString(dis);
    String name = Protocol.readString(dis);

    User user = m_ui.mainChat().user(nick);
    if (user == null)
      return;

    user.setGender(gender);
    user.setFullName(name);

    if (newNick != null) {
      user.setNick(newNick);
      Window mainChat = m_ui.mainChat();
      mainChat.deleteUser(nick);
      mainChat.deleteUser(newNick);
      mainChat.addUser(user);
      String info = nick + " теперь: " + newNick;
      mainChat.writeInfo(info, 0xfc);

      Window oldWindow = m_ui.privateWindow(nick, false);
      Window newWindow = m_ui.privateWindow(newNick, false);

      if (oldWindow != null && newWindow == null) {
        oldWindow.setHeader(newNick);
        m_ui.renameWindow(nick, oldWindow);
      }

      if (oldWindow != null)
        oldWindow.writeInfo(info, 0xfc);

      if (newWindow != null)
        newWindow.writeInfo(info, 0xfc);
    }
  }


  /**
   * Разбор пакета Protocol.OPCODE_NEW_USER.
   * @param dis
   * @throws IOException
   * @throws EOFException
   */
  private void parseNewUser(DataInputStream dis) throws IOException, EOFException {
    int flag = dis.read();
    dis.read();

    User user = new User(dis);
    if (!user.isValid())
      throw new IOException();

    if (flag == 1 && !m_connection.nick().equals(user.nick())) {
      String info = user.nick() + (user.gender() == User.GENDER_MALE ? " зашёл" : " зашла");
      m_ui.mainChat().writeInfo(info);

      try {
        m_ui.privateWindow(user.nick(), false).writeInfo(info);
      } catch (NullPointerException e) {}
    }

    m_ui.mainChat().addUser(user);
  }


  /**
   * Разбор пакета Protocol.OPCODE_SERVER_MESSAGE.
   * @param dis
   * @throws IOException
   * @throws EOFException
   */
  private void parseServerMessage(DataInputStream dis) throws IOException, EOFException {
    String message = Utils.stripHTMLTags(Protocol.readString(dis));

    int color = 0xfc;
    if (message.length() == 5 && message.toLowerCase().equals("/pong")) {
      long latency = new Date().getTime() - ping;
      message = latency + " мс";

      if (latency > 200 && latency < 500) {
        color = 0xfb;
      } else if (latency >= 500) {
        color = 0xf9;
      }
      else {
        color = 0xf2;
      }
    }

    m_ui.mainChat().writeInfo(message, color);
  }


  /**
   * Разбор пакета Protocol.OPCODE_ACCESS_GRANDED.
   * Завершение синхрнизации списка пользователей
   * сигнализирует об успешном подключении.
   * @throws IOException
   * @throws EOFException
   */
  private void parseSyncUsersEnd() throws IOException, EOFException {
    m_ui.mainChat().setHeader(m_host);
    m_ui.mainChat().writeInfo("Подключились к " + m_host + ", ваш ник: " + m_connection.nick(), 0xf2);
    setState(STATE_CONNECTED);
  }


  /**
   * Разбор пакета Protocol.OPCODE_UNIVERSAL.
   * @param dis
   * @throws IOException
   * @throws EOFException
   */
  private void parseUniversal(DataInputStream dis) throws IOException, EOFException {
    dis.readUnsignedShort();
    int size1 = dis.readInt();
    int status = dis.readInt();
    int verbose = 0;
    if (size1 > 1)
      verbose = dis.readInt();

    int size2 = dis.readInt();
    String[] nicks = new String[size2];
    Window mainChat = m_ui.mainChat();

    try {
      for (int i = 0; i < size2; i++) {
        String nick = Protocol.readString(dis);
        nicks[i] = nick;
        User user = mainChat.user(nick);
        if (user != null) {
          user.setStatus(status);
        }
      }

      if (verbose > 0 && mainChat.user(nicks[0]) != null) {
        Window p = m_ui.privateWindow(nicks[0], false);
        if (p != null) {
          User user = mainChat.user(nicks[0]);
          p.writeInfo(user.nick() + (user.gender() == User.GENDER_MALE ? " сменил" : " сменила") + " статус на: " + user.statusDesc());
        }
      }
    } catch (IndexOutOfBoundsException e) {
    }
  }


  /**
   * Разбор пакета Protocol.OPCODE_USER_LEAVE.
   * @param dis
   * @throws IOException
   * @throws EOFException
   */
  private void parseUserLeave(DataInputStream dis) throws IOException, EOFException {
    int flag = dis.read();
    String nick = Protocol.readString(dis);
    String bye = Protocol.readString(dis);

    if (flag == 1) {
      User user = m_ui.mainChat().user(nick);
      if (user != null) {
        bye = bye != null ? ": " + bye : "";
        String info = user.nick() + (user.gender() == User.GENDER_MALE ? " вышел" : " вышла") + bye;
        m_ui.mainChat().writeInfo(info);

       try {
         m_ui.privateWindow(user.nick(), false).writeInfo(info);
       } catch (NullPointerException e) {}
      }
    }

    m_ui.mainChat().deleteUser(nick);

    if (Simple.storage().nick.toLowerCase().equals(nick.toLowerCase())) {
      m_connection.setNick(Simple.storage().nick);
    }
  }


  /**
   * Основной цикл потока.
   */
  public void run() {
    System.out.println("SimpleClient runned");

    m_active = true;
    m_connection.start();

    while (m_active) {

      System.out.println("SimpleClient iteration " + Thread.currentThread());

      try {
        synchronized (m_wait) {
          m_wait.wait();
        }
      } catch (InterruptedException e) {}

      // Разбор очереди пакетов.
      while (m_connection.available() > 0) {
        parse(m_connection.packet());
      }
    }

    try {
      m_connection.join();
    } catch (InterruptedException e) {}

    System.out.println("SimpleClient thread loop end");
  }


  /**
   * @return Число полученных байт.
   */
  public synchronized long rx() {
    return m_connection.rx();
  }


  /**
   * Отправка сырого пакета.
   * @param packet
   * @return false если при отправке произошла ошибка.
   */
  public boolean sendPacket(RawPacket packet) {
    return m_connection.sendPacket(packet);
  }


  /**
   * Устанавливает состояние.
   * @param state
   */
  public synchronized void setState(int state) {
    m_state = state;
  }


  /**
   * @return Возвращает состояние подключения.
   */
  public synchronized int state() {
    return m_state;
  }


  /**
   * @return Возвращает пользователя.
   */
  public static User user() {
    return m_user;
  }


  /**
   * Обработка виртуальных пакетов.
   * @param packet
   */
  private void vCode(RawPacket packet) {
    Window mainChat = m_ui.mainChat();

    if (packet.opcode() < -19000) {
      int reason = packet.opcode() + 20000;

      String info;
      switch (reason) {
        case Protocol.ERROR_BAD_NICK:
          info = "Недопустимый ник: " + m_connection.nick();
          break;

        case Protocol.ERROR_OLD_CLIENT_PROTOCOL:
        case Protocol.ERROR_OLD_SERVER_PROTOCOL:
          info = "Несовместимая версия протокола";
          break;

        default:
          info = "Фатальная ошибка: " + reason;
      }

      mainChat.writeInfo(info, 0xf9);
      mainChat.setHeader("Отключено");
      mainChat.offline();
      return;
    }

    switch (packet.opcode()) {
      case Protocol.VCODE_CONNECTING:
        System.out.println("VCODE_CONNECTING");
        setState(STATE_CONNECTING);
        mainChat.setHeader("Подключение...");
        break;

      case Protocol.VCODE_DISCONNECTED:
        System.out.println("VCODE_DISCONNECTED");
        if (state() == STATE_CONNECTED) {
          mainChat.setHeader("Отключено");
          mainChat.offline();
          mainChat.writeInfo("Соединение разорвано", 0xf9);
        }
        setState(STATE_UNCONNECTED);
        break;
    }
  }


  /**
   * @return Объект блокировки.
   */
  static public Object waitObj() {
    return m_wait;
  }


  /**
   * @return Число отправленных байт.
   */
  public synchronized long tx() {
    return m_connection.tx();
  }
}
