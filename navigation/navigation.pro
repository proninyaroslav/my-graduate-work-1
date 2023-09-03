QT += widgets quick location
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    graph/graph.cpp \
    graph/node.cpp \
    graph/edge.cpp \
    osmparser.cpp \
    quadtree/quadtree.cpp \
    quadtree/treenode.cpp \
    quadtree/bounds.cpp \
    appcontroller.cpp \
    utils.cpp \
    finder/nodequeue.cpp \
    geopoint.cpp \
    exception.cpp \
    finder/astarfinder.cpp \
    finder/nbastarfinder.cpp \
    finder/abstractstate.cpp \
    finder/abstractfinder.cpp \
    finder/nbastartstate.cpp \
    finder/astarstate.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    graph/graph.h \
    graph/node.h \
    graph/edge.h \
    osmparser.h \
    quadtree/quadtree.h \
    quadtree/treenode.h \
    quadtree/bounds.h \
    appcontroller.h \
    utils.h \
    finder/nodequeue.h \
    geopoint.h \
    exception.h \
    graph/id.h \
    finder/astarfinder.h \
    finder/nbastarfinder.h \
    finder/abstractstate.h \
    finder/abstractfinder.h \
    finder/nbastarstate.h \
    finder/astarstate.h
