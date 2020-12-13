#ifndef BMP_H
#define BMP_H

#include <stdio.h>
#include "DB_TypeDef.h"

/* 文件头 */
struct BmpFileHeader
{
	DB_UINT16	m_u16Type;	            /* 表示位图类型 */
	DB_UINT32	m_u32Size;	            /* 4字节 表示文件的大小 */
	DB_UINT16	m_u16reserved1;	        /* 2字节 这个是保留的，为0 */
	DB_UINT16	m_u16reserved2;         /* 2字节 这个也是保留的，为0 */
	DB_UINT32	m_u32OffBits;           /* 4字节 文件开始到实际图像数据之间的字节偏移量 */
} __attribute__ ((packed));
 
/* 位图信息头 */
struct BmpInfoHeader 
{
	DB_UINT32	m_u32Size;              /* 4字节 表示位图文件信息头的大小（单位是字节） */
	DB_INT32	m_s32Width;             /* 4字节 表示位图文件的宽度（像素） */
	DB_INT32	m_s32Height;            /* 4字节 表示位图文件的高度（像素） */
	DB_UINT16	m_u16planes;            /* 2字节 恒为1 */
	DB_UINT16	m_u16BitCount;          /* 2字节 每个像素占用的位数即bpp */
	DB_UINT32	m_u32Compression;       /* 4字节 文件压缩方式 */
	DB_UINT32	m_u32SizeImage;         /* 4字节 表示图像的大小，当没有压缩的时候，这个值可以为0 */
	DB_INT32	m_s32x_pels_per_meter;  /* 4字节 表示水平分辨率 */
	DB_INT32	m_s32y_pels_per_meter;  /* 4字节 表示垂直分辨率 */
	DB_UINT32	m_u32ClrUsed;           /* 4字节 表示彩色表中的颜色索引数目 */
	DB_UINT32	m_u32ClrImportant;      /* 4字节 表示有重要影响到颜色索引数目 */
} __attribute__ ((packed));

 
#endif /* BMP_H */