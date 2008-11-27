/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

!ifndef RUSSIAN_NSH_
!define RUSSIAN_NSH_

!insertmacro MUI_UNSET CURRENT_LANG
!define CURRENT_LANG "L_RU"

${L} STR100                 "Ярлыки:"
${L} STR101                 "Рабочий стол"
${L} STR102                 "Быстрый запуск"
${L} STR103                 "Меню $\"Все программы$\""
${L} STR104                 "Добавить в автозагрузку"
${L} STR105                 "Добавить сервер в автозагрузку"
${L} STR200                 "Выберите дополнительные задачи"
${L} STR201                 "Какие дополнительные задачи необходимо выполнить?"
${L} STR300                 "Удаление программы"
${L} STR301                 "Управление сервером"
${L} STR400                 "Обнаружена работающая копия ${SCHAT_NAME}. Завершите работу ${SCHAT_NAME} и попробуйте снова."
${L} STR1004                "Темы смайликов"
${L} sec_Core               "Основные компоненты"
${L} desc_Core              "Основные компоненты ${SCHAT_NAME}"
${L} sec_Qt                 "Qt Open Source Edition ${SCHAT_QT_VERSION}"
${L} desc_Qt                "Библиотека Qt Open Source Edition ${SCHAT_QT_VERSION}"
${L} sec_Emoticons.Kolobok  "Kolobok"
${L} desc_Emoticons.Kolobok "Смайлики Kolobok от Aiwan http://kolobok.us/"
${L} sec_Emoticons.Simple   "Simple Smileys"
${L} desc_Emoticons.Simple  "Смайлики Simple Smileys от Leo Bolin http://leobolin.net/simplesmileys/"
${L} sec_Daemon             "Cервер"
${L} desc_Daemon            "Cервер чата и программа для управления"
${L} sec_Customize          "${SCHAT_NAME_SHORT} Customize"
${L} desc_Customize         "${SCHAT_NAME} Customize"

!endif /* RUSSIAN_NSH_ */
