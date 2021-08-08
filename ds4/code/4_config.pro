
TEMPLATE = subdirs
CONFIG *= ordered

# Build
CONFIG(android) {
	SUBDIRS = 0_cppnetlib 0_cryptopp 0_msgpack 4_core 4_core_client 4_core_gui 4_mobile
	4_mobile.subdir = 4_mobile	
	4_mobile.depends = 0_cppnetlib 0_cryptopp 0_msgpack 4_core 4_core_gui
} else {
	SUBDIRS = 0_cppnetlib 0_cryptopp 0_msgpack 4_core 4_router_cli 4_core_client 4_client_cli 4_core_gui 4_client_desktop
	4_router_cli.subdir = 4_router_cli
	4_router_cli.depends = 0_cppnetlib 0_cryptopp 0_msgpack 4_core
	4_client_cli.subdir = 4_client_cli
	4_client_cli.depends = 0_cppnetlib 0_cryptopp 0_msgpack 4_core 4_core_client
	4_client_desktop.subdir = 4_client_desktop	
	4_client_desktop.depends = 0_cppnetlib 0_cryptopp 0_msgpack 4_core 4_core_client 4_core_gui
}

0_cppnetlib.subdir = 0_cppnetlib
0_cryptopp.subdir = 0_cryptopp
0_msgpack.subdir = 0_msgpack
4_core.subdir = 4_core
4_core_client.subdir = 4_core_client
4_core_gui.subdir = 4_core_gui

# Here's where you build up the hierarchical relationship between components.
4_core.depends = 0_cppnetlib 0_cryptopp 0_msgpack
4_core_client.depends = 0_cppnetlib 0_cryptopp 0_msgpack 4_core
4_core_gui.depends = 0_cppnetlib 0_cryptopp 0_msgpack 4_core
