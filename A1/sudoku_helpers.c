#include <stdio.h>

/* Each of the n elements of array elements, is the address of an
 * array of n integers.
 * Return 0 if every integer is between 1 and n^2 and all
 * n^2 integers are unique, otherwise return 1.
 */
int check_group(int **elements, int n) {
    // TODO: replace this return statement with a real function body
    int i;
    int j = 0;
    int num[n * n];

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++){
            num[(i * 3) + j] = ((*(elements + i))[j]);
        }
    }

    for (i = 0; i < (n * n); i++){
        int result = 0;
        for (j = 0; j < (n * n); j++) {
            if (num[i] == num[j]) {
                result++;
            }
        }
        if (result != 1) {
            return 1;
        }
        result = 0;
    }
    return 0;
}

/* puzzle is a 9x9 sudoku, represented as a 1D array of 9 pointers
 * each of which points to a 1D array of 9 integers.
 * Return 0 if puzzle is a valid sudoku and 1 otherwise. You must
 * only use check_group to determine this by calling it on
 * each row, each column and each of the 9 inner 3x3 squares
 */
int check_regular_sudoku(int **puzzle) {

    // TODO: replace this return statement with a real function body
    int n;
    int i;
    int arow1[3];
    int arow2[3];
    int arow3[3];
    int arow4[3];
    int arow5[3];
    int arow6[3];
    int arow7[3];
    int arow8[3];
    int arow9[3];

    for (n = 0; n < 9; n++){
        for (i = 0; i < 3; i++){
            arow1[i] = (*(puzzle + n))[i];
            arow2[i] = (*(puzzle + n))[i + 3];
            arow3[i] = (*(puzzle + n))[i + 6];
        }
        int *test1[3] = {arow1, arow2, arow3};
        if (check_group(test1, 3)){
            return 1;
        }
    }
    
    for (n = 0; n < 9; n++) {
        for (i = 0; i < 3; i++) {
            arow4[i] = (*(puzzle + i))[n];
            arow5[i] = (*(puzzle + i + 3))[n];
            arow6[i] = (*(puzzle + i + 6))[n];
        }
        int *test2[3] = {arow4, arow5, arow6};
        if (check_group(test2, 3)) {
            return 1;
        }
    }
    int m;
    for (n = 0; n < 3; n++) {
    for (m = 0; m < 3; m++) {
        for (i = 0; i < 3; i++){
            arow7[i] = (*(puzzle + (n * 3)))[(m * 3) + i];
            arow8[i] = (*(puzzle + 1 + (n * 3)))[(m * 3) + i];
            arow9[i] = (*(puzzle + 2 + (n * 3)))[(m * 3) + i];
        }
        int *test3[3] = {arow7, arow8, arow9};
        if(check_group(test3, 3)){
            return 1;
        }
    }
    }
    return 0;
}

