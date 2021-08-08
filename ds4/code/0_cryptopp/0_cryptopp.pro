
include($$PWD/../4_platform.pri)
message("0_cryptopp:" $$PLATFORM $$COMPILER $$BUILD $$LIBRARY)

# Config
TEMPLATE = lib
TARGET = 0_cryptopp-$$BOOST_LNK

CONFIG *= staticlib
CONFIG -= app_bundle qt

# Path
DESTDIR = $$DEV_LIB
OBJECTS_DIR = $$DEV_TMP
INCLUDEPATH *= $$PWD
LIBS *= -lws2_32

# ASM (64 bit only)
CONFIG(win32, unix|macx|win32) {
	contains(QMAKE_HOST.arch, x86_64) {
		asm.input = ASM_SOURCES
		asm.targetdir = $$DEV_TMP
		asm.output = $$asm.targetdir/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
		asm.commands = ml64.exe /c /nologo /Fo"$$asm.targetdir/${QMAKE_FILE_BASE}.obj" /Zi "${QMAKE_FILE_IN}"
		asm.depends = $$ASM_SOURCES
		asm.variable_out = PRE_TARGETDEPS
		asm.clean = "$$DEV_TMP\x64*.obj"
		QMAKE_EXTRA_COMPILERS *= asm
	}
}

# Include
include($$PWD/cryptopp/0_cryptopp.pri)
