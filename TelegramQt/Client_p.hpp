#ifndef TELEGRAM_CLIENT_P_HPP
#define TELEGRAM_CLIENT_P_HPP

#include "Client.hpp"

#include "CAppInformation.hpp"

namespace Telegram {

class AppInformation : public CAppInformation { };

namespace Client {

class Client::Private : public QObject
{
    Q_OBJECT
public:
    explicit Private(Client *parent) : QObject(parent) { }
    static const Private *get(const Client *c) { return c->d; }
    static Private *get(Client *c) { return c->d; }

    PendingOperation *connectToServer();

    AppInformation *m_appInformation = nullptr;
    Client *m_client;

    bool m_signedIn = false;
};

} // Client

} // Telegram

#endif // TELEGRAM_CLIENT_P_HPP
