#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freq_list.h"
#include "worker.h"

int main() {
    Node *head = NULL;
    char **filenames = init_filenames();
    char *listfile = "./a3-2018/labs/lab1/index";
    char *namefile = "./a3-2018/labs/lab1/filenames";

    read_list(listfile, namefile, &head, filenames);
    // display_list(head, filenames);
    FreqRecord *frp = get_word("sort", head, filenames);
    print_freq_records(frp);
}