#include "TelegramServerClient.hpp"

#include "../TelegramQt/TLTypes.hpp"
#include "../TelegramQt/CTelegramTransport.hpp"
#include "ServerDhLayer.hpp"

#include <QLoggingCategory>

namespace Telegram {

namespace Server {

Client::Client(QObject *parent) :
    CTelegramConnection(nullptr, parent)
{
    m_dhLayer = new DhLayer(this);
    connect(m_dhLayer, &BaseDhLayer::stateChanged, this, &Client::onDhStateChanged);
}

void Client::setTransport(CTelegramTransport *newTransport)
{
    m_transport = newTransport;
    connect(m_transport, &CTelegramTransport::stateChanged, this, &Client::onTransportStateChanged);
    connect(m_transport, &CTelegramTransport::packageReceived, this, &Client::onTransportPackageReceived);
//    connect(m_transport, &CTelegramTransport::timeout, this, &CTelegramConnection::onTransportTimeout);
    onTransportStateChanged();
    m_dhLayer->setTransport(m_transport);
}

void Client::onTransportStateChanged()
{
    switch (m_transport->state()) {
    case QAbstractSocket::ConnectedState:
        m_dhLayer->setServerRsaKey(m_rsaKey);
        m_dhLayer->init();
        setStatus(ConnectionStatusConnected, ConnectionStatusReasonRemote);
        break;
    case QAbstractSocket::UnconnectedState:
        setStatus(ConnectionStatusDisconnected, status() == ConnectionStatusDisconnecting ? ConnectionStatusReasonLocal : ConnectionStatusReasonRemote);
        break;
    default:
        break;
    }
}

void Client::onTransportPackageReceived(const QByteArray &package)
{
    if (package.size() < 8) {
        return;
    }
    const quint64 *authKeyIdBytes = reinterpret_cast<const quint64*>(package.constData());
    if (*authKeyIdBytes) {
//        onEncryptedData(stream, authId);
    } else {
        m_dhLayer->processPlainPackage(package);
    }
}

void Client::onDhStateChanged()
{
    if (m_dhLayer->state() == DhLayer::State::HasKey) {
        setAuthKey(m_dhLayer->authKey());
        m_serverSalt = m_dhLayer->serverSalt();
        setAuthState(AuthStateHaveAKey);
    }
}

} // Server

} // Telegram
