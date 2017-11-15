#include "TelegramServerUser.hpp"

namespace Telegram {

namespace Server {

User::User(QObject *parent) :
    QObject(parent)
{
}

void User::setPhoneNumber(const QString &phoneNumber)
{
    m_phoneNumber = phoneNumber;
    m_id = qHash(m_phoneNumber);
}

bool User::isOnline()
{
    return true;
}

Session User::getSession(quint64 authId) const
{
    for (const Session &s : m_sessions) {
        if (s.authId == authId) {
            return s;
        }
    }
    return Session();
}

void User::addSession(const Session &session)
{
    m_sessions.append(session);
}

} // Server

} // Telegram
