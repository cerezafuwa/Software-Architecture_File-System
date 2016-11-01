// DOC.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include"stdafx.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>

#define NULL 0
#define BLOCKSIZE 1024 //���̿��С
#define SIZE 1024000//������̿ռ��С
#define END 65535//FAT�е��ļ�������־
#define FREE 0//FAT���̿���б�־
#define ROOTBLOCKNUM 2 //��Ŀ¼����ռ�̿�����
#define MAXOPENFILE 10//���ͬʱ���ļ�����
#define MAXFILENUM 10//��ǰĿ¼������ŵ��ļ�
#define DATA_DISK_NUM 995//�������̿�����
#define AFTERROOTPOSITION 2//��Ŀ¼֮���������

struct FCB 
{
	char filename[8];//�ļ���
	char exname[3];//�ļ���չ��
	unsigned char attribute;//�ļ������ֶ�:ֵΪ0ʱ��ʾĿ¼�ļ���ֵΪ1ʱ��ʾ�����ļ�
	struct tm date_time;//���������ں�ʱ��
	unsigned short first;//�ļ���ʼ�̿��
	unsigned long length;//�ļ�����(�ֽ���)
	char free;//��ʾĿ¼���Ƿ�Ϊ�գ���ֵΪ0����ʾ�գ�ֵΪ1����ʾ�ѷ���
};

struct FAT
{
	unsigned short id;
};

typedef struct USEROPEN //�û����ļ��� ֻ�����ļ�
{
    char filename[8];//�ļ���
	char exname[3];//�ļ���չ��
	unsigned char attribute;//�ļ����ԣ�ֵΪ0ʱ��ʾ�ļ�Ŀ¼��ֵΪ1ʱ��ʾ�����ļ�
	unsigned short time;//�ļ�����ʱ��
	unsigned short data;//�ļ���������
	unsigned short first;//�ļ���ʼ�̿��
	unsigned long length;//�ļ�����(�������ļ����ֽ�������Ŀ¼�ļ�������Ŀ¼�����)
	char free;//��ʾĿ¼���Ƿ�Ϊ�գ���ֵΪ0����ʾ�գ�ֵΪ1����ʾ�ѷ���
	int dirno;// ��Ӧ���ļ���Ŀ¼���ڸ�Ŀ¼�ļ��е��̿��
	int diroff;//��Ӧ���ļ���Ŀ¼���ڸ�Ŀ¼�ļ���dirno�̿��е�Ŀ¼�����
	char dir[80];//��Ӧ���ļ����ڵ�Ŀ¼��������������ټ���ָ���ļ��Ƿ��Ѿ���
	int count;//��дָ�����ļ��е�λ��
	char fcbstate;//�Ƿ��޸����ļ���FCB�����ݣ�����޸�����Ϊ1������Ϊ0
	char topenfile;//��ʾ���û��򿪱����Ƿ�Ϊ�գ���ֵΪ0����ʾΪ�գ������ʾ�ѱ�ĳ���ļ�ռ��
}useropen;

struct BLOCK0
{
	char information[200];//�洢һЩ������Ϣ������̿��С�����̿������������ļ�����
	unsigned short root;//��Ŀ¼�ļ�����ʼ�̿��
	char *startblock;//�����������������ʼλ��
};

FILE *fp;
struct FAT *fat; //FAT�����ʼλ��
struct FCB *root; //��Ŀ¼
struct FCB *cur_dir; //��ǰĿ¼
struct FCB *father_dir; //��Ŀ¼��ַ

int fd = -1; //open()�����ķ���ֵ���ļ�������
time_t timep;
char filename[10];
int flag = 0;
int block_num = 0; //��ռ�̿�����

char *myvhard; //ָ��������̵���ʼ��ַ
useropen openfilelist[MAXOPENFILE];//�û����ļ�������
useropen *ptrcurdir; //ָ���û����ļ����еĵ�ǰĿ¼���ڴ��ļ������λ��
char currentdir[80]; //��¼��ǰĿ¼��Ŀ¼��
char *startp; //��¼�����������������ʼλ��


void filesys_ui();
void show();
void startsys();
void format();
void cd(char *dirname);
void mkdir(char *dirname);
void rmdir(char *dirname);
void ls(void);
int create(char *filename);
void rm(char *filename);
int open(char *filename);
void close(int fd);
int write(int fd);
int do_write(int fd, char *text, int len, char wstyle);
int read(int fd, int len);
int do_read(int fd, int len, char *text);
int exitsys();

void filesys_ui()
{
	printf("��ʽ���洢��format\n");
    printf("������Ŀ¼mkdir\n");
	printf("ɾ����Ŀ¼rmdir\n");
	printf("��ʾĿ¼�е�����ls\n");
    printf("���ĵ�ǰĿ¼cd\n");
	printf("�����ļ�create\n");
	printf("���ļ�open\n");
    printf("�ر��ļ�close\n");
	printf("д�ļ�write\n");
	printf("���ļ�read\n");
	printf("ɾ���ļ�rm\n");
	printf("�˳�ϵͳexitsys\n");
}

void startsys()
{
	char data;//�ݴ�����
	int i;
	char fileflag[10] = { "10101010" };//�ļ�ħ��
	myvhard = (char *)malloc(SIZE*sizeof(char));
	startp = myvhard + BLOCKSIZE + BLOCKSIZE * 4;
	fp = NULL;
	fopen_s(&fp,"myfsys.txt", "rb");
	if (fp != NULL)
	{
		fread(myvhard, BLOCKSIZE, 1000, fp);
		data = myvhard[8];
		myvhard[8] = '\0';
		if (strcmp(myvhard, fileflag) == 0)
		{
			myvhard[8] = data;
 		    fwrite(myvhard, BLOCKSIZE, 1000, fp);
		}
		else
		{
			printf("myfsys �ļ�ϵͳ�����ڣ����ڿ�ʼ�����ļ�ϵͳ\n");
			format();
			fwrite(myvhard, BLOCKSIZE, 1000, fp);
		}
	}
	else
	{
		printf("myfsys �ļ�ϵͳ�����ڣ����ڿ�ʼ�����ļ�ϵͳ\n");
		format();
	    fopen_s(&fp,"myfsys.txt", "wb");
		fwrite(myvhard, BLOCKSIZE, 1000, fp);
	}
	fclose(fp);
	fat = (struct FAT *)(myvhard + BLOCKSIZE);
    startp = myvhard + BLOCKSIZE + 4 * BLOCKSIZE;

	for (i = 1; i<MAXOPENFILE; i++)//��ʼ���û��򿪱���
	{
		openfilelist[i].topenfile = 0;//Ŀ¼Ϊ��
	}
	strcpy_s(openfilelist[0].filename, "root");// ��Ŀ¼�ļ�
	strcpy_s(openfilelist[0].exname, "\0");
	openfilelist[0].attribute = 0;
	openfilelist[0].first = 5;//��������������ʼ
	openfilelist[0].length = 0;
	openfilelist[0].free = 1;
	openfilelist[0].dirno = 5;
	openfilelist[0].diroff = 0;
	strcpy_s(openfilelist[0].dir, ".");
	openfilelist[0].count = 0;
	openfilelist[0].fcbstate = 0;
	openfilelist[0].topenfile = 0;
	ptrcurdir = openfilelist;
	strcpy_s(currentdir, "\\");
	cur_dir = (struct FCB *)startp;//���õ�ǰĿ¼
}

//��ʽ���洢��
void format()
{
	struct BLOCK0 *block0;
	int i;
	cur_dir = (struct FCB *)startp;
	
	//���������ʼ��
	block0 = (struct BLOCK0 *)(myvhard);
	strcpy_s(block0->information, "10101010");//�ļ�ħ��
	strcat_s(block0->information, "2,1,1,5,1024,1000,5120");
	block0->root = 5;
	block0->startblock = startp;

	//����FAT��
	fat = (struct FAT *)(myvhard + BLOCKSIZE);
	for (i = 0; i<BLOCKSIZE; i++)
	{
		fat[i].id = FREE;//����������������δ���� 
	}
	fat[0].id = fat[1].id = 1;//��Ŀ¼���Ѿ���������
	fat = (struct FAT *)(myvhard + BLOCKSIZE + BLOCKSIZE + BLOCKSIZE);
	for (i = 0; i<BLOCKSIZE; i++)//���ݱ�
	{
		fat[i].id = FREE;//����������������δ���� 
	}
	fat[0].id = fat[1].id = 1;//��Ŀ¼���Ѿ���������
	root = (struct FCB *)(startp);//��Ŀ¼��ַ����������ʼ��ַ

	//���ø�Ŀ¼�����"."�ļ�
	strcpy_s(root[0].filename, ".");
	root[0].exname[0] = '\0';
	root[0].attribute = 0;
	root[0].first = 0;
	root[0].length = 0;
	root[0].free = 0;
	time(&timep);
	root[0].date_time = *localtime(&timep);

	//����Ŀ¼�����".."�ļ�
	strcpy(root[1].filename, "..");
	root[1].exname[0] = '\0';
	root[1].attribute = 0;
	root[1].first = 0;
	root[1].length = 0;
	root[1].free = 0;
	time(&timep);
	root[1].date_time = *localtime(&timep);

	//��Ŀ¼��ʼ��
	for (i = 2; i<MAXFILENUM + 2; i++)
	{
		strcpy(cur_dir[i].filename, "");
		strcpy(cur_dir[i].exname, "");
		cur_dir[i].attribute = -1;
		cur_dir[i].first = FREE;
		cur_dir[i].length = 0;
		cur_dir[i].free = 0;
	}
}

void init()
{
	int i = 0;
	myvhard = NULL;
	for (i = 0; i<MAXOPENFILE; i++)
		openfilelist[i].free = 0;//δ����
	ptrcurdir = NULL;
	strcpy(currentdir, "\0");
}

int my_exitsys()
{
	fp = fopen("myfsys.txt", "wb");
	fwrite(myvhard, BLOCKSIZE, 1000, fp);
	fclose(fp);
	free(myvhard);
	return 1;
}

void show()
{
	printf("%s>:", currentdir);
}

//���ĵ�ǰĿ¼
void cd(char *dirname)
{
	int i;
	struct FCB *cd_dir;//����·��  Ŀ¼��ָ̽��
	int cur_num;
	char temp[10];	//�ݴ���Ŀ¼��
	if (strcmp(dirname, "\\") == 0)//����ʼĿ¼
	{
		strcpy(currentdir, "\\");
		cur_dir = (struct FCB *)startp;
		return;
	}
	if (dirname[0] == '\\')//����·������
	{
		if (dirname[strlen(dirname) - 1] == '\\')
			dirname[strlen(dirname) - 1] = '\0';
		cur_num = 0;//��ǰ�α�
		cd_dir = (struct FCB *)startp;
		for (i = 1; dirname[i]; i++)
		{
			if (dirname[i] != '\\')
				temp[cur_num++] = dirname[i];
			else
			{
				temp[cur_num] = '\0';
				for (i = 2; i<MAXFILENUM + 2; i++)
					if (strcmp(cd_dir[i].filename, temp) == 0 && cd_dir[i].attribute == 0)
					{
						break;
					}
				if (i >= MAXFILENUM)
				{
					printf("�Ҳ�����Ŀ¼\n");
					return;
				}
				cd_dir = (struct FCB *)(startp + cd_dir[i].first*BLOCKSIZE);
				cur_num = 0;
			}
		}
		temp[cur_num] = '\0';
		for (i = 2; i<MAXFILENUM + 2; i++)
			if (strcmp(cd_dir[i].filename, temp) == 0 && cd_dir[i].attribute == 0)
				break;
		if (i >= MAXFILENUM + 2)
		{
			printf("û�д�Ŀ¼�ļ�\n");
			return;
		}
		cd_dir = (struct FCB *)(startp + cd_dir[i].first*BLOCKSIZE);
		cur_num = 0;
		strcpy(currentdir, dirname);
		strcat(currentdir, "\\");//���Ŀ¼
		cur_dir = cd_dir;//�ѽ����Ŀ¼������ǰĿ¼
    	return;
	}
	if (strcmp(dirname, "..") == 0)
	{
		if ((char *)cur_dir == startp)//�ڸ�Ŀ¼�²���
			return;
		cur_dir = (struct FCB *)(startp + cur_dir[1].first*BLOCKSIZE);//�ҵ���Ŀ¼
		currentdir[strlen(currentdir) - 1] = '\0';
		for (i = strlen(currentdir) - 2; i >= 0; i--)
			if (currentdir[i] == '\\')
				break;
		currentdir[i + 1] = '\0';//�޸�dirname���ݣ���ǰĿ¼
		return;
	}
	if (strcmp(dirname, ".") == 0)
		return;
	for (i = 2; i<MAXFILENUM + 2; i++)
		if (strcmp(cur_dir[i].filename, dirname) == 0 && cur_dir[i].attribute == 0)
			break;
	if (i >= MAXFILENUM + 2)//�Ҳ����л���Ŀ¼
	{
		printf("û�д�Ŀ¼\n");
		return;
	}

	strcat(currentdir, cur_dir[i].filename);
	cur_dir = (struct FCB *)(startp + cur_dir[i].first*BLOCKSIZE);//��ǰĿ¼ָ��䵽��ǰ
	strcat(currentdir, "\\");//���Ŀ¼
}

//������Ŀ¼
void mkdir(char *dirname)
{
	int i, j;  //j����Ŀ¼�ļ��е����  i����FAT���ҵ��Ŀ��п��
	struct FCB *cur_mkdir;
	if (strchr(dirname, '\\'))/*���Ŀ¼������ '\'�ַ�*/
	{
		printf("Ŀ¼�ļ����в��ܴ���'\'\n");
		return;
	}
	if (!strcmp(dirname, "."))
	{
		printf("Ŀ¼�ļ����в��ܴ���'.'\n");
		return;
	}
	if (!strcmp(dirname, ".."))
	{
		printf("Ŀ¼�ļ����в��ܴ���'..'\n");
		return;
	}
	for (j = 2; j<MAXFILENUM + 2; j++)//��¼��ǰĿ¼�е�δʹ�õĺ�
		if (strcmp(cur_dir[j].filename, "") == 0)
			break;
	for (i = 2; i<MAXFILENUM + 2; i++)
		if (strcmp(cur_dir[i].filename, dirname) == 0)
			break;
	if (i<MAXFILENUM + 2)//��������
	{
		printf("��ǰ������Ŀ¼�ļ���������\n");
		return;
	}
	for (i = AFTERROOTPOSITION; i<DATA_DISK_NUM; i++)//��FAT���ҿ��д��̿�
	{
		if (fat[i].id == FREE)
			break;
	}
	if (i >= DATA_DISK_NUM)
	{
		printf("���̿�������\n");
		return;
	}
	fat[i].id = END;// �˿���Է�����

	//��дĿ¼��
	strcpy(cur_dir[j].filename, dirname);
	cur_dir[j].first = i;
	cur_dir[j].attribute = 0;
	cur_dir[j].length = 0;//��ǰĿ¼���е��ļ�����������
	cur_dir[j].free = 0;
	time(&timep);
	cur_dir[j].date_time = *localtime(&timep);//����ʱ��

	//��ʼ����������Ŀ¼����Ϣ.
	cur_mkdir = (struct FCB *)(startp + cur_dir[j].first*BLOCKSIZE);//�ҵ���������Ŀ¼�ļ���ĵ�ַ
	strcpy(cur_mkdir[0].filename, ".");
	cur_mkdir[0].attribute = 0;
	cur_mkdir[0].first = i;
	cur_mkdir[0].length = 0;
	cur_mkdir[0].free = 1;
	time(&timep);
	cur_mkdir[0].date_time = *localtime(&timep);//����ʱ��
	strcpy(cur_mkdir[1].filename, "..");
	cur_mkdir[1].attribute = 0;
	cur_mkdir[1].first = ((char *)cur_dir - startp) / BLOCKSIZE;//������Ŀ¼��.. �ҵ���Ŀ¼
	cur_mkdir[1].length = 0;
	cur_mkdir[1].free = 1;
	time(&timep);
	cur_mkdir[1].date_time = *localtime(&timep);//����ʱ��

	//��Ŀ¼��ʼ��
	for (i = 2; i<MAXFILENUM + 2; i++)
	{
		strcpy(cur_mkdir[i].filename, "");
		strcpy(cur_mkdir[i].exname, "");
		cur_mkdir[i].attribute = -1;
		cur_mkdir[i].first = FREE;
		cur_mkdir[i].length = 0;
		cur_mkdir[i].free = 0;
	}
	father_dir = (struct FCB*)(startp + cur_dir[1].first*BLOCKSIZE);//��ǰĿ¼�ĸ�Ŀ¼ //��ǰĿ¼�ļ�������
	for (i = 2; i < MAXFILENUM + 2; i++)//��Ŀ¼�ж�Ӧ�ĵ�ǰĿ¼����ļ�������
	{
		if ((struct FCB*)(startp + father_dir[i].first*BLOCKSIZE) == cur_dir)
			break;
		father_dir[i].length++;
	}
	printf("Ŀ¼�ļ���ʼ�ɹ�\n");
}

//ɾ����Ŀ¼
void rmdir(char *dirname)
{
	int i;
	struct FCB *rm_dir;//Ҫɾ����Ŀ¼�ļ�

	//��鵱ǰĿ¼�������޸�Ŀ¼
	for (i = 2; i < MAXFILENUM + 2; i++)
	{
		if (strcmp(cur_dir[i].filename, dirname) == 0 && cur_dir[i].attribute == 0)
			break;
		if (i >= MAXFILENUM + 2)
		{
			printf("�Ҳ������ļ�\n");
			return;
		}
	}
	
	//�ҵ�����Ҫɾ�����ļ�Ŀ¼,��ɾ����Ŀ¼�ļ��е��ļ��Ѿ���
	rm_dir = cur_dir + i;
	if (rm_dir->length == 0)//��Ҫɾ�����ļ�û���ļ���
	{
		strcpy_s(rm_dir->filename, "");
		strcpy_s(rm_dir->exname, "");
    	rm_dir->attribute = -1;
		fat[rm_dir->first].id = FREE;
		rm_dir->first = -1;
		rm_dir->length = 0;
		rm_dir->free = 0;
		printf("ɾ���ɹ�\n");
	}
	else
	{
		printf("��Ҫɾ����Ŀ¼�ļ��ڴ����ļ���Ŀ¼�ļ������Բ���ɾ����Ŀ¼�ļ�\n");
		return;
	}
}

//��ʾĿ¼�е�����
void ls()
{
	int i;
	for (i = 0; i < MAXFILENUM; i++)
	{
		if (strcmp(cur_dir[i].filename, "") != 0)
		{
			if (cur_dir[i].attribute == 0)//Ŀ¼�ļ�
			{
				printf("%s\t\t\t\t\t", cur_dir[i].filename);
				printf("%d %d %d ", (1900 + cur_dir[i].date_time.tm_year), (1 + cur_dir[i].date_time.tm_mon), cur_dir[i].date_time.tm_mday);
   			    printf("%d:%d:%d\n", cur_dir[i].date_time.tm_hour, cur_dir[i].date_time.tm_min, cur_dir[i].date_time.tm_sec);
			}
			else
			{
				printf("%s\t\t\t%ld\t\t", cur_dir[i].filename, cur_dir[i].length);
				printf("%d %d %d ", (1900 + cur_dir[i].date_time.tm_year), (1 + cur_dir[i].date_time.tm_mon), cur_dir[i].date_time.tm_mday);
				printf("%d:%d:%d\n", cur_dir[i].date_time.tm_hour, cur_dir[i].date_time.tm_min, cur_dir[i].date_time.tm_sec);
			}
		}
	}
}

//�����ļ�
int create(char *filename)
{
	int i, j, k;//i�����ҵ��˵�ǰĿ¼�е�Ŀ¼�k����fat�еĿ��п�
	for (i = 2; i<MAXFILENUM + 2; i++)
	{
		if (cur_dir[i].first == FREE)
			break;
	}
	for (j = 2; j < MAXFILENUM + 2; j++)
	{
		if (strcmp(cur_dir[j].filename, filename) == 0)
			break;
	}
	if (i >= MAXFILENUM + 2)
	{
    	printf("û�пյ�Ŀ¼����,���ܴ����ļ�\n");
		return -1;
	}
	if (j<MAXFILENUM + 2)
	{
		printf("���������ļ������ܴ����ļ�\n");
		return -1;
	}
	for (k = 2; k < DATA_DISK_NUM; k++)//�ҿ��еĿ�
	{
		if (fat[k].id == FREE)
			break;
	}
	if (k >= DATA_DISK_NUM)//�Ҳ������еĿ�
		return -1;
	if (strcmp(filename, "..") == 0 || strcmp(filename, ".") == 0)
	{
		printf("�����ļ�ʧ�ܣ����ܽ�%s���ļ�\n", filename);
		return -1;
	}
	fat[k].id = END;

	//д�����ļ�Ŀ¼��
	time(&timep);	//��ȡʱ��
	cur_dir[i].date_time = *localtime(&timep);
	strcpy_s(cur_dir[i].filename, filename);
	strcpy_s(cur_dir[i].exname, "txt");
	cur_dir[i].attribute = 1;
	cur_dir[i].first = k;
	cur_dir[i].free = 0;
	cur_dir[i].length = 0;
	father_dir = (struct FCB*)(startp + cur_dir[1].first*BLOCKSIZE);//��ǰĿ¼�ļ�������

	for (i = 2; i < MAXFILENUM + 2; i++)//��Ŀ¼�ж�Ӧ�ĵ�ǰĿ¼����ļ�������
	{
		if ((struct FCB*)(startp + father_dir[i].first*BLOCKSIZE) == cur_dir)
			break;
	}
	father_dir[i].length++;
	fd = open(filename);
}

//ɾ���ļ�
void rm(char *filename)
{
	int i;
	int id, idnext;//FAT�� �ļ�����Ӧ���������
	for (i = 1; i<MAXOPENFILE; i++)
		if (strcmp(openfilelist[i].filename, filename) == 0 && openfilelist[i].topenfile == 1)
		{
			printf("���ļ����ڱ�ʹ�ã����ȹر�\n");
			return;
		}
	for (i = 2; i<MAXFILENUM + 2; i++)
		if (strcmp(cur_dir[i].filename, filename) == 0 && cur_dir[i].attribute == 1)
			break;
	if (i >= MAXFILENUM + 2)
	{
		printf("�Ҳ������ļ�\n");
		return;
	}
	id = cur_dir[i].first;
	while (1)  //ɾ��FAT���ļ�����Ӧ�������̿��
	{
		if (fat[id].id == END)
		{
			fat[id].id = FREE;
			break;
		}
		else
		{
			idnext = fat[id].id;
			fat[id].id = FREE;
			id = idnext;
		}
	}

	//�ͷ�Ŀ¼��
	strcpy_s(cur_dir[i].filename, "");
	strcpy_s(cur_dir[i].exname, "");
	cur_dir[i].attribute = -1;
	cur_dir[i].first = FREE;
	cur_dir[i].length = 0;
	cur_dir[i].free = 0;
}

//���ļ�
int open(char *filename)
{
	int i, j;
	for (i = 1; i<MAXOPENFILE + 2; i++)
		if (openfilelist[i].topenfile == 1 && strcmp(openfilelist[i].dir, currentdir) == 0 && strcmp(openfilelist[i].filename, filename) == 0)
		{
			fd = i;
			return -1;//���ļ��Ѿ���
		}
	for (i = 2; i<MAXFILENUM + 2; i++)
		if (strcmp(cur_dir[i].filename, filename) == 0 && cur_dir[i].attribute == 1)
			break;
	if (i >= MAXFILENUM + 2)//��ǰĿ¼�ļ��µ�FCB���
	{
		printf("�ļ������ڣ��ļ���ʧ��\n");
		return -1;
	}
	for (j = 1; j<MAXFILENUM; j++)//���ļ����ҵ����  j��־
		if (openfilelist[j].topenfile == 0)
			break;

	//���ļ�����Ϣ�ŵ����ļ�������
	strcpy_s(openfilelist[j].filename, cur_dir[i].filename);
	strcpy_s(openfilelist[j].exname, cur_dir[i].exname);
	openfilelist[j].attribute = cur_dir[i].attribute;
	openfilelist[j].first = cur_dir[i].first;
	openfilelist[j].length = cur_dir[i].length;
	openfilelist[j].free = cur_dir[i].free;
	openfilelist[j].dirno = i;//���ļ��ڸ�Ŀ¼�ļ��е�λ��
	openfilelist[j].diroff = 0;
	strcpy(openfilelist[j].dir, currentdir);
	openfilelist[j].count = 0;
	openfilelist[j].fcbstate = 0;
	openfilelist[j].topenfile = 1;//���ļ���ʹ����
	fd = j;  //openfilelist �������򿪵��ļ�
	return j;
}

//�ر��ļ�
void close(int fd)
{
	int dirno;
	int i;
	if (fd >= MAXOPENFILE)
	{
		printf("�ر��ļ����������ļ��򿪱���������\n");
		return;
	}
	dirno = openfilelist[fd].dirno;

	//���ļ����޸İ�����д�ص���ӦFCB��
	if (openfilelist[fd].fcbstate == 1)
	{
		strcpy_s(cur_dir[dirno].filename, openfilelist[fd].filename);
		strcpy_s(cur_dir[dirno].exname, openfilelist[fd].exname);
		cur_dir[dirno].attribute = openfilelist[fd].attribute;
		cur_dir[dirno].first = openfilelist[fd].first;
		cur_dir[dirno].length = openfilelist[fd].length;
		cur_dir[dirno].free = openfilelist[fd].free;
		time(&timep);
		cur_dir[dirno].date_time = *localtime(&timep);//����ʱ��
	}
	openfilelist[fd].topenfile = 0;
}

//���ļ�
int read(int fd, int len)
{
	char *text;
	int i, item, reallen;
	text = (char *)malloc(BLOCKSIZE*sizeof(char));
	int realbytenum;
	if (fd >= MAXOPENFILE || fd<0)
	{
		printf("��ȡ�ļ����������û��ļ��򿪱���\n");
		return -1;
	}
	printf("�������ļ�������:      \n");
	item = openfilelist[fd].first;
	while (1)
	{
		if (len>BLOCKSIZE)
			reallen = BLOCKSIZE;
		else
			reallen = len;
		block_num++;
		realbytenum = do_read(fd, reallen, text);
		for (i = 0; i<realbytenum; i++)
			putchar(text[i]);
		if (fat[item].id == END)
			break;
		item = fat[item].id;
		len -= BLOCKSIZE;
	}
	printf("\n");
	block_num = 0;//��־�ļ���ռ������
	return realbytenum;//����ʵ�ʶ������ֽ���
}


int do_read(int fd, int len, char *text)
{
	char *cur_first;//�ļ��ĵ�ǰ���̿�
	char *buf = NULL;
	int blocknum, blocknum_off;//�ļ���ռ�Ĵ��̿�����
	int i, j;
	int item;
	int realreadnum = 0;
	buf = (char *)malloc(BLOCKSIZE*sizeof(char));
	if (buf == NULL)
	{
		printf("����ʧ��\n");//����ʧ��
		return -1;
	}
	item = openfilelist[fd].first;//ָ��Ҫ���ĵ�һ��
	for (i = 1; i<block_num; i++)
		item = fat[item].id;//���¸��ļ��̿�
	cur_first = startp + item*BLOCKSIZE;//�ļ����̿�λ��
	for (j = 0; j<len; j++)
	{
		text[realreadnum++] = cur_first[j];
	}
	return realreadnum;
}

//д�ļ�
int write(int fd)
{
	char getcommand;
	int cur_first, leftbyte=0;
	int temp, file_position;
	char text[BLOCKSIZE * 2];
	char ch;
	int realbytewrite;
	int i, j;
	if (fd>MAXOPENFILE)
	{
		printf("�����ٴ��ļ���\n");
		return -1;
	}

	//�ļ��ĵ�һ��λ��
	if (fd == -1)
	{
		printf("���ȴ������ļ�\n");
		return -1;
	}
	printf("��������������д��ʽ\n");
	printf("1.�ض�д2.����д 3.׷��д\n");
		getchar();//����û�õĻس���
	scanf("%c", &getcommand);
	if (getcommand == ' ')
		scanf("%c", &getcommand);
	getchar();
	j = 0;
	cur_first = openfilelist[fd].first;
	if (getcommand == '1')
		if (fat[cur_first].id != END)
		{
			temp = fat[cur_first].id;
			fat[cur_first].id = END;
			cur_first = temp;//cur_first ָ����һ��
			while (fat[cur_first].id != END)
			{
				temp = fat[cur_first].id;
				fat[cur_first].id = FREE;
				cur_first = temp;
			}
			fat[cur_first].id = FREE;//�ͷų���ǰ�ļ��ĵ�һ����������������Ŀռ�
		}
	//jָʾ���������ݵĳ���
	if (getcommand == '1')
	{
		openfilelist[fd].length = 0;
		openfilelist[fd].count = 0;
		printf("�������ļ����� �� CTRL+Z����\n");
		while (1)
		{
			while (ch = getchar())//�����������CTRL+Z
			{
				if (ch == -1 || j == BLOCKSIZE)
					break;
				else
					text[j++] = ch;
			}
			realbytewrite = do_write(fd, text, j, getcommand);
			if (realbytewrite<0)
			{
				printf("���̿�������\n");
				return -1;
			}
			j = 0;
			openfilelist[fd].count += realbytewrite;
			openfilelist[fd].length += realbytewrite;
			openfilelist[fd].fcbstate = 1;
			if (ch == -1)
				break;
		}
	}
	else if (getcommand == '2')
	{
		printf("������Ӵ��ļ����ļ���λ�ÿ�ʼд\n");
		scanf("%d", &file_position);
		if (file_position>openfilelist[fd].length)
		{
			printf("�������,��λ�ñ��ļ���󳤶Ȼ���\n");
			return -1;
		}
		openfilelist[fd].count = file_position;//ָ��λ��
		printf("�������ļ����� �� CTRL+Z����\n");
		getchar();
		while (1)
		{
			while (ch = getchar())
			{
				if (ch == -1 || j == BLOCKSIZE)
					break;
				else
				{
					text[j++] = ch;
					leftbyte++;
					if (leftbyte == BLOCKSIZE - openfilelist[fd].count%BLOCKSIZE)
					{
						leftbyte = BLOCKSIZE + 1;//��leftbyteʧЧ
						flag = 1;
						break;
					}
				}
			}
			if (ch == -1 && leftbyte<BLOCKSIZE)
				flag = 1;
			realbytewrite = do_write(fd, text, j, getcommand);
			j = 0;
			flag = 0;
			openfilelist[fd].count += realbytewrite;
			if (openfilelist[fd].length<openfilelist[fd].count)
				openfilelist[fd].length = openfilelist[fd].count;
			openfilelist[fd].count = 0;//�ļ�ָ��
			openfilelist[fd].fcbstate = 1;
			if (ch == -1)
				break;
		}
	}
	else if (getcommand == '3')//׷��д
	{
		flag = 0;
		leftbyte = 0;
		printf("�������ļ����� �� CTRL+Z����\n");
		while (1)
		{
			while (ch = getchar())
			{
				if (ch == -1 || j == BLOCKSIZE)
					break;
				else
				{
					text[j++] = ch;
					leftbyte++;
				}
				if (leftbyte == BLOCKSIZE - openfilelist[fd].length%BLOCKSIZE)
				{
					leftbyte = BLOCKSIZE + 1;
					flag = 1;
					break;
				}
			}
			if (ch == -1 && leftbyte<BLOCKSIZE)//׷�ӵĵ�һ�鵱ǰ��
				flag = 1;
			realbytewrite = do_write(fd, text, j, getcommand);
			flag = 0;
			j = 0;
			if (realbytewrite<0)
			{
				printf("���̿�������\n");
				return -1;
			}
			openfilelist[fd].count += realbytewrite;
			openfilelist[fd].length += realbytewrite;
			openfilelist[fd].fcbstate = 1;
			if (ch == -1)
				break;
		}
	}
	else
	{
		printf("�����������������\n");
	}
	return realbytewrite;
}

int do_write(int fd, char *text, int len, char wstyle)
{
	int i, j, k;//�α�
	int textlen;
	char *file_point;
	int item, freeitem;
	int cur_block, cur_blockoff;
	int tmplen;//ʵ��д����ֽ���
	if (wstyle == '1')
	{
		if (openfilelist[fd].length == 0)
		{
			file_point = startp + openfilelist[fd].first*BLOCKSIZE;
			for (j = 0; j<len; j++)
				file_point[j] = text[j];
			return j;
		}
		for (i = 2; i<DATA_DISK_NUM; i++)//�ҵ��յĴ��̿�
			if (fat[i].id == FREE)
				break;
		if (i >= DATA_DISK_NUM)
		{
			printf("���̿�������\n");
			return -1;
		}
		item = openfilelist[fd].first;
		while (fat[item].id != END)
			item = fat[item].id;//�ҵ����ļ���Խ���������̿�
		fat[item].id = i;//�̿�� �ļ���ʼҪд���̿��
		file_point = startp + i*BLOCKSIZE;//�ļ�ָ�� Ҫд������ 

		for (j = 0; j<len; j++)
			file_point[j] = text[j];

		fat[i].id = END;//���ҵ��Ŀ��п������ 
	}

	else if (wstyle == '2')
	{
		if (flag == 1)//��������д
		{
			block_num = openfilelist[fd].count / BLOCKSIZE;
			cur_blockoff = openfilelist[fd].count%BLOCKSIZE;
			item = openfilelist[fd].first;
			for (i = 1; i<block_num; i++)
				item = fat[item].id;
			file_point = startp + item*BLOCKSIZE;
			for (i = 0; i<len; i++)
				file_point[cur_blockoff + i] = text[i];
		}
		else  //���¿��
		{
			for (i = 2; i<DATA_DISK_NUM; i++)//�ҵ��յĴ��̿�
				if (fat[i].id == FREE)
					break;
			if (i >= DATA_DISK_NUM)
			{
				printf("���̿�������\n");
				return -1;
			}
			item = openfilelist[fd].first;
			while (fat[item].id != END)
				item = fat[item].id;
				fat[item].id = i;
			file_point = startp + i*BLOCKSIZE;
			for (j = 0; j<len; j++)
				file_point[j] = text[j];
			fat[i].id = END;//���ҵ��Ŀ��п������
		}
	}

	else if (wstyle == '3')
	{
		if (flag == 1)
		{
			item = openfilelist[fd].first;
			while (fat[item].id != END)
				item = fat[item].id;
			cur_blockoff = openfilelist[fd].length%BLOCKSIZE;
			file_point = startp + item*BLOCKSIZE;
			for (i = 0; i<len; i++)
				file_point[cur_blockoff + i] = text[i];//׷��д����
		}
		else
		{
			for (i = 2; i<DATA_DISK_NUM; i++)//�ҵ��յĴ��̿�
				if (fat[i].id == FREE)
					break;
			if (i >= DATA_DISK_NUM)
			{
				printf("���̿�������\n");
				return -1;
			}

			item = openfilelist[fd].first;
			while (fat[item].id != END)
				item = fat[item].id;//�ҵ����ļ���Խ���������̿�
			fat[item].id = i;//�̿�� �ļ���ʼҪд���̿��
			file_point = startp + i*BLOCKSIZE;//�ļ�ָ�� Ҫд������ 
			for (j = 0; j<len; j++)
				file_point[j] = text[j];
			fat[i].id = END;//���ҵ��Ŀ��п������
		}
	}
	return len;
}


int main()

{
	int i;
	char getcommand[30];
	char dirname[10];
	char absolute_dir[80];
	int len;
	init();
	char code[12][10];
	startsys();
	filesys_ui();
	strcpy(code[0], "format");//���������code������
	strcpy(code[1], "mkdir");
	strcpy(code[2], "rmdir");
	strcpy(code[3], "ls");
	strcpy(code[4], "cd");
	strcpy(code[5], "create");
	strcpy(code[6], "open");
	strcpy(code[7], "close");
	strcpy(code[8], "write");
	strcpy(code[9], "read");
	strcpy(code[10], "rm");
	strcpy(code[11], "exitsys");
	while (1)
	{
		show();
		scanf("%s", getcommand);
		for (i = 0; i<12; i++)
			if (strcmp(getcommand, code[i]) == 0)
				break;
		switch (i)
		{
		case 0:
			format();
			break;
		case 1:
			scanf("%s", dirname);
			mkdir(dirname);
			break;
		case 2:
			scanf("%s", dirname);
			rmdir(dirname);
			break;
		case 3:
			ls();
			break;
		case 4:
			scanf("%s", dirname);
			cd(dirname);
			break;
		case 5:
			scanf("%s", filename);
			create(filename);
			break;
		case 6:
			scanf("%s", filename);
			open(filename);
			break;
		case 7:
			scanf("%s", filename);
			if (filename[0] == '\\')//�򿪾���·�����ļ�
			{
				for (i = 1; i<MAXOPENFILE; i++)
				{
					strcpy_s(absolute_dir, openfilelist[i].dir);
					strcat(absolute_dir, openfilelist[i].filename);
					if (strcmp(filename, openfilelist[i].dir) == 0 && openfilelist[i].topenfile == 1)
					{
						fd = i;
						break;
					}
				}
			}
			else
			{
				for (i = 1; i<MAXOPENFILE; i++)
				{
					if (strcmp(openfilelist[i].filename, filename) == 0 && strcmp(openfilelist[i].dir, currentdir) == 0 && openfilelist[i].topenfile == 1)
						fd = i;
				}
				if (fd >= MAXOPENFILE)
				{
					printf("���ļ������ļ��򿪱���\n");
					break;
				}
			}
			close(fd);
			fd = -1;
			break;
		case 8:
			scanf("%s", filename);
			if (filename[0] == '\\')				
			{
				for (i = 1; i<MAXOPENFILE; i++)
				{
					strcpy_s(absolute_dir, openfilelist[i].dir);
					strcat(absolute_dir, openfilelist[i].filename);
					if (strcmp(filename, openfilelist[i].dir) == 0 && openfilelist[i].topenfile == 1)
					{
						fd = i;
						break;
					}
				}
			}
			else
			{
				for (i = 1; i<MAXOPENFILE; i++)
				{
					if (strcmp(openfilelist[i].filename, filename) == 0 && strcmp(openfilelist[i].dir, currentdir) == 0 && openfilelist[i].topenfile == 1)
						fd = i;
				}
				if (fd >= MAXOPENFILE)
				{
					printf("���ļ������ļ��򿪱���\n");
					break;
				}
			}
			write(fd);
			break;
		case 9:
			scanf("%s", filename);
			if (filename[0] == '\\')				
			{
				for (i = 1; i<MAXOPENFILE; i++)
				{
					strcpy(absolute_dir, openfilelist[i].dir);
					strcat(absolute_dir, openfilelist[i].filename);
					if (strcmp(filename, openfilelist[i].dir) == 0 && openfilelist[i].topenfile == 1)
					{
						fd = i;
						break;
					}
				}
			}
			else
			{
				for (i = 1; i<MAXOPENFILE; i++)
				{
					if (strcmp(openfilelist[i].filename, filename) == 0 && strcmp(openfilelist[i].dir, currentdir) == 0 && openfilelist[i].topenfile == 1)
						fd = i;
				}
				if (fd >= MAXOPENFILE)
				{
					printf("���ļ������ļ��򿪱���\n");
					break;
				}
			}
			len = openfilelist[fd].length;
			read(fd, len);
			break;
		case 10:
			scanf("%s", filename);
			rm(filename);
			break;
		case 11:
			if (my_exitsys() == 1)
				return 0;
			break;
		default:
			printf("���������������������\n");
			break;
		}
	}
	return 0;
}
