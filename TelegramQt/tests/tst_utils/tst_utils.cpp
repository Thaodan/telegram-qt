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

#include "Utils.hpp"
#include "TelegramNamespace.hpp"

#include <QTest>
#include <QDebug>
#include <QStandardPaths>

using namespace Telegram;

class tst_utils : public QObject
{
    Q_OBJECT
public:
    explicit tst_utils(QObject *parent = nullptr);

private slots:
    void testAesEncryption();
    void testRsaEncryption();
    void testRsaFingersprint();
    void testRsaKey();
    void testRsaKeyIsValid();

};

tst_utils::tst_utils(QObject *parent) :
    QObject(parent)
{
}

void tst_utils::testAesEncryption()
{
    const QByteArray key(32, char(1));
    const QByteArray iv(32, char(2));
    const SAesKey aesKey(key, iv);
    const QByteArray sourceData = QByteArrayLiteral("TestData12345678");
    const QByteArray encodedData = Utils::aesEncrypt(sourceData, aesKey);
    QVERIFY(!encodedData.isEmpty());
    const QByteArray decodedData = Utils::aesDecrypt(encodedData, aesKey);
    QCOMPARE(sourceData, decodedData);
}

void tst_utils::testRsaEncryption()
{
    Telegram::RsaKey key = Utils::loadRsaPrivateKeyFromFile(QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first() + QStringLiteral("/TelegramServer/private_key_2048.pem"));
    QVERIFY(key.isValid());

    const QByteArray sourceData = QByteArrayLiteral("TestData12345678");
    const QByteArray encodedData = Utils::rsa(sourceData, key);
    QVERIFY(!encodedData.isEmpty());
    const QByteArray decodedData = Utils::binaryNumberModExp(encodedData, key.modulus, key.secretExponent);
    QCOMPARE(sourceData, decodedData);

    const QByteArray encData = QByteArray::fromHex(QByteArrayLiteral("b5f54f843a80a21dccaa88f80d3d1a23c788b3090089265005c6e94c1f2b38b749f20799a63d0fd516f0912597735096c2d644579ba53a217a5070615ffabf450baaa6504ff4196a6f520e190d77cdbb7f78684c4b260e31f5108e7b6a20316e569ea2725ed6eeed49cbd357a982f75d38bbd623f9a02eb7f9af83fcd9f14e2a147a87dae0932949e44a06751926cd97563162833a4809c1116fc6d262c2c3d428a529ef38ccb045e4e7769efcfcf0a4ae3088b008fbc9e4417a52e70dbb2eaaa7c018f3eeda0977cb945c364b0a539f48c9617499dc59c8be8fb6d69a2146f502fb9a0642d93526eef3acf8e6c4f7ce30f46e1a13d4b48016157d3562262043"));
    const QByteArray decData = Utils::binaryNumberModExp(encData, key.modulus, key.secretExponent);
    qDebug() << "dec data:" << decData.toHex();
}

void tst_utils::testRsaFingersprint()
{
    const Telegram::RsaKey builtInKey = Utils::loadHardcodedKey();
    const quint64 fingerprint = Utils::getRsaFingersprint(builtInKey);
    QCOMPARE(fingerprint, builtInKey.fingerprint);
}

void tst_utils::testRsaKey()
{
    Telegram::RsaKey key;
    key.modulus = QByteArrayLiteral("keykeykey");
    key.exponent = QByteArrayLiteral("exp");
    key.fingerprint = 123456ull;

    Telegram::RsaKey key2;
    QVERIFY(key2.modulus.isEmpty());
    key2 = key;
    QCOMPARE(key.modulus, key2.modulus);
    QCOMPARE(key.exponent, key2.exponent);
    QCOMPARE(key.fingerprint, key2.fingerprint);
}

void tst_utils::testRsaKeyIsValid()
{
    Telegram::RsaKey key;
    QVERIFY2(!key.isValid(), "An emtpy should not be valid");
    key.modulus = QByteArrayLiteral("mod");
    QVERIFY2(!key.isValid(), "A key without an exponent and a fingerprint is not valid");
    key.exponent = QByteArrayLiteral("exp");
    QVERIFY2(!key.isValid(), "A key without a fingerprint is not valid");
    key.fingerprint = 123456ull;
    QVERIFY(!key.isValid());
    QVERIFY2(!key.isValid(), "The key fingerprint does not match the modulus and exponend, so the key should not be valid");
    key.updateFingersprint();
    QVERIFY2(key.isValid(), "The key should be valid");
    key.modulus.clear();
    QVERIFY2(!key.isValid(), "A key without a modulus is not valid");
}

QTEST_APPLESS_MAIN(tst_utils)

#include "tst_utils.moc"
