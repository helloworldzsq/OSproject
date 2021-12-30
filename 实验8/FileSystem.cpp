#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <bitset>

using namespace std;
const int BLOCKNUM_SIZE=2;					//盘块号大小
const int BLOCK_SIZE=1024;				    //一个盘块大小数
const int BLOCK_NUM=10001;				    //盘块数量
const int DISK_SIZE=1024*1000*10;			//磁盘大小
const int LIST_SIZE=32;						//目录项大小
const int MAP_SIZE=10001;						//MAP 长度
const int FATNUM=125;						//FAT的盘块数 第块没有用
const int FATLIST=512;					//每个盘口FAT的记录数
const int DATABEG=128;					//数据项开始FAT号

struct FCB{
    char fname[8];						//文件名
    char exname[3];						//扩展名
    short  fnum;						//首块号
    int length;							//文件大小，  目录则文件大小为；
};

struct fatid{
    short id[FATNUM*FATLIST];   //FAT 大小512个记录 一块
}*FAT;

struct map{
    bitset<MAP_SIZE> maplist;
}*MAP;

struct DIR{
    struct FCB list[LIST_SIZE+1];
}*filedir;

int currentid=128;                  //当前FAT号
int currentdir=128;                 //当前目录块号初始化是+1  由于第个单元没有使用
char *file;                          //磁盘的首地址
char *FilePath="myfat";              //window文件保存地址
FILE *fp;                           //window 文件地址
string CURRENT="root\\";				//当前路径
char  cmd[30];						//输入指令
char command[16];



/*
*对文件存储器进行格式化
*创建根目录
*
*/
void init(struct fatid *FAT){
    int i,j;
    for(i=1;i<FATNUM*FATLIST;i++)			//第块 不使用
    {
        if(i>DATABEG)
            FAT->id[i]=0;
        else
            FAT->id[i]=-1;
    }
}

void format(){
    bool i;
    FAT=(struct fatid *)(file+BLOCK_SIZE);                         //当前FAT地址
    MAP=(struct map *)(file+(FATNUM+1)*BLOCK_SIZE);       //初始化位示图
    init(FAT);
    FAT->id[0]=9872;
    filedir=(struct DIR *)(file+(FATNUM+1+2)*BLOCK_SIZE);		  //当前目录指针地址
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
    printf("初始化已经完成,现在可以进行操作了!\n\n");
}

/*
*创建子目录
*/
int mkdir(char *str)
{
    int i,j;
    int blockid;			//将要创建的FAT号
    int blockdir;			//将要创建的目录块号
    int listnum;			//目录块内编号

    struct fatid *flagid;
    struct DIR *dir;        //当前目录指针
    struct map *MAP;
    struct fatid *FAT;

    if(strcmp(str,"")==0)
    {
        printf("目录名称不能为空\n");
        return 0;
    }

    dir=(struct DIR *)(file+(currentdir)*BLOCK_SIZE);
    MAP=(struct map *)(file+(FATNUM+1)*BLOCK_SIZE);
    FAT=(struct fatid *)(file+BLOCK_SIZE);

    for(i=DATABEG+1;i<BLOCK_NUM;i++)   //从128块数据块 实际上的块开始搜索
        {
        if(MAP->maplist[i]==0)
            break;

        }

    if(i>BLOCK_NUM)
    {
        printf("内存不足\n");
        return 0;
    }

    MAP->maplist[i]=1;				//map 置即已用


    dir=(struct DIR *)(file+(currentdir)*BLOCK_SIZE);


    for(i=2;i<LIST_SIZE;i++)
    {
        if(strcmp(dir->list[i].fname,str)==0)
        {
            printf("目录下有同名文件夹\n");
            return 0;
        }
    }
    for(i=2;i<LIST_SIZE;i++)
    {

        if(strcmp(dir->list[i].fname,"")==0)			//有空的目录块且无重名，第一版本的时候与上面的循环放在一起，存在一个情况是前面的建立的目录删除后，直接被同名的覆盖了
            break;
        if(i>LIST_SIZE)
        {
            printf("内存不足\n");
            return 0;
        }
    }
    flagid=(struct fatid *)(file+BLOCK_SIZE);	//fat 首位地址

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


    dir=(struct DIR *)(file+blockdir*BLOCK_SIZE);   //为新目录项创建根目录
    strcpy(dir->list[0].fname,".");
    strcpy(dir->list[0].exname,"dir");
    dir->list[0].fnum=blockdir;
    dir->list[0].length=0;

    strcpy(dir->list[1].fname,"..");
    strcpy(dir->list[1].exname,"dir");
    dir->list[1].fnum=currentdir;
    dir->list[1].length=0;

    flagid->id[j]=-1;                 //修改FAT  目录尾部
    FAT->id[0]=FAT->id[0]-1;

    printf("已经成功创建目录%s \n",str);

    return 0;
}

/*
*显示目录
*/
int listshow()
{
    int i,sumfile,sumdir,fl[100],dr[100];//fl 为文件的号数，dr为目录的号数
    sumfile=sumdir=0;

    struct DIR *dir;
    struct fatid *FAT;

    dir=(struct DIR *)(file+currentdir*BLOCK_SIZE);

    for(i=0;i<LIST_SIZE;i++)
    {
        if(dir->list[i].length==0&&(strcmp(dir->list[i].fname,"")!=0)&&(dir->list[i].fnum!=0))			//为目录的
            {
            dr[sumdir]=i;
            sumdir++;

            }
        if(dir->list[i].length!=0&&strcmp(dir->list[i].fname,"")!=0)			//为目录的
            {
            fl[sumfile]=i;
            sumfile++;

            }


    }

    for(i=0;i<sumdir;i++)
        printf("   %s       文件夹\n",dir->list[dr[i]].fname);

    for(i=0;i<sumfile;i++)
        printf("   %s       %s文件\n",dir->list[fl[i]].fname,dir->list[fl[i]].exname);

    printf("\n");

    printf("\n在该目录下共有%d 个文件, %d 个文件夹\n\n",sumfile,sumdir-2);
    return 0;
}

/*
*删除子目录
*/
int rmdir(char *str)
{
    int  i;
    int blockid;
    int flag=0;
    //FAT号
    int blocknum;				//目录块

    struct fatid *FAT;
    struct DIR *dir;
    struct DIR *flagdir;		//标记目录块


    char c='a';			   //做用户交互
    int m=2;				//从第三个子目录项开始搜索要删除的目录项情况

    FAT=(struct fatid *)(file+BLOCK_SIZE);
    dir=(struct DIR *)(file+currentdir*BLOCK_SIZE);			//当前目录指针
    MAP=(struct map *)(file+(FATNUM+1)*BLOCK_SIZE);


    for(i=2;i<LIST_SIZE;i++)
    {
        if(strcmp(dir->list[i].fname,str)==0)		//找到要删除的子目录
            {
            break;
            }
    }

    if(i>LIST_SIZE)
    {
        printf("该文件夹下不存在%s",str);
        return 0;
    }
    while(1)
    {
        printf("是否确认？（Y/N）");
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
            printf("该目录下有子文件或者子目录，不能删除该目录");
        }
        m++;
    }

    strcpy(dir->list[i].fname,"");		//父目录DIR
    strcpy(dir->list[i].exname,"");
    dir->list[i].fnum=0;


    strcpy(flagdir->list[0].fname,"");			//要删除目录的DIR
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
*更改当前目录(cd命令)
*/
int changedir(char *str)
{
    int i,j;
    int blocknum;			//当前目录位置
    int flagnum;			//temp的目录位置
    struct DIR * flagdir,*dir;
    struct fatid * FAT;
    string strflag;     //为了改当前显示的代码


    dir=(struct DIR *)(file+currentdir*BLOCK_SIZE);

    if(strcmp("..",str)==0)					//判断是不是上层目录
        {
        blocknum=currentdir;
        if(dir->list[0].fnum==dir->list[1].fnum)	//根目录的特征
            {
            return 1;
            }

        currentdir=dir->list[1].fnum;							//改变当前目录指针

        flagdir=(struct DIR *)(file+currentdir*BLOCK_SIZE);		//去上层的目录地址

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

    for(i=2;i<LIST_SIZE;i++)									//子目录
        {
        if(strcmp(dir->list[i].fname,str)==0&&strcmp(dir->list[i].exname,"dir")==0)
        {
            currentdir=dir->list[i].fnum;
            break;
        }
        }


    if(i>LIST_SIZE)
    {
        printf("找不到指定的目录%s\n",str);
        return 0;
    }

    CURRENT=CURRENT+str+"\\";
    return 1;
}

/*
*创建文件
*/
int create(char *str,int length)
{
    //getfilename
    int i,j,l,t,k;
    int blockdir;
    int fid;		//FAT的首块号
    int flag;		//文件的首块判断
    char name[8]={0};	//文件名称
    char exname[3]={0};	//文件扩展名
    int templength;      	//temp文件长度

    struct fatid * FAT;
    struct DIR *dir;
    struct map *MAP;

    dir=(struct DIR *)(file+(currentdir)*BLOCK_SIZE);
    MAP=(struct map *)(file+(FATNUM+1)*BLOCK_SIZE);
    FAT=(struct fatid *)(file+BLOCK_SIZE);
    templength=length;

    l=strlen(str);
    //取文件名
    for(i=0;i<l;i++)
    {

        name[i]=str[i];
        if(str[i+1]=='.')
            break;
    }

    if(i>=8)
    {
        printf("文件名称过长\n");
        return 0;
    }

    //去扩展名
    j=0;
    i++;
    i++;//除去点
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
        printf("文件名称不能为空\n");
        return 0;
    }

    if(length>FAT->id[0])
    {
        printf("文件超出磁盘容纳空间\n");
        return 0;
    }


    for(i=2;i<LIST_SIZE;i++)
    {
        if(strcmp(dir->list[i].fname,name)==0&&strcmp(dir->list[i].exname,exname)==0)
        {
            printf("该文件夹下，已经有同名文件");
            return 0;
        }
        if(strcmp(dir->list[i].fname,"")==0)
        {
            break;
        }

    }
    if(i>LIST_SIZE)
    {
        printf("内存不足\n");
        return 0;
    }
    strcpy(dir->list[i].fname,name);
    strcpy(dir->list[i].exname,exname);
    dir->list[i].length=length;

    flag=1;
    j=DATABEG+1;

    while(1){//不断循环
        if(MAP->maplist[j]!=1)
        {
            if(!templength--)	//当length全部被分配完截止
                break;
            //上一块的地址
            if(flag)		//第一次分配是的首地址
                {
                dir->list[i].fnum=j;		//给文件的首块

                }
            MAP->maplist[j]=1;				//MAP减少
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
复制文件
*/
int cp(char *str,char *newname)
{
    int i,j,k,l,length;
    char name[8]={0};	//文件名称
    char exname[3]={0};	//文件扩展名
    struct DIR *dir;
    l=strlen(str);
    //取文件名
    for(i=0;i<l;i++)
    {
        name[i]=str[i];
        if(str[i+1]=='.')
            break;
    }
    //去扩展名
    j=0;
    i++;
    i++;//除去点
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
        printf("文件名不能为空\n");
        return 0;
    }
    dir=(struct DIR *)(file+currentdir*BLOCK_SIZE);

    for(i=2;i<LIST_SIZE;i++)									//子文件
        {
        if(strcmp(dir->list[i].fname,name)==0&&strcmp(dir->list[i].exname,exname)==0)
            break;

        }
    if(i>LIST_SIZE)
    {
        printf("找不到指定的文件%s\n",str);
        return 0;
    }
    length=dir->list[i].length ;
    create(newname,length);
}

/*
*删除文件
*/
int delfile(char *str)
{
    int i,j,l,k;
    int blocknum;       //要删除的首块地址
    int temp;
    char name[8]={0};	//文件名称
    char exname[3]={0};	//文件扩展名
    char c='a';

    struct DIR *dir;
    struct fatid *FAT;
    struct map *MAP;

    l=strlen(str);
    //取文件名
    for(i=0;i<l;i++)
    {

        name[i]=str[i];
        if(str[i+1]=='.')
            break;
    }


    //去扩展名
    j=0;
    i++;
    i++;//除去点
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
        printf("文件名不能为空\n");
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
        printf("找不到%s 文件\n",str);
        return 0;
    }

    if(c=='n'||c=='N')
        return 0;

    blocknum=dir->list[i].fnum;
    dir->list[i].fnum=0;				//把目录项还原
    strcpy(dir->list[i].exname,"");
    strcpy(dir->list[i].fname,"");
    dir->list[i].length=0;


    //处理FAT AND MAP 表

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
 *重命名文件（将当前文件复制给一个新文件，并删除当前文件）
 */

void rename(char *str,char *newname){
    cp(str,newname);
    delfile(str);
}

/**
退出系统
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
    //欢迎列表

    printf("--------------------------------------------------\n");
    printf("\n以下是使用说明\n");
    printf("format          : 对磁盘格式化.\n");
    printf("exit            : 安全退出该文件系统,保存信息.\n");
    printf("mkdir  dirname  ; 创建子目录.\n");
    printf("rmdir  dirname  : 删除子目录.\n");
    printf("ls     dirname  : 显示当前目录下信息.\n");
    printf("cd     dirname  : 更改当前目录.\n");
    printf("create filename length : 创建一个新文件.\n");
    printf("rm     filename : 删除文件.\n");
    printf("cp oldname newname: 复制文件.\n");
    printf("rename oldname newname : 重命名文件.\n");
    printf("\n--------------------------------------------\n");

    //申请虚拟空间
    file=(char *)malloc(DISK_SIZE*sizeof(char));

    //加载
    if((fp=fopen(FilePath,"r"))!=NULL)
    {
        fread(file,sizeof(char),DISK_SIZE,fp);
        printf("加载磁盘文件%s文件成功，现在可以操作\n\n",FilePath);
    }else
    {
        printf("这是第一次使用文件管理系统");
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
            file=(char*)malloc(DISK_SIZE*sizeof(char));  //重新分配空间
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
            printf("无效指令,请重新输入:\n");
        }
    }
    printf("Thank you for using my file system!\n");
    return 0;
}
