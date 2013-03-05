
TEMPLATE = lib
TARGET = common
DEPENDPATH += . ../lib
INCLUDEPATH += . \
        ../lib/common \
        ../lib/facebook \
        ../lib/flickr \
        ../lib/gpicasa
CONFIG += qt static
QT += xml

PRE_TARGETDEPS += $$OUT_PWD/../lib/libqmediashare.a

# Input
HEADERS += facebookservice.h \
	flickrservice.h \
	gpicasaservice.h \
	service.h \
	imagetagger.h \
	tagselector.h
FORMS += facebooklogin.ui \
	flickrlogin.ui \
	gdatalogin.ui \
	tagselector.ui
SOURCES += facebookservice.cpp \
	flickrservice.cpp \
	gpicasaservice.cpp \
	imagetagger.cpp \
	tagselector.cpp
