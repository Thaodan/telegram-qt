/*
   Copyright (C) 2014-2017 Alexandr Akulich <akulichalexander@gmail.com>

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

#ifndef TELEGRAM_CLIENT_HPP
#define TELEGRAM_CLIENT_HPP

#include "telegramqt_global.h"
#include "TelegramNamespace.hpp"

#include "PendingOperation.hpp"

#include <QObject>
#include <QVector>
#include <QStringList>

QT_FORWARD_DECLARE_CLASS(QIODevice)

namespace Telegram {

class AppInformation;

namespace Client {

class TELEGRAMQT_EXPORT Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AppInformation *applicationInformation READ appInformation WRITE setAppInformation)
public:
    explicit Client(QObject *parent = nullptr);
    ~Client();

    bool isSignedIn() const;

    AppInformation *appInformation() const;
    Q_INVOKABLE void setAppInformation(AppInformation *newAppInfo);

    Q_INVOKABLE static QVector<Telegram::DcOption> defaultServerConfiguration();
//    Q_INVOKABLE static quint32 defaultPingInterval();

//    Q_INVOKABLE Telegram::RsaKey defaultServerPublicRsaKey() const;
//    Q_INVOKABLE Telegram::RsaKey serverPublicRsaKey() const;
//    Q_INVOKABLE QVector<Telegram::DcOption> serverConfiguration();
//    QByteArray connectionSecretInfo() const;

//    Q_INVOKABLE TelegramNamespace::ConnectionState connectionState() const;
//    Q_INVOKABLE QString selfPhone() const;
//    Q_INVOKABLE quint32 selfId() const;
//    // maxMessageId is an id of the last sent or received message. Updated *after* messageReceived and sentMessageIdReceived signal emission.
//    Q_INVOKABLE quint32 maxMessageId() const;
//    Q_INVOKABLE QVector<quint32> contactList() const;
//    Q_INVOKABLE QVector<Telegram::Peer> dialogs() const;

//    Q_INVOKABLE QString peerPictureToken(const Telegram::Peer &peer, const Telegram::PeerPictureSize size = Telegram::PeerPictureSize::Small) const;

//    static qint32 localTypingRecommendedRepeatInterval(); // Recommended application local typing state re-set interval.

//    bool getDialogInfo(Telegram::DialogInfo *info, const Telegram::Peer &peer) const;
//    bool getUserInfo(Telegram::UserInfo *info, quint32 userId) const;
//    bool getChatInfo(Telegram::ChatInfo *info, const Telegram::Peer peer) const;
//    bool getChatParticipants(QVector<quint32> *participants, quint32 chatId);

//    bool getMessageMediaInfo(Telegram::MessageMediaInfo *messageInfo, quint32 messageId, const Telegram::Peer &peer) const;

public Q_SLOTS:
//    void setMessageReceivingFilter(TelegramNamespace::MessageFlags flags); // Messages with at least one of the passed flags will be filtered out.
//    void setAcceptableMessageTypes(TelegramNamespace::MessageTypeFlags types);
//    void setAutoReconnection(bool enable);
//    void setUpdatesEnabled(bool enable);

//    // By default, the app would ping server every 15 000 ms and instruct the server to close connection after 10 000 more ms. Pass interval = 0 to disable ping.
//    void setPingInterval(quint32 interval, quint32 serverDisconnectionAdditionalTime = 10000);
//    void setMediaDataBufferSize(quint32 size);

    PendingOperation *connectToServer();
//    void disconnectFromServer();
//    PendingAuthOperation *checkPhoneNumber();
    PendingAuthOperation *signIn(const QString &phoneNumber);

    bool setServerPublicRsaKey(const Telegram::RsaKey &key);
    bool setServerConfiguration(const QVector<Telegram::DcOption> &dcs);
    bool resetServerConfiguration();

//    void resetConnectionData();
//    bool setSecretInfo(const QByteArray &secret);

//    bool logOut();

//    void requestPhoneStatus(const QString &phoneNumber);
//    void requestPhoneCode(const QString &phoneNumber); // Deprecated, use requestAuthCode() instead
//    bool requestAuthCode(const QString &phoneNumber);
//    quint64 getPassword();
//    void tryPassword(const QByteArray &salt, const QString &password);
//    void tryPassword(const QByteArray &salt, const QByteArray &password);
//    void tryPassword(const QString &password);
//    void signIn(const QString &phoneNumber, const QString &authCode);
//    void signUp(const QString &phoneNumber, const QString &authCode, const QString &firstName, const QString &lastName);

//    void addContact(const QString &phoneNumber);
//    void addContacts(const QStringList &phoneNumbers);

//    void deleteContact(quint32 userId);
//    void deleteContacts(const QVector<quint32> &userIds);

//    PendingOperation *requestFile(const RemoteFile *file);
//    PendingOperation *requestHistory(const Peer &peer, int offset, int limit);
//    PendingOperation *resolveUsername(const QString &userName);
//    PendingOperation *uploadFile(const QByteArray &fileContent, const QString &fileName);
//    PendingOperation *uploadFile(QIODevice *source, const QString &fileName);

//    quint64 sendMessage(const Telegram::Peer &peer, const QString &message); // Message id is a random number
//    quint64 sendMedia(const Telegram::Peer &peer, const Telegram::MessageMediaInfo &messageInfo);
//    quint64 forwardMessage(const Telegram::Peer &peer, quint32 messageId);
//    /* Typing status is valid for 6 seconds. It is recommended to repeat typing status with localTypingRecommendedRepeatInterval() interval. */
//    void setTyping(const Telegram::Peer &peer, TelegramNamespace::MessageAction action);
//    void setMessageRead(const Telegram::Peer &peer, quint32 messageId);

    // Set visible (not actual) online status.
//    void setOnlineStatus(bool onlineStatus);

    // Both methods result in userNameStatusUpdated() emission
//    void checkUserName(const QString &userName);
//    Telegram::PendingOperation *getUserName(const QString &userName);
//    void setUserName(const QString &newUserName);
//    Telegram::PendingOperation *createBroadcast(const QString &name, const QString &description = QString());
    // Returns unique 64-bit id for the request. Null id means that the request is not valid.
//    quint64 createChat(const QVector<quint32> &userIds, const QString &title);
//    bool addChatUser(quint32 chatId, quint32 userId, quint32 forwardMessages = 0);

//Q_SIGNALS:
//    void connectionStateChanged(TelegramNamespace::ConnectionState state);
//    void selfUserAvailable(quint32 userId);

Q_SIGNALS:
    void signedInChanged(bool signedIn);

private:
    class Private;
    friend class Private;
    Private *d;
};

//inline void Client::addContact(const QString &phoneNumber)
//{
//    addContacts(QStringList() << phoneNumber);
//}

//inline void Client::deleteContact(quint32 userId)
//{
//    deleteContacts(QVector<quint32>() << userId);
//}

//inline quint64 Client::sendChatMessage(quint32 chatId, const QString &message)
//{
//    return sendMessage(Telegram::Peer(chatId, Telegram::Peer::Chat), message);
//}

//inline void Client::setChatTyping(quint32 chatId, TelegramNamespace::MessageAction action)
//{
//    setTyping(Telegram::Peer(chatId, Telegram::Peer::Chat), action);
//}

//inline void Client::setChatMessageRead(const quint32 &chatId, quint32 messageId)
//{
//    setMessageRead(Telegram::Peer(chatId, Telegram::Peer::Chat), messageId);
//}

} // Client

} // Telegram

#endif // TELEGRAM_CLIENT_HPP
