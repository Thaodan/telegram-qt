#include "Client_p.hpp"

#include <QTimer>

namespace Telegram {

namespace Client {

Client::Client(QObject *parent) :
    QObject(parent),
    d(new Private(this))
{
}

Client::~Client()
{
}

bool Client::isSignedIn() const
{
    return d->m_signedIn;
}

PendingAuthOperation *Client::signIn(const QString &phoneNumber)
{
//    if (!m_private->m_appInfo || !m_private->m_appInfo->isValid()) {
//        qWarning() << "CTelegramCore::connectToServer(): App information is null or is not valid.";
//        return false;
//    }

//    m_private->m_dispatcher->setAppInformation(m_private->m_appInfo);
//    return m_private->m_dispatcher->connectToServer();
    // connectToServer(),
    // checkPhoneNumber()

    PendingAuthOperation *op = new PendingAuthOperation(this);

    QTimer::singleShot(500, op, &PendingAuthOperation::authCodeRequired);
//    op->setClient(this);
//    op->start();

    connect(op, &PendingOperation::finished, [this](PendingOperation *operation) {
        d->m_signedIn = operation->isSucceeded();
        emit signedInChanged(d->m_signedIn);
    });
    return op;
}

bool Client::setServerPublicRsaKey(const RsaKey &key)
{
    return false;
}

bool Client::setServerConfiguration(const QVector<DcOption> &dcs)
{
    return false;
}

bool Client::resetServerConfiguration()
{
    return false;
}

AppInformation *Client::appInformation() const
{
    return d->m_appInformation;
}

void Client::setAppInformation(AppInformation *newAppInfo)
{
    d->m_appInformation = newAppInfo;
}

QVector<DcOption> Client::defaultServerConfiguration()
{
    return {};
}

PendingOperation *Client::connectToServer()
{
    return d->connectToServer();
}

PendingOperation *Client::Private::connectToServer()
{
    return nullptr;
}

//void Client::setAppInformation(AppInformation *newAppInfo)
//{
//    d->m_appInformation = newAppInfo;
//}

} // Client

} // Telegram
