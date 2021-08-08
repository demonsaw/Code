
# Include
include($$PWD/algorithm/algorithm.pri)
include($$PWD/checksum/checksum.pri)
include($$PWD/cipher/cipher.pri)
include($$PWD/filter/filter.pri)
include($$PWD/hash/hash.pri)

# Header
HEADERS += $$PWD/salt.h
HEADERS += $$PWD/security.h

# Source
SOURCES += $$PWD/security.cpp
