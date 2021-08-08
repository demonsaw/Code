
# Build
CONFIG(release,debug|release) {
	BUILD = release
	DEFINES *= NDEBUG QT_NO_DEBUG_OUTPUT QT_NO_DEBUG
	
	# Flags
	QMAKE_CFLAGS_RELEASE *= -O2
	QMAKE_CXXFLAGS_RELEASE *= -O2
	
	# Stack (Does this work?)
	# QMAKE_CXXFLAGS *= /F 134217728
	# QMAKE_LFLAGS   *= /STACK:134217728
	
	# Symbol File
	#QMAKE_CFLAGS *= /Zi
	#QMAKE_CXXFLAGS *= /Zi
	#QMAKE_CXXFLAGS_RELEASE *= /Od
	#QMAKE_LFLAGS *= /INCREMENTAL:NO /Debug
	#QMAKE_CFLAGS_RELEASE -= -O2
	#QMAKE_CXXFLAGS_RELEASE -= -O2
    
	# LTCG
	QMAKE_CFLAGS *= $$QMAKE_CFLAGS_LTCG
	QMAKE_CXXFLAGS *= $$QMAKE_CXXFLAGS_LTCG
	QMAKE_LFLAGS *= $$QMAKE_LFLAGS_LTCG
} else {
	BUILD = debug
}

# Config
COMPILER = vc141
PLATFORM = msvc2017_32
PLATFORM_TAG = x32

# Config
# CONFIG -= debug_and_release debug_and_release_target
CONFIG *= -std=c++11 thread exceptions rtti no_batch warn_off
CONFIG(win32, unix|macx|win32) {
	contains(QMAKE_HOST.arch, x86_64) {		
		PLATFORM = msvc2017_64
		PLATFORM_TAG = x64
	}
}

!CONFIG(shared,shared|static) {
	CONFIG *= static
}

# Defines
DEFINES -= UNICODE _UNICODE
DEFINES *= _MBCS BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE _CRT_SECURE_NO_WARNINGS
DEFINES *= WINVER=0x0601 _WIN32_WINNT=0x0601
# DEFINES *= CRYPTOPP_DISABLE_ASM 
# CRYPTOPP_BOOL_AESNI_INTRINSICS_AVAILABLE

# Code
ROOT = C:/demonsaw
SRC = $$ROOT/code

# Build
DEV = $$ROOT/build
DEV_LIB = $$DEV/lib/$$PLATFORM/$$BUILD
DEV_BIN = $$DEV/bin/$$PLATFORM/$$BUILD
DEV_TMP = $$DEV/tmp/$$TARGET/$$PLATFORM/$$BUILD

# Library
LIB = $$ROOT/lib

# Boost
BOOST = $$SRC/0_boost
BOOST_INC = $$BOOST
BOOST_LIB = $$LIB/0_boost/$$PLATFORM
BOOST_SRC = $$BOOST/boost
BOOST_VER = 1_69

ATOMIC_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_atomic-", $$COMPILER-$$BOOST_LNK-$$PLATFORM_TAG-$$BOOST_VER, ".lib")
CHRONO_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_chrono-", $$COMPILER-$$BOOST_LNK-$$PLATFORM_TAG-$$BOOST_VER, ".lib")
DATE_TIME_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_date_time-", $$COMPILER-$$BOOST_LNK-$$PLATFORM_TAG-$$BOOST_VER, ".lib")
FILESYSTEM_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_filesystem-", $$COMPILER-$$BOOST_LNK-$$PLATFORM_TAG-$$BOOST_VER, ".lib")
LOCALE_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_locale-", $$COMPILER-$$BOOST_LNK-$$PLATFORM_TAG-$$BOOST_VER, ".lib")
REGEX_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_regex-", $$COMPILER-$$BOOST_LNK-$$PLATFORM_TAG-$$BOOST_VER, ".lib")
SYSTEM_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_system-", $$COMPILER-$$BOOST_LNK-$$PLATFORM_TAG-$$BOOST_VER, ".lib")
THREAD_LIB = $$sprintf("%1%2%3%4", $$BOOST_LIB, "/libboost_thread-", $$COMPILER-$$BOOST_LNK-$$PLATFORM_TAG-$$BOOST_VER, ".lib")
BOOST_LIBS = $$ATOMIC_LIB $$CHRONO_LIB $$DATE_TIME_LIB $$FILESYSTEM_LIB $$LOCALE_LIB $$REGEX_LIB $$SYSTEM_LIB $$THREAD_LIB

# 3rd Party
CRYPTOPP = $$SRC/0_cryptopp
CRYPTOPP_INC = $$CRYPTOPP
CRYPTOPP_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/tmp/0_cryptopp/", $$PLATFORM, "/", $$BUILD)
CRYPTOPP_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/0_cryptopp-", $$BOOST_LNK, ".lib")

CONFIG(win32, unix|macx|win32) {
	contains(QMAKE_HOST.arch, x86_64) {
		CRYPTOPP_LIB *= $$CRYPTOPP_TMP/x64masm.obj $$CRYPTOPP_TMP/x64dll.obj
	}
}

CPPNETLIB = $$SRC/0_cppnetlib
CPPNETLIB_INC = $$CPPNETLIB
CPPNETLIB_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/tmp/0_cppnetlib/", $$PLATFORM, "/", $$BUILD)
CPPNETLIB_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/0_cppnetlib-", $$BOOST_LNK, ".lib")

CPUID = $$SRC/0_cpuid
CPUID_INC = $$CPUID
CPUID_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/tmp/0_cpuid/", $$PLATFORM, "/", $$BUILD)
CPUID_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/0_cpuid-", $$BOOST_LNK, ".lib")

CONFIG(win32, unix|macx|win32) {
	contains(QMAKE_HOST.arch, x86_64) {
		CPUID_LIB *= $$CPUID_TMP/masm-x64.obj
	}
}

MSGPACK = $$SRC/0_msgpack
MSGPACK_INC = $$MSGPACK/include
MSGPACK_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/tmp/0_msgpack/", $$PLATFORM, "/", $$BUILD)
MSGPACK_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/0_msgpack-", $$BOOST_LNK, ".lib")

# Library
CORE = $$SRC/4_core
CORE_INC = $$CORE
CORE_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/tmp/4_core/", $$PLATFORM, "/", $$BUILD)
CORE_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/4_core-", $$BOOST_LNK, ".lib")

CORE_CLIENT = $$SRC/4_core_client
CORE_CLIENT_INC = $$CORE_CLIENT
CORE_CLIENT_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/tmp/4_core_client/", $$PLATFORM, "/", $$BUILD)
CORE_CLIENT_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/4_core_client-", $$BOOST_LNK, ".lib")

CORE_GUI = $$SRC/4_core_gui
CORE_GUI_INC = $$CORE_GUI
CORE_GUI_TMP = $$sprintf("%1%2%3%4%5", $$DEV, "/tmp/4_core_gui/", $$PLATFORM, "/", $$BUILD)
CORE_GUI_LIB = $$sprintf("%1%2%3%4", $$DEV_LIB, "/4_core_gui-", $$BOOST_LNK, ".lib")

TELEMETRY_OBJ = $$sprintf("%1%2%3%4", $$LIB, "/0_telemetry/", $$PLATFORM, "/notelemetry.obj")

# HACK: LZ4
#CORE_LIB *= $$CORE_TMP/lz4_lib.obj $$CORE_TMP/lz4hc_lib.obj 
