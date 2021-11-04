#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define TRUE 1
#define FALSE 0
#define INVALID -1
#define NULL  0
#define  total_instruction 320     //ָ����
#define  total_vp  32               //����ҳ����
#define  clear_period  50           //��0����

    //ҳ��ṹ
    typedef struct
    {
        int pn;      //���̵�ҳ��
        int pfn;     //�����ڴ�Ŀ��
        int counter; //һ�������ڷ��ʸ�ҳ��Ĵ���
        int time;    //����ʱ��
    }pl_type;

    pl_type pl[total_vp];                      /*ҳ�����Խṹ---ָ��������Ҫʹ�õ�ַ*/

    //ҳ����ƽṹ
    typedef struct pfc_struct
    {
        int pn;
        int pfn;
        struct pfc_struct *next;
    }pfc_type;
    //1.�����û�������ҳ���ƽṹ 2.��ҳ��ͷָ�� 3. æҳ��ͷָ��  4. æҳ��βָ��
    pfc_type pfc[total_vp], *freepf_head, *busypf_head, *busypf_tail;
    //1.ҳ��ʧЧ����  2.a[]  ָ������
    int diseffect,  a[total_instruction];
    //1. ÿ��ָ�����ڵ�ҳ�� 2.ÿҳװ��10��ָ���ȡģ����ҳ��ƫ��ֵ
    int page[total_instruction],  offset[total_instruction];

    int  initialize(int);
    int  FIFO(int);
    int  LRU(int);
    int  LFU(int);
    int  NUR(int);
    int  OPT(int);
    int main( )
    {
        int s,i;

        srand(10*getpid());                    /*����ÿ������ʱ���̺Ų�ͬ���ʿ�������Ϊ��ʼ����������еġ����ӡ�*/

        //rand() ����һ����Χ�� 0 �� RAND_MAX ֮���α�������
        //RAND_MAX ��һ������������Ĭ��ֵ�ڲ�ͬ��ʵ���л�������ͬ������ֵ������ 32767
        s=(float)319*rand()/32767+1;

        for(i=0;i<total_instruction;i+=4)        /*����ָ�����*/
            {
            if(s<0||s>319)
            {
                printf("When i==%d,Error,s==%d\n",i,s);
                exit(0);
            }
            a[i]=s;                                   /*��ѡһָ����ʵ�m*/
            a[i+1]=a[i]+1;                            /*˳��ִ��һ��ָ��*/
            a[i+2]=(float)a[i]*rand()/32767;          /*ִ��ǰ��ַָ��m*/
            a[i+3]=a[i+2]+1;                          /*˳��ִ��һ��ָ��*/
            s=(float)(318-a[i+2])*rand( )/32767+a[i+2]+2;
            if((a[i+2]> 318)|| (s>319))
                printf("a[%d+2],a number which is :%d and s==%d\n",i,a[i+2],s);
            }
        //��ָ������ת����ҳ�ţ�ƫ��ֵ
        for (i=0;i<total_instruction;i++) /*��ָ�����б任��ҳ��ַ��*/
            {
                page[i]=a[i]/10;
                offset[i]=a[i]%10;
            }
        for(i=4;i<=32;i++)   //������ 4��ҳ�浽32��ҳ��
            {
                printf("---%2d page frames---\n",i);
                //�Ƚ��ȳ��û��㷨
                FIFO(i);
                //������δʹ���㷨
                LRU(i);
                //���������ʹ���㷨
                LFU(i);
                //��������ʹ���㷨
                NUR(i);
                //����û��㷨
                OPT(i);
            }
            return 0;
    }

    /*��ʼ��������ݽṹ total_pf��ʾ�ڴ�Ŀ��� */
    int initialize(int total_pf)
    {
        int i;
        diseffect=0;
        for(i=0;i<total_vp;i++)
        {

            pl[i].pfn=INVALID;       /*��ҳ����ƽṹ�е�ҳ�ţ�ҳ��Ϊ��*/
            pl[i].counter=0;         /*ҳ����ƽṹ�еķ��ʴ���Ϊ0*/
            pl[i].time=-1;           /*���ʵ�ʱ��*/
        }

        for(i=0;i<total_pf-1;i++)	/*����pfc[i-1]��pfc[i]֮�������*/
        {
            pfc[i].next=&pfc[i+1];
            pfc[i].pfn=i;
        }
        pfc[total_pf-1].next=NULL;
        pfc[total_pf-1].pfn=total_pf-1;
        freepf_head=&pfc[0];         /*��ҳ����е�ͷָ��Ϊpfc[0]*/
        return 0;
    }

    int FIFO(int total_pf)              /*�Ƚ��ȳ��㷨total_pf:�û����̵��ڴ�ҳ����*/
    {
        int i;
        pfc_type *p;					/*�м����*/
        initialize(total_pf);         /*��ʼ�����ҳ����������ݽṹ*/
        busypf_head=busypf_tail=NULL; /*æҳ�����ͷ������β����*/
        for(i=0;i<total_instruction;i++)
        {
            if(pl[page[i]].pfn==INVALID)   /*ҳ��ʧЧ*/
                {
                diseffect+=1;                  /*ʧЧ����*/
                if(freepf_head==NULL)         /*�޿���ҳ��*/
                {
                    p=busypf_head->next;
                    //����һ��æҳ���ͷ�
                    pl[busypf_head->pn].pfn=INVALID;
                    freepf_head=busypf_head;  /*�ͷ�æҳ����еĵ�һ��ҳ��*/
                    freepf_head->next=NULL;  /*��������ȱҳ*/
                    busypf_head=p;
                }
                p=freepf_head->next;
                freepf_head->pn=page[i];
                pl[page[i]].pfn=freepf_head->pfn;
                freepf_head->next=NULL;
                if(busypf_tail==NULL)
                {
                    busypf_tail = busypf_head = freepf_head;
                }
                else
                {
                    busypf_tail->next=freepf_head;
                    busypf_tail=freepf_head;
                }
                freepf_head=p;
                }
        }
        //������  %6.4���壺�ܹ���λ������λС����һ��С���㣬����1λ
        printf("FIFO : %6.4f " , 1-(float)diseffect/320);
        return 0;
    }
    int LRU (int total_pf)       /*������δʹ���㷨least recently used*/
    {
        int min,minj,i,j,present_time; /*minjΪ��Сֵ�±�*/
        initialize(total_pf);
        present_time=0;
        for(i=0;i<total_instruction;i++)
        {
            if(pl[page[i]].pfn==INVALID)             /*ҳ��ʧЧ*/
                {
                diseffect++;
                if(freepf_head==NULL)              /*�޿���ҳ��*/
                    {
                    min=32767;						/*�������ֵ*/
                    for(j=0;j<total_vp;j++)            /*�ҳ�time����Сֵ*/
                        {
                        if(min>pl[j].time&&pl[j].pfn!=INVALID)
                        {
                            min=pl[j].time;
                            minj=j;
                        }
                        }
                        freepf_head=&pfc[pl[minj].pfn];   //�ڳ�һ����Ԫ
                        pl[minj].pfn=INVALID;
                        pl[minj].time=0;
                        freepf_head->next=NULL;
                    }
                    pl[page[i]].pfn=freepf_head->pfn;   //�п���ҳ��,��Ϊ��Ч
                    pl[page[i]].time=present_time;
                    freepf_head=freepf_head->next;      //����һ��free ҳ��
                }
                else
                {
                    pl[page[i]].time=present_time;        //���������Ӹõ�Ԫ�ķ��ʴ���
                    present_time++;
                }
        }
        printf("LRU:%6.4f ",1-(float)diseffect/320);
        return 0;
    }

    int NUR(int  total_pf )                  /*���δʹ���㷨Not Used recently count��ʾ*/
    {
        int i,j,dp,cont_flag,old_dp;
        pfc_type *t;
        initialize(total_pf);
        dp=0;

        for(i=0;i<total_instruction;i++)
        {
            if (pl[page[i]].pfn==INVALID)         /*ҳ��ʧЧ*/
                {
                diseffect++;
                if(freepf_head==NULL)               /*�޿���ҳ��*/
                    {
                    cont_flag=TRUE;
                    old_dp=dp;

                    while(cont_flag)
                    {

                        if(pl[dp].counter==0&&pl[dp].pfn!=INVALID)
                            cont_flag=FALSE;
                        else
                        {
                            dp++;
                            if(dp==total_vp)
                                dp=0;
                            if(dp==old_dp)
                                for(j=0;j<total_vp;j++)
                                    pl[j].counter=0;
                        }
                    }
                    freepf_head=&pfc[pl[dp].pfn];
                    pl[dp].pfn=INVALID;
                    freepf_head->next=NULL;
                    }

                    pl[page[i]].pfn=freepf_head->pfn;

                    freepf_head->pn=page[i];

                    freepf_head=freepf_head->next;
                }
                else
                    pl[page[i]].counter=1;
                if(i%clear_period==0)
                    for(j=0;j<total_vp;j++)
                        pl[j].counter=0;
        }
        printf("NUR:%6.4f ",1-(float)diseffect/320);
        return 0;
    }

    int OPT(int total_pf)       /*����û��㷨*/
    {
        int i,j, max,maxpage,d,dist[total_vp];
        pfc_type *t;
        initialize(total_pf);
        for(i=0;i<total_instruction;i++)
        {
            if(pl[page[i]].pfn==INVALID)      /*ҳ��ʧЧ*/
                {
                diseffect++;
                if(freepf_head==NULL)         /*�޿���ҳ��*/
                    {
                    for(j=0;j<total_vp;j++)
                    {
                        if(pl[j].pfn!=INVALID)
                            dist[j]=32767;
                        else
                            dist[j]=0;
                    }
                    for(j=0;j<total_vp;j++)
                    {
                        if((pl[j].pfn!=INVALID)&&(dist[j]==32767))
                        {
                            dist[j]=j;
                        }
                    }
                    max=0;
                    for(j=0;j<total_vp;j++)
                        if(max<dist[j])
                        {
                            max=dist[j];
                            maxpage=j;
                        }
                    freepf_head=&pfc[pl[maxpage].pfn];
                        freepf_head->next=NULL;
                        pl[maxpage].pfn=INVALID;
                    }
                    pl[page[i]].pfn=freepf_head->pfn;
                    freepf_head=freepf_head->next;
                }
        }
        printf("OPT:%6.4f\n",1-(float)diseffect/320);
        return 0;
    }
    /*���㷨ʱ������֪��Ԥ��δ֪�ģ�least frequency  Used�������ʹ���û���*/
    int  LFU(int total_pf)
    {
        int i,j,min,minpage;
        pfc_type *t;
        initialize(total_pf);
        for(i=0;i<total_instruction;i++)
        {
            if(pl[page[i]].pfn==INVALID)      /*ҳ��ʧЧ*/
                {
                diseffect++;
                if(freepf_head==NULL)          /*�޿���ҳ��*/
                    {
                    min=32767;
                    /*��ȡcounter��ʹ����Ƶ����С���ڴ�*/
                    for(j=0;j<total_vp;j++)
                    {
                        if(min>pl[j].counter&&pl[j].pfn!=INVALID)
                        {
                            min=pl[j].counter;
                            minpage=j;
                        }
                    }
                    freepf_head=&pfc[pl[minpage].pfn];
                    pl[minpage].pfn=INVALID;
                    pl[minpage].counter=0;
                    freepf_head->next=NULL;
                    }
                    pl[page[i]].pfn=freepf_head->pfn;   //�п���ҳ��,��Ϊ��Ч
                    pl[page[i]].counter++;
                    freepf_head=freepf_head->next;      //����һ��free ҳ��
                }
                else
                {
                    pl[page[i]].counter;
                    pl[page[i]].counter=pl[page[i]].counter+1;
                }
        }
        printf("LFU:%6.4f ",1-(float)diseffect/320);
        return 0;
    }