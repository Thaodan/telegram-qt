#ifndef TELEGRAMSERVERCLIENT_HPP
#define TELEGRAMSERVERCLIENT_HPP

#include <QObject>
#include "../TelegramQt/CTelegramConnection.hpp"

namespace Telegram {

namespace Server {

class DhLayer;

class User;

class Client : public CTelegramConnection
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);

    void setTransport(CTelegramTransport *newTransport); // Hide CTelegramConnection::setTransport()

signals:
    void becomeOnline();

protected slots:
    void onTransportStateChanged();
    void onTransportPackageReceived(const QByteArray &package);

protected:
    void onDhStateChanged();

private:
    DhLayer *m_dhLayer;
    User *m_data;

};

} // Server

} // Telegram

#endif // TELEGRAMSERVERCLIENT_HPP
