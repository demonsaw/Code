
# Config
CONFIG *= $$(TYPE) 
CONFIG *= $$(DISTRO)

CONFIG(shared,shared|static) {
	CONFIG(debug,debug|release) {
            BOOST_LNK = mt-gd
            CONFIG(android): BOOST_LNK=mt-d
	} else {
		BOOST_LNK = mt
	}
} 

CONFIG(static,shared|static) {
	CONFIG(debug,debug|release) {
		BOOST_LNK = mt-sgd
	} else {
		BOOST_LNK = mt-s
	}
}

# Platform
CONFIG(win32) {
	include($$PWD/4_win.pri)
} else:CONFIG(android) {
	include($$PWD/4_and.pri)	
} else:CONFIG(macx) {
	include($$PWD/4_osx.pri)
} else:CONFIG(unix) {
	include($$PWD/4_deb.pri)
} else {
	message("Unknown Platform")
}
