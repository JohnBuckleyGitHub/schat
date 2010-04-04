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

/**
 * Инкапсулирует информацию о пользователе.
 */
public class User {

  public static final int MAX_NICK_LENGTH    = 24;
  public static final int MAX_NAME_LENGTH    = 130;
  public static final int MAX_BYE_MSG_LENGTH = 130;

  public static final int STATUS_NORMAL     = 0;
  public static final int STATUS_AWAY       = 1;
  public static final int STATUS_AUTO_AWAY  = 2;
  public static final int STATUS_DND        = 3;
  public static final int STATUS_OFFLINE    = 32768;

  public static final int GENDER_MALE       = 0;
  public static final int GENDER_FEMALE     = 1;

  private boolean m_valid;
  private int m_gender;
  private int m_status;
  private String m_byeMsg;
  private String m_fullName;
  private String m_host;
  private String m_nick;
  private String m_userAgent;


  /**
   * Базовый конструктор
   */
  public User() {
    this(null, null, null, GENDER_MALE, STATUS_NORMAL);
  }


  /**
   * Конструктор создающий нового пользователя.
   * @param nick
   * @param fullName
   * @param byeMsg
   * @param gender
   * @param status
   */
  public User(String nick, String fullName, String byeMsg, int gender, int status) {
    this(nick, fullName, byeMsg, gender, status, defaultAgent(), null);
  }


  /**
   * Констуктор инициализирующий все внутрении данные.
   * @param nick
   * @param fullName
   * @param byeMsg
   * @param gender
   * @param status
   * @param userAgent
   * @param host
   */
  public User(String nick, String fullName, String byeMsg, int gender, int status, String userAgent, String host) {
    m_valid = true;
    setNick(nick);
    setFullName(fullName);
    setByeMsg(byeMsg);
    setGender(gender);
    setStatus(status);
    setUserAgent(userAgent);
    m_host = host;
  }


  /**
   * Созадение пользователя из потока.
   * @param dis
   */
  public User(DataInputStream dis) {
    m_valid = true;

    try {
      setGender(dis.read());
      setNick(Protocol.readString(dis));
      setFullName(Protocol.readString(dis));
      setByeMsg(Protocol.readString(dis));
      setUserAgent(Protocol.readString(dis));
      m_host = Protocol.readString(dis);
    } catch (Exception e) {
      m_valid = false;
    }
  }


  /**
   * @return Сообщение о выходе.
   */
  public String byeMsg() {
    return m_byeMsg;
  }


  /**
   * @return Агент по умолчанию.
   */
  public static String defaultAgent() {
    return "Simple Chat Mobile/" + Simple.VERSION;
  }


  /**
   * @return ФИО.
   */
  public String fullName() {
    return m_fullName;
  }


  /**
   * @return Пол.
   */
  public int gender() {
    return m_gender;
  }


  /**
   * @return Адрес.
   */
  public String host() {
    return m_host;
  }


  /**
   * @return true в случае корректности данных профиля.
   */
  public boolean isValid() {
    return m_valid;
  }


  /**
   * @return Ник.
   */
  public String nick() {
    return m_nick;
  }


  /**
   * @param byeMsg Сообщение о выходе.
   */
  public void setByeMsg(String byeMsg) {
    m_byeMsg = Utils.simplified(byeMsg, MAX_BYE_MSG_LENGTH);
  }


  /**
   * @param fullName ФИО.
   */
  public void setFullName(String fullName) {
    m_fullName = Utils.simplified(fullName, MAX_NAME_LENGTH);
  }


  /**
   * @param gender Пол.
   */
  public void setGender(int gender) {
    m_gender = gender;
  }


  /**
   * @param nick Ник.
   */
  public void setNick(String nick) {
    m_nick = Utils.simplified(nick, MAX_NICK_LENGTH);
  }


  /**
   * @param status Статус.
   */
  public void setStatus(int status) {
    m_status = status;
  }


  /**
   * @param userAgent Агент.
   */
  public void setUserAgent(String userAgent) {
    m_userAgent = Utils.simplified(userAgent, 0);
  }


  /**
   * @return Статус.
   */
  public int status() {
    return m_status;
  }


  /**
   * @return Текстовое представление статуса.
   */
  public String statusDesc() {
    if (m_status == STATUS_AWAY || m_status == STATUS_AUTO_AWAY)
      return "Отсутствую";

    if (m_status == STATUS_DND)
      return "Не беспокоить";

    return "В сети";
  }


  /**
   * @return Агент.
   */
  public String userAgent() {
    return m_userAgent;
  }
}
