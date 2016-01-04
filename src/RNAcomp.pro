TEMPLATE = app

CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    RnaIndex.cpp \
    Query.cpp \
    RnaMatch.cpp \
    Util.cpp \
    RnaDataBase.cpp \
    QueryBuilder.cpp \
    OptimalQuerySequence.cpp \
    RnaAlignment.cpp \
    IsomirQuerySequence.cpp \
    IsomirMatch.cpp

#include(deployment.pri)
#qtcAddDeployment()

HEADERS += \
    RnaIndex.h \
    Query.h \
    QueryResult.h \
    QueryBuilder.h \
    BlockMeta.h \
    RnaDataBase.h \
    RnaMatch.h \
    Util.h \
    OptimalQuerySequence.h \
    RnaAlignment.h \
    QueryMeta.h \
    VariadicList.h \
    OptimalQuerySequenceTemplate.h \
    main.h \
    IsomirQuerySequence.h \
    IsomirMatch.h

CONFIG += c++11

#LIBS += -pthread

#DEFINES += RNACOMP_COMPUTE_OPTIMAL_QUERY_SEQUENCE

#QMAKE_CXXFLAGS_DEBUG += -gdwarf-3
QMAKE_CXXFLAGS_DEBUG += -g
QMAKE_CXXFLAGS += -std=c++11
