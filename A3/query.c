#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>

#include "freq_list.h"
#include "worker.h"

/* A program to model calling run_worker and to test it. Notice that run_worker
 * produces binary output, so the output from this program to STDOUT will 
 * not be human readable.  You will need to work out how to save it and view 
 * it (or process it) so that you can confirm that your run_worker 
 * is working properly.
 */
int main(int argc, char **argv) {
    char ch;
    char path[PATHLENGTH];
    char *startdir = ".";
    int count = 0;
    int parent_to_child[MAXWORKERS][2];
    int child_to_parent[MAXWORKERS][2];
    int r = -1;

    /* this models using getopt to process command-line flags and arguments */
    while ((ch = getopt(argc, argv, "d:")) != -1) {
        switch (ch) {
        case 'd':
            startdir = optarg;
            break;
        default:
            fprintf(stderr, "Usage: query [-d DIRECTORY_NAME]\n");
            exit(1);
        }
    }

    // Open the directory provided by the user (or current working directory)
    DIR *dirp;
    if ((dirp = opendir(startdir)) == NULL) {
        perror("opendir");
        exit(1);
    }

    /* For each entry in the directory, eliminate . and .., and check
     * to make sure that the entry is a directory, then call run_worker
     * to process the index file contained in the directory.
     * Note that this implementation of the query engine iterates
     * sequentially through the directories, and will expect to read
     * a word from standard input for each index it checks.
     */
    struct dirent *dp;
    while ((dp = readdir(dirp)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 ||
            strcmp(dp->d_name, "..") == 0 ||
            strcmp(dp->d_name, ".svn") == 0 ||
            strcmp(dp->d_name, ".git") == 0) {
                continue;
        }

        strncpy(path, startdir, PATHLENGTH);
        strncat(path, "/", PATHLENGTH - strlen(path));
        strncat(path, dp->d_name, PATHLENGTH - strlen(path));
        path[PATHLENGTH - 1] = '\0';

        struct stat sbuf;
        if (stat(path, &sbuf) == -1) {
            // This should only fail if we got the path wrong
            // or we don't have permissions on this entry.
            perror("stat");
            exit(1);
        }

        // Only call run_worker if it is a directory
        // Otherwise ignore it.
        if (S_ISDIR(sbuf.st_mode)) {
            if (count == 10) {
                fprintf(stderr, "too many subdirectories");
                exit(1);
            }

            if (((pipe(parent_to_child[count])) == -1)) { // parent to child
                perror("pipe");
                exit(1);
            }
            if (((pipe(child_to_parent[count])) == -1)) {// child to parent
                perror("pipe");
                exit(1);
            }

            r = fork();
            if (r < 0) {
                perror("fork");
                exit(1);
            } else if (r > 0) { // parent process
                if (close(parent_to_child[count][0]) == -1) {
                    perror("close1");
                    exit(1);
                }
                if (close(child_to_parent[count][1]) == -1) {
                    perror("close2");
                    exit(1);
                }

            } else { // child process
                if (close(parent_to_child[count][1]) == -1) {
                    perror("close3");
                    exit(1);
                }
                if (close(child_to_parent[count][0]) == -1) {
                    perror("close4");
                    exit(1);
                }
                
                for (int i = 0; i < count; i++) {
                    if (close(parent_to_child[i][1]) == -1) {
                        perror("close5");
                        exit(1);
                    }
                    if (close(child_to_parent[i][0]) == -1) {
                        perror("close6");
                        exit(1);
                    }
                }
                run_worker(path, parent_to_child[count][0], child_to_parent[count][1]);
                if (close(parent_to_child[count][0]) == -1) {
                    perror("close7");
                    exit(1);
                }
                if (close(child_to_parent[count][1]) == -1) {
                    perror("close8");
                    exit(1);
                }
                exit(0);
            }
            
            count ++;
        }
    }

    // read word
    int n;
    char word[MAXWORD];

    if (r > 0) {
        FreqRecord freq_item;

        while ((n = scanf("%s", word)) == 1) {
            FreqRecord master_array[MAXRECORDS * MAXWORKERS]; // new array
            int num_added = 0;
            if (n < 0) {
                perror("scanf error");
                exit(1);
            }
            for (int j = 0; j < count; j++) {
                if ((n = write(parent_to_child[j][1], word, MAXWORD)) == -1) {
                    perror("write fail for parent to child");
                    exit(1);
                }
            }

            for (int k = 0; k < count; k++) {
                n = read(child_to_parent[k][0], &freq_item, sizeof(FreqRecord));
                if (n == -1) {
                    perror("read fail for child to parent1");
                    exit(1);
                } 
                while (freq_item.freq != 0) {
                    // add to master frequency array
                    master_array[num_added] = freq_item;
                    num_added++;
                    // read next freq
                    if ((n = read(child_to_parent[k][0], &freq_item, sizeof(FreqRecord))) == -1) {
                        perror("read fail for child to parent2");
                        exit(1);
                    }
                }
                
            }
            // sort and print master array
            FreqRecord item;
            item.freq = 0;
            master_array[num_added] = item;
            sort_array(master_array, num_added);
            print_freq_records(master_array);
        }

        // stdin close, close all write pipes from parents
        for (int m = 0; m < count; m++) {
            if (close(parent_to_child[m][1]) == -1) {
                perror("close123");
            }
        }

        // wait all child process exit
        int status;
        while (count > 0) {
            wait(&status);
            if (WIFEXITED(status)) {
                count--;
            }
        }

    }

    if (closedir(dirp) < 0)
        perror("closedir");

    return 0;
}
