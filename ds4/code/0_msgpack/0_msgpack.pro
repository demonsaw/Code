
# Include
include($$PWD/../4_platform.pri)
message("0_msgpack:" $$PLATFORM $$COMPILER $$BUILD)

# Config
TEMPLATE = lib
TARGET = 0_msgpack-$$BOOST_LNK

CONFIG *= staticlib
CONFIG -= app_bundle qt

# Path
DESTDIR = $$DEV_LIB
OBJECTS_DIR = $$DEV_TMP
INCLUDEPATH *= $$PWD $$MSGPACK_INC
win32:LIBS *= $$TELEMETRY_OBJ

include($$PWD/0_msgpack.pri)
