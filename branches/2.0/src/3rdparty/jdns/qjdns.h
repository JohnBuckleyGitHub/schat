/*
 * Copyright (C) 2005,2006  Justin Karneges
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

// this is the Qt wrapper to jdns.  it requires Qt 4.1+

#ifndef QJDNS_H
#define QJDNS_H

#include <QHostAddress>
#include <QObject>

#include "schat.h"

class QJDnsRecord
{
public:
  QByteArray owner;
  int ttl;
  int type;
  QByteArray rdata;
  bool haveKnown;

  // known
  QHostAddress address;    // for A, Aaaa
  QByteArray name;         // for Mx, Srv, Cname, Ptr, Ns
  int priority;            // for Mx, Srv
  int weight;              // for Srv
  int port;                // for Srv
  QList<QByteArray> texts; // for Txt
  QByteArray cpu;          // for Hinfo
  QByteArray os;           // for Hinfo

  QJDnsRecord();
  bool verify() const;
};


class QJDnsResponse
{
public:
  QList<QJDnsRecord> answerRecords;
  QList<QJDnsRecord> authorityRecords;
  QList<QJDnsRecord> additionalRecords;
};


class SCHAT_EXPORT QJDns : public QObject
{
	Q_OBJECT
public:
	enum Mode
	{
		Unicast,
		Multicast
	};

	enum PublishMode
	{
		Unique,
		Shared
	};

	enum Type
	{
		A       = 1,
		Aaaa    = 28,
		Mx      = 15,
		Srv     = 33,
		Cname   = 5,
		Ptr     = 12,
		Txt     = 16,
		Hinfo   = 13,
		Ns      = 2,
		Any     = 255
	};

	enum Error
	{
		ErrorGeneric,
		ErrorNXDomain, // query only
		ErrorTimeout,  // query only
		ErrorConflict  // publish only
	};

	class NameServer
	{
	public:
		QHostAddress address;
		int port;

		NameServer();
		NameServer(const QHostAddress &address, int port = 53)
		: address(address)
		, port(port)
		{}
	};

	class DnsHost
	{
	public:
		QByteArray name;
		QHostAddress address;
	};

	class SystemInfo
	{
	public:
		QList<NameServer> nameServers;
		QList<QByteArray> domains;
		QList<DnsHost> hosts;
	};

	QJDns(QObject *parent = 0);
	~QJDns();

	bool init(Mode mode, const QHostAddress &address);
	void shutdown();
	QStringList debugLines();

	static SystemInfo systemInfo();
	static QHostAddress detectPrimaryMulticast(const QHostAddress &address);

	void setNameServers(const QList<NameServer> &list);

	int queryStart(const QByteArray &name, int type);
	void queryCancel(int id);

	// for multicast mode only
	int publishStart(PublishMode m, const QJDnsRecord &record);
	void publishUpdate(int id, const QJDnsRecord &record);
	void publishCancel(int id);

signals:
	void resultsReady(int id, const QJDnsResponse &results);
	void published(int id);
	void error(int id, int e);
	void shutdownFinished();
	void debugLinesReady();

private:
	class Private;
	friend class Private;
	Private *d;
};

#endif
