
include($$PWD/../4_platform.pri)
message("4_client_cli:" $$PLATFORM $$COMPILER $$BUILD $$LIBRARY)
				 
# Config
TEMPLATE = app
TARGET = 4_client_cli

CONFIG *= console
CONFIG -= qt app_bundle

# Path
DESTDIR = $$DEV_BIN
OBJECTS_DIR = $$DEV_TMP
INCLUDEPATH *= $$PWD $$CORE_CLIENT_INC $$CORE_INC $$CRYPTOPP_INC $$CPUID_INC $$MSGPACK_INC $$CPPNETLIB_INC $$BOOST_INC
DEPENDPATH *= $$CORE_CLIENT_LIB $$CORE_LIB $$CRYPTOPP_LIB $$CPUID_LIB $$MSGPACK_LIB $$CPPNETLIB_LIB $$CORE_CLIENT_INC $$CORE_INC $$CRYPTOPP_INC $$CPUID_INC $$MSGPACK_INC $$CPPNETLIB_INC $$BOOST_INC
PRE_TARGETDEPS *= $$CORE_CLIENT_LIB $$CORE_LIB $$CRYPTOPP_LIB
LIBS *= $$CORE_CLIENT_LIB $$CORE_LIB $$CRYPTOPP_LIB $$CPUID_LIB $$MSGPACK_LIB $$CPPNETLIB_LIB $$BOOST_LIBS
win32:LIBS *= $$TELEMETRY_OBJ

# Windows
win32:RC_FILE = $$PWD/4_client_cli.rc
macx:ICON = $$PWD/4_client_cli.icns

# Include
include($$PWD/4_client_cli.pri)
