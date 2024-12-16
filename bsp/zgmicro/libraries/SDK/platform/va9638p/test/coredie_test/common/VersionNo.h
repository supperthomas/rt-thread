#ifndef _VERSIONNO__H_
#define _VERSIONNO__H_

#include "revision.h"

#define VERSION_BIN_TYPE           "XXXX" 
#define VERSION_TIME_FORMAT        "XXXX-XX-XX XX:XX:XX"
#define VERSION_FILESTR_FORMAT     "XX,XX,XX,XX"
#define VERSION_HASH_FORMAT        "XXXXXXXX"

#define VERSION_TYPE_TOP_IMAGE     1
#define VERSION_TYPE_BT_IMAGE      2
#define VERSION_TYPE_ECK_IMAGE     3
#define VERSION_TYPE_DSP_IMAGE     4
#define VERSION_TYPE_EBT_IMAGE     5 /* external flash image */

typedef struct version_str {
    char ver_type[sizeof ( VERSION_BIN_TYPE )];
    char ver_hash[sizeof ( VERSION_HASH_FORMAT )];
    char ver_file[sizeof ( VERSION_FILESTR_FORMAT )];
    char ver_time[sizeof ( VERSION_TIME_FORMAT )];
} VERSION_STR __attribute__ ( ( aligned ( 1 ) ) );


#endif

