#ifndef _STRUCT_H
#define _STRUCT_H

#define MEM_D_SIZE 1024*1024    //�ܴ��̿ռ�Ϊ1M
#define DISKSIZE 1024			//���̿�Ĵ�С1K
#define DISK_NUM 1024			//���̿���Ŀ1K
#define FATSIZE  DISK_NUM*sizeof(struct fatitem)	//FAT���С
#define ROOT_DISK_NO FATSIZE/DISKSIZE+1				//��Ŀ¼��ʼ�̿��
#define ROOT_DISK_SIZE sizeof(struct direct)		//��Ŀ¼��С
#define DIR_MAXSIZE  1024		//·����󳤶�Ϊ1KB
#define MSD   5					//�����Ŀ¼��5
#define MOFN  5					//����ļ����Ϊ5
#define MAX_WRITE 1024*128		//���д�����ֳ���128KB  

struct fatitem  /* size 8*/ 
{ 
	int item;  /*����ļ���һ�����̵�ָ��*/ 
	char em_disk; /*���̿��Ƿ���б�־λ 0 ����*/ 
}; 

struct direct  
{ 
	/*-----�ļ����ƿ���Ϣ-----*/ 
	struct FCB 
	{ 
		char name[9];	/*�ļ�/Ŀ¼ �� 8λ*/ 
		char property;  /*���� 1λĿ¼ 0λ��ͨ�ļ�*/ 
		char protect_hold[5];	/*�ļ������߱���λ*/ 
		int size;		/*�ļ�/Ŀ¼ �ֽ������̿���)*/ 
		int firstdisk;  /*�ļ�/Ŀ¼ ��ʼ�̿��*/ 
		int next;		/*��Ŀ¼ ��ʼ�̿��*/ 
		int sign;		/*1�Ǹ�Ŀ¼ 0���Ǹ�Ŀ¼*/ 
		
	}directitem[MSD+2]; 
	
}; 

struct opentable  
{ 
	struct openttableitem  
	{ 
		char name[9];	/*�ļ���*/ 
		int firstdisk;	/*��ʼ�̿��*/  
		int size;		/*�ļ��Ĵ�С*/ 
	}openitem[MOFN]; 
	int cur_size;		/*��ǰ���ļ�����Ŀ*/ 
}; 

struct fatitem *fat;   /*FAT��*/ 
struct direct *root;   /*��Ŀ¼*/ 
struct direct *cur_dir;   /*��ǰĿ¼*/ 
struct opentable u_opentable; /*�ļ��򿪱�*/ 
int    fd=-1;     /*�ļ��򿪱�����*/ 
char*  bufferdir;    /*��¼��ǰ·��������*/ 
char*  fdisk;     /*���������ʼ��ַ*/ 

#endif