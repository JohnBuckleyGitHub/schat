/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#ifndef CHATICONS_H_
#define CHATICONS_H_

#include <QIcon>

#include "Channel.h"

#define SCHAT_ICON(x) ChatIcons::icon(ChatIcons::x)

class SCHAT_CORE_EXPORT ChatIcons
{
  ChatIcons() {}

public:
  /// Иконки.
  enum IconName {
    Channel            = 0x4368, ///< "Ch"
    ChannelAlert       = 0x4341, ///< "CA"
    Gear               = 0x4765, ///< "Ge"
    MainTabMenu        = 0x4D54, ///< "MT"
    NetworkError       = 0x4E45, ///< "NE"
    NetworkOffline     = 0x4E6F, ///< "No"
    NetworkOnline      = 0x4E4F, ///< "NO"
    Connect            = 0x436F, ///< "Co"
    Disconnect         = 0x4469, ///< "Di"
    Quit               = 0x5175, ///< "Qu"
    Secure             = 0x5365, ///< "Se"
    Settings           = 0x5374, ///< "St"
    Sound              = 0x536F, ///< "So"
    SoundMute          = 0x534D, ///< "SM"
    Users              = 0x5573, ///< "Us"
    SmallLogo          = 0x534C, ///< "SL"
    SmallLogoNY        = 0x4C4E, ///< "LN"
    TextBold           = 0x5442, ///< "TB"
    TextItalic         = 0x5449, ///< "TI"
    TextStrike         = 0x5453, ///< "TS"
    TextUnderline      = 0x5455, ///< "TU"
    Send               = 0x5364, ///< "Sd"
    Globe              = 0x476C, ///< "Gl"
    Add                = 0x4164, ///< "Ad"
    Remove             = 0x5265, ///< "Re"
    Profile            = 0x5072, ///< "Pr"
    OK                 = 0x4F4B, ///< "OK"
    InfoBalloon        = 0x4942, ///< "IB"
    Information        = 0x4932, ///< "I2"
    EditClear          = 0x436C, ///< "Cl"
    EditCopy           = 0x4543, ///< "EC"
    EditCut            = 0x4563, ///< "Ec"
    EditPaste          = 0x4570, ///< "Ep"
    EditSelectAll      = 0x4553, ///< "ES"
    Balloon            = 0x4261, ///< "Ba"
    Slash              = 0x536C, ///< "Sl"
    TopicEdit          = 0x5445, ///< "TE"
    Key                = 0x4B65, ///< "Ke"
    Exclamation        = 0x4578, ///< "Ex"
    ExclamationRed     = 0x4552, ///< "ER"
    ArrowRight         = 0x4152, ///< "AR"
    SignIn             = 0x5349, ///< "SI"
    SignOut            = 0x534F, ///< "SO"
    SignUp             = 0x5355, ///< "SU"
    Password           = 0x5073, ///< "Ps"
    Locale             = 0x4C6F, ///< "Lo"
    Computer           = 0x4370, ///< "Cp"
    Reload             = 0x526C  ///< "Rl"
  };

  /// Опции получения иконки канала.
  enum ChannelIcon {
    NoOptions = 0,    ///< Нет специальных опций
    Statuses = 1,     ///< Отображать статусы, кроме статуса "не в сети".
    OfflineStatus = 2 ///< Также отображать статус "не в сети".
  };

  static QIcon icon(ClientChannel channel, int options = Statuses | OfflineStatus);
  static QIcon icon(const QIcon &icon, const QString &overlay);
  static QIcon icon(const QString &file, const QString &overlay);
  static QIcon icon(int name);
  static QString overlay(int status);
  static void init();

private:
  static QMap<int, QString> m_icons;
};

#endif /* CHATICONS_H_ */
