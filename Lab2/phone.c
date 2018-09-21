#include <stdio.h>

int main(){
    
    char phone[11];
    scanf("%c%c%c%c%c%c%c%c%c%c", &phone[0], &phone[1], &phone[2], &phone[3], &phone[4], &phone[5], &phone[6],
 &phone[7], &phone[8], &phone[9]);

    int num;
    scanf("%d", &num);
    if (num == -1){
        int i;
        for (i=0; i<11; i++){
        printf("%c", phone[i]);
        }
    } 
    else if (num <= 9 && num >= 0){
        printf("%c", phone[num]);
    }
    else{
        printf("ERROR");
        return 1;
    }
    return 0;
}
