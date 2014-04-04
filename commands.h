#ifndef _FUNCTION_H
#define _FUNCTION_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "directory.h"

int ci = 0;
void initfile();
void format();
void enter();
void halt();
int  create(char *name);
int  open(char *name);
int  close(char *name);
int  write(int fd,char *buf,int len);
int  read(int fd,char *buf);
int  del(char *name);
int  mkdir(char *name);
int  rmdir(char *name);
void dir();
int  cd(char *name);
void print();
void show();

void initfile()
{
	fdisk = (char *)malloc(MEM_D_SIZE*sizeof(char)); /*申请 1M空间*/
	format();

}

void format()
{
	int i;
	FILE *fp;

	fat = (struct fatitem *)(fdisk+DISKSIZE); /*计算FAT表地址，引导区向后偏移 1k)*/
	/*-----初始化FAT表------------*/
	fat[0].item=-1;  /*引导块*/
	fat[0].em_disk='1';

	for(i=1;i<ROOT_DISK_NO-1;i++) /*存放 FAT表的磁盘块号*/
	{
		fat[i].item=i+1;
		fat[i].em_disk='1';
	}

	fat[ROOT_DISK_NO].item=-1;  /*存放根目录的磁盘块号*/
	fat[ROOT_DISK_NO].em_disk='1';

	for(i=ROOT_DISK_NO+1;i<DISK_NUM;i++)
	{
		fat[i].item = -1;
		fat[i].em_disk = '0';
	}
	/*-----------------------------------------------*/
	root = (struct direct *)(fdisk+DISKSIZE+FATSIZE); /*根目录的地址*/
	/*初始化目录*/
	/*---------指向当前目录的目录项---------*/
	root->directitem[0].sign = 1;
	root->directitem[0].firstdisk = ROOT_DISK_NO;
	strcpy(root->directitem[0].name,".");
	root->directitem[0].next = root->directitem[0].firstdisk;
	root->directitem[0].property = '1';
	root->directitem[0].size = ROOT_DISK_SIZE;
	/*-------指向上一级目录的目录项---------*/
	root->directitem[1].sign = 1;
	root->directitem[1].firstdisk = ROOT_DISK_NO;
	strcpy(root->directitem[1].name,"..");
	root->directitem[1].next = root->directitem[0].firstdisk;
	root->directitem[1].property = '1';
	root->directitem[1].size = ROOT_DISK_SIZE;
	if((fp = fopen("disk.dat","wb"))==NULL)
	{
		printf("Error:\n Cannot open file \n");
		return;
	}
	for(i=2;i<MSD+2;i++) /*子目录初始化为空*/
	{
		root->directitem[i].sign = 0;
		root->directitem[i].firstdisk = -1;
		strcpy(root->directitem[i].name,"");
		root->directitem[i].next = -1;
		root->directitem[i].property = '0';
		root->directitem[i].size = 0;
	}


	if((fp = fopen("disk.dat","wb"))==NULL)
	{
		printf("Error:\n Cannot open file \n");
		return;
	}
	if(fwrite(fdisk,MEM_D_SIZE,1,fp)!=1) /*把虚拟磁盘空间保存到磁盘文件中*/
	{
		printf("Error:\n File write error! \n");
	}

	fclose(fp);

}
void enter()
{
	FILE *fp;
	int i;

	fdisk = (char *)malloc(MEM_D_SIZE*sizeof(char)); /*申请 1M空间*/
	if((fp=fopen("disk.dat","rb"))==NULL)
	{
		printf("Error:\nCannot open file\n");
		return;
	}
	if(!fread(fdisk,MEM_D_SIZE,1,fp))  /*把磁盘文件disk.dat 读入虚拟磁盘空间(内存)*/
	{
		printf("Error:\nCannot read file\n");
		exit(0);
	}
	fat = (struct fatitem *)(fdisk+DISKSIZE);  /*找到FAT表地址*/
	root = (struct direct *)(fdisk+DISKSIZE+FATSIZE);/*找到根目录地址*/
	fclose(fp);
	/*--------------初始化用户打开表------------------*/
	for(i=0;i<MOFN;i++)
	{
		strcpy(u_opentable.openitem[i].name,"");
		u_opentable.openitem[i].firstdisk = -1;
		u_opentable.openitem[i].size = 0;
	}
	u_opentable.cur_size = 0;

	cur_dir = root; /*当前目录为根目录*/
	bufferdir = (char *)malloc(DIR_MAXSIZE*sizeof(char));
	strcpy(bufferdir,"Root:");
}

void halt()
{
	FILE *fp;
	int i;

	if((fp=fopen("disk.dat","wb"))==NULL)
	{
		printf("Error:\nCannot open file\n");
		return;
	}
	if(!fwrite(fdisk,MEM_D_SIZE,1,fp)) /*把虚拟磁盘空间(内存)内容读入磁盘文件disk.dat */
	{
		printf("Error:\nFile write error!\n");
	}
	fclose(fp);

	free(fdisk);
	free(bufferdir);
	return;
}

int create(char *name)
{

    int i,j;

    if(strlen(name)>8) /*文件名大于 8位*/
		return(-1);

    for(j=2;j<MSD+2;j++) /*检查创建文件是否与已存在的文件重名*/
    {
		if(!strcmp(cur_dir->directitem[j].name,name))
			break;
    }
	if(j<MSD+2)     /*文件已经存在*/
		return(-4);

    for(i=2;i<MSD+2;i++) /*找到第一个空闲子目录*/
    {
		if(cur_dir->directitem[i].firstdisk==-1)
			break;
    }
    if(i>=MSD+2) /*无空目录项*/
		return(-2);
    if(u_opentable.cur_size>=MOFN) /*打开文件太多*/
		return(-3);


    for(j=ROOT_DISK_NO+1;j<DISK_NUM;j++) /*找到空闲盘块 j 后退出*/
    {
		if(fat[j].em_disk=='0')
			break;
    }
	if(j>=DISK_NUM)
		return(-5);
    fat[j].em_disk = '1';  /*将空闲块置为已经分配*/
    /*-----------填写目录项-----------------*/
	ci = i;
    strcpy(cur_dir->directitem[i].name,name);
	strcpy(cur_dir->directitem[i].protect_hold, "rwxr");
    cur_dir->directitem[i].firstdisk = j;
    cur_dir->directitem[i].size = 0;
    cur_dir->directitem[i].next = j;
    cur_dir->directitem[i].property = '0';
    /*---------------------------------*/
    fd = open(name);
    return 0;

}

int open(char *name)
{
    int i, j;

    for(i=2;i<MSD+2;i++) /*文件是否存在*/
    {
		if(!strcmp(cur_dir->directitem[i].name,name))
			break;
    }

    if(i>=MSD+2)
		return(-1);
	ci = i;
	/*--------是文件还是目录-----------------------*/
	if(cur_dir->directitem[i].property=='1')
		return(-4);

    /*--------文件是否打开-----------------------*/
    for(j=0;j<MOFN;j++)
    {
		if(!strcmp(u_opentable.openitem[j].name,name))
			break;
    }

	if(j<MOFN)  /*文件已经打开*/
		return(-2);


    if(u_opentable.cur_size>=MOFN) /*文件打开太多*/
		return(-3);

    /*--------查找一个空闲用户打开表项-----------------------*/
    for(j=0;j<MOFN;j++)
    {
		if(u_opentable.openitem[j].firstdisk==-1)
			break;
    }
    /*--------------填写表项的相关信息------------------------*/
    u_opentable.openitem[j].firstdisk = cur_dir->directitem[i].firstdisk;
    strcpy(u_opentable.openitem[j].name,name);
    u_opentable.openitem[j].size = cur_dir->directitem[i].size;
    u_opentable.cur_size++;
    /*----------返回用户打开表表项的序号--------------------------*/
    return(j);
}

int close(char *name)
{
    int i;

    for(i=0;i<MOFN;i++)
    {
		if(!strcmp(u_opentable.openitem[i].name,name))
			break;
    }
    if(i>=MOFN)
		return(-1);
    /*-----------清空该文件的用户打开表项的内容---------------------*/
    strcpy(u_opentable.openitem[i].name,"");
    u_opentable.openitem[i].firstdisk = -1;
    u_opentable.openitem[i].size = 0;
    u_opentable.cur_size--;


    return 0;
}

int write(int fd, char *buf, int len)
{
	char *first;
	int item, i, j, k;
	int ilen1, ilen2, modlen, temp;
	/*----------用 $ 字符作为空格 # 字符作为换行符-----------------------*/
	char Space = 32;
	char Endter= '\n';

	for(i=0;i<len;i++)
	{
		if(buf[i] == '$')
			buf[i] = Space;
		else if(buf[i] == '#')
			buf[i] = Endter;
	}

	/*----------读取用户打开表对应表项第一个盘块号-----------------------*/

	item = u_opentable.openitem[fd].firstdisk;

	/*-------------找到当前目录所对应表项的序号-------------------------*/
	for(i=2;i<MSD+2;i++)
	{
		if(cur_dir->directitem[i].firstdisk==item)
			break;
	}
	temp = i; /*-存放当前目录项的下标-*/
	/*------找到的item 是该文件的最后一块磁盘块-------------------*/
	while(fat[item].item!=-1)
	{
		item =fat[item].item; /*-查找该文件的下一盘块--*/
	}

	/*-----计算出该文件的最末地址-------*/
	first = fdisk+item*DISKSIZE+u_opentable.openitem[fd].size%DISKSIZE;

	/*-----如果最后磁盘块剩余的大小大于要写入的文件的大小-------*/
	if(DISKSIZE-u_opentable.openitem[fd].size%DISKSIZE>len)
	{
		strcpy(first,buf);
		u_opentable.openitem[fd].size = u_opentable.openitem[fd].size+len;
		cur_dir->directitem[temp].size = cur_dir->directitem[temp].size+len;
	}
	else
	{
		for(i=0;i<(DISKSIZE-u_opentable.openitem[fd].size%DISKSIZE);i++)
		{/*写一部分内容到最后一块磁盘块的剩余空间(字节)*/
			first[i] = buf [i];
		}
		/*-----计算分配完最后一块磁盘的剩余空间(字节) 还剩下多少字节未存储-------*/
		ilen1 = len-(DISKSIZE-u_opentable.openitem[fd].size%DISKSIZE);
		ilen2 = ilen1/DISKSIZE;
		modlen = ilen1%DISKSIZE;
		if(modlen>0)
			ilen2 = ilen2+1; /*--还需要多少块磁盘块-*/

		for(j=0;j<ilen2;j++)
		{
			for(i=ROOT_DISK_NO+1;i<DISK_NUM;i++)/*寻找空闲磁盘块*/
			{
				if(fat[i].em_disk=='0')
					break;
			}
			if(i>=DISK_NUM) /*--如果磁盘块已经分配完了-*/
				return(-1);
			first = fdisk+i*DISKSIZE; /*--找到的那块空闲磁盘块的起始地址-*/
			if(j==ilen2-1) /*--如果是最后要分配的一块-*/
			{
				for(k=0;k<len-(DISKSIZE-u_opentable.openitem[fd].size%DISKSIZE)-j*DISKSIZE;k++)
					first[k] = buf[k];
			}
			else/*-如果不是要最后分配的一块--*/
			{
				for(k=0;k<DISKSIZE;k++)
					first[k] =buf[k];
			}

			fat[item].item = i;  /*--找到一块后将它的序号存放在上一块的指针中-*/
			fat[i].em_disk = '1'; /*--置找到的磁盘快的空闲标志位为已分配-*/
			fat[i].item = -1;  /*--它的指针为 -1 (即没有下一块)-*/
		}
		/*--修改长度-*/
		u_opentable.openitem[fd].size = u_opentable.openitem[fd].size+len;
		cur_dir->directitem[temp].size = cur_dir->directitem[temp].size+len;
	}
	return 0;
}

int read(int fd, char *buf)
{
	int len = u_opentable.openitem[fd].size;
	char *first;
	int i, j, item;
	int ilen1, modlen;

	item = u_opentable.openitem[fd].firstdisk;


	ilen1 = len/DISKSIZE;
	modlen = len%DISKSIZE;
	if(modlen!=0)
		ilen1 = ilen1+1; /*--计算文件所占磁盘的块数-*/

	first = fdisk+item*DISKSIZE; /*--计算文件的起始位置-*/

	for(i=0;i<ilen1;i++)
	{
		if(i==ilen1-1) /*--如果在最后一个磁盘块-*/
		{
			for(j=0;j<len-i*DISKSIZE;j++)
				buf[i*DISKSIZE+j] = first[j];
		}
		else /*--不在最后一块磁盘块-*/
		{
			for(j=0;j<len-i*DISKSIZE;j++)
				buf[i*DISKSIZE+j] = first[j];
			item = fat[item].item; /*-查找下一盘块-*/
			first = fdisk+item*DISKSIZE;
		}
	}
	return 0;
}

int del(char *name)
{
	int i,cur_item,item,temp;

	for(i=2;i<MSD+2;i++) /*--查找要删除文件是否在当前目录中-*/
	{
		if(!strcmp(cur_dir->directitem[i].name,name))
			break;
	}
	cur_item = i; /*--用来保存目录项的序号,供释放目录中-*/

    if(i>=MSD+2) /*--如果不在当前目录中-*/
		return(-1);

	if(cur_dir->directitem[cur_item].property!='0') /*--如果删除的(不)是目录-*/
		return(-3);

	for(i=0;i<MOFN;i++) /*--如果文件打开,则不能删除,退出-*/
	{
		if(!strcmp(u_opentable.openitem[i].name,name))
			return(-2);
	}
	item = cur_dir->directitem[cur_item].firstdisk;/*--该文件的起始盘块号-*/
	while(item!=-1) /*--释放空间,将FAT表对应项进行修改-*/
	{
		temp = fat[item].item;
		fat[item].item = -1;
		fat[item].em_disk = '0';
		item = temp;
	}
	/*-----------------释放目录项-----------------------*/
	cur_dir->directitem[cur_item].sign = 0;
	cur_dir->directitem[cur_item].firstdisk = -1;
	strcpy(u_opentable.openitem[cur_item].name,"");
    cur_dir->directitem[cur_item].next = -1;
    cur_dir->directitem[cur_item].property = '0';
	cur_dir->directitem[cur_item].size = 0;

	return 0;
}

int mkdir(char *name)
{
	int i,j;
	struct direct *cur_mkdir;

	if(!strcmp(name,"."))
		return(-4);
	if(!strcmp(name,".."))
		return(-4);
	if(strlen(name)>8)  /*-如果目录名长度大于 8位-*/
		return(-1);

	for(i=2;i<MSD+2;i++) /*-如果有空闲目录项退出-*/
	{
		if(cur_dir->directitem[i].firstdisk==-1)
			break;
	}
	if(i>=MSD+2) /*-目录/文件 已满-*/
		return(-2);
	for(j=2;j<MSD+2;j++) /*-判断是否有重名-*/
	{
		if(!strcmp(cur_dir->directitem[j].name,name))
			break;
	}
	if(j<MSD+2)  /*-如果有重名-*/
		return(-3);
	for(j=ROOT_DISK_NO+1;j<DISK_NUM;j++) /*-找到空闲磁盘块 j 后退出-*/
	{
		if(fat[j].em_disk=='0')
			break;
	}
	if(j>=DISK_NUM)
		return(-5);
	fat[j].em_disk='1'; /*-将该空闲块设置为已分配-*/

	/*-------------填写目录项----------*/
	strcpy(cur_dir->directitem[i].name,name);
	cur_dir->directitem[i].firstdisk=j;
	cur_dir->directitem[i].size=ROOT_DISK_SIZE;
	cur_dir->directitem[i].next=j;
	cur_dir->directitem[i].property='1';


	/*-所创目录在虚拟磁盘上的地址(内存物理地址)-*/
	cur_mkdir=(struct direct *)(fdisk+cur_dir->directitem[i].firstdisk*DISKSIZE);

	/*-初始化目录-*/
	/*-指向当前目录的目录项-*/
	cur_mkdir->directitem[0].sign=0;
	cur_mkdir->directitem[0].firstdisk=cur_dir->directitem[i].firstdisk;
	strcpy(cur_mkdir->directitem[0].name,".");
	cur_mkdir->directitem[0].next=cur_mkdir->directitem[0].firstdisk;
	cur_mkdir->directitem[0].property='1';
	cur_mkdir->directitem[0].size=ROOT_DISK_SIZE;

	/*-指向上一级目录的目录项-*/
	cur_mkdir->directitem[1].sign=cur_dir->directitem[0].sign;
	cur_mkdir->directitem[1].firstdisk=cur_dir->directitem[0].firstdisk;
	strcpy(cur_mkdir->directitem[1].name,"..");
	cur_mkdir->directitem[1].next=cur_mkdir->directitem[1].firstdisk;
	cur_mkdir->directitem[1].property='1';
	cur_mkdir->directitem[1].size=ROOT_DISK_SIZE;
	for(i=2;i<MSD+2;i++) /*-子目录都初始化为空-*/
	{
		cur_mkdir->directitem[i].sign=0;
		cur_mkdir->directitem[i].firstdisk=-1;
		strcpy(cur_mkdir->directitem[i].name,"");
		cur_mkdir->directitem[i].next=-1;
		cur_mkdir->directitem[i].property='0';
		cur_mkdir->directitem[i].size=0;
	}
	return 0;
}

int rmdir(char *name)
{
	int i,j,item;
	struct direct *temp_dir;
	/*-检查当前目录项中有无该目录-*/
	for(i=2;i<MSD+2;i++)
	{
		if(!strcmp(cur_dir->directitem[i].name,name))
			break;
	}
	if(i>=MSD+2) /*-没有这个文件或目录-*/
		return(-1);
	if(cur_dir->directitem[i].property!='1')/*-删除的不是目录-*/
		return(-3);

	/*-判断要删除的目录有无子目录-*/

	temp_dir=(struct direct *)(fdisk+cur_dir->directitem[i].next*DISKSIZE);
	for(j=2;j<MSD+2;j++)
	{
		if(temp_dir->directitem[j].next!=-1)
			break;
	}
	if(j<MSD+2)  /*-有子目录或文件-*/
		return(-2);
	/*------------找到起始盘块号,并将其释放----------------*/
	item=cur_dir->directitem[i].firstdisk;
	fat[item].em_disk='0';
	/*-修改目录项-*/
	cur_dir->directitem[i].sign=0;
	cur_dir->directitem[i].firstdisk=-1;
	strcpy(cur_dir->directitem[i].name,"");
	cur_dir->directitem[i].next=-1;
	cur_dir->directitem[i].property='0';
	cur_dir->directitem[i].size=0;

	return 0;
}

void dir()
{
	int i, j = 0;
	for(i=2;i<MSD+2;i++)
	{
		if(cur_dir->directitem[i].firstdisk!=-1) /*-如果存在子目录-*/
		{
			printf("%s\t",cur_dir->directitem[i].name);
			if(cur_dir->directitem[i].property=='0') /*-文件-*/
			{
				printf("%X\t%s\t%d\n",fdisk+cur_dir->directitem[i].firstdisk*DISKSIZE,
					cur_dir->directitem[i].protect_hold, cur_dir->directitem[i].size);
				j++;
			}
			else
				printf("\t<directory>\t\n");
		}
	}
}

int cd(char *name)
{
	int i,j,item;
	char *str;
	char *temp,*point,*point1;
	struct direct *temp_dir;
	temp_dir=cur_dir;
	str=name;

	if(!strcmp("\\",name))
	{
		cur_dir = root;
		strcpy(bufferdir,"Root:");
		return 0;
	}

	temp = (char *)malloc(DIR_MAXSIZE*sizeof(char));/*-最长路径名字分配空间-*/

	for(i=0;i<(int)strlen(str);i++)
		temp[i]=str[i];
	temp[i]='\0';

	for(j=0;j<MSD+2;j++) /*-查找该子目录是否在当前目录中-*/
	{
		if(!strcmp(temp_dir->directitem[j].name,temp))
			break;
	}

	free(temp);/*释放申请的临时空间*/
	//if(temp_dir->directitem[j].property!='1') /*-打开的不是目录-*/
	//return(-2);
	if(j>=MSD+2)  /*-不在当前目录-*/
		return(-1);

	item=temp_dir->directitem[j].firstdisk;
	/*-当前目录在磁盘中位置-*/
	temp_dir=(struct direct *)(fdisk+item*DISKSIZE);


	if(!strcmp("..",name))
	{
		if(cur_dir->directitem[j-1].sign!=1) /*-如果上级目录不是根目录-*/
		{
			point=strchr(bufferdir,'\\');  //查找字符串bufferdir中首次出现字符\ 的位置

			while(point!=NULL)
			{
				point1=point+1; /*-减去'\'所占的空间,记录下次查找的起始地址-*/
				point=strchr(point1,'\\');
			}
			*(point1-1)='\0'; /*-将上一级目录删除-*/
		}

	}

	else
	{
		//if(name[0] !='\\')
		bufferdir = strcat(bufferdir,"\\"); /*-修改当前目录-*/
		bufferdir = strcat(bufferdir,name);
	}
	cur_dir=temp_dir;  /*-将当前目录确定下来-*/
	return 0;
}

void show()
{
	printf("%s>",bufferdir);
}

void print()
{
	printf("*************************************************************\n");
	printf("**********************file system****************************\n");
	printf("*command style           illustration                       *\n");
	printf("*cd directory name       change current directory           *\n");
	printf("*mkdir directory name    create subdirectory                *\n");
	printf("*rmdir directory name    delete	subdirectory                *\n");
	printf("*dir                     list files                         *\n");
	printf("*create file name        create file                        *\n");
	printf("*del file name           delete file                        *\n");
	printf("*open file name          open file                          *\n");
	printf("*close file name         close file                         *\n");
	printf("*read                    read file                          *\n");
	printf("*write                   write file                         *\n");
	printf("*chmod                   change mode                        *\n");
	printf("*exit                    exit the file system               *\n");
	printf("*help                    check the help file                *\n");
	printf("*************************************************************\n");

}
#endif
