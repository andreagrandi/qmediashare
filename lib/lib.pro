
TEMPLATE = lib
TARGET = qmediashare
DEPENDPATH += . common facebook flickr gpicasa ../common
INCLUDEPATH += . common facebook flickr gpicasa ../common
CONFIG += qt staticlib
QT += network xml

# Input
HEADERS += common/album.h \
	common/md5.h \
	common/restrequest.h \
	facebook/facebookalbum.h \
	facebook/facebookclient.h \
	facebook/facebookrequest.h \
	facebook/facebooksession.h \
	facebook/facebookfriend.h \
	flickr/flickrclient.h \
	flickr/flickrphotoset.h \
	flickr/flickrrequest.h \
	flickr/flickrsession.h \
	flickr/flickrexception.h \
	flickr/flickrcomment.h \
	gpicasa/gdatarequest.h \
	gpicasa/gdatasession.h \
	gpicasa/gpicasaclient.h
SOURCES += common/album.cpp \
	common/md5.c \
	common/restrequest.cpp \
	facebook/facebookalbum.cpp \
	facebook/facebookclient.cpp \
	facebook/facebookrequest.cpp \
	facebook/facebooksession.cpp \
	flickr/flickrclient.cpp \
	flickr/flickrphotoset.cpp \
	flickr/flickrrequest.cpp \
	flickr/flickrsession.cpp \
	flickr/flickrexception.cpp \
	flickr/flickrcomment.cpp \
	gpicasa/gdatarequest.cpp \
	gpicasa/gdatasession.cpp \
	gpicasa/gpicasaclient.cpp
