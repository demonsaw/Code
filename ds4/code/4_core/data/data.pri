
# Include
include($$PWD/request/request.pri)
include($$PWD/response/response.pri)

# Header
HEADERS += $$PWD/data.h
HEADERS += $$PWD/checksum_data.h
HEADERS += $$PWD/chunk_data.h
HEADERS += $$PWD/client_data.h
HEADERS += $$PWD/data_packer.h
HEADERS += $$PWD/data_type.h
HEADERS += $$PWD/data_unpacker.h
HEADERS += $$PWD/file_data.h
HEADERS += $$PWD/folder_data.h
HEADERS += $$PWD/header_data.h
HEADERS += $$PWD/room_data.h
HEADERS += $$PWD/router_data.h
HEADERS += $$PWD/version_data.h

# Source
SOURCES += $$PWD/chunk_data.cpp
SOURCES += $$PWD/data_packer.cpp
SOURCES += $$PWD/data_unpacker.cpp
SOURCES += $$PWD/version_data.cpp
