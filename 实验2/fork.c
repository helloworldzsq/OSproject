#include <stdio.h>
#include <stdlib.h>

int main(){
    int p1,p2;
    while((p1=fork())==-1);
    if (p1==0)
        printf("Child1 \n");
    else
        {
          while((p2=fork())==-1);
          if (p2==0)
            printf("Child2 \n");
          else
            printf("Parent \n"); 
        }
    return 0;  
}
