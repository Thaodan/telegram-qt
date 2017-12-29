import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.0
import TelegramQtTheme 1.0

AuthScreen {
    id: baseColumn
    objectName: "AuthEnterNameComponent"
    width: loginStack.width
    title: qsTr("Your name")
    description: qsTr("Enter your name and add a profile picture")
    signal submitName(string firstName, string lastName)

    onSubmit: submitName(firstNameInput.text, lastNameInput.text)
    onActivate: firstNameInput.forceActiveFocus()

    TextField {
        id: firstNameInput
        placeholderText: qsTr("First name")
        font.pixelSize: theme.fontSizeMedium
        onAccepted: lastNameInput.focus = true
    }
    TextField {
        id: lastNameInput
        placeholderText: qsTr("Last name")
        font.pixelSize: theme.fontSizeMedium
        onAccepted: baseColumn.accept()
    }
    Item {
        id: spacer
        Layout.fillHeight: true
    }
}
