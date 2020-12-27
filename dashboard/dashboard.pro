TEMPLATE = app
TARGET = dashboard
INCLUDEPATH += .
QT += quick

SOURCES += \
    main.cpp \
    thread.cpp

RESOURCES += \
    dashboard.qrc

OTHER_FILES += \
    qml/dashboard.qml \
    qml/DashboardGaugeStyle.qml \
    qml/ValueSource.qml

target.path = $$[QT_INSTALL_EXAMPLES]/quickcontrols/extras/dashboard
INSTALLS += target

HEADERS += \
    thread.h

LIBS += -L$$PWD
LIBS += -lmyaio

INCLUDEPATH += $$PWD/../libmyaio/inc
DEPENDPATH += $$PWD/../libmyaio/inc
