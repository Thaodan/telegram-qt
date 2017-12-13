#include "PendingOperation.hpp"

#include <QDebug>
#include <QTimer>

namespace Telegram {

PendingOperation::PendingOperation(QObject *parent) :
    QObject(parent),
    m_finished(false),
    m_succeeded(true)
{
}

bool PendingOperation::isFinished() const
{
    return m_finished;
}

bool PendingOperation::isSucceeded() const
{
    return m_finished && m_succeeded;
}

void PendingOperation::setFinished()
{
    if (m_finished) {
        qWarning() << "Operation is already finished" << this;
        return;
    }
    m_finished = true;
    if (m_succeeded) {
        emit succeeded(this);
    }
    emit finished(this);
}

void PendingOperation::setFinishedWithError(const QVariantHash &details)
{
    qDebug() << "finished with error" << this << details;
    m_succeeded = false;
    m_errorDetails = details;
    emit failed(this, details);
    setFinished();
}

void PendingOperation::setDelayedFinishedWithError(const QVariantHash &details)
{
    QMetaObject::invokeMethod(this, "setFinishedWithError", Qt::QueuedConnection, Q_ARG(QVariantHash, details)); // Invoke after return
}

void PendingRpcOperation::setPeer(const Peer &peer)
{
    m_peer = peer;
}

PendingAuthOperation::PendingAuthOperation(QObject *parent) :
    PendingOperation(parent)
{
}

void PendingAuthOperation::abort()
{
    qWarning() << Q_FUNC_INFO << "STUB";
}

void PendingAuthOperation::submitAuthCode(const QString &code)
{
    qWarning() << Q_FUNC_INFO << "STUB" << code;

    QTimer::singleShot(1000, [this]() {
        setPasswordHint(QStringLiteral("My hint"));
        emit passwordRequired();
    });
}

void PendingAuthOperation::submitPassword(const QString &password)
{
    qWarning() << Q_FUNC_INFO << "STUB" << password;
    QTimer::singleShot(1000, this, &PendingAuthOperation::setFinished);
}

void PendingAuthOperation::requestCall()
{
    qWarning() << Q_FUNC_INFO << "STUB";
}

void PendingAuthOperation::requestSms()
{
    qWarning() << Q_FUNC_INFO << "STUB";
}

void PendingAuthOperation::recovery()
{
    qWarning() << Q_FUNC_INFO << "STUB";
}

void PendingAuthOperation::setPasswordHint(const QString &hint)
{
    if (m_passwordHint == hint) {
        return;
    }
    m_passwordHint = hint;
    emit passwordHintChanged(hint);
}

}
