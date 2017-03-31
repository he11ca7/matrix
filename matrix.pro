QT += core
TEMPLATE = lib

OBJECTS_DIR = obj
MOC_DIR = moc
CONFIG += staticlib
DESTDIR = ../libs

HEADERS += \
    matrix.h \
    defines.h

SOURCES += \
    matrix.cpp

# Определение разрядности
ARCH_STR = _x86
contains(QMAKE_HOST.arch, x86_64):{
  ARCH_STR = _x64
}

win32 {
  CONFIG(debug, debug|release) {
    TARGET  = matrix_wd$${ARCH_STR}
  } else {
    TARGET  = matrix_w$${ARCH_STR}
  }
}

unix {
  CONFIG(debug, debug|release) {
    TARGET  = matrix_ud$${ARCH_STR}
  } else {
    TARGET  = matrix_u$${ARCH_STR}
  }
}