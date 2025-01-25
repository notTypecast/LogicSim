TEMPLATE = app
TARGET = logicsim

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32:RC_ICONS += res/logo.ico

CONFIG += c++17
DEFINES += QT_DISABLE_DEPRECATED_UP_TO=0x050F00

INCLUDEPATH += ./include/

SOURCES += \
    main.cpp \
    src/model/circuit.cpp \
    src/model/component.cpp \
    src/model/gates.cpp \
    src/model/inputs.cpp \
    src/model/memory.cpp \
    src/model/outputs.cpp \
    src/model/control.cpp \
    src/utils.cpp \
    src/gui/main_window.cpp \
    src/gui/tab_handler.cpp \
    src/gui/design_area.cpp \
    src/gui/component_label.cpp \
    src/gui/resource_loader.cpp \
    src/gui/wire.cpp \
    src/gui/properties.cpp \
    src/gui/undo_commands.cpp \
    src/gui/clipboard.cpp \
    src/gui/doc_window.cpp

HEADERS += \
    include/model/circuit.hpp \
    include/model/component.hpp \
    include/model/gates.hpp \
    include/model/inputs.hpp \
    include/model/mapped_data.hpp \
    include/model/memory.hpp \
    include/model/outputs.hpp \
    include/model/control.hpp \
    include/utils.hpp \
    include/gui/main_window.hpp \
    include/gui/tab_handler.hpp \
    include/gui/design_area.hpp \
    include/gui/component_label.hpp \
    include/gui/resource_loader.hpp \
    include/gui/wire.hpp \
    include/gui/properties.hpp \
    include/gui/undo_commands.hpp \
    include/gui/clipboard.hpp \
    include/gui/doc_window.hpp

DISTFILES += \
    saves/flip_flop.lsc \
    saves/incomplete.lsc \
    saves/keypad_display.lsc \
    saves/simple_circuit.lsc

FORMS += \
    ui/doc_window.ui \
    ui/main_window.ui
