import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import TelegramQt 1.0 as Telegram
import TelegramQtTheme 1.0

ApplicationWindow {
    id: window
    visible: true
    title: qsTr("TelegramQt Example")

//    visibility: Window.Maximized

//    width: recommendedDialogWidth
//    height: recommendedDialogHeight
    width: Screen.width * 1/4
    height: Screen.height * 2/4

    readonly property int recommendedDialogWidth: width < Screen.width * 2/4 ? width : Screen.width * 2/4
    readonly property int recommendedDialogHeight: height < Screen.height * 2/4 ? height : Screen.height * 2/4

//    header: Label {
//        text: view.currentItem.title
//        horizontalAlignment: Text.AlignHCenter
//    }

//    Telegram.AppInformation {
//        id: appInfo
//        appId: 14617
//        appHash: "e17ac360fd072f83d5d08db45ce9a121" // Telepathy-Morse app hash
//        appVersion: "0.1"
//        deviceInfo: "pc"
//        osInfo: "GNU/Linux"
//        languageCode: "en"
//    }

    QtObject {
        id: theme
        readonly property int fontSizeSmall: Theme.fontSizeSmall
        readonly property int fontSizeMedium: Theme.fontSizeMedium
        readonly property int fontSizeLarge: Theme.fontSizeLarge
    }

//    AccountStorage {
    QtObject {
        id: accountStorage
        property bool hasAccount: false
//        property bool hasAccount: true


    }
    //        Telegram.AccountSecretHelper {
    //            id: secretHelper
    //            secretDirectory: StandardPaths.writableLocation(StandardPaths.HomeLocation) + "/.cache/telepathy-morse/secrets"
    //            phoneNumber: phoneNumberField.text
    //            format: AccountSecretHelper.FormatBinary
    //        }
//    Telegram.Settings {
//        id: clientSettings
//        pingInterval: 15000
//        proxy.host: "192.168.0.103"
//        proxy.port: 9050
//        serverConfiguration: [
//            Telegram.ServerOption {
//                ipAddress: "149.154.175.50"
//                port: 443
//            },
//            Telegram.ServerOption {
//                ipAddress: "149.154.167.51"
//                port: 443
//            }
//        ]
//    }

    Telegram.Client {
        id: telegramClient
//        updatesEnabled: false
//        applicationInformation: appInfo
//        settings: clientSettings
    }

    Loader {
        id: loginScreen
        active: visible
        anchors.centerIn: parent
        width: recommendedDialogWidth
        height: recommendedDialogHeight
        visible: !telegramClient.signedIn
        sourceComponent: Login {
        }
    }

    Loader {
        id: dialogsScreen
        active: visible
        visible: telegramClient.signedIn
        sourceComponent: ChatList {

        }
    }

    Shortcut {
        sequence: StandardKey.Quit
        onActivated: window.close()
    }
}
