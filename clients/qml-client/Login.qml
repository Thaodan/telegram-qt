import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.0
import TelegramQt 1.0 as Telegram
import TelegramQtTheme 1.0

import "login"

Item {
    id: loginScreen
    width: 800
    property bool editMode: true
    property string phoneNumber
    property string authCode
    property string firstName
    property string lastName
    property int authError: 0

    Rectangle {
        id: screenBackground
        color: Theme.backgroundColor
        anchors.fill: parent
    }

    Telegram.AuthOperation {
        id: signInOperation
        target: telegramClient
//        phoneNumber: "79222988314"

        onAuthCodeRequired: {
            loginStack.replace("login/EnterCode.qml")
        }

        onPasswordRequired: {
            loginStack.replace("login/EnterPassword.qml")
        }

        onFinished: {
            console.log("Sign in finished:" + succeed)
        }
    }

    ListModel {
        id: phoneCodeModel
        ListElement {
            code: "44"
            country: "United Kingdom"
            length: 10
        }
        ListElement {
            code: "7"
            country: "Russian Federation"
            length: 10
        }
        ListElement {
            code: "34"
            country: "Spain"
            length: 9 // 6 or 7, followed by 8 digits
        }
    }

    signal submitPhone()
    signal submitCode()
    signal submitPassword()
    signal submitName()
    signal recoveryPassword()

    Component {
        id: dummyPage
        AuthScreen {
            Rectangle {
                anchors.fill: parent
                color: Theme.highlightDimmerColor
            }
        }
    }

    StackView {
        id: loginStack
        anchors.fill: parent
        anchors.margins: Theme.paddingMedium
        initialItem: "login/EnterPhone.qml"
        property int current: 0
        onCurrentItemChanged: {
            if (currentItem) {
                currentItem.activate()
            }
        }
        function next() { currentItem.accept() }

        Connections {
            target: loginStack.currentItem
            ignoreUnknownSignals: true
            onSubmitAuthCode: {
                signInOperation.submitAuthCode(authCode)
            }
            onSubmitPassword: {
                signInOperation.submitPassword(password)
            }
            onSubmitPhoneNumber: {
                signInOperation.phoneNumber = phoneNumber
                signInOperation.start()
            }
        }
    }

    Rectangle {
        id: screenBorder
        color: "transparent"
        border.color: Theme.highlightColor
        border.width: 2
        anchors.fill: parent
    }

    Button {
        id: next
        text: "Next"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 80
        anchors.horizontalCenter: loginScreen.horizontalCenter
        onClicked: loginStack.next()
    }

    BusyIndicator {
        running: signInOperation.busy
        anchors.centerIn: parent
    }
}
