
#include <stdio.h>

int main(){
    int a = 1;
    
    char phone[11];
    scanf("%c%c%c%c%c%c%c%c%c%c", &phone[0], &phone[1], &phone[2], &phone[3], &phone[4], &phone[5], &phone[6],
 &phone[7], &phone[8], &phone[9]);

    while(a) {
    int num;
    scanf("%d", &num);
    if (num == -1){
        int i;
        for (i=0; i<10; i++){
        printf("%c", phone[i]);
        }
    printf("\n");
    } 
    else if (num <= 9 && num >= 0){
        printf("%c\n", phone[num]);
    }
    else{
        printf("ERROR");
        return 1;
    }
    }
    return 0;
}
