//
// Created by jw5t on 2022/3/5.
//

#ifndef __TASK_PROC_H
#define __TASK_PROC_H

#include <vector>
#include <iostream>
#include <cstring>
#include<stdlib.h>
#include <dirent.h>
#include <exception>
#include <exception>
#include <pwd.h>
#include<stdio.h>
#include <unistd.h>

using namespace std;


typedef struct proc_node {
    int pid;                    //进程号pid
    char cmd[16];               //进程命令
    char cmdline[256];          //进程详细命令，包含运行命令的参数等
    int uid;                    //进程所属用户的uid
    char username[64];          //进程所属用户的用户名
    int vsz;                    //虚拟消耗内存（KB）
    int rss;                    //实际使用物理内存（KB）
    double proc_cpu_rate;        //CPU使用率
    double proc_mem_rate;        //内存使用率
} proc_node;


class CProcInfo {
private:
    vector<proc_node> proc_info;    //所有进程的信息都存在vector容器proc_info中
    int proc_number;                //系统中所有进程的数量
    int processor_number;           //本机中cpu的核心数
    double sys_cpu_rate;            //系统的CPU使用率
    double sys_mem_rate;            //系统的内存使用率
    unsigned long long total_mem;   //系统总共内存数量
    unsigned long long free_mem;    //系统使用的内存数量


public:


/*
*构造函数
*/
    CProcInfo();

/*
 * 判断文件名是不是纯数字
 */
    bool isNumber(char *file_name);  //okay

/*
 * 获取本系统中cpu的核心数
 */
    int searchTheNumberOfCPU();   //okay
/*
 * 获取本系统中所有进程的数量
 */
    int searchTheNumberOfprocess(); //okay
/*
 * 获得当前的proc_node 并且通过temp返回
 */
    void getCurProcNode(char *pid_path, proc_node *temp);

/*
 * 获得系统的内存总量和当前使用量
 */
    double getCurMemSize(unsigned long long *total_mem, unsigned long long *free_mem);

    void show();

    void check();
};

CProcInfo::CProcInfo() {
    this->proc_number = 0;
    this->processor_number = searchTheNumberOfprocess();


//    {
    sys_cpu_rate = 0;
    for (int i = 0; i < proc_info.size(); ++i) {
        sys_cpu_rate += proc_info[i].proc_cpu_rate;
    }

//    }

//    {
//        sys_cpu_rate=0;
//        for (int i = 0; i <  proc_info.size(); ++i) {
//            sys_cpu_rate+=proc_info.proc_cpu_rate;
//        }
//
//    }



}

bool CProcInfo::isNumber(char *file_name) {
    {
        int lenth = strlen(file_name);;

        for (int i = 0; i < lenth; i++) {
            if (isdigit(file_name[i]) != 0) {
                return true;  //isdigit只能看字符，我强转string对象识别卡了好久
            }
        }
        return false;
    }
}

int CProcInfo::searchTheNumberOfCPU() {


//    int processor = 0;
//    char pro[] = "processor";
//    char buffer[1024];
//    FILE *fd = popen("/proc/cpuinfo","r");
//    fgets(buffer,1024,fd);
//    printf("%s",buffer);
//        while(!feof(fd))
//    {
//        fgets(buffer,1000,fd);
//        if(strstr(buffer,pro) != NULL)
//            processor++ ;   //此处我还想用system 调用grep过滤实现的
//    }
//    pclose(fd);
//    return processor;
// 问题1，此处使用管道会提示 /proc/cpuinfo: Permission denied，然而使用fopen却不会，为什么？


    int processor = 0;
    char pro[] = "processor";
    char path[] = "/proc/cpuinfo";
    char line[1000];
    FILE *fd = fopen(path, "r");

    while (!feof(fd)) {
        fgets(line, 1000, fd);
        if (strstr(line, pro) != NULL)
            processor++;
    }

    fclose(fd);

    return processor;


}

void CProcInfo::getCurProcNode(char *pid_path, proc_node *temp) {
    int uid;
    char path[256];
    FILE *fd;

    this->sys_mem_rate = getCurMemSize(&this->total_mem, &this->free_mem);


    strcpy(path, "/proc/");   //此处用strcpy的原因是字符数组不能直接赋值
    strcat(path, pid_path);  //字符串拼接
    strcat(path, "/stat");
//    printf("%s\n",path);
//最后的结果是类似这样的
//    /proc/856/stat
    fd = fopen(path, "r");
    fscanf(fd, "%d", &temp->pid);    //获取进程的pid

    fseek(fd, 2, SEEK_CUR);  //刚好是在第二个
    fscanf(fd, "%s", temp->cmd);
    temp->cmd[strlen(temp->cmd) - 1] = '\0';  //获取进程的cmd

    char stat[4];
    long long trash, cpu_time = 0;
    fscanf(fd, "%s ", stat);
    for (int i = 0; i < 10; i++) {
        fscanf(fd, "%d ", &trash);
    }
    for (int i = 0; i < 4; i++) {
        fscanf(fd, "%ld ", &trash);
        cpu_time += trash;
    }

    fclose(fd);


//    while(!feof(fd))
//    {
//        fgets(buffer,1000,fd);
//        if(strstr(buffer,pro) != NULL)
//            processor++ ;
//    }
//    printf("",buffer);
//    cout<<buffer<<endl;
    strcpy(path, "(ps -aux | grep ");
    strcat(path, pid_path);
    //    {
//        fgets(buffer,1000,fd);
//        if(strstr(buffer,pro) != NULL)
//            processor++ ;

//        fgets(buffer,100,fd);
//        if(strstr(buffer,pro) != NULL)
//            processor++ ;
//        fgets(buffer,10,fd);
//        if(strstr(buffer,pro) != NULL)
//            processor++ ;
//    }
    strcat(path, "|awk '{print $3}') 2>/dev/null ");
    FILE *fp;
    char buffer[80];
    fp = popen(path, "r");
    fgets(buffer, sizeof(buffer), fp);
//    strcat(temp->proc_cpu_rate, buffer);
//    temp->proc_cpu_rate=buffer;
    string str = buffer;
    double temp_str = double(stod(str));
    temp->proc_cpu_rate = temp_str;

//    cout<<stod(str)<<endl;
//    double temp1
//    for(int i=0;i<80;i++){
//        buffer
//    }
//    cout<<double(buffer[2])<<endl;
//    temp->proc_cpu_rate =double(buffer[0]);
//    cout<<strtol(buffer, nullptr, 10);
//    cout<<buffer;
//    fseek(fd, 3, SEEK_CUR);  //刚好是在第三个
//    fscanf(fd,"%lf", temp->proc_cpu_rate);


//    while(!feof(fd))
//    {
//        fgets(buffer,1000,fd);
//        if(strstr(buffer,pro) != NULL)
//            processor++ ;
//    }
//    printf("",buffer);
//    cout<<buffer<<endl;
    pclose(fp);


//    strcat(path, "/");
//    fd = fopen(path,"r");
//    fgets(temp->cmdline, 256, fd); //获取进程的cmdline
//    temp->cmdline[255] = '\0';

//    fclose(fd);

    strcpy(path, "/proc/");
    strcat(path, pid_path);
    strcat(path, "/cmdline");
    fd = fopen(path, "r");
    fgets(temp->cmdline, 256, fd); //获取进程的cmdline
    temp->cmdline[255] = '\0';

    fclose(fd);

    char line[1000];
    struct passwd *pw;
    strcpy(path, "/proc/");
    strcat(path, pid_path);
    strcat(path, "/status");
    fd = fopen(path, "r");
    while (!feof(fd)) {
        fgets(line, 1000, fd);
        if (strstr(line, "Uid") != NULL) {
            sscanf(line, "%s%d", path, &temp->uid);
            pw = getpwuid(temp->uid);
            strcpy(temp->username, pw->pw_name);
        }
    }
    fclose(fd);


    strcpy(path, "/proc/");
    strcat(path, pid_path);
    strcat(path, "/statm");
    fd = fopen(path, "r");
    fscanf(fd, "%d%d", &temp->vsz, &temp->rss);
    temp->vsz *= 4;             //获取进程的vsz 虚拟内存
    temp->rss *= 4;             //获取进程的rss 常驻内存，不进入交换分区的内存
    temp->proc_mem_rate = (temp->rss * 1.0) / this->total_mem * 100.0;  //获取进程的内存使用率
    fclose(fd);


}

/*
 * 获得系统的内存总量和当前使用量
 */




void CProcInfo::show() {
    printf("%15s\t%15s\t%20s\t%24s\t%18s\t%12s\t%15s\n",
           "pid",
           "username",
           "cpu占用",
           "交换分区占用",
           "内存占用",
           "cmd",
           "cmd位置"
    );

    for (int i = 0; i < this->proc_number; i++) {
        printf(
                "%15d\t"
                "%15s\t"
                "%15.2lf\%\t"
                "%12d\t"
                "%15d\t"
                "%15s\t"
                "%15s\n",
                this->proc_info[i].pid,
                this->proc_info[i].username,
                this->proc_info[i].proc_cpu_rate,
                this->proc_info[i].proc_mem_rate,
                this->proc_info[i].vsz,
                this->proc_info[i].rss,
                this->proc_info[i].cmd,
                this->proc_info[i].cmdline
        );
    }
    int a = searchTheNumberOfCPU();
    printf("cpu核心数为:%d\n", a);
    printf("cpu占用 = %.2lf\%\n任务数 = %d\n内存占用 = %.2lf\%\n",
           this->sys_cpu_rate, this->processor_number, this->sys_mem_rate);
}

int CProcInfo::searchTheNumberOfprocess() {
    struct dirent *dir_info;  //记载目录信息
    DIR *dir = opendir("/proc");
    if (dir == NULL) {
        printf("[fail]:无法打开 /proc !\n");
        return false;
    }
//尝试用异常试试看

//    DIR * dir
//    try{
//        dir = opendir("/proc"); //目录指针
//    }catch (exception* e ){
//        printf("无法打开 /proc !\n");
//        cerr<<e<<endl;
//    }

    while ((dir_info = readdir(dir)) != NULL) {
        //省略目录下的 . 和 ..
        if (strcmp(dir_info->d_name, ".") == 0 ||
            strcmp(dir_info->d_name, "..") == 0)
            continue;
        else if (!isNumber(dir_info->d_name)) {
            //非进程文件
            continue;
        } else {
            proc_node temp;   //创建一个新的结构体

            getCurProcNode(dir_info->d_name, &temp);  //获得现在结构体的信息，就是获得单个进程的所以信息

            this->proc_info.push_back(temp);      //把这个结构体推进vector中
            this->proc_number++;       //所有进程的数量加一
        }
    }
    closedir(dir);

    return proc_number;
}

double CProcInfo::getCurMemSize(unsigned long long int *total_mem, unsigned long long int *free_mem) {
    char dirty[10];
    FILE *fd = fopen("/proc/meminfo", "r");

    fscanf(fd, "%s%llu%s%s%llu", dirty, total_mem, dirty, dirty, free_mem);

    return (100.0 - (*free_mem) * 1.0 / (*total_mem) * 100.0) * 0.589;

}

void CProcInfo::check(){
//    //我知道了，这个地方程序运行一次就会产生一次新的pid，运行完成后pid又会消失，所以永远不可能一样，只能人工比对
//    char buffer[800];
//    for (int i = 0; i < 800; ++i) {
//        buffer[i]=0;
//    }
//
//    FILE *result=popen("(ps -aux|awk '{print $2}') 2>/dev/null", "r");
//    fread(buffer, sizeof(buffer),800, result);

//    int a=0;
//    for (int i = 0; i < 800; ++i) {
//        if (buffer[i]=='\n'){
//            a++;
//        }
//    }
//    cout<<a<<endl;





//    char buffer1[800];
//    for (int i = 0; i < 800; ++i) {
//        buffer1[i]=0;
//    }



//    for (int i = 0; i < proc_number; ++i) {
//        buffer1[i]=this->proc_info[i].pid;
//        char a= (this->proc_info[i].pid);
//        strcat(buffer1, a);
//        strcat(buffer1, "\n");
//    }
//    cout<<buffer<<" "<<buffer1<<endl;





//    cout<<buffer;


//    for (int i = 0; i < this->proc_number; ++i) {
//        printf("%d\n",this->proc_info[i].pid);
//    }





//    for (int i = 0; i < 76; ++i) {
//        if(buffer1[i]==proc_info[i].pid){
//            continue;
//        } else{
//            int j =i;
//            while(true){
//
//                cout<<buffer1[j];
//                j++;
//                if((j=='\0')|| (j=='\n')){
//                    cout<<endl;
//                    i=j+1;
//                    break;
//                }
//                //break;
//
//            }
//        }
//    }


//    char buffer1[80];
//    for (int i = 0; i < 80; ++i) {
//        buffer1[i]=0;
//    }
//
//    for (int i = 0; i < sizeof(buffer)+4; ++i) {
//        buffer1[i]=buffer[i+4];
//    }
//
//    cout<<buffer1<<endl;

//
//    for(int i=0;i<proc_number;i++){
//        if((proc_info[i].pid)==int(buffer1[i])){
//            continue;
//        }else{
//            cout<<buffer1[i];
//        }
//    }


//    for(int i = 0 ;i<80 ;i ++){
//        cout<<temp_str[i+3];
//        cout<<proc_info[i].pid;
//    }

//    cout<<buffer<<endl;
//    for(int i=0;i<100;i++){
//        if(char(proc_info[i].pid)==buffer[i+3]){
//            continue;
//        }else{
//            cout<<buffer[i+3];
//        }
//    }


//    for(int i=0 ;i<1024;i++){
//        cout<<buffer[i]<<endl;
//    }

//    string str = buffer;
//    printf(buffer[1])
}



//2.18、/proc/loadavg
//        保存关于CPU和磁盘I/O的负载平均值，其前三列分别表示每1秒钟、每5秒钟及每15秒的负载平均值，类似于uptime命令输出的相关信息；第四列是由斜线隔开的两个数值，前者表示当前正由内核调度的实体（进程和线程）的数目，后者表示系统当前存活的内核调度实体的数目；第五列表示此文件被查看前最近一个由内核创建的进程的PID；

#endif //__TASK_PROC_H


