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
import java.io.DataOutputStream;

/**
 * Базовый пакет.
 */
public class RawPacket {
  protected byte[] m_body; // Тело пакета.
  protected int m_opcode;  // Опкод пакета.


  /**
   * Создание пустого пакета.
   */
  public RawPacket() {
    this(0, null);
  }


  /**
   * Создание инициализированного пакета.
   * @param opcode
   * @param body
   */
  public RawPacket(int opcode, byte[] body) {
    m_opcode = opcode;
    m_body = body;
  }



  /**
   * @return Тело пакета.
   */
  public final byte[] body() {
    return m_body;
  }


  /**
   * @return Возвращает опкод пакета.
   */
  public final int opcode() {
    return m_opcode;
  }


  /**
   * @return Возвращает массив байт для отправки в сокет.
   */
  public byte[] toByteArray() {
    int size = (m_body != null ? m_body.length : 0) + 4;

    byte[] out = null;
    ByteArrayOutputStream os = new ByteArrayOutputStream(size);
    DataOutputStream dos = new DataOutputStream(os);

    try {
      dos.writeShort(size - 2);
      dos.writeShort(m_opcode);

      if (m_body != null)
        dos.write(m_body);

      out = os.toByteArray();
    } catch (Exception e) {
    }

    return out;
  }
}
