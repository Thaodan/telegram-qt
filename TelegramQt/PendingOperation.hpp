#ifndef TELEGRAMQT_PENDING_OPERATION
#define TELEGRAMQT_PENDING_OPERATION

#include <QObject>
#include <QVariantHash>

#include "TelegramNamespace.hpp"

namespace Telegram {

class PendingOperation : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(PendingOperation)
public:
    explicit PendingOperation(QObject *parent = nullptr);
    bool isFinished() const;
    bool isSucceeded() const;

Q_SIGNALS:
    void finished(PendingOperation *operation);
    void succeeded(PendingOperation *operation);
    void failed(PendingOperation *operation, const QVariantHash &details);

public Q_SLOTS:
//    void start();
    void setFinished();
    void setFinishedWithError(const QVariantHash &details);
    void setDelayedFinishedWithError(const QVariantHash &details);

private:
    QVariantHash m_errorDetails;
    bool m_finished;
    bool m_succeeded;
};

class PendingRpcOperation : public PendingOperation
{
    Q_OBJECT
    Q_DISABLE_COPY(PendingRpcOperation)
public:
    explicit PendingRpcOperation(QObject *parent = nullptr);

    Peer peer() const { return m_peer; }

public Q_SLOTS:
    void setPeer(const Peer &peer);

private:
    friend class ::CTelegramDispatcher;
    Peer m_peer;
    quint64 m_requestId;
};

class PendingAuthOperation : public PendingOperation
{
    Q_OBJECT
public:
    explicit PendingAuthOperation(QObject *parent = nullptr);

    QString phoneNumber() const { return m_phoneNumber; }
    QString passwordHint() const { return m_passwordHint; }
    bool hasRecovery() const { return m_hasRecovery; }

public slots:
    void abort();

    void submitAuthCode(const QString &code);
    void submitPassword(const QString &password);

    void requestCall();
    void requestSms();
    void recovery();

Q_SIGNALS:
    void authCodeRequired();
    void passwordRequired();
//    void callAvailable();

    void passwordHintChanged(const QString &hint);
    void hasRecoveryChanged(bool hasRecovery);

    void authSignErrorReceived(TelegramNamespace::AuthSignError errorCode, const QString &errorMessage); // Error message description: https://core.telegram.org/api/errors#400-bad-request
    void authorizationErrorReceived(TelegramNamespace::UnauthorizedError errorCode, const QString &errorMessage);

protected:
    void setPasswordHint(const QString &hint);

    QString m_phoneNumber;
    QString m_passwordHint;
    bool m_hasRecovery;
};

class PlainPacketOperation : public PendingOperation
{
    Q_OBJECT
public:
    explicit PlainPacketOperation(const QByteArray &requestData, QObject *parent = nullptr);

    QByteArray requestData() const { return m_requestData; }
    QByteArray replyData() const { return m_replyData; }
    void setReplyData(const QByteArray &data) { m_replyData = data; }

    quint64 requestId() const { return m_requestId; } // RPC message id
    void setRequestId(quint64 id) { m_requestId = id; }

signals:
    void finished(PlainPacketOperation *operation);

protected:
    quint64 m_requestId;
    QByteArray m_replyData;
    QByteArray m_requestData;
};

}

#endif // TELEGRAMQT_PENDING_OPERATION
