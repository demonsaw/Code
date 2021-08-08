
include($$PWD/../4_platform.pri)
message("4_core:" $$PLATFORM $$COMPILER $$BUILD $$LIBRARY)

# Config
TEMPLATE = lib
TARGET = 4_core-$$BOOST_LNK

CONFIG *= staticlib 
CONFIG -= app_bundle qt

# Path
DESTDIR = $$DEV_LIB
OBJECTS_DIR = $$DEV_TMP
INCLUDEPATH *= $$PWD $$CRYPTOPP_INC $$CPUID_INC $$MSGPACK_INC $$CPPNETLIB_INC $$BOOST_INC 
DEPENDPATH *= $$CRYPTOPP_LIB $$CPUID_LIB $$MSGPACK_LIB $$CPPNETLIB_LIB $$CRYPTOPP_INC $$CPUID_INC $$MSGPACK_INC $$CPPNETLIB_INC $$BOOST_INC $$LIBS
LIBS *= $$CRYPTOPP_LIB $$CPUID_LIB $$MSGPACK_LIB $$CPPNETLIB_LIB $$BOOST_LIBS 
win32:LIBS *= $$TELEMETRY_OBJ

# Include
include($$PWD/4_core.pri)
