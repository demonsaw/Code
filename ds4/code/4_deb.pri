
# Build
CONFIG(release,debug|release) {
	BUILD = release
	DEFINES *= NDEBUG QT_NO_DEBUG_OUTPUT QT_NO_DEBUG
	
	# Flags
	QMAKE_LFLAGS += -Wl,-rpath,"'\$$ORIGIN/lib'"
	
	QMAKE_CFLAGS_RELEASE -= -O2
	QMAKE_CXXFLAGS_RELEASE -= -O2
	QMAKE_CFLAGS_RELEASE *= -O3
	QMAKE_CXXFLAGS_RELEASE *= -O3
	
	QMAKE_CFLAGS_WARN_ON -= -Wall
	QMAKE_CXXFLAGS_WARN_ON -= -Wall

	# Remove all debug info
	QMAKE_LFLAGS_RELEASE = -Wl,-s
	QMAKE_STRIP = /usr/bin/strip
	
	# Symbol file
	#QMAKE_CFLAGS_RELEASE *= -g
	#QMAKE_CXXFLAGS_RELEASE *= -g

	# LTCG
	# NOTE: This is broken on gcc 4.9.2
	#QMAKE_CFLAGS *= $$QMAKE_CFLAGS_LTCG
	#QMAKE_CXXFLAGS *= $$QMAKE_CXXFLAGS_LTCG
	#QMAKE_LFLAGS *= $$QMAKE_LFLAGS_LTCG
} else {
	BUILD = debug 
}
	
# Info
COMPILER = gcc8
PLATFORM = linux_32
PLATFORM_TAG = x32
DEFINES *= BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE CRYPTOPP_DISABLE_ASM

# Config
CONFIG -= debug_and_release debug_and_release_target
CONFIG *= -std=c++11 thread exceptions rtti no_batch warn_off
CONFIG(unix, unix|macx|win32) {
	contains(QMAKE_HOST.arch, x86_64) {
		PLATFORM = linux_64
		PLATFORM_TAG = x64
	}
	else {
		CONFIG *= -std=c++0x -std=gnu++11 
		QMAKE_CFLAGS *= -std=c++11 -std=gnu++11
		QMAKE_CXXFLAGS *= -std=c++11 -std=gnu++11
	}
}

!CONFIG(shared,shared|static) {	
	CONFIG *= static
}

# Flags
#QMAKE_CFLAGS += -Wl,--stack,4194304
#QMAKE_CXXFLAGS += -Wl,--stack,4194304

# Code
ROOT = /demonsaw_4
LIB = $$ROOT/lib
SRC = $$ROOT/code

# Build
DEV = $$ROOT/build
DEV_LIB = $$DEV/lib/$$PLATFORM/$$BUILD
DEV_BIN = $$DEV/bin/$$PLATFORM/$$BUILD
DEV_TMP = $$DEV/tmp/$$TARGET/$$PLATFORM/$$BUILD

# Boost
BOOST = $$SRC/0_boost
BOOST_INC = $$BOOST
BOOST_LIB = $$LIB/0_boost/$$PLATFORM
BOOST_SRC = $$BOOST/boost
BOOST_VER = 1_69

BOOST_ATOMIC_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_atomic-", $$COMPILER-$$BOOST_LNK-$$PLATFORM_TAG-$$BOOST_VER, ".a")
BOOST_CHRONO_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_chrono-", $$COMPILER-$$BOOST_LNK-$$PLATFORM_TAG-$$BOOST_VER, ".a")
BOOST_DATE_TIME_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_date_time-", $$COMPILER-$$BOOST_LNK-$$PLATFORM_TAG-$$BOOST_VER, ".a")
BOOST_FILESYSTEM_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_filesystem-", $$COMPILER-$$BOOST_LNK-$$PLATFORM_TAG-$$BOOST_VER, ".a")
BOOST_LOCALE_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_locale-", $$COMPILER-$$BOOST_LNK-$$PLATFORM_TAG-$$BOOST_VER, ".a")
BOOST_REGEX_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_regex-", $$COMPILER-$$BOOST_LNK-$$PLATFORM_TAG-$$BOOST_VER, ".a")
BOOST_SYSTEM_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_system-", $$COMPILER-$$BOOST_LNK-$$PLATFORM_TAG-$$BOOST_VER, ".a")
BOOST_THREAD_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_thread-", $$COMPILER-$$BOOST_LNK-$$PLATFORM_TAG-$$BOOST_VER, ".a")
BOOST_LIBS = $$BOOST_ATOMIC_LIB $$BOOST_CHRONO_LIB $$BOOST_DATE_TIME_LIB $$BOOST_FILESYSTEM_LIB $$BOOST_LOCALE_LIB $$BOOST_REGEX_LIB $$BOOST_SYSTEM_LIB $$BOOST_THREAD_LIB

# 3rd Party
CRYPTOPP = $$SRC/0_cryptopp
CRYPTOPP_INC = $$CRYPTOPP
CRYPTOPP_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/tmp/0_cryptopp/", $$PLATFORM, "/", $$BUILD)
CRYPTOPP_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/lib0_cryptopp-", $$BOOST_LNK, ".a")

CPPNETLIB = $$SRC/0_cppnetlib
CPPNETLIB_INC = $$CPPNETLIB
CPPNETLIB_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/tmp/0_cppnetlib/", $$PLATFORM, "/", $$BUILD)
CPPNETLIB_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/lib0_cppnetlib-", $$BOOST_LNK, ".a")

MSGPACK = $$SRC/0_msgpack
MSGPACK_INC = $$MSGPACK/include
MSGPACK_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/tmp/0_msgpack/", $$PLATFORM, "/", $$BUILD)
MSGPACK_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/lib0_msgpack-", $$BOOST_LNK, ".a")

# Library
CORE = $$SRC/4_core
CORE_INC = $$CORE
CORE_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/tmp/4_core/", $$PLATFORM, "/", $$BUILD)
CORE_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/lib4_core-", $$BOOST_LNK, ".a")

CORE_CLIENT = $$SRC/4_core_client
CORE_CLIENT_INC = $$CORE_CLIENT
CORE_CLIENT_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/tmp/4_core_client/", $$PLATFORM, "/", $$BUILD)
CORE_CLIENT_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/lib4_core_client-", $$BOOST_LNK, ".a")

CORE_GUI = $$SRC/4_core_gui
CORE_GUI_INC = $$CORE_GUI
CORE_GUI_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/tmp/4_core_gui/", $$PLATFORM, "/", $$BUILD)
CORE_GUI_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/lib4_core_gui-", $$BOOST_LNK, ".a")

# HACK: LZ4
#CORE_LIB *= $$CORE_TMP/lz4_lib.o $$CORE_TMP/lz4hc_lib.o
