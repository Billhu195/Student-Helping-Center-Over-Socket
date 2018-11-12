#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "freq_list.h"
#include "worker.h"

/* Complete this function for Task 1. Including fixing this comment.
*/
FreqRecord *get_word(char *word, Node *head, char **file_names) {
    Node *cur = head;
    int count = 0;
    // FreqRecord *result;

    while (cur != NULL) {
        if(strcmp(cur->word, word) == 0) {
            int length = sizeof(cur->freq) / sizeof(int);
            for(int i = 0; i < length; i++) {
                if(cur->freq[i] != 0) {
                    count++;
                }
            }

            FreqRecord *result_a = malloc(sizeof(FreqRecord) * (count + 1));
            printf("%d\n\n", count);
            printf("%lu\n\n", (sizeof(result_a) / sizeof(FreqRecord)));
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
            // result = result_a;
            printf("%lu\n\n", (sizeof(result_a) / sizeof(FreqRecord)));

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
    return;
}





int main() {
    char *word = "spinach";
    char *file_names[2] = {"Menu1", "Menu2"};

    Node *node_0 = malloc(sizeof(Node));
    strcpy(node_0->word, "pepper");
    memset(node_0->freq, 0, MAXFILES * sizeof(int));
    node_0->freq[0] = 0;
    node_0->freq[1] = 1;

    Node *node_1 = malloc(sizeof(Node));
    strcpy(node_1->word, "spinach");
    memset(node_1->freq, 0, MAXFILES * sizeof(int));
    node_1->freq[0] = 2;
    node_1->freq[1] = 6;

    node_0->next = node_1;

    Node *head = node_0;

    FreqRecord *result = get_word(word, head, file_names);
    FreqRecord *curr = result;
    // while (curr != NULL) {
    //     printf("%d %s\n", curr->freq, curr->filename);
    //     curr = curr + 1;
    // }

    int length = sizeof(curr->freq) / sizeof(int);
    printf("%d\n", length);
    printf("%d %s\n", curr->freq, curr->filename);
    curr = curr + 1;
    printf("%d %s\n", curr->freq, curr->filename);
    curr = curr + 1;
    printf("%d %s\n", curr->freq, curr->filename);
    curr = curr + 1;
    printf("%d %s\n", curr->freq, curr->filename);

    free(node_0->word);
    free(node_1->word);
    free(node_0);
    free(node_1);
}