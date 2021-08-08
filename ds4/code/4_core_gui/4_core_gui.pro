
include($$PWD/../4_platform.pri)
message("4_core_gui:" $$PLATFORM $$COMPILER $$BUILD $$LIBRARY)

# Config
TEMPLATE = lib
TARGET = 4_core_gui-$$BOOST_LNK

QT *= widgets
CONFIG *= staticlib
CONFIG -= app_bundle

# Path
DESTDIR = $$DEV_LIB
OBJECTS_DIR = $$DEV_TMP
GENERATED_DIR = $$DEV_TMP/generated
INCLUDEPATH *= $$PWD $$GENERATED_DIR $$CORE_INC $$CRYPTOPP_INC $$CPUID_INC $$MSGPACK_INC $$CPPNETLIB_INC $$BOOST_INC
DEPENDPATH *= $$CORE_LIB $$CRYPTOPP_LIB $$CPUID_LIB $$MSGPACK_LIB $$CPPNETLIB_LIB $$CORE_INC $$CRYPTOPP_INC $$CPUID_INC $$MSGPACK_INC $$CPPNETLIB_INC $$BOOST_INC 
PRE_TARGETDEPS *= $$CORE_LIB $$CRYPTOPP_LIB
LIBS *= $$CORE_LIB $$CRYPTOPP_LIB $$CPUID_LIB $$MSGPACK_LIB $$CPPNETLIB_LIB $$BOOST_LIBS
win32:LIBS *= $$TELEMETRY_OBJ

# Include
include($$PWD/4_core_gui.pri)