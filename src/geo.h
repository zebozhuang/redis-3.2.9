#ifndef __GEO_H__
#define __GEO_H__

#include "server.h"

/* Structures used inside geo.c in order to represent points and array of
 * points on the earth. */
/* 地理数据库实现 */
typedef struct geoPoint {
    double longitude;   /* 经度 */
    double latitude;    /* 纬度 */
    double dist;        /* ？*/
    double score;       /* ? */
    char *member;       /* ? */
} geoPoint;

/* 地理数组 */
typedef struct geoArray {
    struct geoPoint *array;
    size_t buckets;
    size_t used;
} geoArray;

#endif
