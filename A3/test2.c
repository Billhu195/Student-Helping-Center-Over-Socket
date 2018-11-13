#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freq_list.h"
#include "worker.h"

int main() {
    FILE *test;
    FreqRecord item;
    if ((test = fopen("task2", "r")) == NULL) {
        perror("fopen for test in test2.c");
        exit(1);
    }
    while (fread(&item, sizeof(FreqRecord), 1, test)) {
        printf("%d    %s\n", item.freq, item.filename);
    }
}