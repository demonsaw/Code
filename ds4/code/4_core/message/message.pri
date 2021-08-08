
# Include
include($$PWD/request/request.pri)
include($$PWD/response/response.pri)

# Header
HEADERS += $$PWD/message.h
HEADERS += $$PWD/checksum_message.h
HEADERS += $$PWD/header_message.h
HEADERS += $$PWD/message_buffer.h
HEADERS += $$PWD/message_packer.h
HEADERS += $$PWD/message_type.h
HEADERS += $$PWD/message_unpacker.h
HEADERS += $$PWD/version_message.h

# Source
SOURCES += $$PWD/message.cpp
SOURCES += $$PWD/message_packer.cpp
SOURCES += $$PWD/message_unpacker.cpp
SOURCES += $$PWD/version_message.cpp
