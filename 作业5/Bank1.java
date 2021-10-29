
import java.util.Scanner;

public class Bank1 {
    static int pcb_nums;  // 进程数量
    static int res_nums;  // 资源种类数量
    static int max[][];     // 最大需求资源向量
    static int alloc[][];   // 拥有资源数向量
    static int need[][];    // 还需要资源数向量
    static int ava[];     // 可用资源数向量
    static int request[];  //本次申请的资源量
    static int safe_seq[]; //安全序列数组
    static void bank_init()
    {
        Scanner in = new Scanner(System.in);
        System.out.println("请输入进程数量：");
        pcb_nums = in.nextInt();
        System.out.println("请输入资源数量");
        res_nums = in.nextInt();
        safe_seq = new int[pcb_nums+1];
        max = new int[pcb_nums][res_nums];
        alloc = new int[pcb_nums][res_nums];
        need = new int[pcb_nums][res_nums];
        ava = new int[res_nums];     // 可用资源
        System.out.println("请输入最大资源：");
        for (int i = 0; i < pcb_nums; i++) {
            for (int j = 0; j < res_nums; j++) {
                max[i][j] = in.nextInt();
            }
        }
        System.out.println("请输入分配资源：");
        for (int i = 0; i < pcb_nums; i++) {
            for (int j = 0; j < res_nums; j++) {
                alloc[i][j] = in.nextInt();
            }
        }
        //计算需求矩阵
        for (int i = 0; i < pcb_nums; i++) {
            for (int j = 0; j < res_nums; j++) {
                need[i][j] = max[i][j] - alloc[i][j];
            }
        }
        System.out.println("请输入可用资源:");
        for (int j = 0; j < res_nums; j++)
        {
            ava[j] = in.nextInt();
        }
    }
    //比较进程为m中的元素全大于n中的元素
    static boolean compare(int m[],int n[]){
        for (int i = 0; i < res_nums; i++) {
            if (m[i]<n[i])
                return false;
        }
        return true;
    }
    //安全性检验函数，检测是否存在安全序列
    static boolean safe(){
        boolean finish[] = new boolean[pcb_nums];
        int work[] = new int[res_nums];
        int num = 0;
        for (int i = 0; i < res_nums; i++) {
            work[i] = ava[i];
        }
        for (int i = 0; i < pcb_nums; i++) {
            if (num == pcb_nums )
                break;
            for (int j = 0; j < pcb_nums; j++) {
                if (finish[j])
                    continue;
                else{
                    if (compare(work,need[j])){
                        finish[j] = true;
                        safe_seq[num] = j+1;
                        num++;
                        //释放进程资源
                        for (int k = 0; k < res_nums; k++) {
                            work[k] = work[k] + alloc[j][k];
                        }
                    }
                }
            }
        }
        for (int i = 0; i < pcb_nums; i++) {
            if (!finish[i])
                return false;
        }
        //输出安全序列
        for (int i = 0; i < pcb_nums; i++) {
            System.out.print(safe_seq[i]+" ");
        }
        return true;
    }
    //申请进程后的安全性检验函数  n代表进程号
    static void resafe(int n){
        //available>=request 并且 need >=request
        if (compare(ava,request) && compare(need[n-1],request)){
            for (int i = 0; i < res_nums; i++) {
                alloc[n-1][i] =alloc[n-1][i] + request[i];
                need[n-1][i] = need[n-1][i] - request[i];
                ava[i] = ava[i] - request[i];
            }
            if (safe()){
                System.out.println("允许进程"+n+"申请资源");
            }
            else{
                System.out.println("不允许进程"+n+"申请资源");
                for (int i = 0; i < res_nums; i++) {
                    alloc[n-1][i] = alloc[n-1][i] - request[i];
                    need[n-1][i] = need[n-1][i] + request[i];
                    ava[i] = ava[i] + request[i];
                }
            }
        }
        else
            System.out.println("申请资源量越界");
    }
    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        //进程编号
        int n;
        bank_init();
        if (safe()){
            System.out.println("  存在安全序列，初始状态安全。");
        }
        else
            System.out.println("不存在安全序列，初始状态不安全。");
        System.out.println("请输入发出请求向量request的进程编号：");
        n = in.nextInt();
        request = new int[res_nums];
        System.out.println("请输入请求向量request:");
        for (int i = 0; i < res_nums; i++) {
            request[i] = in.nextInt();
        }
        resafe(n);
    }
}
