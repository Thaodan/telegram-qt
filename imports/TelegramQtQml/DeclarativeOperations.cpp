#include "DeclarativeOperations.hpp"

#include <QTimer>
#include <QDebug>

namespace Telegram {

using namespace Client;

DeclarativeClient::DeclarativeClient(QObject *parent) :
    QObject(parent),
    m_client(new Client(this))
{
    connect(m_client, &Client::signedInChanged, this, &DeclarativeClient::signedInChanged);
}

bool DeclarativeClient::isSignedIn() const
{
    return m_client->isSignedIn();
}

DeclarativeOperation::DeclarativeOperation(QObject *parent) :
    QObject(parent)
{
}

DeclarativeClient *DeclarativeOperation::target() const
{
    return m_target;
}

void DeclarativeOperation::start()
{
//    m_running = true;
    emit started();
    startEvent();
}

void DeclarativeOperation::setTarget(DeclarativeClient *target)
{
    m_target = target;
}

void DeclarativeOperation::startEvent()
{
}

DeclarativeAuthOperation::DeclarativeAuthOperation(QObject *parent) :
    DeclarativeOperation(parent)
{
//    connect(this, &DeclarativeAuthOperation::statusChanged, [this](AuthStatus status) {
//        switch (status) {
//        case AuthCodeRequired:
//            emit authCodeRequired();
//            break;
//        case PasswordRequired:
//            setPasswordHint("My hint");
//            emit passwordRequired();
//            break;
//        default:
//            break;
//        }
    //    });
}

bool DeclarativeAuthOperation::isBusy() const
{
    return m_busy;
}

DeclarativeAuthOperation::AuthStatus DeclarativeAuthOperation::status() const
{
    return m_status;
}

QString DeclarativeAuthOperation::phoneNumber() const
{
    return m_phoneNumber;
}

QString DeclarativeAuthOperation::passwordHint() const
{
    return m_authOperation ? m_authOperation->passwordHint() : QString();
}

bool DeclarativeAuthOperation::hasRecovery() const
{
    return m_authOperation ? m_authOperation->hasRecovery() : false;
}

void DeclarativeAuthOperation::abort()
{
    m_authOperation->abort();
}

bool DeclarativeAuthOperation::submitAuthCode(const QString &code)
{
    if (!m_authOperation) {
        return false;
    }
    m_authOperation->submitAuthCode(code);
    setBusy(true);
    return true;
}

bool DeclarativeAuthOperation::submitPassword(const QString &password)
{
    if (!m_authOperation) {
        return false;
    }
    m_authOperation->submitPassword(password);
    setBusy(true);
    return true;
}

void DeclarativeAuthOperation::setPhoneNumber(const QString &phoneNumber)
{
    if (m_phoneNumber == phoneNumber) {
        return;
    }
    m_phoneNumber = phoneNumber;
    setBusy(true);
    emit phoneNumberChanged();
}

bool DeclarativeAuthOperation::recovery()
{
    if (!m_authOperation || !m_authOperation->hasRecovery()) {
        return false;
    }
    m_authOperation->recovery();
    setBusy(true);
    return true;
}

void DeclarativeAuthOperation::setStatus(const AuthStatus status)
{
    qWarning() << Q_FUNC_INFO << status;
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit statusChanged(status);
}

void DeclarativeAuthOperation::setBusy(bool busy)
{
    if (m_busy == busy) {
        return;
    }
    m_busy = busy;
    emit busyChanged(busy);

    QTimer::singleShot(1100, [this]() {
        setBusy(false);
    });
}

void DeclarativeAuthOperation::startEvent()
{
    m_authOperation = m_target->backend()->signIn(m_phoneNumber);
    connect(m_authOperation, &PendingAuthOperation::authCodeRequired, this, &DeclarativeAuthOperation::authCodeRequired);
    connect(m_authOperation, &PendingAuthOperation::passwordRequired, this, &DeclarativeAuthOperation::passwordRequired);

    connect(m_authOperation, &PendingAuthOperation::passwordHintChanged, this, &DeclarativeAuthOperation::passwordHintChanged);
    connect(m_authOperation, &PendingAuthOperation::hasRecoveryChanged, this, &DeclarativeAuthOperation::hasRecoveryChanged);
}

bool DeclarativeAuthOperation::requestCall()
{
    if (!m_authOperation) {
        return false;
    }
    m_authOperation->requestCall();
    return true;
}

bool DeclarativeAuthOperation::requestSms()
{
    if (!m_authOperation) {
        return false;
    }
    m_authOperation->requestSms();
    return true;
}

}
