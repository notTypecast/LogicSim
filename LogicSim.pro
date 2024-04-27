TEMPLATE = app
TARGET = logicsim

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += ./include/

SOURCES += \
    main.cpp \
    src/model/circuit.cpp \
    src/model/component.cpp \
    src/model/gates.cpp \
    src/model/inputs.cpp \
    src/model/memory.cpp \
    src/model/outputs.cpp \
    src/utils.cpp \
    src/gui/main_window.cpp \
    src/gui/tab_handler.cpp \
    src/gui/design_area.cpp \
    src/gui/component_label.cpp \
    src/gui/resource_loader.cpp \
    src/gui/wire.cpp \
    src/gui/properties.cpp \
    src/gui/undo_commands.cpp \

HEADERS += \
    include/model/circuit.hpp \
    include/model/component.hpp \
    include/model/gates.hpp \
    include/model/inputs.hpp \
    include/model/mapped_data.hpp \
    include/model/memory.hpp \
    include/model/outputs.hpp \
    include/utils.hpp \
    include/gui/main_window.hpp \
    include/gui/tab_handler.hpp \
    include/gui/design_area.hpp \
    include/gui/component_label.hpp \
    include/gui/resource_loader.hpp \
    include/gui/wire.hpp \
    include/gui/properties.hpp \
    include/gui/undo_commands.hpp

DISTFILES += \
    saves/flip_flop.lsc \
    saves/incomplete.lsc \
    saves/keypad_display.lsc \
    saves/simple_circuit.lsc

FORMS += \
    ui/main_window.ui
