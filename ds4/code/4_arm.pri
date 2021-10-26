
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
COMPILER = gcc49
PLATFORM = arm_32

# Config
CONFIG *= -std=c++11 -std=c++0x -std=gnu++11 static thread exceptions rtti no_batch warn_off
QMAKE_CFLAGS *= -std=c++11 -std=gnu++11
QMAKE_CXXFLAGS *= -std=c++11 -std=gnu++11

# Code
SRC = /code

# Dev
DEV = /build
DEV_LIB = $$DEV/_lib/$$PLATFORM/$$BUILD
DEV_BIN = $$DEV/_bin/$$PLATFORM/$$BUILD
DEV_TMP = $$DEV/_tmp/$$TARGET/$$PLATFORM/$$BUILD

# Boost
# HACK: cppnetlib doesn't use latest boost cause they fucking suck
BOOST = $$SRC/boost_1_64_0
BOOST_INC = $$BOOST
BOOST_LIB = $$BOOST/lib/$$PLATFORM
BOOST_SRC = $$BOOST/boost
BOOST_VER = 1_64

ATOMIC_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_atomic-", $$COMPILER-$$BOOST_LNK-$$BOOST_VER, ".a")
CHRONO_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_chrono-", $$COMPILER-$$BOOST_LNK-$$BOOST_VER, ".a")
DATE_TIME_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_date_time-", $$COMPILER-$$BOOST_LNK-$$BOOST_VER, ".a")
FILESYSTEM_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_filesystem-", $$COMPILER-$$BOOST_LNK-$$BOOST_VER, ".a")
LOCALE_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_locale-", $$COMPILER-$$BOOST_LNK-$$BOOST_VER, ".a")
REGEX_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_regex-", $$COMPILER-$$BOOST_LNK-$$BOOST_VER, ".a")
SYSTEM_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_system-", $$COMPILER-$$BOOST_LNK-$$BOOST_VER, ".a")
THREAD_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_thread-", $$COMPILER-$$BOOST_LNK-$$BOOST_VER, ".a")
BOOST_LIBS = $$ATOMIC_LIB $$CHRONO_LIB $$DATE_TIME_LIB $$FILESYSTEM_LIB $$LOCALE_LIB $$REGEX_LIB $$SYSTEM_LIB $$THREAD_LIB

BOOST_LEGACY = $$SRC/boost_1_59_0
BOOST_LEGACY_INC = $$BOOST_LEGACY
BOOST_LEGACY_LIB = $$BOOST_LEGACY/lib/$$PLATFORM
BOOST_LEGACY_SRC = $$BOOST_LEGACY/boost
BOOST_LEGACY_VER = 1_59

LEGACY_DATE_TIME_LIB = $$sprintf("%1%2%3%4", $$BOOST_LEGACY_LIB, "/libboost_date_time-", $$COMPILER-$$BOOST_LNK-$$BOOST_LEGACY_VER, ".a")
LEGACY_SYSTEM_LIB = $$sprintf("%1%2%3%4", $$BOOST_LEGACY_LIB, "/libboost_system-", $$COMPILER-$$BOOST_LNK-$$BOOST_LEGACY_VER, ".a")
BOOST_LEGACY_LIBS = $$LEGACY_DATE_TIME_LIB $$LEGACY_SYSTEM_LIB 

# 3rd Party
CRYPTOPP = $$SRC/0_cryptopp
CRYPTOPP_INC = $$CRYPTOPP
CRYPTOPP_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/_tmp/0_cryptopp/", $$PLATFORM, "/", $$BUILD)
CRYPTOPP_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/lib0_cryptopp-", $$BOOST_LNK, ".a")

CPPNETLIB = $$SRC/0_cppnetlib
CPPNETLIB_INC = $$CPPNETLIB
CPPNETLIB_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/_tmp/0_cppnetlib/", $$PLATFORM, "/", $$BUILD)
CPPNETLIB_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/lib0_cppnetlib-", $$BOOST_LNK, ".a")

LIBCPUID = $$SRC/0_libcpuid
LIBCPUID_INC = $$LIBCPUID
LIBCPUID_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/_tmp/0_libcpuid/", $$PLATFORM, "/", $$BUILD)
LIBCPUID_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/lib0_libcpuid-", $$BOOST_LNK, ".a")

MSGPACK = $$SRC/0_msgpack
MSGPACK_INC = $$MSGPACK/include
MSGPACK_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/_tmp/0_msgpack/", $$PLATFORM, "/", $$BUILD)
MSGPACK_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/lib0_msgpack-", $$BOOST_LNK, ".a")

# Library
CORE = $$SRC/4_core
CORE_INC = $$CORE
CORE_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/_tmp/4_core/", $$PLATFORM, "/", $$BUILD)
CORE_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/lib4_core-", $$BOOST_LNK, ".a")

