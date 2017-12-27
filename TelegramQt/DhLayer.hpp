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

#ifndef DH_LAYER_HPP
#define DH_LAYER_HPP

#include "TLNumbers.hpp"
#include "crypto-aes.hpp"
#include "CTelegramStream.hpp"
#include "PendingOperation.hpp"

class CTelegramTransport;

namespace Telegram {

class BaseDhLayer : public QObject
{
    Q_OBJECT
public:
    enum class AuthState {
        Idle,
        PqRequested,
        DhRequested,
        DhGenerationResultRequested,
        HasKey,
    };
    Q_ENUM(AuthState)

    /*
     * Client:
     *   Idle -> RequestPq -> AcceptPqAuthorization
     * Server:
     *   Idle -> ReqPqProcessed ->
     *
     *
     */

    // Diffie-Hellman key exchange; https://core.telegram.org/mtproto/auth_key

//    void requestPqAuthorization(); // #1 Client
//    void processReqPq(CTelegramStream &stream); // #2 Server

//    bool acceptPqAuthorization(const QByteArray &payload); // #3 Client
//    void requestDhParameters(); // #4 Client

//    bool processReqDHParams(CTelegramStream &inputStream); // #5.1 Server
//    bool acceptDhParams(); // #5.2a Server (DH OK)
//    bool declineDhParams(); // #5.2b Server (DH Fail)

//    bool acceptDhAnswer(const QByteArray &payload); // #5.3 Client processes 5.1
//    bool processServerDHParamsOK(const QByteArray &encryptedAnswer); // #5.4a Client processes 5.2a
//    bool processServerDHParamsFail(const QByteArray &encryptedAnswer); // #5.4b Client processes 5.2b

//    void generateDh(); // #6.1 Client
//    void requestDhGenerationResult(); // #6.2 Client

//    bool processSetClientDHParams(CTelegramStream &stream); // #7, #8, #9 Server
//    bool processServerDhAnswer(const QByteArray &payload); // #7, #8, #9 Client

    explicit BaseDhLayer(QObject *parent = nullptr);
    virtual void init() = 0;

    void setTransport(CTelegramTransport *transport);
    void setServerRsaKey(const RsaKey &key);

    // AES
    SAesKey generateTmpAesKey() const;

    // Helpers
    bool checkClientServerNonse(CTelegramStream &stream) const;
    PlainPacketOperation *sendPlainPackage(const QByteArray &payload);
    PlainPacketOperation *readPlainPackage();
    bool processPlainPackage(const QByteArray &buffer);
    virtual quint64 newMessageId() = 0;

    bool hasPendingOperation() { return m_plainOperation; }

    // Extra
    TLNumber128 clientNonce() const { return m_clientNonce; }
    TLNumber128 serverNonce() const { return m_serverNonce; }

    quint64 serverSalt() const { return m_serverSalt; }
    void setServerSalt(const quint64 salt) { m_serverSalt = salt; }

    QByteArray authKey() const { return m_authKey; }
    AuthState authState() { return m_authState; }

signals:
    void finished();

protected:
    void setAuthState(AuthState state);
    void setDeltaTime(const qint32 newDt);
    void setAuthKey(const QByteArray &authKey);

    CTelegramTransport *m_transport = nullptr;

    TLNumber128 m_clientNonce;
    TLNumber128 m_serverNonce;
    TLNumber256 m_newNonce;

    quint64 m_pq;
    quint32 m_p;
    quint32 m_q;

    RsaKey m_rsaKey;
    SAesKey m_tmpAesKey;

    quint32 m_g;
    QByteArray m_dhPrime;
    QByteArray m_gA;
    QByteArray m_a; // Server side
    QByteArray m_b; // Client side

    QByteArray m_authKey;
    AuthState m_authState;

    quint64 m_authRetryId;
    quint64 m_serverSalt;

    qint32 m_deltaTime = 0;

private:
    PlainPacketOperation *m_plainOperation = nullptr;

};

} // Telegram

#endif // DH_LAYER_HPP
