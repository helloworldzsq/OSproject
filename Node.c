
#include "stdio.h"
#include "stdlib.h"				//提供malloc()和free()
#include "string.h"				//提供strcpy()等

//实现链表的增删改查
struct Node{
    int a;				//数据域
    struct Node* next;	//指针域(指向节点的指针）
};

struct Node* head= NULL;
struct Node* end = NULL;
//尾部添加一个节点
void AddListTill(int a)
{
    //创建一个节点
    struct Node* temp=(struct Node*)malloc(sizeof(struct Node));		//此处注意强制类型转换
    //节点数据进行赋值
    temp->a=a;
    temp->next=NULL;

    //连接分两种情况1.一个节点都没有2.已经有节点了，添加到尾巴上
    if (NULL==head)
        head=temp;
    else
        end->next=temp;
    end=temp;			//尾结点应该始终指向最后一个
}

//遍历链表
void ScanList()
{
    struct Node *temp =head;		//定义一个临时变量来指向头
    while (temp !=NULL)
    {
        printf("%d\n",temp->a);
        temp = temp->next;		//temp指向下一个的地址 即实现++操作
    }
}

// 找特定节点
struct Node* FindNode(int a){
    struct Node *temp = head;
    while(temp !=NULL)
    {
        if(a == temp->a)
        {
            return temp;
        }
        temp = temp->next;
    }
    //没找到
    return NULL;
}

//删除尾部节点
void DeleteListTail(){
    //链表为空
    if (NULL == end)
    {
        printf("链表为空，无需删除\n");
        return;
    }
    //链表不为空
    //链表有一个节点
    if (head==end)
    {
        free(head);
        head=NULL;
        end=NULL;
    }
    else
    {
        //找到尾巴前一个节点
        struct Node* temp =head;
        while (temp->next!=end)
        {
            temp = temp->next;
        }
        //找到了，删尾巴
        //释放尾巴
        free(end);
        //尾巴迁移
        end=temp;
        //尾巴指针为NULL
        end->next=NULL;
    }
}