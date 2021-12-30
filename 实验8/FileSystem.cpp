#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <bitset>

using namespace std;
const int BLOCKNUM_SIZE=2;					//�̿�Ŵ�С
const int BLOCK_SIZE=1024;				    //һ���̿��С��
const int BLOCK_NUM=10001;				    //�̿�����
const int DISK_SIZE=1024*1000*10;			//���̴�С
const int LIST_SIZE=32;						//Ŀ¼���С
const int MAP_SIZE=10001;						//MAP ����
const int FATNUM=125;						//FAT���̿��� �ڿ�û����
const int FATLIST=512;					//ÿ���̿�FAT�ļ�¼��
const int DATABEG=128;					//�����ʼFAT��

struct FCB{
    char fname[8];						//�ļ���
    char exname[3];						//��չ��
    short  fnum;						//�׿��
    int length;							//�ļ���С��  Ŀ¼���ļ���СΪ��
};

struct fatid{
    short id[FATNUM*FATLIST];   //FAT ��С512����¼ һ��
}*FAT;

struct map{
    bitset<MAP_SIZE> maplist;
}*MAP;

struct DIR{
    struct FCB list[LIST_SIZE+1];
}*filedir;

int currentid=128;                  //��ǰFAT��
int currentdir=128;                 //��ǰĿ¼��ų�ʼ����+1  ���ڵڸ���Ԫû��ʹ��
char *file;                          //���̵��׵�ַ
char *FilePath="myfat";              //window�ļ������ַ
FILE *fp;                           //window �ļ���ַ
string CURRENT="root\\";				//��ǰ·��
char  cmd[30];						//����ָ��
char command[16];



/*
*���ļ��洢�����и�ʽ��
*������Ŀ¼
*
*/
void init(struct fatid *FAT){
    int i,j;
    for(i=1;i<FATNUM*FATLIST;i++)			//�ڿ� ��ʹ��
    {
        if(i>DATABEG)
            FAT->id[i]=0;
        else
            FAT->id[i]=-1;
    }
}

void format(){
    bool i;
    FAT=(struct fatid *)(file+BLOCK_SIZE);                         //��ǰFAT��ַ
    MAP=(struct map *)(file+(FATNUM+1)*BLOCK_SIZE);       //��ʼ��λʾͼ
    init(FAT);
    FAT->id[0]=9872;
    filedir=(struct DIR *)(file+(FATNUM+1+2)*BLOCK_SIZE);		  //��ǰĿ¼ָ���ַ
    FAT->id[128]=-1;
    FAT->id[0]=9872-1;
    strcpy(filedir->list[0].fname,".");
    strcpy(filedir->list[0].exname,"dir");
    filedir->list[0].fnum=currentdir;
    filedir->list[0].length=0;
    strcpy(filedir->list[1].fname,"..");
    strcpy(filedir->list[1].exname,"dir");
    filedir->list[1].fnum=currentdir;
    filedir->list[1].length=0;
    fp=fopen(FilePath,"w+");
    fwrite(file,sizeof(char),DISK_SIZE,fp);
    fclose(fp);
    printf("��ʼ���Ѿ����,���ڿ��Խ��в�����!\n\n");
}

/*
*������Ŀ¼
*/
int mkdir(char *str)
{
    int i,j;
    int blockid;			//��Ҫ������FAT��
    int blockdir;			//��Ҫ������Ŀ¼���
    int listnum;			//Ŀ¼���ڱ��

    struct fatid *flagid;
    struct DIR *dir;        //��ǰĿ¼ָ��
    struct map *MAP;
    struct fatid *FAT;

    if(strcmp(str,"")==0)
    {
        printf("Ŀ¼���Ʋ���Ϊ��\n");
        return 0;
    }

    dir=(struct DIR *)(file+(currentdir)*BLOCK_SIZE);
    MAP=(struct map *)(file+(FATNUM+1)*BLOCK_SIZE);
    FAT=(struct fatid *)(file+BLOCK_SIZE);

    for(i=DATABEG+1;i<BLOCK_NUM;i++)   //��128�����ݿ� ʵ���ϵĿ鿪ʼ����
        {
        if(MAP->maplist[i]==0)
            break;

        }

    if(i>BLOCK_NUM)
    {
        printf("�ڴ治��\n");
        return 0;
    }

    MAP->maplist[i]=1;				//map �ü�����


    dir=(struct DIR *)(file+(currentdir)*BLOCK_SIZE);


    for(i=2;i<LIST_SIZE;i++)
    {
        if(strcmp(dir->list[i].fname,str)==0)
        {
            printf("Ŀ¼����ͬ���ļ���\n");
            return 0;
        }
    }
    for(i=2;i<LIST_SIZE;i++)
    {

        if(strcmp(dir->list[i].fname,"")==0)			//�пյ�Ŀ¼��������������һ�汾��ʱ���������ѭ������һ�𣬴���һ�������ǰ��Ľ�����Ŀ¼ɾ����ֱ�ӱ�ͬ���ĸ�����
            break;
        if(i>LIST_SIZE)
        {
            printf("�ڴ治��\n");
            return 0;
        }
    }
    flagid=(struct fatid *)(file+BLOCK_SIZE);	//fat ��λ��ַ

    for(j=DATABEG+1;j<BLOCK_NUM;j++)
    {
        if(flagid->id[j]==0)
        {
            blockdir=j;
            break;
        }
    }

    strcpy(dir->list[i].fname,str);
    dir->list[i].fnum=blockdir;
    strcpy(dir->list[i].exname,"dir");
    dir->list[i].length=0;


    dir=(struct DIR *)(file+blockdir*BLOCK_SIZE);   //Ϊ��Ŀ¼�����Ŀ¼
    strcpy(dir->list[0].fname,".");
    strcpy(dir->list[0].exname,"dir");
    dir->list[0].fnum=blockdir;
    dir->list[0].length=0;

    strcpy(dir->list[1].fname,"..");
    strcpy(dir->list[1].exname,"dir");
    dir->list[1].fnum=currentdir;
    dir->list[1].length=0;

    flagid->id[j]=-1;                 //�޸�FAT  Ŀ¼β��
    FAT->id[0]=FAT->id[0]-1;

    printf("�Ѿ��ɹ�����Ŀ¼%s \n",str);

    return 0;
}

/*
*��ʾĿ¼
*/
int listshow()
{
    int i,sumfile,sumdir,fl[100],dr[100];//fl Ϊ�ļ��ĺ�����drΪĿ¼�ĺ���
    sumfile=sumdir=0;

    struct DIR *dir;
    struct fatid *FAT;

    dir=(struct DIR *)(file+currentdir*BLOCK_SIZE);

    for(i=0;i<LIST_SIZE;i++)
    {
        if(dir->list[i].length==0&&(strcmp(dir->list[i].fname,"")!=0)&&(dir->list[i].fnum!=0))			//ΪĿ¼��
            {
            dr[sumdir]=i;
            sumdir++;

            }
        if(dir->list[i].length!=0&&strcmp(dir->list[i].fname,"")!=0)			//ΪĿ¼��
            {
            fl[sumfile]=i;
            sumfile++;

            }


    }

    for(i=0;i<sumdir;i++)
        printf("   %s       �ļ���\n",dir->list[dr[i]].fname);

    for(i=0;i<sumfile;i++)
        printf("   %s       %s�ļ�\n",dir->list[fl[i]].fname,dir->list[fl[i]].exname);

    printf("\n");

    printf("\n�ڸ�Ŀ¼�¹���%d ���ļ�, %d ���ļ���\n\n",sumfile,sumdir-2);
    return 0;
}

/*
*ɾ����Ŀ¼
*/
int rmdir(char *str)
{
    int  i;
    int blockid;
    int flag=0;
    //FAT��
    int blocknum;				//Ŀ¼��

    struct fatid *FAT;
    struct DIR *dir;
    struct DIR *flagdir;		//���Ŀ¼��


    char c='a';			   //���û�����
    int m=2;				//�ӵ�������Ŀ¼�ʼ����Ҫɾ����Ŀ¼�����

    FAT=(struct fatid *)(file+BLOCK_SIZE);
    dir=(struct DIR *)(file+currentdir*BLOCK_SIZE);			//��ǰĿ¼ָ��
    MAP=(struct map *)(file+(FATNUM+1)*BLOCK_SIZE);


    for(i=2;i<LIST_SIZE;i++)
    {
        if(strcmp(dir->list[i].fname,str)==0)		//�ҵ�Ҫɾ������Ŀ¼
            {
            break;
            }
    }

    if(i>LIST_SIZE)
    {
        printf("���ļ����²�����%s",str);
        return 0;
    }
    while(1)
    {
        printf("�Ƿ�ȷ�ϣ���Y/N��");
        cin>>c;
        if((c=='y'||c=='Y')||(c=='n'||c=='N'))
            break;

    }

    if(c=='n'||c=='N')
        return 0;

    blocknum=dir->list[i].fnum;

    flagdir=(struct DIR *)(file+blocknum*BLOCK_SIZE);

    while(m!=LIST_SIZE)
    {
        if(strcmp(flagdir->list[m].fname,"")!=0)
        {
            printf("��Ŀ¼�������ļ�������Ŀ¼������ɾ����Ŀ¼");
        }
        m++;
    }

    strcpy(dir->list[i].fname,"");		//��Ŀ¼DIR
    strcpy(dir->list[i].exname,"");
    dir->list[i].fnum=0;


    strcpy(flagdir->list[0].fname,"");			//Ҫɾ��Ŀ¼��DIR
    strcpy(flagdir->list[0].exname,"");
    flagdir->list[0].fnum=0;

    strcpy(flagdir->list[1].fname,"");
    strcpy(flagdir->list[1].exname,"");
    flagdir->list[0].fnum=0;

    MAP->maplist[blocknum]=0;
    FAT->id[blocknum]=0;
    FAT->id[0]=FAT->id[0]+1;

    return 0;
}

/*
*���ĵ�ǰĿ¼(cd����)
*/
int changedir(char *str)
{
    int i,j;
    int blocknum;			//��ǰĿ¼λ��
    int flagnum;			//temp��Ŀ¼λ��
    struct DIR * flagdir,*dir;
    struct fatid * FAT;
    string strflag;     //Ϊ�˸ĵ�ǰ��ʾ�Ĵ���


    dir=(struct DIR *)(file+currentdir*BLOCK_SIZE);

    if(strcmp("..",str)==0)					//�ж��ǲ����ϲ�Ŀ¼
        {
        blocknum=currentdir;
        if(dir->list[0].fnum==dir->list[1].fnum)	//��Ŀ¼������
            {
            return 1;
            }

        currentdir=dir->list[1].fnum;							//�ı䵱ǰĿ¼ָ��

        flagdir=(struct DIR *)(file+currentdir*BLOCK_SIZE);		//ȥ�ϲ��Ŀ¼��ַ

        for(int j=0;j<LIST_SIZE;j++)
        {
            if(flagdir->list[j].fnum==blocknum)
            {
                strflag=flagdir->list[j].fname;
                break;

            }
        }
        CURRENT=CURRENT.substr(0,(CURRENT.length()-strflag.length())-1);
        return 1;
        }

    for(i=2;i<LIST_SIZE;i++)									//��Ŀ¼
        {
        if(strcmp(dir->list[i].fname,str)==0&&strcmp(dir->list[i].exname,"dir")==0)
        {
            currentdir=dir->list[i].fnum;
            break;
        }
        }


    if(i>LIST_SIZE)
    {
        printf("�Ҳ���ָ����Ŀ¼%s\n",str);
        return 0;
    }

    CURRENT=CURRENT+str+"\\";
    return 1;
}

/*
*�����ļ�
*/
int create(char *str,int length)
{
    //getfilename
    int i,j,l,t,k;
    int blockdir;
    int fid;		//FAT���׿��
    int flag;		//�ļ����׿��ж�
    char name[8]={0};	//�ļ�����
    char exname[3]={0};	//�ļ���չ��
    int templength;      	//temp�ļ�����

    struct fatid * FAT;
    struct DIR *dir;
    struct map *MAP;

    dir=(struct DIR *)(file+(currentdir)*BLOCK_SIZE);
    MAP=(struct map *)(file+(FATNUM+1)*BLOCK_SIZE);
    FAT=(struct fatid *)(file+BLOCK_SIZE);
    templength=length;

    l=strlen(str);
    //ȡ�ļ���
    for(i=0;i<l;i++)
    {

        name[i]=str[i];
        if(str[i+1]=='.')
            break;
    }

    if(i>=8)
    {
        printf("�ļ����ƹ���\n");
        return 0;
    }

    //ȥ��չ��
    j=0;
    i++;
    i++;//��ȥ��
    k=l-i;
    for(j=0;j<k;j++)
    {

        if(str[i]=='\0')
            break;
        exname[j]=str[i];
        i++;
    }
    if(strcmp(name,"0")==0)
    {
        printf("�ļ����Ʋ���Ϊ��\n");
        return 0;
    }

    if(length>FAT->id[0])
    {
        printf("�ļ������������ɿռ�\n");
        return 0;
    }


    for(i=2;i<LIST_SIZE;i++)
    {
        if(strcmp(dir->list[i].fname,name)==0&&strcmp(dir->list[i].exname,exname)==0)
        {
            printf("���ļ����£��Ѿ���ͬ���ļ�");
            return 0;
        }
        if(strcmp(dir->list[i].fname,"")==0)
        {
            break;
        }

    }
    if(i>LIST_SIZE)
    {
        printf("�ڴ治��\n");
        return 0;
    }
    strcpy(dir->list[i].fname,name);
    strcpy(dir->list[i].exname,exname);
    dir->list[i].length=length;

    flag=1;
    j=DATABEG+1;

    while(1){//����ѭ��
        if(MAP->maplist[j]!=1)
        {
            if(!templength--)	//��lengthȫ�����������ֹ
                break;
            //��һ��ĵ�ַ
            if(flag)		//��һ�η����ǵ��׵�ַ
                {
                dir->list[i].fnum=j;		//���ļ����׿�

                }
            MAP->maplist[j]=1;				//MAP����
            if(!flag)
            {
                FAT->id[t]=j;
                FAT->id[0]=FAT->id[0]-1;
            }
            t=j;
            flag=0;
        }
        j++;
    }
    FAT->id[t]=-1;
    FAT->id[0]=FAT->id[0]-1;
    return 1;
}

/**
�����ļ�
*/
int cp(char *str,char *newname)
{
    int i,j,k,l,length;
    char name[8]={0};	//�ļ�����
    char exname[3]={0};	//�ļ���չ��
    struct DIR *dir;
    l=strlen(str);
    //ȡ�ļ���
    for(i=0;i<l;i++)
    {
        name[i]=str[i];
        if(str[i+1]=='.')
            break;
    }
    //ȥ��չ��
    j=0;
    i++;
    i++;//��ȥ��
    k=l-i;
    for(j=0;j<k;j++)
    {

        if(str[i]=='\0')
            break;
        exname[j]=str[i];
        i++;
    }
    if(strcmp(newname,"")==0)
    {
        printf("�ļ�������Ϊ��\n");
        return 0;
    }
    dir=(struct DIR *)(file+currentdir*BLOCK_SIZE);

    for(i=2;i<LIST_SIZE;i++)									//���ļ�
        {
        if(strcmp(dir->list[i].fname,name)==0&&strcmp(dir->list[i].exname,exname)==0)
            break;

        }
    if(i>LIST_SIZE)
    {
        printf("�Ҳ���ָ�����ļ�%s\n",str);
        return 0;
    }
    length=dir->list[i].length ;
    create(newname,length);
}

/*
*ɾ���ļ�
*/
int delfile(char *str)
{
    int i,j,l,k;
    int blocknum;       //Ҫɾ�����׿��ַ
    int temp;
    char name[8]={0};	//�ļ�����
    char exname[3]={0};	//�ļ���չ��
    char c='a';

    struct DIR *dir;
    struct fatid *FAT;
    struct map *MAP;

    l=strlen(str);
    //ȡ�ļ���
    for(i=0;i<l;i++)
    {

        name[i]=str[i];
        if(str[i+1]=='.')
            break;
    }


    //ȥ��չ��
    j=0;
    i++;
    i++;//��ȥ��
    k=l-i;
    for(j=0;j<k;j++)
    {

        if(str[i]=='\0')
            break;
        exname[j]=str[i];
        i++;
    }


    if(strcmp(str,"")==0)
    {
        printf("�ļ�������Ϊ��\n");
        return 0;
    }

    dir=(struct DIR *)(file+(currentdir)*BLOCK_SIZE);
    MAP=(struct map *)(file+(FATNUM+1)*BLOCK_SIZE);
    FAT=(struct fatid *)(file+BLOCK_SIZE);

    for(i=2;i<LIST_SIZE;i++)
    {
        if((strcmp(dir->list[i].fname,name)==0)&&(strcmp(dir->list[i].exname,exname)==0))
            break;

    }
    if(i>LIST_SIZE)
    {
        printf("�Ҳ���%s �ļ�\n",str);
        return 0;
    }

    if(c=='n'||c=='N')
        return 0;

    blocknum=dir->list[i].fnum;
    dir->list[i].fnum=0;				//��Ŀ¼�ԭ
    strcpy(dir->list[i].exname,"");
    strcpy(dir->list[i].fname,"");
    dir->list[i].length=0;


    //����FAT AND MAP ��

    while(FAT->id[blocknum]!=-1)
    {
        temp=FAT->id[blocknum];;
        FAT->id[blocknum]=0;
        FAT->id[0]=FAT->id[0]+1;
        MAP->maplist[blocknum]=0;
        blocknum=temp;
    }
    return 0;
}
/*
 *�������ļ�������ǰ�ļ����Ƹ�һ�����ļ�����ɾ����ǰ�ļ���
 */

void rename(char *str,char *newname){
    cp(str,newname);
    delfile(str);
}

/**
�˳�ϵͳ
*/
int exit()
{
    fp=fopen(FilePath,"w+");
    fwrite(file,sizeof(char),DISK_SIZE,fp);
    fclose(fp);
    free(file);
    return 1;
}


void welcome()
{
    //��ӭ�б�

    printf("--------------------------------------------------\n");
    printf("\n������ʹ��˵��\n");
    printf("format          : �Դ��̸�ʽ��.\n");
    printf("exit            : ��ȫ�˳����ļ�ϵͳ,������Ϣ.\n");
    printf("mkdir  dirname  ; ������Ŀ¼.\n");
    printf("rmdir  dirname  : ɾ����Ŀ¼.\n");
    printf("ls     dirname  : ��ʾ��ǰĿ¼����Ϣ.\n");
    printf("cd     dirname  : ���ĵ�ǰĿ¼.\n");
    printf("create filename length : ����һ�����ļ�.\n");
    printf("rm     filename : ɾ���ļ�.\n");
    printf("cp oldname newname: �����ļ�.\n");
    printf("rename oldname newname : �������ļ�.\n");
    printf("\n--------------------------------------------\n");

    //��������ռ�
    file=(char *)malloc(DISK_SIZE*sizeof(char));

    //����
    if((fp=fopen(FilePath,"r"))!=NULL)
    {
        fread(file,sizeof(char),DISK_SIZE,fp);
        printf("���ش����ļ�%s�ļ��ɹ������ڿ��Բ���\n\n",FilePath);
    }else
    {
        printf("���ǵ�һ��ʹ���ļ�����ϵͳ");
    }


}

int main()
{
    int length;
    char newname[20];
    welcome();
    format();
    while(1)
    {
        cout<<CURRENT<<"#";
        cin>>cmd;
        if(strcmp(cmd,"format")==0){
            free(file);
            file=(char*)malloc(DISK_SIZE*sizeof(char));  //���·���ռ�
            format();
        }else if(strcmp(cmd,"mkdir")==0)
        {
            scanf("%s",command);
            mkdir(command);
        }else if(strcmp(cmd,"rmdir")==0)
        {
            scanf("%s",command);
            rmdir(command);
        }
        else if(strcmp(cmd,"ls")==0)
        {
            listshow();
        }else if(strcmp(cmd,"cd")==0)
        {
            scanf("%s",command);
            changedir(command);
        }else if(strcmp(cmd,"create")==0)
        {
            cin>>command>>length;
            create(command,length);
        }else if(strcmp(cmd,"cp")==0)
        {
            cin>>command>>newname;
            cp(command,newname);
        }else if(strcmp(cmd,"rm")==0)
        {
            scanf("%s",command);
            delfile(command);
        }
        else if (strcmp(cmd,"rename")==0){
            cin>>command>>newname;
            rename(command,newname);
        }
        else if(strcmp(cmd,"exit")==0){
            exit();
            break;
        }
        else {
            printf("��Чָ��,����������:\n");
        }
    }
    printf("Thank you for using my file system!\n");
    return 0;
}
