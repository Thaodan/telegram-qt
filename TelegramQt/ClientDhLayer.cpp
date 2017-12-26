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

#include "ClientDhLayer.hpp"

#include "CTelegramTransport.hpp"
#include "Utils.hpp"
#include "TelegramUtils.hpp"

#include <QDebug>
#include <QtEndian>
#include <QDateTime>

namespace Telegram {

//namespace Client {

ClientDhLayer::ClientDhLayer(QObject *parent) :
    BaseDhLayer(parent)
{
}

void ClientDhLayer::init()
{
    m_authRetryId = 0;
    Utils::randomBytes(m_clientNonce.data, m_clientNonce.size());
    PlainPacketOperation *op = requestPqAuthorization();
    setState(State::PqRequested);
    connect(op, &PlainPacketOperation::finished, this, &ClientDhLayer::onPqAuthorizationAnswer);
}

quint64 ClientDhLayer::newMessageId()
{
    quint64 ts = TelegramUtils::formatTimeStamp(QDateTime::currentMSecsSinceEpoch());
    ts &= ~quint64(3);
    return m_transport->getNewMessageId(ts);
}

PlainPacketOperation *ClientDhLayer::requestPqAuthorization()
{
    QByteArray output;
    CTelegramStream outputStream(&output, /* write */ true);
    outputStream << TLValue::ReqPq;
    outputStream << m_clientNonce;
    return sendPlainPackage(output);
}

void ClientDhLayer::onPqAuthorizationAnswer(PlainPacketOperation *operation)
{
    if (!operation->isSucceeded()) {
        qCritical() << Q_FUNC_INFO << "Bad1";
        setState(State::Failed);
        return;
    }
    if (!acceptPqAuthorization(operation->replyData())) {
        qCritical() << Q_FUNC_INFO << "Bad2";
        setState(State::Failed);
        return;
    }
    PlainPacketOperation *op = requestDhParameters();
    setState(State::DhRequested);
    connect(op, &PlainPacketOperation::finished, this, &ClientDhLayer::onDhParametersAnswer);
}

bool ClientDhLayer::acceptPqAuthorization(const QByteArray &payload)
{
    qDebug() << Q_FUNC_INFO;
    CTelegramStream inputStream(payload);

    TLValue responsePqValue;
    inputStream >> responsePqValue;

    if (responsePqValue != TLValue::ResPQ) {
        qDebug() << "Error: Unexpected operation code";
        return false;
    }

    TLNumber128 clientNonce;
    inputStream >> clientNonce;

    if (clientNonce != m_clientNonce) {
        qWarning() << "Error: Client nonce in incoming package is different from our own.";
#ifdef TELEGRAMQT_DEBUG_REVEAL_SECRETS
    qDebug() << Q_FUNC_INFO << "Remote client nonce:" << clientNonce << "local:" << m_clientNonce;
#endif
        return false;
    }

    TLNumber128 serverNonce;
    inputStream >> serverNonce;
    m_serverNonce = serverNonce;

    QByteArray pq;

    inputStream >> pq;

    if (pq.size() != 8) {
        qDebug() << "Error: PQ should be 8 bytes in length";
        return false;
    }

    qDebug() << "PQ data:" << pq.toHex();
    m_pq = qFromBigEndian<quint64>(reinterpret_cast<const uchar*>(pq.constData()));

    qDebug() << "PQ:" << m_pq;

    quint64 div1 = Utils::findDivider(m_pq);

    if (div1 == 1) {
        qDebug() << "Error: Can not solve PQ.";
        return false;
    }
    quint64 div2 = m_pq / div1;

    if (div1 < div2) {
        m_p = div1;
        m_q = div2;
    } else {
        m_p = div2;
        m_q = div1;
    }

    TLVector<quint64> fingerprints;
    inputStream >> fingerprints;
    if (fingerprints.count() != 1) {
        qDebug() << "Error: Unexpected Server RSA Fingersprints vector size:" << fingerprints.size();
        return false;
    }
#ifdef TELEGRAMQT_DEBUG_REVEAL_SECRETS
    qDebug() << Q_FUNC_INFO << "Client nonce:" << clientNonce;
    qDebug() << Q_FUNC_INFO << "Server nonce:" << serverNonce;
    qDebug() << Q_FUNC_INFO << "PQ:" << m_pq;
    qDebug() << Q_FUNC_INFO << "P:" << m_p;
    qDebug() << Q_FUNC_INFO << "Q:" << m_q;
    qDebug() << Q_FUNC_INFO << "Fingerprints:" << fingerprints;
#endif
    for (quint64 serverFingerprint : fingerprints) {
        if (serverFingerprint == m_rsaKey.fingerprint) {
            return true;
        }
    }
    qDebug() << "Error: Server RSA fingersprints" << fingerprints << " do not match to the loaded key" << m_rsaKey.fingerprint;
    return false;
}

PlainPacketOperation *ClientDhLayer::requestDhParameters()
{
    qDebug() << Q_FUNC_INFO;
    Utils::randomBytes(m_newNonce.data, m_newNonce.size());

#ifdef TELEGRAMQT_DEBUG_REVEAL_SECRETS
    qDebug() << Q_FUNC_INFO << "New nonce:" << m_newNonce;
#endif

    QByteArray bigEndianNumber;
    bigEndianNumber.fill(char(0), 8);

    QByteArray encryptedPackage;
    {
        static const int requestedEncryptedPackageLength = 255;
        QByteArray innerData;
        CTelegramStream encryptedStream(&innerData, /* write */ true);

        encryptedStream << TLValue::PQInnerData;

        qToBigEndian(m_pq, (uchar *) bigEndianNumber.data());
        encryptedStream << bigEndianNumber;

        bigEndianNumber.fill(char(0), 4);
        qToBigEndian(m_p, (uchar *) bigEndianNumber.data());
        encryptedStream << bigEndianNumber;

        qToBigEndian(m_q, (uchar *) bigEndianNumber.data());
        encryptedStream << bigEndianNumber;

        encryptedStream << m_clientNonce;
        encryptedStream << m_serverNonce;
        encryptedStream << m_newNonce;

        QByteArray sha = Utils::sha1(innerData);
        QByteArray randomPadding;
        randomPadding.resize(requestedEncryptedPackageLength - (sha.length() + innerData.length()));
        Utils::randomBytes(&randomPadding);

        qDebug() << Q_FUNC_INFO << "sha length:" << sha.size();
        qDebug() << "Encrypt with key" << m_rsaKey.modulus << m_rsaKey.exponent << m_rsaKey.fingerprint;
        encryptedPackage = Utils::rsa(sha + innerData + randomPadding, m_rsaKey);
#ifdef TELEGRAMQT_DEBUG_REVEAL_SECRETS
        qDebug() << Q_FUNC_INFO << "Inner sha:" << QByteArrayLiteral("0x") + sha.toHex();
        qDebug() << Q_FUNC_INFO << "Inner data:" << QByteArrayLiteral("0x") + innerData.toHex();
    #endif
    }

    QByteArray output;
    CTelegramStream outputStream(&output, /* write */ true);

    outputStream << TLValue::ReqDHParams;
    outputStream << m_clientNonce;
    outputStream << m_serverNonce;

    bigEndianNumber.fill(char(0), 4);
    qToBigEndian(m_p, (uchar *) bigEndianNumber.data());
    outputStream << bigEndianNumber;

    qToBigEndian(m_q, (uchar *) bigEndianNumber.data());
    outputStream << bigEndianNumber;

    qDebug() << Q_FUNC_INFO << "public server fs:" << m_rsaKey.fingerprint;
    outputStream << m_rsaKey.fingerprint;

    outputStream << encryptedPackage;
    return sendPlainPackage(output);
}

void ClientDhLayer::onDhParametersAnswer(PlainPacketOperation *operation)
{
    if (!operation->isSucceeded()) {
        qCritical() << Q_FUNC_INFO << "Bad1";
        setState(State::Failed);
        return;
    }
    if (!acceptDhAnswer(operation->replyData())) {
        qCritical() << Q_FUNC_INFO << "Bad2";
        setState(State::Failed);
        return;
    }
    generateDh();
    PlainPacketOperation *op = requestDhGenerationResult();
    setState(State::DhGenerationResultRequested);
    connect(op, &PlainPacketOperation::finished, this, &ClientDhLayer::onDhGenerationResultAnswer);
}

bool ClientDhLayer::acceptDhAnswer(const QByteArray &payload)
{
    qDebug() << Q_FUNC_INFO;
    CTelegramStream inputStream(payload);

    TLValue responseTLValue;
    inputStream >> responseTLValue;

    if (!checkClientServerNonse(inputStream)) {
        return false;
    }

    switch (responseTLValue) {
    case TLValue::ServerDHParamsOk: {
        QByteArray encryptedAnswer;
        inputStream >> encryptedAnswer;
        return processServerDHParamsOK(encryptedAnswer);
    }
    default:
        qDebug() << "Error: Server did not accept our DH params.";
    }
    return false;
}

bool ClientDhLayer::processServerDHParamsOK(const QByteArray &encryptedAnswer)
{
    qDebug() << Q_FUNC_INFO << "encryptedAnswer.size():" << encryptedAnswer.size();
    m_tmpAesKey = generateTmpAesKey();

    const QByteArray answerWithHash = Utils::aesDecrypt(encryptedAnswer, m_tmpAesKey);
    const QByteArray sha1OfAnswer = answerWithHash.mid(0, 20);
    const QByteArray answer = answerWithHash.mid(20, 564);

    qDebug() << "answer size:" << answer.size();

//    qDebug() << "Taking sha of" << answer.toHex();
    if (Utils::sha1(answer) != sha1OfAnswer) {
        qDebug() << "Error: SHA1 of encrypted answer is different from announced.";
        return false;
    }

    CTelegramStream encryptedInputStream(answer);

    TLValue responseTLValue;
    encryptedInputStream >> responseTLValue;

    if (responseTLValue != TLValue::ServerDHInnerData) {
        qDebug() << "Error: Unexpected TL Value in encrypted answer.";
        return false;
    }

    if (!checkClientServerNonse(encryptedInputStream)) {
        return false;
    }

    encryptedInputStream >> m_g;
    encryptedInputStream >> m_dhPrime;
    encryptedInputStream >> m_gA;

    if ((m_g < 2) || (m_g > 7)) {
        qDebug() << "Error: Received 'g' number is out of acceptable range [2-7].";
        return false;
    }

    if (m_dhPrime.length() != 2048 / 8) {
        qDebug() << "Error: Received dhPrime number length is not correct." << m_dhPrime.length() << 2048 / 8;
        return false;
    }

    if (!(m_dhPrime.at(0) & 128)) {
        qDebug() << "Error: Received dhPrime is too small.";
        return false;
    }

    qDebug() << "dhPrime size:" << m_dhPrime.size() << m_dhPrime.toHex();
    qDebug() << "gA size:" << m_gA.size() << m_gA.toHex();

    quint32 serverTime;
    encryptedInputStream >> serverTime;
    setDeltaTime(qint64(serverTime) - (QDateTime::currentMSecsSinceEpoch() / 1000));
    return true;
}

void ClientDhLayer::generateDh()
{
    qDebug() << Q_FUNC_INFO;
    // #6 Client computes random 2048-bit number b (using a sufficient amount of entropy) and sends the server a message
    m_b.resize(256);
    Utils::randomBytes(&m_b);

    // IMPORTANT: Apart from the conditions on the Diffie-Hellman prime dh_prime and generator g,
    // both sides are to check that g, g_a and g_b are greater than 1 and less than dh_prime - 1.
    // We recommend checking that g_a and g_b are between 2^{2048-64} and dh_prime - 2^{2048-64} as well.

#ifdef TELEGRAMQT_DEBUG_REVEAL_SECRETS
    qDebug() << "m_b" << m_b;
#endif
}

PlainPacketOperation *ClientDhLayer::requestDhGenerationResult()
{
    qDebug() << Q_FUNC_INFO;
    QByteArray output;
    CTelegramStream outputStream(&output, /* write */ true);

    outputStream << TLValue::SetClientDHParams;
    outputStream << m_clientNonce;
    outputStream << m_serverNonce;

    QByteArray encryptedPackage;
    {
        QByteArray innerData;
        CTelegramStream encryptedStream(&innerData, /* write */ true);

        encryptedStream << TLValue::ClientDHInnerData;

        encryptedStream << m_clientNonce;
        encryptedStream << m_serverNonce;
        encryptedStream << m_authRetryId;

        QByteArray binNumber;
        binNumber.resize(sizeof(m_g));
        qToBigEndian(m_g, (uchar *) binNumber.data());

        binNumber = Utils::binaryNumberModExp(binNumber, m_dhPrime, m_b);

        encryptedStream << binNumber;

        QByteArray sha = Utils::sha1(innerData);
        QByteArray randomPadding;

        int packageLength = sha.length() + innerData.length();
        if ((packageLength) % 16) {
            randomPadding.resize(16 - (packageLength % 16));
            Utils::randomBytes(&randomPadding);

            packageLength += randomPadding.size();
        }
        qDebug() << Q_FUNC_INFO << "Inner data size:" << innerData.size();

        encryptedPackage = Utils::aesEncrypt(sha + innerData + randomPadding, m_tmpAesKey);
        encryptedPackage.truncate(packageLength);
    }

    outputStream << encryptedPackage;
    return sendPlainPackage(output);
}

void ClientDhLayer::onDhGenerationResultAnswer(PlainPacketOperation *operation)
{
    if (!operation->isSucceeded()) {
        qCritical() << Q_FUNC_INFO << "Bad1";
        setState(State::Failed);
        return;
    }
    if (!processServerDhAnswer(operation->replyData())) {
        qCritical() << Q_FUNC_INFO << "Bad2";
        setState(State::Failed);
        return;
    }
    setState(State::HasKey);
}

bool ClientDhLayer::processServerDhAnswer(const QByteArray &payload)
{
    CTelegramStream inputStream(payload);
    TLValue responseTLValue;
    inputStream >> responseTLValue;
    qDebug() << Q_FUNC_INFO << responseTLValue;

    if (!checkClientServerNonse(inputStream)) {
        return false;
    }

    TLNumber128 newNonceHashLower128;
    inputStream >> newNonceHashLower128;
    const QByteArray readedHashPart(newNonceHashLower128.data, newNonceHashLower128.size());
    const QByteArray newAuthKey = Utils::binaryNumberModExp(m_gA, m_dhPrime, m_b);
    const QByteArray newAuthKeySha = Utils::sha1(newAuthKey);
    QByteArray expectedHashData(m_newNonce.data, m_newNonce.size());
    expectedHashData.append(newAuthKeySha.left(8));
    if (responseTLValue == TLValue::DhGenOk) {
        qDebug() << Q_FUNC_INFO << "Answer OK";
        expectedHashData.insert(32, char(1));
    } else if (responseTLValue == TLValue::DhGenRetry) {
        qDebug() << Q_FUNC_INFO << "Answer RETRY";
        expectedHashData.insert(32, char(2));
    } else if (responseTLValue == TLValue::DhGenFail) {
        qDebug() << Q_FUNC_INFO << "Answer FAIL";
        expectedHashData.insert(32, char(3));
    } else {
        qDebug() << "Error: Unexpected server response.";
        return false;
    }
    qDebug() << "readedHashPart..." << readedHashPart.toHex();

    if (Utils::sha1(expectedHashData).mid(4) != readedHashPart) {
        qDebug() << "Error: Server (newNonce + auth key) hash is not correct.";
        return false;
    }
    if (responseTLValue == TLValue::DhGenOk) {
        qDebug() << "Server DH answer is accepted. Setup the auth key...";
        m_serverSalt = m_serverNonce.parts[0] ^ m_newNonce.parts[0];
        setAuthKey(newAuthKey);
    } else {
        qDebug() << "Server DH answer is not accepted. Retry...";
        if (newAuthKey.isEmpty()) {
            m_authRetryId = 0;
        } else {
            m_authRetryId = Utils::getFingersprint(newAuthKey, /* lower-order */ false);// 64 higher-order bits of SHA1(auth_key)
        }
        generateDh();
        requestDhGenerationResult();
    }
    return true;
}

PlainPacketOperation *ClientDhLayer::sendPlainPackage(const QByteArray &payload)
{
    const quint64 authKeyId = 0;
    const quint64 messageId = newMessageId();
    const quint32 messageLength = payload.length();
    constexpr int headerSize = sizeof(authKeyId) + sizeof(messageId) + sizeof(messageLength);

    QByteArray output;
    output.reserve(headerSize + payload.size());
    CRawStream outputStream(&output, /* write */ true);

    outputStream << authKeyId;
    outputStream << messageId;
    outputStream << messageLength;
    outputStream << payload;
    m_transport->sendPackage(output);

#ifdef NETWORK_LOGGING
    CTelegramStream readBack(buffer);
    TLValue val1;
    readBack >> val1;

    QTextStream str(m_logFile);

    str << QDateTime::currentDateTime().toString(QLatin1String("yyyyMMdd HH:mm:ss:zzz")) << QLatin1Char('|');
    str << QLatin1String("pln|");
    str << QString(QLatin1String("size: %1|")).arg(buffer.length(), 4, 10, QLatin1Char('0'));
    str << formatTLValue(val1) << QLatin1Char('|');
    str << buffer.toHex();
    str << endl;
    str.flush();
#endif

    m_plainOperation = new PlainPacketOperation(payload, this);
    m_plainOperation->setRequestId(messageId);
    return m_plainOperation;
}

void ClientDhLayer::processReceivedPacket(const QByteArray &payload)
{
    if (!m_plainOperation) {
        qCritical() << Q_FUNC_INFO << "Unexpected unencrypted message";
        return;
    }
    PlainPacketOperation *op = m_plainOperation;
    m_plainOperation = nullptr;
    op->setReplyData(payload);
    op->setFinished();
}

} // Telegram namespace
