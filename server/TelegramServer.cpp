#include "TelegramServer.hpp"

#include <QLoggingCategory>
#include <QTcpServer>
#include <QTcpSocket>

#include "TelegramServerUser.hpp"
#include "TelegramServerClient.hpp"

#include "CServerTcpTransport.hpp"

Q_LOGGING_CATEGORY(loggingCategoryServer, "telegram.server.main", QtDebugMsg)

namespace Telegram {

namespace Server {

Server::Server(QObject *parent) :
    QObject(parent)
{
    m_serverSocket = new QTcpServer(this);
    connect(m_serverSocket, &QTcpServer::newConnection, this, &Server::onNewConnection);
}

void Server::setDcOption(const TLDcOption &option)
{
    m_dcOption = option;
}

void Server::setServerPrivateRsaKey(const Telegram::RsaKey &key)
{
    m_key = key;
}

bool Server::start()
{
    if (!m_serverSocket->listen(QHostAddress::Any, m_dcOption.port)) {
        qWarning() << "Unable to listen port" << m_dcOption.port;
        return false;
    }
    qDebug() << "Start a server" << m_dcOption.id << "on port" << m_dcOption.port << "Key:" << m_key.fingerprint;
    return true;
}

void Server::loadData()
{
    const int number = 10;
    m_phoneToUserId.reserve(number);
    for (int i = 0; i < number; ++i) {
        User *newUser = new User(this);
        newUser->setPhoneNumber(QString::number(i + 1));
        insertUser(newUser);
    }
}

void Server::onNewConnection()
{
    QTcpSocket *newConnection = m_serverSocket->nextPendingConnection();
    if (newConnection == nullptr) {
        qCDebug(loggingCategoryServer) << "expected pending connection does not exist";
        return;
    }
    qCDebug(loggingCategoryServer) << "A new incoming connection from" << newConnection->peerAddress().toString();
    TcpTransport *transport = new TcpTransport(newConnection, this);
    Client *client = new Client(this);
    client->setServerRsaKey(m_key);
    client->setTransport(transport);
    connect(client, &CTelegramConnection::authStateChanged, [client, this](CTelegramConnection::AuthState status, quint32 dc) mutable {
        Q_UNUSED(dc)

        qDebug() << "USER AUTH CHANGED" << Q_FUNC_INFO << status;
        if (status != CTelegramConnection::AuthStateHaveAKey) {
            return;
        }
        User *newUser = new User();
        Session s;
        s.authId = client->authId();
        s.authKey = client->authKey();
        newUser->addSession(s);
        newUser->setPhoneNumber(QString::number(m_users.count() + 1));
        insertUser(newUser);
    });
}

User *Server::getUser(const QString &phoneNumber)
{
    quint32 id = m_phoneToUserId.value(phoneNumber);
    if (!id) {
        return nullptr;
    }
    return m_users.value(id);
}

User *Server::getUser(quint64 authId)
{
    quint32 id = m_authIdToUserId.value(authId);
    if (!id) {
        return nullptr;
    }
    return m_users.value(id);
}

void Server::insertUser(User *newUser)
{
    qDebug() << Q_FUNC_INFO << newUser << newUser->phoneNumber() << newUser->id();
    m_users.insert(newUser->id(), newUser);
    m_phoneToUserId.insert(newUser->phoneNumber(), newUser->id());
    for (const Session &session : newUser->sessions()) {
        m_authIdToUserId.insert(session.authId, newUser->id());
    }
}

} // Server

} // Telegram
