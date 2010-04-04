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

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.IOException;

public class Protocol {

  public static final int OPCODE_GREETING                 = 100;
  public static final int OPCODE_ACCESS_GRANDED           = 101;
  public static final int OPCODE_ACCESS_DENIED            = 9000;
  public static final int OPCODE_PING                     = 400;
  public static final int OPCODE_PONG                     = 401;
  public static final int OPCODE_MESSAGE                  = 200;
  public static final int OPCODE_PRIVATE_MESSAGE          = 201;
  public static final int OPCODE_SERVER_MESSAGE           = 202;
  public static final int OPCODE_NEW_USER                 = 310;
  public static final int OPCODE_USER_LEAVE               = 302;
  public static final int OPCODE_SYNC_USERS_END           = 1110;
  public static final int OPCODE_NEW_PROFILE              = 600;
  public static final int OPCODE_NEW_NICK                 = 602;
  public static final int OPCODE_UNIVERSAL                = 7666;
  public static final int OPCODE_NEW_LINK                 = 502;
  public static final int OPCODE_LINK_LEAVE               = 503;

  public static final int UNI_STATUS                      = 10;
  public static final int UNI_STATUS_LIST                 = 20;

  public static final int VCODE_CONNECTING                = -1;
  public static final int VCODE_DISCONNECTED              = -2;

  public static final int ERROR_NICK_ALREADY_USE          = 200; // Выбранный ник уже занят.
  public static final int ERROR_OLD_CLIENT_PROTOCOL       = 100; // Клиент использует устаревшую версию протокола.
  public static final int ERROR_OLD_SERVER_PROTOCOL       = 104; // Сервер использует устаревшую версию протокола.
  public static final int ERROR_BAD_GREETING_FLAG         = 101; // Клиент отправил неподдерживаемый флаг приветствия.
  public static final int ERROR_BAD_NICK                  = 102; // Выбранный клиентом ник, не допустим в чате.
  public static final int ERROR_BAD_USER_AGENT            = 103; // Клиент отправил недопустимый UserAgent.
  public static final int ERROR_USERS_LIMIT_EXCEEDED      = 500; // Превышено максимально допустимое количество пользователей на этом сервере.
  public static final int ERROR_MAX_USERS_PER_IP_EXCEEDED = 502; // Превышено максимальное число пользователей с этого ип адреса.

  public Protocol() {
  }


  /**
   * @param user
   * @return Пакет OPCODE_GREETING.
   */
  public static RawPacket greetingPacket(User user) {
    RawPacket out = null;
    ByteArrayOutputStream os = new ByteArrayOutputStream();
    DataOutputStream dos = new DataOutputStream(os);

    try {
      dos.writeShort(3);
      dos.writeByte(0);
      dos.writeByte(user.gender());
      writeString(dos, user.nick());
      writeString(dos, user.fullName());
      writeString(dos, User.defaultAgent());
      writeString(dos, user.byeMsg());
      out = new RawPacket(OPCODE_GREETING, os.toByteArray());
    } catch (Exception e) {
    }

    return out;
  }


  /**
   * @param user
   * @return Пакет OPCODE_NEW_PROFILE.
   */
  public static RawPacket newProfilePacket(User user) {
    RawPacket out = null;
    ByteArrayOutputStream os = new ByteArrayOutputStream();
    DataOutputStream dos = new DataOutputStream(os);

    try {
      dos.writeByte(user.gender());
      writeString(dos, user.nick());
      writeString(dos, user.fullName());
      out = new RawPacket(OPCODE_NEW_PROFILE, os.toByteArray());
    } catch (Exception e) {
    }

    return out;
  }


  /**
   * @param nick
   * @param message
   * @return Пакет OPCODE_MESSAGE.
   */
  public static RawPacket messagePacket(String nick, String message) {
    RawPacket out = null;
    ByteArrayOutputStream os = new ByteArrayOutputStream();
    DataOutputStream dos = new DataOutputStream(os);
    try {
      writeString(dos, nick);
      writeString(dos, Utils.escapeHTML(message));
      out = new RawPacket(OPCODE_MESSAGE, os.toByteArray());
    } catch (Exception e) {
    }

    return out;
  }


  /**
   * @param dis
   * @return Строка полученная при чтения потока.
   * @throws IOException
   */
  public static String readString(DataInputStream dis) throws IOException, EOFException {
    int size = dis.readInt();
    if (size == 0)
      return "";

    size /= 2;
    char[] data = new char[size];
    for (int i = 0; i < size; i++) {
      data[i] = dis.readChar();
    }

    return new String(data);
  }


  /**
   * @param status
   * @return Пакет OPCODE_UNIVERSAL.
   */
  public static RawPacket statusPacket(int status) {
    RawPacket out = null;
    ByteArrayOutputStream os = new ByteArrayOutputStream();
    DataOutputStream dos = new DataOutputStream(os);
    try {
      dos.writeShort(UNI_STATUS);
      dos.writeInt(1);
      dos.writeInt(status);
      dos.writeInt(0);
      out = new RawPacket(OPCODE_UNIVERSAL, os.toByteArray());
    } catch (Exception e) {
    }

    return out;
  }


  /**
   * Записывает строку в поток.
   * @param dos
   * @param text
   * @throws IOException
   */
  public static void writeString(DataOutputStream dos, String text) throws IOException {
    if (text == null || text.length() == 0) {
      dos.writeInt(0);
    }
    else {
      dos.writeInt(text.length() * 2);
      dos.writeChars(text);
    }
  }
}
