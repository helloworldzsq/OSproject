#include<iostream>
using namespace std;
// p ��������r��Դ����
int p ;
int r ;
int maxs[10][10]; //����������
int allocation[10][10]; //�������
int need[10][10];   //�������
int available[10]; //������Դ����
int request[10];   //����������ǰ���̶Ը�����Դ��������,�㷨����ڲ���
//���뺯��
void infInput()
{
    int i,j;
    cout<<"����������������max\n";
    for(i=0; i<p; i++)
    {
        for(j=0; j<r; j++)
        {
            cin>>maxs[i][j];
        }
    }
    cout<<"������������allocation\n";
    for(i=0; i<p; i++)
    {
        for(j=0; j<r; j++)
        {
            cin>>allocation[i][j];
        }
    }
    cout<<"�������������need\n";
    for(i=0; i<p; i++)
    {
        for(j=0; j<r; j++)
        {
            cin>>need[i][j];
        }
    }
    cout<<"�����������Դ����available\n";
    for(i=0; i<r; i++)
    {
        cin>>available[i];
    }
}

//�ȽϺ���
//�ȽϽ���Ϊm�е�Ԫ��ȫ����n�е�Ԫ�ط���1�����򷵻�0
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


//��ȫ�Լ��麯��������Ƿ���ڰ�ȫ����
int stest()
{
    int i,j,k,l,flag=0;
    int finish[p];
    int work[r];
    for(i=0; i<p; i++)
    {
        finish[i]=0;
        //visΪ1����ʾavailable�����i���̵���Դ��Ҫ
    }
    for(i=0; i<r; i++)
    {
        work[i]=available[i];
    }
    cout<<"�������У�\n";
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
                    cout<<'\n'<<"����"<<i+1<<'\t';
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
                        //������ɣ��ͷ���Դ
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
            return 0;//�����ڰ�ȫ����
        }
    }
    return 1;//���ڰ�ȫ����
}

//������̺�İ�ȫ�Լ��麯��

void rtest(int n)
{
    int j;
    //n=n-1;
    if(compare(available,request)&&compare(need[n-1],request))//available>=request ���� need >=request
        {
        for(j=0; j<r; j++)
        {
            allocation[n-1][j]=allocation[n-1][j]+request[j];
            need[n-1][j]=need[n-1][j]-request[j];
            available[j]=available[j]-request[j];
        }
        if(stest())
        {
            cout<<"����"<<n<<"����������Դ��\n";
        }
        else
        {
            cout<<"������"<<n<<"����������Դ��\n";
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
        cout<<"������Դ��Խ�磡\n";
    }
}

int main()
{
    int i,n;       //n-��n����Դ����
    cout<<"�������������";
    cin>>p;
    cout<<"��������Դ��������";
    cin>>r;
    //Ĭ��״̬4��3
    infInput();//���뺯��
    if(stest()==1)
    {
        cout<<"���ڰ�ȫ���У���ʼ״̬��ȫ��\n";
    }
    else
    {
        cout<<"�����ڰ�ȫ���У���ʼ״̬����ȫ��\n";
    }
    cout<<"�����뷢����������request�Ľ��̱�ţ�";
    cin>>n;
    cout<<"��������������request\n";
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