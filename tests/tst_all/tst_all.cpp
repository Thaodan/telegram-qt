/*
   Copyright (C) 2017 Alexandr Akulich <akulichalexander@gmail.com>

   This file is a part of TelegramQt library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

 */

#include <QObject>

#include "Client.hpp"
#include "Utils.hpp"
#include "TelegramNamespace.hpp"
#include "CAppInformation.hpp"

#include "TelegramServer.hpp"
#include "TelegramServerUser.hpp"
#include "CTelegramCore.hpp"

#include <QTest>
#include <QDebug>
#include <QStandardPaths>

using namespace Telegram;

CAppInformation *getAppInfo()
{
    static CAppInformation *appInfo = nullptr;
    if (!appInfo) {
        appInfo = new CAppInformation();
        appInfo->setAppId(14617);
        appInfo->setAppHash(QLatin1String("e17ac360fd072f83d5d08db45ce9a121"));
        appInfo->setAppVersion(QLatin1String("0.1"));
        appInfo->setDeviceInfo(QLatin1String("pc"));
        appInfo->setOsInfo(QLatin1String("GNU/Linux"));
        appInfo->setLanguageCode(QLatin1String("en"));
    }
    return appInfo;
}

class tst_all : public QObject
{
    Q_OBJECT
public:
    explicit tst_all(QObject *parent = nullptr);

private slots:
    void testClientConnection();

};

tst_all::tst_all(QObject *parent) :
    QObject(parent)
{
}

void tst_all::testClientConnection()
{
    TelegramNamespace::registerTypes();
    Server::Server server;
    TLDcOption option;
    option.ipAddress = QStringLiteral("127.0.0.1");
    option.port = 11443;
    option.id = 1;
    RsaKey key = Utils::loadRsaPrivateKeyFromFile(QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first()
                                                                      + QStringLiteral("/TelegramServer/private_key.pem"));
    QVERIFY2(key.isValid(), "Unable to read RSA key");
    server.setServerPrivateRsaKey(key);
    server.setDcOption(option);
    QVERIFY(server.start());

#ifdef CLIENT_WORKS
    Client::Client client;
#else
    CTelegramCore client;
#endif
    client.setAppInformation(getAppInfo());
    QVERIFY(client.setServerConfiguration({DcOption(option.ipAddress, option.port)}));
    QVERIFY(client.setServerPublicRsaKey(key));

#ifdef CLIENT_WORKS
    PendingOperation *op = client.connectToServer();
    QTRY_VERIFY(op->isSucceeded());
#else
    QVERIFY(client.connectToServer());
    QTRY_COMPARE_WITH_TIMEOUT(client.connectionState(), TelegramNamespace::ConnectionStateConnected, 500);
#endif
    quint64 clientAuthId = client.authKeyId();
    QVERIFY(clientAuthId);
    Server::User *user = server.getUser(clientAuthId);
    QVERIFY(user);
    QVERIFY(!user->sessions().isEmpty());
    QCOMPARE(user->sessions().first().authId, clientAuthId);
}

QTEST_GUILESS_MAIN(tst_all)

#include "tst_all.moc"
