
QT += core network testlib
TEMPLATE = app
CONFIG += c++11

INCLUDEPATH += $$PWD/../TelegramQt
INCLUDEPATH += $$PWD/../server

LIBS += -lssl -lcrypto

LIBS += -L$$OUT_PWD/../../TelegramQt
LIBS += -lTelegramQt$${QT_MAJOR_VERSION}

SOURCES += $$PWD/../server/ServerDhLayer.cpp
SOURCES += $$PWD/../server/TelegramServer.cpp
SOURCES += $$PWD/../server/TelegramServerClient.cpp
SOURCES += $$PWD/../server/TelegramServerUser.cpp
SOURCES += $$PWD/../server/CServerTcpTransport.cpp

HEADERS += $$PWD/../server/ServerDhLayer.hpp
HEADERS += $$PWD/../server/TelegramServer.hpp
HEADERS += $$PWD/../server/TelegramServerClient.hpp
HEADERS += $$PWD/../server/TelegramServerUser.hpp
HEADERS += $$PWD/../server/CServerTcpTransport.hpp
