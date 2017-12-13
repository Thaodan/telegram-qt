#ifndef TELEGRAM_DECLARATIVE_OPERATIONS_HPP
#define TELEGRAM_DECLARATIVE_OPERATIONS_HPP

#include <QObject>

#include "../../TelegramQt/TelegramNamespace.hpp"
#include "../../TelegramQt/Client.hpp"

namespace Telegram {

namespace Client {

class DeclarativeClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool signedIn READ isSignedIn NOTIFY signedInChanged)
public:
    explicit DeclarativeClient(QObject *parent = nullptr);

    bool isSignedIn() const;
    Client *backend() { return m_client; }

signals:
    void signedInChanged(bool signedIn);

protected:
    Client *m_client;
};

class DeclarativeOperation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DeclarativeClient *target READ target WRITE setTarget NOTIFY targetChanged)
public:
    explicit DeclarativeOperation(QObject *parent = nullptr);

    DeclarativeClient *target() const;

public slots:
    void start();
    void setTarget(DeclarativeClient *target);

Q_SIGNALS:
    void targetChanged();

    void started();
    void finished();
    void succeeded();
    void failed(const QVariantHash &details);

protected:
    virtual void startEvent();

    DeclarativeClient *m_target;
};

class DeclarativeAuthOperation : public DeclarativeOperation
{
    Q_OBJECT
    Q_PROPERTY(QString phoneNumber READ phoneNumber WRITE setPhoneNumber NOTIFY phoneNumberChanged)
    Q_PROPERTY(QString passwordHint READ passwordHint NOTIFY passwordHintChanged)
    Q_PROPERTY(bool hasRecovery READ hasRecovery NOTIFY hasRecoveryChanged)
//    Q_PROPERTY(bool callAvailable READ isCallAvailable NOTIFY callAvailableChanged)
    Q_PROPERTY(AuthStatus status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
public:
    explicit DeclarativeAuthOperation(QObject *parent = nullptr);

    enum AuthStatus {
        Idle,
        Connecting,
        Handshake,
        Phone,
        AuthCodeRequired,
        PasswordRequired,
        SignedIn
    };
    Q_ENUM(AuthStatus)

    bool isBusy() const;
    AuthStatus status() const;

    QString phoneNumber() const;
    QString passwordHint() const;
    bool hasRecovery() const;

public slots:
    void abort();

    bool submitAuthCode(const QString &code);
    bool submitPassword(const QString &password);

    void setPhoneNumber(const QString &phoneNumber);

    bool recovery();

    bool requestCall();
    bool requestSms();

Q_SIGNALS:
    void busyChanged(bool busy);

    void passwordHintChanged(const QString &hint);
    void hasRecoveryChanged();

    void authCodeRequired();
    void passwordRequired();

    void phoneNumberChanged();
//    void callAvailable();

    void statusChanged(AuthStatus newStatus);

    void authSignErrorReceived(TelegramNamespace::AuthSignError errorCode, const QString &errorMessage); // Error message description: https://core.telegram.org/api/errors#400-bad-request
    void authorizationErrorReceived(TelegramNamespace::UnauthorizedError errorCode, const QString &errorMessage);

protected:
    void setStatus(const DeclarativeAuthOperation::AuthStatus status);
    void setBusy(bool busy);

    PendingAuthOperation *m_authOperation;

    void startEvent() override;

    bool m_busy = false;
    AuthStatus m_status;
    QString m_phoneNumber;

};

class DeclarativeSignOperation : public DeclarativeAuthOperation
{
    Q_OBJECT
public:
    explicit DeclarativeSignOperation(QObject *parent = nullptr);

};

} // Client

} // Telegram

#endif // TELEGRAM_DECLARATIVE_OPERATIONS_HPP
