QT += core network widgets gui
CONFIG += c++17 strict_c++ warn_on release
TARGET = Fydelis-Mail
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/mailcore.cpp \
    src/crypto_fydelis.cpp

HEADERS += \
    include/fydelis_mail.h \
    src/mailcore.h \
    src/auth_guard.h

RESOURCES += resources/hacker_style.qrc
DEFINES += FIDYELIS_MAIL_VERSION=\\\"1.0.0-firebird\\\"