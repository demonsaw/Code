
include($$PWD/../4_platform.pri)
message("0_cppnetlib:" $$PLATFORM $$COMPILER $$BUILD $$LIBRARY)

# Config
TEMPLATE = lib
TARGET = 0_cppnetlib-$$BOOST_LNK

CONFIG *= staticlib
CONFIG -= app_bundle qt

# Path
DESTDIR = $$DEV_LIB
OBJECTS_DIR = $$DEV_TMP
INCLUDEPATH *= $$PWD $$BOOST_INC
DEPENDPATH *= $$BOOST_INC
win32:LIBS *= $$TELEMETRY_OBJ

# Include
include($$PWD/0_cppnetlib.pri)
