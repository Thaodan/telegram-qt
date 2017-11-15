QT += core network
CONFIG += c++11

TEMPLATE = app

INCLUDEPATH += $$PWD/../TelegramQt

LIBS += -lssl -lcrypto -lz
LIBS += -L$$OUT_PWD/../TelegramQt
LIBS += -lTelegramQt$${QT_MAJOR_VERSION}

TARGET = testApp

SOURCES = main.cpp \
    ServerDhLayer.cpp \
    TelegramServer.cpp \
    TelegramServerClient.cpp \
    TelegramServerUser.cpp \
    CServerTcpTransport.cpp

#OTHER_FILES += CMakeLists.txt

HEADERS += \
    ServerDhLayer.hpp \
    TelegramServer.hpp \
    TelegramServerClient.hpp \
    TelegramServerUser.hpp \
    CServerTcpTransport.hpp
