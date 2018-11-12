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

    char *listfile = "index";
    char *namefile = "filenames";
    Node *head;
    char **filenames = malloc(sizeof(char) * 10 * MAXFILES); //不知道file_name的长度
    read_list(listfile, namefile, &head, filenames);

    int n;
    char word[MAXWORD];
    while ((n = read(in, word, MAXWORD)) != 0) {
        if (n == -1) {
            perror("run worker read");
            exit(1);
        } else {
            Node *cur = head;
            while (cur != NULL) {
                if (strcmp(cur->word, word) == 0) {
                    char *name = strrstr("/", dirname);
                    char *fname = {'\0'};
                    strcpy(fname, (&name)[1]);
                    add_word(head, filenames, word, fname); // 不知道现在的file name ?
                    free(fname);
                }
                cur = cur->next;
            }
        }
    }

    Node *curr = head;
    while (curr != NULL) {
        FreqRecord *array_to_write = get_word(curr->word, head, filenames);
        // int i = 0;
        // int m;
        FreqRecord *cur_freq = array_to_write;
        while (cur_freq->freq != 0) {
            if ((write(out, cur_freq, sizeof(FreqRecord))) == -1) {
                perror("run worker write");
                exit(1);
            }
            cur_freq = cur_freq + 1;
        }
    }

    // int o;
    FreqRecord *item = malloc(sizeof(FreqRecord));
    item->freq =  0;
    strcpy(item->filename, "");
    if ((write(out, item, sizeof(FreqRecord))) == -1) {
        perror("run worker final write");
        exit(1);
    }
    free(item);

    return;
}





// int main() {
//     char *word = "spinach";
//     char *file_names[2] = {"Menu1", "Menu2"};

//     Node *node_0 = malloc(sizeof(Node));
//     strcpy(node_0->word, "pepper");
//     memset(node_0->freq, 0, MAXFILES * sizeof(int));
//     node_0->freq[0] = 0;
//     node_0->freq[1] = 1;

//     Node *node_1 = malloc(sizeof(Node));
//     strcpy(node_1->word, "spinach");
//     memset(node_1->freq, 0, MAXFILES * sizeof(int));
//     node_1->freq[0] = 2;
//     node_1->freq[1] = 6;

//     node_0->next = node_1;

//     Node *head = node_0;

//     FreqRecord *result = get_word(word, head, file_names);
//     FreqRecord *curr = result;
//     while (curr != NULL && curr->freq != 0) {
//         printf("%d %s\n", curr->freq, curr->filename);
//         curr = curr + 1;
//     }

//     // int length = sizeof(curr->freq) / sizeof(int);
//     // printf("%d\n", length);
//     // printf("%d %s\n", curr->freq, curr->filename);
//     // curr = curr + 1;
//     // printf("%d %s\n", curr->freq, curr->filename);
//     // curr = curr + 1;
//     // printf("%d %s\n", curr->freq, curr->filename);
//     // curr = curr + 1;
//     // printf("%d %s\n", curr->freq, curr->filename);

//     // free(node_0->word);
//     // free(node_1->word);
//     // free(node_0);
//     // free(node_1);
// }