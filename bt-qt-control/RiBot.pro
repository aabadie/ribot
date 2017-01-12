QT += quick bluetooth core

TEMPLATE = app

TARGET = ribot

CONFIG += app_bundle

SOURCES += main.cpp

RESOURCES += ribot.qrc

OTHER_FILES += qml/main.qml\
               qml/Button.qml\
               qml/Scanner.qml\
               qml/Joystick.qml

INSTALLS += target

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
