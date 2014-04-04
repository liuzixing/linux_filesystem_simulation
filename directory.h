#ifndef _STRUCT_H
#define _STRUCT_H

#define MEM_D_SIZE 1024*1024    //总磁盘空间为1M
#define DISKSIZE 1024			//磁盘块的大小1K
#define DISK_NUM 1024			//磁盘块数目1K
#define FATSIZE  DISK_NUM*sizeof(struct fatitem)	//FAT表大小
#define ROOT_DISK_NO FATSIZE/DISKSIZE+1				//根目录起始盘块号
#define ROOT_DISK_SIZE sizeof(struct direct)		//根目录大小
#define DIR_MAXSIZE  1024		//路径最大长度为1KB
#define MSD   5					//最大子目录数5
#define MOFN  5					//最大文件深度为5
#define MAX_WRITE 1024*128		//最大写入文字长度128KB  

struct fatitem  /* size 8*/ 
{ 
	int item;  /*存放文件下一个磁盘的指针*/ 
	char em_disk; /*磁盘块是否空闲标志位 0 空闲*/ 
}; 

struct direct  
{ 
	/*-----文件控制快信息-----*/ 
	struct FCB 
	{ 
		char name[9];	/*文件/目录 名 8位*/ 
		char property;  /*属性 1位目录 0位普通文件*/ 
		char protect_hold[5];	/*文件所有者保护位*/ 
		int size;		/*文件/目录 字节数、盘块数)*/ 
		int firstdisk;  /*文件/目录 起始盘块号*/ 
		int next;		/*子目录 起始盘块号*/ 
		int sign;		/*1是根目录 0不是根目录*/ 
		
	}directitem[MSD+2]; 
	
}; 

struct opentable  
{ 
	struct openttableitem  
	{ 
		char name[9];	/*文件名*/ 
		int firstdisk;	/*起始盘块号*/  
		int size;		/*文件的大小*/ 
	}openitem[MOFN]; 
	int cur_size;		/*当前打文件的数目*/ 
}; 

struct fatitem *fat;   /*FAT表*/ 
struct direct *root;   /*根目录*/ 
struct direct *cur_dir;   /*当前目录*/ 
struct opentable u_opentable; /*文件打开表*/ 
int    fd=-1;     /*文件打开表的序号*/ 
char*  bufferdir;    /*记录当前路径的名称*/ 
char*  fdisk;     /*虚拟磁盘起始地址*/ 

#endif