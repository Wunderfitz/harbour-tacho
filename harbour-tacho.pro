TARGET = harbour-tacho

CONFIG += sailfishapp sailfishapp_i18n

QT += core dbus xml positioning

SOURCES += src/harbour-tacho.cpp \
    src/speedometer.cpp \
    src/variaconnectivity.cpp

DISTFILES += qml/harbour-tacho.qml \
    qml/components/Threat.qml \
    qml/cover/CoverPage.qml \
    qml/pages/AboutPage.qml \
    qml/pages/TitlePage.qml \
    rpm/harbour-tacho.changes \
    rpm/harbour-tacho.spec \
    rpm/harbour-tacho.yaml \
    translations/*.ts \
    harbour-tacho.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172 256x256

TRANSLATIONS += translations/harbour-tacho-de.ts

gui.files = qml
gui.path = /usr/share/$${TARGET}

images.files = images
images.path = /usr/share/$${TARGET}

ICONPATH = /usr/share/icons/hicolor

86.png.path = $${ICONPATH}/86x86/apps/
86.png.files += icons/86x86/harbour-tacho.png

108.png.path = $${ICONPATH}/108x108/apps/
108.png.files += icons/108x108/harbour-tacho.png

128.png.path = $${ICONPATH}/128x128/apps/
128.png.files += icons/128x128/harbour-tacho.png

172.png.path = $${ICONPATH}/172x172/apps/
172.png.files += icons/172x172/harbour-tacho.png

256.png.path = $${ICONPATH}/256x256/apps/
256.png.files += icons/256x256/harbour-tacho.png

tacho.desktop.path = /usr/share/applications/
tacho.desktop.files = harbour-tacho.desktop

INSTALLS += 86.png 108.png 128.png 172.png 256.png \
            tacho.desktop gui images

HEADERS += \
    src/speedometer.h \
    src/variaconnectivity.h
