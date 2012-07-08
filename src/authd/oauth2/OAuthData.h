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

#ifndef OAUTHDATA_H_
#define OAUTHDATA_H_

#include <QString>
#include <QVariant>

class OAuthData
{
public:
  OAuthData(const QString &provider = QString());
  virtual ~OAuthData() {}
  inline const QByteArray id() const        { return m_id; }
  inline const QByteArray redirect() const  { return m_redirect; }
  inline const QByteArray secret() const    { return m_secret; }
  inline const QString provider() const     { return m_provider; }
  virtual bool isValid() const;
  virtual inline bool read()                { return isValid(); }
  virtual inline QByteArray toUrl() const   { return QByteArray(); }
  virtual QVariantMap toJSON() const;

protected:
  QByteArray m_id;       ///< Client ID.
  QByteArray m_redirect; ///< Redirect URI.
  QByteArray m_secret;   ///< Client secret.
  QString m_provider;    ///< Кодовое имя OAuth2 провайдера.
};

#endif /* OAUTHDATA_H_ */
