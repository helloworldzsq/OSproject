#include<iostream>
using namespace std;
// p 进程数，r资源种类
int p ;
int r ;
int maxs[10][10]; //最大需求矩阵
int allocation[10][10]; //分配矩阵
int need[10][10];   //需求矩阵
int available[10]; //可用资源向量
int request[10];   //请求向量当前进程对各类资源的申请量,算法的入口参数
//输入函数
void infInput()
{
    int i,j;
    cout<<"请输入最大需求矩阵max\n";
    for(i=0; i<p; i++)
    {
        for(j=0; j<r; j++)
        {
            cin>>maxs[i][j];
        }
    }
    cout<<"请输入分配矩阵allocation\n";
    for(i=0; i<p; i++)
    {
        for(j=0; j<r; j++)
        {
            cin>>allocation[i][j];
        }
    }
    cout<<"请输入需求矩阵need\n";
    for(i=0; i<p; i++)
    {
        for(j=0; j<r; j++)
        {
            cin>>need[i][j];
        }
    }
    cout<<"请输入可用资源向量available\n";
    for(i=0; i<r; i++)
    {
        cin>>available[i];
    }
}

//比较函数
//比较进程为m中的元素全大于n中的元素返回1，否则返回0
int compare(int m[],int n[])
{
    int i;
    for(i=0; i<r; i++)
    {
        if(m[i]<n[i])
        {
            return 0;
        }
    }
    return 1;
}


//安全性检验函数，检测是否存在安全序列
int stest()
{
    int i,j,k,l,flag=0;
    int finish[p];
    int work[r];
    for(i=0; i<p; i++)
    {
        finish[i]=0;
        //vis为1即表示available满足第i进程的资源需要
    }
    for(i=0; i<r; i++)
    {
        work[i]=available[i];
    }
    cout<<"分配序列：\n";
    cout<<"            allocation            need              avilable"<<endl;
    for(k=0; k<p; k++)
    {
        for(i=0; i<p; i++)
        {
            if(finish[i]==1)
            {
                continue;
            }
            else
            {
                if(compare(work,need[i]))//available>=need
                    {
                    finish[i]=1;
                    cout<<'\n'<<"进程"<<i+1<<'\t';
                    flag=1;
                    for (j =0; j<r; j++)
                    {
                        printf("  %2d ", allocation[i][j]);
                    }
                    cout<<"     ";
                    for (j = 0; j < r; j++)
                    {
                        printf("  %2d ", need[i][j]);
                    }
                    cout<<"     ";
                    for (j = 0; j <r; j++)
                    {
                        printf("  %2d ", work[j] +allocation[i][j]);
                    }
                    for(l=0; l<r; l++)
                    {
                        work[l]=work[l]+allocation[i][l];
                        //进程完成，释放资源
                    }
                    break;
                    }
            }
            if(flag==1)
            {
                break;
            }
        }
    }
    cout<<'\n';
    for(l=0; l<p; l++)
    {
        if(finish[l]==0)
        {
            return 0;//不存在安全序列
        }
    }
    return 1;//存在安全序列
}

//申请进程后的安全性检验函数

void rtest(int n)
{
    int j;
    //n=n-1;
    if(compare(available,request)&&compare(need[n-1],request))//available>=request 并且 need >=request
        {
        for(j=0; j<r; j++)
        {
            allocation[n-1][j]=allocation[n-1][j]+request[j];
            need[n-1][j]=need[n-1][j]-request[j];
            available[j]=available[j]-request[j];
        }
        if(stest())
        {
            cout<<"允许"<<n<<"进程申请资源！\n";
        }
        else
        {
            cout<<"不允许"<<n<<"进程申请资源！\n";
            for(j=0; j<r; j++)
            {
                allocation[n-1][j]=allocation[n-1][j]-request[j];
                need[n-1][j]=need[n-1][j]+request[j];
                available[j]=available[j]+request[j];
            }
        }
        }
    else
    {
        cout<<"申请资源量越界！\n";
    }
}

int main()
{
    int i,n;       //n-第n个资源申请
    cout<<"请输入进程数：";
    cin>>p;
    cout<<"请输入资源种类数：";
    cin>>r;
    //默认状态4、3
    infInput();//输入函数
    if(stest()==1)
    {
        cout<<"存在安全序列，初始状态安全。\n";
    }
    else
    {
        cout<<"不存在安全序列，初始状态不安全。\n";
    }
    cout<<"请输入发出请求向量request的进程编号：";
    cin>>n;
    cout<<"请输入请求向量request\n";
    for(i=0; i<r; i++)
    {
        cin>>request[i];
    }
    rtest(n);
    return 0;
}
/*
4
3
3 result result
6 1 3
3 1 4
4 result result
1 0 0
5 1 1
result 1 1
0 0 result
result result result
1 0 result
1 0 3
4 result 0
1 1 result
*/