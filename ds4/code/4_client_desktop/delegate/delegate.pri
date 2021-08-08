
# Include
include($$PWD/browse/browse.pri)
include($$PWD/chat/chat.pri)
include($$PWD/client/client.pri)
include($$PWD/error/error.pri)
include($$PWD/group/group.pri)
include($$PWD/io/io.pri)
include($$PWD/share/share.pri)

# Header
HEADERS += $$PWD/delegate_util.h
HEADERS += $$PWD/radio_button_delegate.h
HEADERS += $$PWD/row_delegate.h
HEADERS += $$PWD/time_delegate.h

# Source
SOURCES += $$PWD/delegate_util.cpp
SOURCES += $$PWD/radio_button_delegate.cpp
SOURCES += $$PWD/row_delegate.cpp
SOURCES += $$PWD/time_delegate.cpp
