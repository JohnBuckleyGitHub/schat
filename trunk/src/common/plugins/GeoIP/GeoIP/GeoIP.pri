win32 {
  DEFINES += _CRT_SECURE_NO_WARNINGS
  LIBS += -lwsock32 -lws2_32
}

SOURCES += \
   GeoIP/GeoIP.h \
   GeoIP/GeoIP_internal.h \
   GeoIP/GeoIPCity.h \
   GeoIP/global.h \
   GeoIP/types.h \

HEADERS += \  
   GeoIP/GeoIP.c \
   GeoIP/GeoIPCity.c \
   GeoIP/regionName.c \
   GeoIP/timeZone.c \