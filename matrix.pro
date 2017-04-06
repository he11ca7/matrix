QT += core
TEMPLATE = lib

OBJECTS_DIR = obj
MOC_DIR = moc
CONFIG += staticlib
DESTDIR = ../libs

INCLUDEPATH += \
    headers

HEADERS += \
    headers/matrix.h \
    headers/defines.h

SOURCES += \
    sources/matrix.cpp

# Определение разрядности
ARCH_STR = _x86
contains(QMAKE_HOST.arch, x86_64):{
  ARCH_STR = _x64
}

win32 {
  CONFIG(debug, debug|release) {
    TARGET  = _matrix_wd$${ARCH_STR}
  } else {
    TARGET  = _matrix_w$${ARCH_STR}
  }
}

unix {
  CONFIG(debug, debug|release) {
    TARGET  = _matrix_ud$${ARCH_STR}
  } else {
    TARGET  = _matrix_u$${ARCH_STR}
  }
}