#ifndef _DB_TYPEDEF_H_
#define _DB_TYPEDEF_H_

#ifndef NULL
#define NULL                              0
#endif

#define ENABLE                            1
#define DISABLE                           0

#ifndef DB_NULL
#define DB_NULL                           ((DB_VOID *)0)
#endif

#define DB_VOID void
//typedef void                              DB_VOID;                              /* 前缀无 */
typedef unsigned char                     DB_UINT8;                             /* 前缀u8 */
typedef unsigned short                    DB_UINT16;                            /* 前缀u16 */
typedef unsigned int                      DB_UINT32;                            /* 前缀u32 */
typedef unsigned long long                DB_UINT64;                            /* 前缀u64 */
typedef signed char                       DB_INT8;                              /* 前缀i8 */
typedef signed short                      DB_INT16;                             /* 前缀i16 */
typedef signed int                        DB_INT32;                             /* 前缀i32 */
typedef signed long long                  DB_INT64;                             /* 前缀i64 */
typedef char                              DB_CHAR;                              /* 前缀c */
typedef float                             DB_FLOAT;                             /* 前缀f */
typedef double                            DB_DOUBLE;                            /* 前缀d */

typedef unsigned long                     DB_ULONG;                             /* 前缀ul *//*为了适配32和64位系统需要定义*/
typedef long							  DB_LONG;


typedef struct
{
    DB_VOID *m_pData;
    DB_UINT32 m_u32Len;
}DB_DataStruct;

typedef enum { DB_FALSE, DB_TRUE }         DB_BOOL;                               /* 前缀b */

#define DB_OK                              0                                      /* 正常返回 */
#define DB_ERR                            -1                                      /* 出错返回,没有明确具体出错原因的返回 */
#define DB_UNMET                          -2                                      /* 出错返回,未满足条件 */

#define DB_OFFSET_OF(type, member)           ((DB_ULONG)&(((type *)0)->member))
#define DB_CONTAINER_OF(ptr, type, member)   ((type *)((DB_ULONG)(ptr)-DB_OFFSET_OF(type,member)))
#define DB_ARRARY_COUNT(arrary)              (sizeof(arrary)/sizeof((arrary)[0]))

#endif /* _DB_TYPEDEF_H_ */
