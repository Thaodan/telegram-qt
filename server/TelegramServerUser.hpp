#ifndef TELEGRAMSERVERUSER_HPP
#define TELEGRAMSERVERUSER_HPP

#include <QObject>
#include <QVector>

#include "../TelegramQt/CAppInformation.hpp"

namespace Telegram {

namespace Server {

struct Session
{
    Session() :
        authId(0),
        appInfo(nullptr),
        timestamp(0)
    {
    }

    Session(const Session &otherSession) :
        authId(otherSession.authId),
        appInfo(nullptr),
        timestamp(otherSession.timestamp)
    {
        if (otherSession.appInfo) {
            appInfo = new CAppInformation(otherSession.appInfo);
        }
    }

    quint64 authId;
    QByteArray authKey;
    CAppInformation *appInfo;
    QString ip;
    quint64 timestamp;
};

class User : public QObject
{
    Q_OBJECT
public:
    explicit User(QObject *parent = nullptr);

    quint32 id() const { return m_id; }
    QString phoneNumber() const { return m_phoneNumber; }
    void setPhoneNumber(const QString &phoneNumber);

    bool isOnline();

    Session getSession(quint64 authId) const;
    QVector<Session> sessions() const { return m_sessions; }
    void addSession(const Session &session);

protected:
    quint32 m_id;
    QString m_phoneNumber;
    QVector<Session> m_sessions;
};

} // Server

} // Telegram

#endif // TELEGRAMSERVERUSER_HPP
