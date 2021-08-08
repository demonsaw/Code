
# Include
include($$PWD/callback/callback.pri)
include($$PWD/cipher/cipher.pri)
include($$PWD/group/group.pri)
include($$PWD/io/io.pri)
include($$PWD/message/message.pri)
include($$PWD/security/security.pri)
include($$PWD/service/service.pri)
include($$PWD/session/session.pri)
include($$PWD/status/status.pri)
include($$PWD/time/time.pri)

# Header
HEADERS += $$PWD/component.h
HEADERS += $$PWD/component_callback.h

HEADERS += $$PWD/chat_component.h
HEADERS += $$PWD/color_component.h
HEADERS += $$PWD/endpoint_component.h
HEADERS += $$PWD/error_component.h
HEADERS += $$PWD/hash_component.h
HEADERS += $$PWD/mute_component.h
HEADERS += $$PWD/name_component.h
HEADERS += $$PWD/notify_component.h
HEADERS += $$PWD/room_component.h
HEADERS += $$PWD/search_component.h
HEADERS += $$PWD/socket_component.h
HEADERS += $$PWD/spam_component.h
HEADERS += $$PWD/state_component.h
HEADERS += $$PWD/ui_component.h
HEADERS += $$PWD/verified_component.h
HEADERS += $$PWD/version_component.h

# Source
SOURCES += $$PWD/component.cpp
SOURCES += $$PWD/component_callback.cpp

SOURCES += $$PWD/chat_component.cpp
SOURCES += $$PWD/color_component.cpp
SOURCES += $$PWD/hash_component.cpp
SOURCES += $$PWD/name_component.cpp
SOURCES += $$PWD/notify_component.cpp
SOURCES += $$PWD/state_component.cpp
SOURCES += $$PWD/ui_component.cpp
SOURCES += $$PWD/version_component.cpp
