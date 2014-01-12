/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2014 IMPOMEZIA <schat@impomezia.com>
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

#include <QFileInfo>
#include <QMimeData>

#include "sglobal.h"
#include "ShareDnD.h"

#define MAX_SIZE 10485760 /* 10 MB */
#define MAX_IMAGES 20

QList<QUrl> ShareDnD::getFiles(const QMimeData *data)
{
  const QList<QUrl> urls = data->urls();
  QList<QUrl> out;
  qint64 size = 0;
  int count   = 0;

  foreach (const QUrl &url, urls) {
    if (count == MAX_IMAGES)
      break;

    if (url.scheme() == LS("file")) {
      const QFileInfo fi(url.toLocalFile());
      if (!fi.exists() || !fi.isFile())
        continue;

      size += fi.size();
      if (size > MAX_SIZE)
        continue;

      const QString suffix = fi.suffix().toLower();
      if (suffix == LS("png") || suffix == LS("gif") || suffix == LS("jpg") || suffix == LS("jpeg")) {
        out.append(url);
        count++;
      }
    }
  }

  return out;
}


QList<QUrl> ShareDnD::getUrls(const QMimeData *data)
{
  const QList<QUrl> urls = data->urls();
  QList<QUrl> out;
  int count = 0;

  foreach (const QUrl &url, urls) {
    if (count == MAX_IMAGES)
      break;

    if (addUrl(out, url))
      count++;
  }

  if (out.isEmpty() && addUrl(out, data->text()))
    count++;

  return out;
}


bool ShareDnD::addUrl(QList<QUrl> &urls, const QUrl &url)
{
  if (!url.isEmpty() && (url.scheme() == LS("http") || url.scheme() == LS("https"))) {
    urls.append(url);
    return true;
  }

  return false;
}
