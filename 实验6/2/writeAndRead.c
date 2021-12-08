#include <stdio.h>
// Ϊ��open()�е�O_RDWR
#include <fcntl.h>

// �����豸����㣨�豸����
#define DEVICE_NAME "/dev/rwbuf"

//�����豸��������
int main()
{
    // �����ļ�������
    int fd;
    // ��¼�ر��豸�ķ���ֵ
    int ret;
    // д�뵽�豸������
    char buff_write[10] = "volume";
    // ��ȡ���豸�Ľ��
    char buff_read[10] = "";

    // ���ô��豸������ע��O_RDWR����ĸO
    fd = open(DEVICE_NAME, O_RDWR);
    // ��ʧ��
    if (fd == -1)
    {
        printf("[open-error]\n");
        return 0;
    }
    else
    {
        printf("[open-success]\n");
    }

    // �������������д�����ӿں���
    if (write(fd, buff_write, 5) == -1)
    {
        printf("[write-error]\n");
        return 0;
    }
    // д��ɹ�
    else
    {
        printf("[write-success] %s\n", buff_write);
    }

    // ������������Ķ������ӿں���
    if (read(fd, buff_read, 5) == -1)
    {
        printf("[read-error]\n");
        return 0;
    }
    // ��ȡ�ɹ�
    else
    {
        buff_read[5] = '\0';
        printf("[read-success] %s\n", buff_read);
    }

    // �豸�ر�
    close(fd);
    printf("[close-success]\n");

    return 0;
}
