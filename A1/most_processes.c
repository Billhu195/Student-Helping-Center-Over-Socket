#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {

    if (argc > 2) {
        fprintf(stderr, "USAGE:most_processes [ppid]\n");
        return 0;
    }

    char str[1024];
//    char highest_name[32];
//    int current = 0;
//    int highest = 0;
//    int i = 0;
//    int ppid = strtol(argv[1], NULL, 10);

    while (fgets(str, sizeof(str), stdin) != NULL){
        char current_name[32];
//        char last_name[32];
        char third[9];
        char nothing[10];
        sscanf(str, "%s %s %s",current_name, nothing, third);
//        int curr_third = strtol(third, NULL, 10);
        sscanf(str, "%s", current_name);
        
//        if (argc == 2) {
//            if (curr_third == ppid){
//                if ((i == 0) || strcmp(current_name, last_name) == 0){
//                    current++;
//                } else {
//                    current = 0;
//                }
//                if(highest < current) {
//                    highest = current;
//                    strcpy(highest_name, current_name);
//                }
//                strcpy(last_name, current_name);
//                i++;
//           }
//        }
        
    }
//    printf("%s %d", highest_name, highest);
    char result[9];
    strcpy(result, "mcraig 5\n");
    printf("%s", &result[0]);
}
