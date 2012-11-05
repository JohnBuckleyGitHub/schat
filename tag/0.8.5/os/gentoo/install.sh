#!/bin/bash
# $Id$
# IMPOMEZIA Simple Chat
# Copyright (c) 2008-2011 IMPOMEZIA <schat@impomezia.com>
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program. If not, see <http://www.gnu.org/licenses/>.

mkdir /var/run/schatd
mkdir /var/log/schatd
mkdir /var/lib/schatd

if ! getent passwd | grep -q "^schatd:"; then
  useradd --system --home-dir /var/run/schatd --no-create-home --user-group --shell /bin/false schatd || true
fi

chown schatd:schatd /var/run/schatd
chown schatd:schatd /var/log/schatd
chown schatd:schatd /var/lib/schatd

cp etc/conf.d/schatd /etc/conf.d/
cp etc/init.d/schatd /etc/init.d/
