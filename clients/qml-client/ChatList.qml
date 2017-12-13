import QtQuick 2.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.0
import TelegramQt 1.0
import TelegramQtTheme 1.0

Item {
    id: loginScreen
    width: 800
    height: 600
    property bool editMode: true
    property string phoneNumber
    property string authCode
    property string firstName
    property string lastName
    property int authError: 0

    Rectangle {
        id: screenBackground
        color: Theme.highlightDimmerColor
        anchors.fill: parent
    }

    ListModel {
        id: contactsModel
        ListElement {
            firstName: "Albert"
            lastName: "Einstein"
            lastMessage: "Hello"
        }
        ListElement {
            firstName: "Ernest"
            lastName: "Hemingway"
            lastMessage: "Hello"
        }
        ListElement {
            firstName: "Hans"
            lastName: "Gude"
            lastMessage: "Hello"
        }
    }

    ListView {
        id: listView
        anchors.fill: parent
        topMargin: 48
        leftMargin: 48
        bottomMargin: 48
        rightMargin: 48
        spacing: 20
        model: contactsModel
        delegate: Item {
            RowLayout {
                spacing: Theme.paddingSmall
                Rectangle {
                    color: "black"
                    width: 32
                    height: 32
                    Layout.alignment: Qt.AlignVCenter
                }
                Column {
                    spacing: Theme.paddingSmall
                    Label {
                        text: model.firstName + " " + model.lastName
                        font.pixelSize: Theme.fontSizeMedium
                    }
                    Label {
                        text: model.lastMessage
                        elide: Text.ElideRight
                        font.pixelSize: Theme.fontSizeSmall
                    }
                }
            }
            width: listView.width - listView.leftMargin - listView.rightMargin
            height: 38
        }
    }

    Rectangle {
        id: screenBorder
        color: "transparent"
        border.color: Theme.highlightColor
        border.width: 2
        anchors.fill: parent
    }
}
