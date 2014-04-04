#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "commands.h"
#include "directory.h"

void main()
{
	FILE *fp;
	char ch;
	char a[100];
	char b[5];
	char code[13][10];
	char name[10];
	int i,flag,r_size;
	char *contect;

	contect = (char *)malloc(1000*sizeof(char));

	if((fp=fopen("disk.dat","rb"))==NULL)
	{
		printf("You have not format,Do you want format?(y/n)");
		scanf("%c",&ch);

		if(ch=='y')
		{
			initfile();
			printf("Successfully format! \n");
		}
		else
		{
			return;
		}
	}

	enter();
	print();
	//show();
	printf("you can enter 'help' to list which commands you can use.\n");
	strcpy(code[0],"exit");
	strcpy(code[1],"create");
	strcpy(code[2],"open");
	strcpy(code[3],"close");
	strcpy(code[4],"write");
	strcpy(code[5],"read");
	strcpy(code[6],"del");
	strcpy(code[7],"mkdir");
	strcpy(code[8],"rmdir");
	strcpy(code[9],"dir");
	strcpy(code[10],"cd");
    strcpy(code[11],"liuzixing");
    strcpy(code[12],"help");
	strcpy(code[13],"chmod");
    while (1){
        printf("welcome to the login interface of  the file system!\n");
        printf("please enter the password!(liuzixing)\n");
        scanf("%s",a);
        if (!strcmp(code[11],a)){
            printf("Successful login!\n");
            break;
        }else{
            printf("wrong password!\n");
        }
    }
	show();

	while(1)
	{


		scanf("%s",a);
		for(i=0;i<13;i++)
		{
			if(!strcmp(code[i],a))
				break;
		}
		switch(i)
		{
		case 0: //退出文件系统
			free(contect);
			halt();
			return;
		case 1: //创建文件
			scanf("%s",name);
			flag = create(name);
			if(flag==-1)
			{
				printf("Error: \n The length is too long !\n");
			}
			else if(flag==-2)
			{
				printf("Error: \n The direct item is already full !\n");
			}
			else if(flag==-3)
			{
				printf("Error: \n The number of openfile is too much !\n");
			}
			else if(flag==-4)
			{
				printf("Error: \n The name is already in the direct !\n");
			}
			else if(flag==-5)
			{
				printf("Error: \n The disk space is full!\n");
			}
			else
			{
				printf("Successfully create a file! \n");
			}

			show();
			break;

		case 2://打开文件
			scanf("%s",name);
			fd = open(name);
			if(fd == -1)
			{
				printf("Error: \n The open file not exit! \n");
			}
			else if(fd == -2)
			{
				printf("Error: \n The file have already opened! \n");
			}
			else if(fd == -3)
			{
				printf("Error: \n The number of open file is too much! \n");
			}
			else if(fd == -4)
			{
				printf("Error: \n It is a direct,can not open for read or write! \n");
			}
			else
			{
				printf("Successfully opened! \n");
			}
			show();
			break;

		case 3://关闭文件
			scanf("%s",name);
			flag = close(name);
			if(flag == -1)
			{
				printf("Error:\n The file is not opened ! \n");
			}
			else
			{
				printf("Successfully closed! \n");
			}
			show();
			break;

		case 4://写文件
			if(fd ==-1)
			{
				printf("Error:\n The file is not opened ! \n");
			}
			else
			{
				if (cur_dir->directitem[ci].protect_hold[1] == '-'){
					printf("The file is protected,you can not write!\n");
					show();
					break;
				}
				printf("Please input the file contect:\n");
				scanf("%s",contect);
				flag=write(fd,contect,strlen(contect));
				if(flag == 0)
				{
					printf("Successfully write! \n");
				}
				else
				{
					printf("Error:\n The disk size is not enough! \n");
				}
			}
			show();
			break;

		case 5://读文件
			if(fd ==-1)
			{
				printf("Error:\n The file is not opened ! \n");
			}
			else
			{
				if (cur_dir->directitem[ci].protect_hold[0] == '-'){
					printf("The file is protected,you can not read!\n");
					show();
					break;
				}
				//scanf("%s",contect);
				flag = read(fd,contect);
				if(flag == 0)
				{
					for(i=0;i<u_opentable.openitem[fd].size;i++)
					{
						printf("%c",contect[i]);
					}
					printf("\t\n");
				}
			}
			show();
			break;

		case 6://删除文件
			scanf("%s",name);
			flag = del(name);
			if(flag == -1)
			{
				printf("Error:\n The file not exit! \n");
			}
			else if(flag == -2)
			{
				printf("Error:\n The file is opened,please first close it ! \n");
			}
			else if(flag == -3)
			{
				printf("Error:\n The delete is not file ! \n");
			}
			else
			{
				printf("Successfully delete! \n");
			}
			show();
			break;

		case 7://创建子目录
			scanf("%s",name);
			flag = mkdir(name);
			if(flag == -1)
			{
				printf("Error:\n The length of name is to long! \n");
			}
			else if(flag == -2)
			{
				printf("Error:\n The direct item is already full ! \n");
			}
			else if(flag == -3)
			{
				printf("Error:\n The name is already in the direct ! \n");
			}
			else if(flag == -4)
			{
				printf("Error: \n '..' or '.' can not as the name of the direct!\n");
			}
			else if(flag == -5)
			{
				printf("Error: \n The disk space is full!\n");
			}
			else if(flag == 0)
			{
				printf("Successfully make direct! \n");
			}
			show();
			break;

		case 8://删除子目录
			scanf("%s",name);
			flag = rmdir(name);
			if(flag == -1)
			{
				printf("Error:\n The direct is not exist! \n");
			}
			else if(flag == -2)
			{
				printf("Error:\nThe direct has son direct ,please first remove the son direct!\n");
			}
			else if(flag == -3)
			{
				printf("Error:\n The remove is not direct ! \n");
			}
			else if(flag == 0)
			{
				printf("Successfully remove direct! \n");
			}
			show();
			break;

		case 9://显示当前子目录
			dir();
			show();
			break;

		case 10://更改当前目录
			scanf("%s",name);
			flag = cd(name);
			if(flag == -1)
			{
				printf("Error:\n The path no correct!\n");
			}
			else if(flag == -2)
			{
				printf("Error:\nThe opened is not direct!\n");
			}

			show();
			break;
        case 12://帮助文件
            print();
			show();
            break;
		case 13:
			scanf("%s %s",name,b);

			for(i=2;i<MSD+2;i++) /*--查找要删除文件是否在当前目录中-*/
			{
				if(!strcmp(cur_dir->directitem[i].name,name))
					break;
			}
			strcpy(cur_dir->directitem[i].protect_hold, b);
			show();
			break;
		default:
			printf("\n Error!\n The command is wrong! \n");
			show();
		}
	}
}
