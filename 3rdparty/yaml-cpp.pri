# yaml-cpp 库集成
# 版本: 0.7.0

YAML_CPP_ROOT = $$PWD/yaml-cpp-yaml-cpp-0.7.0

INCLUDEPATH += $$YAML_CPP_ROOT/include

HEADERS += \
    $$YAML_CPP_ROOT/src/collectionstack.h \
    $$YAML_CPP_ROOT/src/directives.h \
    $$YAML_CPP_ROOT/src/emitterstate.h \
    $$YAML_CPP_ROOT/src/emitterutils.h \
    $$YAML_CPP_ROOT/src/exp.h \
    $$YAML_CPP_ROOT/src/indentation.h \
    $$YAML_CPP_ROOT/src/nodebuilder.h \
    $$YAML_CPP_ROOT/src/nodeevents.h \
    $$YAML_CPP_ROOT/src/ptr_vector.h \
    $$YAML_CPP_ROOT/src/regex_yaml.h \
    $$YAML_CPP_ROOT/src/regeximpl.h \
    $$YAML_CPP_ROOT/src/scanner.h \
    $$YAML_CPP_ROOT/src/scanscalar.h \
    $$YAML_CPP_ROOT/src/scantag.h \
    $$YAML_CPP_ROOT/src/setting.h \
    $$YAML_CPP_ROOT/src/singledocparser.h \
    $$YAML_CPP_ROOT/src/stream.h \
    $$YAML_CPP_ROOT/src/streamcharsource.h \
    $$YAML_CPP_ROOT/src/stringsource.h \
    $$YAML_CPP_ROOT/src/tag.h \
    $$YAML_CPP_ROOT/src/token.h

SOURCES += \
    $$YAML_CPP_ROOT/src/binary.cpp \
    $$YAML_CPP_ROOT/src/convert.cpp \
    $$YAML_CPP_ROOT/src/depthguard.cpp \
    $$YAML_CPP_ROOT/src/directives.cpp \
    $$YAML_CPP_ROOT/src/emit.cpp \
    $$YAML_CPP_ROOT/src/emitfromevents.cpp \
    $$YAML_CPP_ROOT/src/emitter.cpp \
    $$YAML_CPP_ROOT/src/emitterstate.cpp \
    $$YAML_CPP_ROOT/src/emitterutils.cpp \
    $$YAML_CPP_ROOT/src/exceptions.cpp \
    $$YAML_CPP_ROOT/src/exp.cpp \
    $$YAML_CPP_ROOT/src/memory.cpp \
    $$YAML_CPP_ROOT/src/node.cpp \
    $$YAML_CPP_ROOT/src/node_data.cpp \
    $$YAML_CPP_ROOT/src/nodebuilder.cpp \
    $$YAML_CPP_ROOT/src/nodeevents.cpp \
    $$YAML_CPP_ROOT/src/null.cpp \
    $$YAML_CPP_ROOT/src/ostream_wrapper.cpp \
    $$YAML_CPP_ROOT/src/parse.cpp \
    $$YAML_CPP_ROOT/src/parser.cpp \
    $$YAML_CPP_ROOT/src/regex_yaml.cpp \
    $$YAML_CPP_ROOT/src/scanner.cpp \
    $$YAML_CPP_ROOT/src/scanscalar.cpp \
    $$YAML_CPP_ROOT/src/scantag.cpp \
    $$YAML_CPP_ROOT/src/scantoken.cpp \
    $$YAML_CPP_ROOT/src/simplekey.cpp \
    $$YAML_CPP_ROOT/src/singledocparser.cpp \
    $$YAML_CPP_ROOT/src/stream.cpp \
    $$YAML_CPP_ROOT/src/tag.cpp
