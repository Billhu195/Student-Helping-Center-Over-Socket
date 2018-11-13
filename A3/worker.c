#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "freq_list.h"
#include "worker.h"

/* Complete this function for Task 1. Including fixing this comment.
get_word will return an array of FreqRecord elements for the word. 
To indicate the end of the valid records, the last record will have 
a freq value of 0 and the filename set to an empty string. If the 
word is not found in the index, get_word returns an array with 
one element where the freq value is 0 and filename is an empty string.
*/
FreqRecord *get_word(char *word, Node *head, char **file_names) {
    Node *cur = head;
    int count = 0;

    while (cur != NULL) {
        if(strcmp(cur->word, word) == 0) {
            int length = sizeof(cur->freq) / sizeof(int);
            for(int i = 0; i < length; i++) {
                if(cur->freq[i] != 0) {
                    count++;
                }
            }

            FreqRecord *result_a = malloc(sizeof(FreqRecord) * (count + 1));
            // printf("%d\n\n", count);
            // printf("%lu\n\n", (sizeof(result_a) / sizeof(FreqRecord)));
            int acc = 0;
            for(int i = 0; i < length; i++) {
                if(cur->freq[i] != 0) {
                    FreqRecord item;
                    item.freq =  cur->freq[i];
                    strcpy(item.filename, file_names[i]);
                    result_a[acc] = item;
                    acc++;
                }
            }

            FreqRecord item;
            item.freq =  0;
            strcpy(item.filename, "");
            result_a[acc] = item;
            // printf("%lu\n\n", (sizeof(result_a) / sizeof(FreqRecord)));

            return result_a;
        }
        cur = cur->next;
    }

    
    FreqRecord *result_b = malloc(sizeof(FreqRecord) * (count + 1));
    FreqRecord item;
    item.freq =  0;
    strcpy(item.filename, "");
    result_b[count] = item;
    return result_b;

}

/* Print to standard output the frequency records for a word.
* Use this for your own testing and also for query.c
*/
void print_freq_records(FreqRecord *frp) {
    int i = 0;

    while (frp != NULL && frp[i].freq != 0) {
        printf("%d    %s\n", frp[i].freq, frp[i].filename);
        i++;
    }
}

/* Complete this function for Task 2 including writing a better comment.
*/
void run_worker(char *dirname, int in, int out) {

    // char *dir = dirname;
    char listfile1[PATHLENGTH] = {'.', '/'};
    char *listfile = listfile1;
    listfile = strcat(listfile, dirname);
    listfile = strcat(listfile, "/index");
    // printf("%s\n", listfile);
    char namefile1[PATHLENGTH] = {'.', '/'};
    char *namefile = namefile1;
    namefile = strcat(namefile, dirname);
    namefile = strcat(namefile, "/filenames");
    // printf("%s\n", namefile);
    Node *head;
    char **filenames = init_filenames();
    read_list(listfile, namefile, &head, filenames);
    // printf("read list work\n");
    // display_list(head, filenames);

    int n;
    char word[MAXWORD];
    while ((n = read(in, word, MAXWORD)) != 0) {
        if (n == -1) {
            perror("run worker read");
            exit(1);
        } else {
            // printf("read work\n");
            word[n] = '\0';
            // printf("%s\n", word);
            FreqRecord *array_to_write = get_word(word, head, filenames);
            // printf("get word work\n");
            // printf("%d    %s\n", array_to_write[0].freq, array_to_write[0].filename);
            // print_freq_records(array_to_write);
            while (array_to_write->freq != 0) {
                if ((write(out, array_to_write, sizeof(FreqRecord))) == -1) {
                    perror("run worker write");
                    exit(1);
                }
                // printf("write work\n");
                array_to_write = array_to_write + 1;
            }
            FreqRecord *item = malloc(sizeof(FreqRecord));
            item->freq =  0;
            strcpy(item->filename, "");
            if ((write(out, item, sizeof(FreqRecord))) == -1) {
                perror("run worker final write");
                exit(1);
                }
            free(item);
            
        }
    }
}
