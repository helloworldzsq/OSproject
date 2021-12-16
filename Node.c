
#include "stdio.h"
#include "stdlib.h"				//�ṩmalloc()��free()
#include "string.h"				//�ṩstrcpy()��

//ʵ���������ɾ�Ĳ�
struct Node{
    int a;				//������
    struct Node* next;	//ָ����(ָ��ڵ��ָ�룩
};

struct Node* head= NULL;
struct Node* end = NULL;
//β�����һ���ڵ�
void AddListTill(int a)
{
    //����һ���ڵ�
    struct Node* temp=(struct Node*)malloc(sizeof(struct Node));		//�˴�ע��ǿ������ת��
    //�ڵ����ݽ��и�ֵ
    temp->a=a;
    temp->next=NULL;

    //���ӷ��������1.һ���ڵ㶼û��2.�Ѿ��нڵ��ˣ���ӵ�β����
    if (NULL==head)
        head=temp;
    else
        end->next=temp;
    end=temp;			//β���Ӧ��ʼ��ָ�����һ��
}

//��������
void ScanList()
{
    struct Node *temp =head;		//����һ����ʱ������ָ��ͷ
    while (temp !=NULL)
    {
        printf("%d\n",temp->a);
        temp = temp->next;		//tempָ����һ���ĵ�ַ ��ʵ��++����
    }
}

// ���ض��ڵ�
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
    //û�ҵ�
    return NULL;
}

//ɾ��β���ڵ�
void DeleteListTail(){
    //����Ϊ��
    if (NULL == end)
    {
        printf("����Ϊ�գ�����ɾ��\n");
        return;
    }
    //����Ϊ��
    //������һ���ڵ�
    if (head==end)
    {
        free(head);
        head=NULL;
        end=NULL;
    }
    else
    {
        //�ҵ�β��ǰһ���ڵ�
        struct Node* temp =head;
        while (temp->next!=end)
        {
            temp = temp->next;
        }
        //�ҵ��ˣ�ɾβ��
        //�ͷ�β��
        free(end);
        //β��Ǩ��
        end=temp;
        //β��ָ��ΪNULL
        end->next=NULL;
    }
}