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

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Random;
import java.util.Vector;
import javax.microedition.io.Connector;
import javax.microedition.io.SocketConnection;

/**
 * Класс обеспечивающий подключение к серверу.
 */
public class Connection extends Thread {
  private boolean m_accepted;            // true в случае успешного рукопожатия.
  private boolean m_fatal;               // true если произошла критическая ошибка при подключении.
  private boolean m_state;               // Connection state, true - connected, false - disconnected.
  private DataInputStream m_in;          // input stream.
  private DataOutputStream m_out;        // output stream.
  private int m_port;                    // Порт сервера.
  private long m_rx;                     // Счётчик полученных (receive) байт.
  private long m_tx;                     // Счётчик отправленных (transmit) байт.
  private User m_user;                   // Информация о пользователе.
  private SocketConnection m_connection; // Соединение.
  private String m_host;                 // Адрес сервера.

  private final Object m_transmitLock = new Object();
  private final Vector m_receiveQueue = new Vector(); // Буфер входящих пакетов.


  /**
   * Конструктор класса SimpleClient.
   * @param user
   * @param host
   * @param port
   */
  public Connection(User user, String host, int port) {
    m_user = user;
    m_host = host;
    m_port = port;
    m_state = false;
    m_rx = 0;
    m_tx = 0;
    m_accepted = false;
    m_fatal = false;
  }


  /**
   * Обработка отказа в подключении.
   * @param reason
   */
  private void accessDenied(int reason) {
    System.out.println("accessDenied() " + reason);
    m_state = false;
    m_accepted = false;

    switch (reason) {
      case Protocol.ERROR_OLD_CLIENT_PROTOCOL:
      case Protocol.ERROR_OLD_SERVER_PROTOCOL:
      case Protocol.ERROR_BAD_GREETING_FLAG:
      case Protocol.ERROR_BAD_NICK:
      case Protocol.ERROR_BAD_USER_AGENT:
        enqueuePacket(new RawPacket(reason - 20000, new byte[] {0}));
        m_fatal = true;
        break;

      case Protocol.ERROR_NICK_ALREADY_USE:
        mangleNick();
        break;
    }
  }


  /**
   * Обработка успешного рукопожатия.
   * @param numeric
   */
  private void accessGranted(int numeric) {
    m_accepted = true;
  }


  /**
   * @return Число доступных для чтения пакетов.
   */
  public synchronized int available() {
    return m_receiveQueue.size();
  }


  /**
   * Закрытие соединения.
   */
  public synchronized void close() {
    try { m_in.close(); } catch (Exception e) {}
    m_in = null;

    try { m_out.close(); } catch (Exception e) {}
    m_out = null;

    try { m_connection.close(); } catch (Exception e) {}
    m_connection = null;

    m_state = false;
    m_accepted = false;

    enqueuePacket(new RawPacket(Protocol.VCODE_DISCONNECTED, new byte[] {0}));
  }


  /**
   * Подключение к серверу.
   */
  private boolean connect() {
    m_state = false;
    m_fatal = false;

    enqueuePacket(new RawPacket(Protocol.VCODE_CONNECTING, new byte[] {0}));

    try {
      m_connection = (SocketConnection) Connector.open("socket://" + m_host + ":" + m_port, Connector.READ_WRITE, true);
      m_in = m_connection.openDataInputStream();
      m_out = m_connection.openDataOutputStream();

      m_state = true;

    } catch (Exception e) {
      //#ifdef SCHAT_DEBUG
      System.out.println("Error trying to connect to server, aborting...");
      System.out.println("Exception: " + e.getMessage());
      e.printStackTrace();
      //#endif
    }
    finally {
      if (!m_state) close();
    }

    return m_state;
  }


  /**
   * Запускает подключение к серверу.
   * Попытка соединения осуществляется каждые 6 секунд.
   */
  private void connectLoop() {
    do {
      if (m_fatal)
        break;

      if (!connect()) {
        try {
          sleep(6 * 1000);
        } catch (Exception e) {
        }
      }
    } while (!m_state);
  }


  /**
   * Добавление пакета в очередь.
   * @param packet
   */
  private void enqueuePacket(RawPacket packet) {
    synchronized (m_receiveQueue) {
      m_receiveQueue.addElement(packet);
    }

    synchronized (SimpleClient.waitObj()) {
      SimpleClient.waitObj().notify();
    }
  }


  /**
   * Создаёт новый ник для устранения коллизии.
   */
  private void mangleNick() {
    int max = 99;
    String nick = Simple.storage().nick;
    if (nick.length() == User.MAX_NICK_LENGTH)
      nick = nick.substring(0, User.MAX_NICK_LENGTH - 2);
    else if (nick.length() == User.MAX_NICK_LENGTH - 1)
      max = 9;

    setNick(nick + new Random().nextInt(max));
  }


  /**
   * @return Возвращает ник.
   */
  public synchronized String nick() {
    return m_user.nick();
  }


  /**
   * @return Возвращает следующий пакет или null если больше нет пакетов.
   */
  public RawPacket packet() {
    RawPacket packet;

    synchronized (m_receiveQueue) {
      if (m_receiveQueue.size() == 0) {
        return null;
      }
      packet = (RawPacket) this.m_receiveQueue.elementAt(0);
      this.m_receiveQueue.removeElementAt(0);
    }

    return packet;
  }


  /**
   * Цикл чтения данных.
   */
  private void read() {
    int size;
    int opcode;
    m_accepted = false;

    synchronized (m_receiveQueue) {
      m_receiveQueue.removeAllElements();
    }

    try {
      while (m_state) {
        size = m_in.readUnsignedShort();
        opcode = m_in.readUnsignedShort();
        m_rx += size + 2;

        if (m_accepted) {
          if (size > 2) {
            byte[] body = new byte[size - 2];
            m_in.readFully(body);
            enqueuePacket(new RawPacket(opcode, body));
          }
          else if (size == 2) {
            switch (opcode) {
              case Protocol.OPCODE_PING:
                sendPacket(new RawPacket(Protocol.OPCODE_PONG, null));
                break;

              case Protocol.OPCODE_SYNC_USERS_END:
                enqueuePacket(new RawPacket(opcode, new byte[] {0}));
                break;
            }
          }
        }
        else {
          if (opcode == Protocol.OPCODE_ACCESS_GRANDED)
            accessGranted(m_in.readUnsignedShort());
          else if (opcode == Protocol.OPCODE_ACCESS_DENIED)
            accessDenied(m_in.readUnsignedShort());
          else
            break;
        }
      }
    } catch (Exception e) {
//      e.printStackTrace();
    }

    close();
  }


  /**
   * Основной цикл потока.
   */
  public void run() {
    System.out.println("Connection runned");

    do {
      connectLoop();
      if (m_state) {
        sendPacket(Protocol.greetingPacket(m_user));
        read();
      }

      if (!m_fatal) {
        try {
          sleep(1000);
        } catch (Exception e) {
        }
      }
    } while (!m_fatal);

    System.out.println("Connection thread loop end");
  }


  /**
   * @return Число полученных байт.
   */
  public synchronized long rx() {
    return m_rx;
  }


  /**
   * Отправка сырого пакета.
   * @param packet
   * @return false если при отправке произошла ошибка.
   */
  public boolean sendPacket(RawPacket packet) {
    if (packet == null || m_out == null)
      return false;

    synchronized (m_transmitLock) {
      try {
        byte[] data = packet.toByteArray();
        m_out.write(data);
        m_out.flush();
        m_tx += data.length;

      } catch (IOException e) {
        close();
        return false;
      }
    }

    return true;
  }


  /**
   * Установка критической ошибки.
   * @param fatal
   */
  public synchronized void setFatal(boolean fatal) {
    m_fatal = fatal;
  }


  /**
   * Устанавливает ник.
   * @param nick
   */
  public synchronized void setNick(String nick) {
    m_user.setNick(nick);

    if (m_accepted)
      sendPacket(Protocol.newProfilePacket(m_user));
  }


  /**
   * Connection state.
   * @return true - connected, false - disconnected.
   */
  public synchronized boolean state() {
    return m_state;
  }


  /**
   * @return Число отправленных байт.
   */
  public synchronized long tx() {
    return m_tx;
  }
}
