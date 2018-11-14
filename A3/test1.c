#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freq_list.h"
#include "worker.h"

/* For testing task 1, I will use the data in a3-2018/labs/lab1
For line 22, display_list is use to print all the words in that
directory (using for checking if get_word work correct).
After call make in Terminal, there will be a file called "test1"
(check Makefile). Run that file will print the freq_records of 
"sort" (change word in line 23 if needed).
*/
int main() {
    Node *head = NULL;
    char **filenames = init_filenames();
    char *listfile = "./a3-2018/labs/lab1/index"; // test case for task1
    char *namefile = "./a3-2018/labs/lab1/filenames"; // test case for task1

    read_list(listfile, namefile, &head, filenames);
    // display_list(head, filenames); // show all the words in index
    FreqRecord *frp = get_word("sort", head, filenames); // change word here
    print_freq_records(frp);
}