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
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;


/**
 *
 * @author IMPOMEZIA
 */
public class Storage {
  public boolean header = true;
  public boolean useColor = true;
  public boolean vibrate = true;
  public int bufLines = 100;
  public int fontSize = 1;
  public int gender = User.GENDER_MALE;
  public String byeMsg = "";
  public String fullName = "";
  public String host = "schat.impomezia.com";
  public String nick = "Ник";

  private final static String STORE_CONFIG = "schatcfgv1";

  /**
   * Конструктор.
   */
  public Storage() {
  }


  /**
   * Чтение настроек.
   */
  public void load() {
    RecordStore rs = null;

    try {
      rs = RecordStore.openRecordStore(STORE_CONFIG, false);

      loadProfile(rs);
      loadConfig(rs);

    } catch (Exception e) {
    } finally {
      if (rs != null) {
        try {
          rs.closeRecordStore();
        } catch (Exception e) {
        }
      }
    }
  }


  /**
   * Чтение насторек.
   * @param rs
   * @throws RecordStoreException
   * @throws IOException
   */
  private void loadConfig(RecordStore rs) throws RecordStoreException, IOException {
    byte[] buf = rs.getRecord(2);
    ByteArrayInputStream bais = new ByteArrayInputStream(buf);
    DataInputStream dis = new DataInputStream(bais);
    fontSize = dis.readShort();
    vibrate = dis.readBoolean();
  }


  /**
   * Чтение профиля.
   * @param rs
   * @throws RecordStoreException
   * @throws IOException
   */
  private void loadProfile(RecordStore rs) throws RecordStoreException, IOException {
    byte[] buf = rs.getRecord(1);
    ByteArrayInputStream bais = new ByteArrayInputStream(buf);
    DataInputStream dis = new DataInputStream(bais);
    nick = dis.readUTF();
    fullName = dis.readUTF();
    gender = dis.readShort();
    host = dis.readUTF();
    byeMsg = dis.readUTF();
  }


  /**
   * Сохранение настроек.
   */
  public void save() {
    RecordStore rs = null;

    try {
      rs = RecordStore.openRecordStore(STORE_CONFIG, true);

      if (rs.getNumRecords() < 3) {
        rs.addRecord(null, 0, 0);
      }

      saveProfile(rs);
      saveConfig(rs);

    } catch (Exception e) {
      e.printStackTrace();
    } finally {
      if (rs != null) {
        try {
          rs.closeRecordStore();
        } catch (Exception e) {
        }
      }
    }
  }


  /**
   * Запись профиля.
   * @param rs
   * @throws RecordStoreException
   */
  private void saveConfig(RecordStore rs) throws RecordStoreException, IOException {
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    DataOutputStream dos = new DataOutputStream(baos);
    dos.writeShort(fontSize);
    dos.writeBoolean(vibrate);
    byte[] buf = baos.toByteArray();
    rs.setRecord(2, buf, 0, buf.length);
  }


  /**
   * Запись профиля.
   * @param rs
   * @throws RecordStoreException
   */
  private void saveProfile(RecordStore rs) throws RecordStoreException, IOException {
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    DataOutputStream dos = new DataOutputStream(baos);
    dos.writeUTF(nick);
    dos.writeUTF(fullName);
    dos.writeShort(gender);
    dos.writeUTF(host);
    dos.writeUTF(byeMsg);
    byte[] buf = baos.toByteArray();
    rs.setRecord(1, buf, 0, buf.length);
  }
}
