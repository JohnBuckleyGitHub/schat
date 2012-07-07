#ifndef QTADAPTER_H
#define QTADAPTER_H

// defines:
#define HTTP_PARSER_STRICT 0
#define HTTP_PARSER_DEBUG 0

// functions:
#define assert(expr) (expr) ? qt_noop() : qt_assert(#expr, __FILE__, __LINE__)
#define memset(p, v, s) { \
        unsigned char *str = (unsigned char*)(p); \
        unsigned char uv = v; \
        size_t r = s; \
        while (r-- != 0) *str++ = uv; \
    }

#endif // QTADAPTER_H
