#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hcq.h"
#define INPUT_BUFFER_SIZE 256

/*
 * Return a pointer to the struct student with name stu_name
 * or NULL if no student with this name exists in the stu_list
 */
Student *find_student(Student *stu_list, char *student_name) {

    if (stu_list == NULL) {
        return NULL;
    }
    
    Student *current = stu_list;
    if (strcmp(current->name, student_name) == 0) {
        return current;
    }
    else {
        while (current != NULL) {
            if (strcmp(current->name, student_name) == 0) {
                return current;
            }
            else {
                current = current->next_overall;
            }
        }
    }

    return NULL;
}



/*   Return a pointer to the ta with name ta_name or NULL
 *   if no such TA exists in ta_list. 
 */
Ta *find_ta(Ta *ta_list, char *ta_name) {
    
    if (ta_list == NULL) {
        return NULL;
    }

    Ta *curr_ta = ta_list;
    if(strcmp(curr_ta->name, ta_name) == 0) {
        return curr_ta;
    }
    else {
        while (curr_ta != NULL) {
            if (strcmp(curr_ta->name, ta_name) == 0) {
                return curr_ta;
            }
            else {
                curr_ta = curr_ta->next;
            }
        }
    }

    return NULL;
}


/*  Return a pointer to the course with this code in the course list
 *  or NULL if there is no course in the list with this code.
 */
Course *find_course(Course *courses, int num_courses, char *course_code) {
    int i = 0;
    for (i = 0; i < num_courses; i++) {
        if (strcmp(courses[i * sizeof(Course)].code, course_code) == 0) {
            return &courses[i * sizeof(Course)];
        }
    }
    return NULL;
}
    

/* Add a student to the queue with student_name and a question about course_code.
 * if a student with this name already has a question in the queue (for any
   course), return 1 and do not create the student.
 * If course_code does not exist in the list, return 2 and do not create
 * the student struct.
 * For the purposes of this assignment, don't check anything about the 
 * uniqueness of the name. 
 */
int add_student(Student **stu_list_ptr, char *student_name, char *course_code,
    Course *course_array, int num_courses) {
    
    Course *found_course = find_course(course_array, num_courses, course_code);
    Student *found_stu = find_student(*stu_list_ptr, student_name);

    if (found_stu != NULL) {
        return 1;
    }
    if (found_course == NULL) {
        return 2;
    }

    Student *new_stu = malloc(sizeof(Student));
    new_stu->name = malloc(strlen(student_name) + 1);
    new_stu->arrival_time = malloc(sizeof(time_t));
    new_stu->course = malloc(sizeof(Course));
    new_stu->next_overall = malloc(sizeof(Student));
    new_stu->next_course = malloc(sizeof(Student));

    strcpy(new_stu->name, student_name);
    *(new_stu->arrival_time) = time(0);
    new_stu->course = found_course;
    new_stu->next_overall = NULL;
    new_stu->next_course = NULL;

    if (*(stu_list_ptr) == NULL) {
        *stu_list_ptr = new_stu;
    }
    else {
        Student *curr_stu = *stu_list_ptr;
        while (curr_stu->next_overall != NULL) {
            curr_stu = curr_stu->next_overall;
        }
        curr_stu->next_overall = new_stu;
    }

    if(found_course->tail == NULL) {
        found_course->head = new_stu;
        found_course->tail = new_stu;
    }
    else if(found_course->head == found_course->tail) {
        found_course->tail = new_stu;
        (found_course->head)->next_course = new_stu;
    }
    else {
        (found_course->tail)->next_course = new_stu;
        found_course->tail = new_stu;
    }

    return 0;
}


/* Student student_name has given up waiting and left the help centre
 * before being called by a Ta. Record the appropriate statistics, remove
 * the student from the queues and clean up any no-longer-needed memory.
 *
 * If there is no student by this name in the stu_list, return 1.
 */
int give_up_waiting(Student **stu_list_ptr, char *student_name) {

    Student *stu_leave = find_student(*stu_list_ptr, student_name);
    if(stu_leave == NULL) {
        return 1;
    }

    Course *curr_course = stu_leave->course;
    curr_course->bailed = curr_course->bailed + 1;
    curr_course->wait_time = time(0) - *(stu_leave->arrival_time) + (curr_course->wait_time);
/*
    // test
    Student *test1 = curr_course->head;
    while (test1 != NULL) {
        printf("%s->", test1->name);
        test1 = test1->next_course;
    }
    printf("NULL\n");
*/
    Student *curr_stu = *stu_list_ptr;

    // special case that stu_leave is the first student of stu_list
    if (strcmp(stu_leave->name, curr_stu->name) == 0) {

        // stu_leave is the only student waiting for this course
       if (stu_leave == curr_course->tail) {
           curr_course->head = NULL;
           curr_course->tail = NULL;
/*          
           // no student behind stu_leave
           if (stu_leave->next_overall == NULL) {
               *stu_list_ptr = (*stu_list_ptr)->next_overall;
           }
           // there is student behind stu_leave
           else {
               *stu_list_ptr = stu_leave->next_overall;
           }
*/
       }

       // stu_leave is head of this course, but not tail
       else {
           curr_course->head = stu_leave->next_course;
       }
       *stu_list_ptr = stu_leave->next_overall;
    }
    // stu_leave is not the first student
    else {
//        Student *curr_student = *stu_list_ptr;
        Student *prev_student = *stu_list_ptr;
        while (prev_student->next_overall != stu_leave) {
            prev_student = prev_student->next_overall;
        }

        if (stu_leave == curr_course->head) {
            if (stu_leave == curr_course->tail) {
                curr_course->head = NULL;
                curr_course->tail = NULL;
            }
            else {
                curr_course->head = stu_leave->next_course;
            }
        }
        else {
            Student *prev_stu_c = curr_course->head;
            while (prev_stu_c->next_course != stu_leave) {
                prev_stu_c = prev_stu_c->next_course;
            }

            prev_stu_c->next_course = stu_leave->next_course;
            if (prev_stu_c->next_course == NULL) {
                curr_course->tail = prev_stu_c;
            }
        }

        prev_student->next_overall = stu_leave->next_overall;
    }

    stu_leave->next_overall = NULL;
    stu_leave->next_course = NULL;

    free(stu_leave->name);
    free(stu_leave->arrival_time);
    // free next_overall and next_course ?
    free(stu_leave);
/*
    // test
    Student *test2 = curr_course->head;
    while (test2 != NULL) {
        printf("%s->", test2->name);
        test2 = test2->next_course;
    }
    printf("NULL\n");
*/
    return 0;
}

/* Create and prepend Ta with ta_name to the head of ta_list. 
 * For the purposes of this assignment, assume that ta_name is unique
 * to the help centre and don't check it.
 */
void add_ta(Ta **ta_list_ptr, char *ta_name) {
    // first create the new Ta struct and populate
    Ta *new_ta = malloc(sizeof(Ta));
    if (new_ta == NULL) {
       perror("malloc for TA");
       exit(1);
    }
    new_ta->name = malloc(strlen(ta_name)+1);
    if (new_ta->name  == NULL) {
       perror("malloc for TA name");
       exit(1);
    }
    strcpy(new_ta->name, ta_name);
    new_ta->current_student = NULL;

    // insert into front of list
    new_ta->next = *ta_list_ptr;
    *ta_list_ptr = new_ta;
}

/* The TA ta is done with their current student. 
 * Calculate the stats (the times etc.) and then 
 * free the memory for the student. 
 * If the TA has no current student, do nothing.
 */
void release_current_student(Ta *ta) {
    if (ta->current_student == NULL) {
        // do nothing
    }
    else {
        Student *curr_stu = ta->current_student;
        Course *curr_course = (ta->current_student)->course;
        ta->current_student = NULL;

        curr_course->help_time = time(0) - *(curr_stu->arrival_time) + (curr_course->help_time);
        curr_course->helped = curr_course->helped + 1;
        
        //free course?
//        free();
        free(curr_stu->name);
//        free(curr_stu->next_overall);
//        free(curr_stu->next_course);
        free(curr_stu);
    }
}

/* Remove this Ta from the ta_list and free the associated memory with
 * both the Ta we are removing and the current student (if any).
 * Return 0 on success or 1 if this ta_name is not found in the list
 */
int remove_ta(Ta **ta_list_ptr, char *ta_name) {
    Ta *head = *ta_list_ptr;
    if (head == NULL) {
        return 1;
    } else if (strcmp(head->name, ta_name) == 0) {
        // TA is at the head so special case
        *ta_list_ptr = head->next;
        release_current_student(head);
        // memory for the student has been freed. Now free memory for the TA.
        free(head->name);
        free(head);
        return 0;
    }
    while (head->next != NULL) {
        if (strcmp(head->next->name, ta_name) == 0) {
            Ta *ta_tofree = head->next;
            //  We have found the ta to remove, but before we do that 
            //  we need to finish with the student and free the student.
            //  You need to complete this helper function
            release_current_student(ta_tofree);

            head->next = head->next->next;
            // memory for the student has been freed. Now free memory for the TA.
            free(ta_tofree->name);
            free(ta_tofree);
            return 0;
        }
        head = head->next;
    }
    // if we reach here, the ta_name was not in the list
    return 1;
}






/* TA ta_name is finished with the student they are currently helping (if any)
 * and are assigned to the next student in the full queue. 
 * If the queue is empty, then TA ta_name simply finishes with the student 
 * they are currently helping, records appropriate statistics, 
 * and sets current_student for this TA to NULL.
 * If ta_name is not in ta_list, return 1 and do nothing.
 */
int take_next_overall(char *ta_name, Ta *ta_list, Student **stu_list_ptr) {

    Ta *found_ta = find_ta(ta_list, ta_name);
    if(found_ta == NULL) {
        return 1;
    }

    // calculate helping time
    Student *prev_stu = found_ta->current_student;
    if (prev_stu != NULL) {
        Course *prev_course = prev_stu->course;
        prev_course->helped = prev_course->helped + 1;
        prev_course->help_time = time(0) - *(prev_stu->arrival_time) + (prev_course->help_time);
        free((found_ta->current_student)->name);
        free((found_ta->current_student)->arrival_time);
        //free next_overall and next_course ?
        free(found_ta->current_student);
        found_ta->current_student = NULL;
    }

    Student *stu_next = *stu_list_ptr;
    if (stu_next == NULL) {
        found_ta->current_student = NULL;
        return 0;
    }

    //update course list
    Course *curr_course = stu_next->course;
    if (curr_course->tail == stu_next) {
        curr_course->head = NULL;
        curr_course->tail = NULL;
    }
    else {
        curr_course->head = stu_next->next_course;
    }
    
    //update student list
    *stu_list_ptr = stu_next->next_overall;

    stu_next->next_overall = NULL;
    stu_next->next_course = NULL;

    //update waiting time
    curr_course->wait_time = time(0) - *(stu_next->arrival_time) + (curr_course->wait_time);
    *(stu_next->arrival_time) = time(0);
   
    found_ta->current_student = stu_next;

    return 0;
}



/* TA ta_name is finished with the student they are currently helping (if any)
 * and are assigned to the next student in the course with this course_code. 
 * If no student is waiting for this course, then TA ta_name simply finishes 
 * with the student they are currently helping, records appropriate statistics,
 * and sets current_student for this TA to NULL.
 * If ta_name is not in ta_list, return 1 and do nothing.
 * If course is invalid return 2, but finish with any current student. 
 */
int take_next_course(char *ta_name, Ta *ta_list, Student **stu_list_ptr, char *course_code, Course *courses, int num_courses) {

    Ta *found_ta = find_ta(ta_list, ta_name);
    if(found_ta == NULL) {
        return 1;
    }

    // finish current student
    Student *curr_stu = found_ta->current_student;
    if (curr_stu != NULL) {
        Course *course = curr_stu->course;
        course->helped = course->helped + 1;
        course->help_time = time(0) - (*(curr_stu->arrival_time)) + (course->help_time);
        free((found_ta->current_student)->name);
        free((found_ta->current_student)->arrival_time);
        // free next_overall and next_course ?
        free(found_ta->current_student);
        found_ta->current_student = NULL;
    }

    Course *found_course = find_course(courses, num_courses, course_code);
    if(found_course == NULL) {
        return 2;
    }

    Student *stu_next = found_course->head;
    if(stu_next == NULL) {
        found_ta->current_student = NULL;
        return 0;
    }

    // update course list
    if (found_course->tail == stu_next) {
        found_course->head = NULL;
        found_course->tail = NULL;
    }
    else {
        found_course->head = stu_next->next_course;
    }

    //update student list
    if (stu_next == *(stu_list_ptr)) {
        *stu_list_ptr = stu_next->next_overall;
    }
    else {
        Student *prev_stu = *stu_list_ptr;
        while (prev_stu->next_overall != stu_next) {
            prev_stu = prev_stu->next_overall;
        }
        prev_stu->next_overall = stu_next->next_overall;
    }
    
    stu_next->next_overall = NULL;
    stu_next->next_course = NULL;

    // update waiting time
    found_course->wait_time = time(0) - *(stu_next->arrival_time) + (found_course->wait_time);
    *(stu_next->arrival_time) = time(0);

    found_ta->current_student = stu_next;
    
    return 0;
}


/* For each course (in the same order as in the config file), print
 * the <course code>: <number of students waiting> "in queue\n" followed by
 * one line per student waiting with the format "\t%s\n" (tab name newline)
 * Uncomment and use the printf statements below. Only change the variable
 * names.
 */
void print_all_queues(Student *stu_list, Course *courses, int num_courses) {

    int i = 0;
    for (i = 0; i < num_courses; i++) {
        Course *curr_course = &courses[i * sizeof(Course)];
        int num_wait = 0;
        Student *head = curr_course->head;
        if (head != NULL) {
            num_wait++;
            while (head->next_course != NULL) {
                head = head->next_course;
                num_wait++;
            }
        }
        printf("%s: %d in queue\n", curr_course->code, num_wait);

        Student *root = curr_course->head;
        while(root != NULL) {
            printf("\t%s\n",root->name);
            root = root->next_course;
        }
    }
}


/*
 * Print to stdout, a list of each TA, who they are serving at from what course
 * Uncomment and use the printf statements 
 */
void print_currently_serving(Ta *ta_list) {
    if (ta_list == NULL) {
        printf("No TAs are in the help centre.\n");
    }

    Ta *curr_ta = ta_list;
    while (curr_ta != NULL) {
        if (curr_ta->current_student == NULL) {
            printf("TA: %s has no student\n", curr_ta->name);
        }
        else {
            printf("TA: %s is serving %s from %s\n", curr_ta->name, (curr_ta->current_student)->name, ((curr_ta->current_student)->course)->code);
        }

        curr_ta = curr_ta->next;
    }    

    //printf("TA: %s is serving %s from %s\n",i var1, var2);
    //printf("TA: %s has no student\n", var3);
}


/*  list all students in queue (for testing and debugging)
 *   maybe suggest it is useful for debugging but not included in marking? 
 */ 
void print_full_queue(Student *stu_list) {
    Student *curr = stu_list;
    while (curr) {
        printf("%s->", curr->name);
        curr = curr->next_overall;
    }
    printf("\n");
}

/* Prints statistics to stdout for course with this course_code
 * See example output from assignment handout for formatting.
 *
 */
int stats_by_course(Student *stu_list, char *course_code, Course *courses, int num_courses, Ta *ta_list) {

    // TODO: students will complete these next pieces but not all of this 
    //       function since we want to provide the formatting
    Course *found;

    found = find_course(courses, num_courses, course_code);
    if (found == NULL) {
        return 1;
    }
    
    int students_waiting = 0;
    Student *first_wait = found->head;
    if (first_wait != NULL) {
        students_waiting++;
        while(first_wait->next_course != NULL) {
            first_wait = first_wait->next_course;
            students_waiting++;
        }
    }
    
    //Loop Ta list find how many student currently helped
    int students_being_helped = 0;
    Ta *curr_ta = ta_list;
    while (curr_ta != NULL) {
        if(curr_ta->current_student != NULL) {
            if ((curr_ta->current_student)->course == found) {
                students_being_helped++;
            }
        }
        curr_ta = curr_ta->next;
    }
    
    // You MUST not change the following statements or your code 
    //  will fail the testing. 

    printf("%s:%s \n", found->code, found->description);
    printf("\t%d: waiting\n", students_waiting);
    printf("\t%d: being helped currently\n", students_being_helped);
    printf("\t%d: already helped\n", found->helped);
    printf("\t%d: gave_up\n", found->bailed);
    printf("\t%f: total time waiting\n", found->wait_time);
    printf("\t%f: total time helping\n", found->help_time);

    return 0;
}


/* Dynamically allocate space for the array course list and populate it
 * according to information in the configuration file config_filename
 * Return the number of courses in the array.
 * If the configuration file can not be opened, call perror() and exit.
 */
int config_course_list(Course **courselist_ptr, char *config_filename) {
    FILE *sourse_file;
    int error = 0;
    int num_course = 0;
    char course_name[7];
    char description[INPUT_BUFFER_SIZE - 7];
    int numc = 0;
    char line[INPUT_BUFFER_SIZE];
    Course *courselist;

    sourse_file = fopen(config_filename, "r");
    if (sourse_file == NULL) {
        perror("OpenError");
        exit(1);
    }
    if ((fgets(line, INPUT_BUFFER_SIZE, sourse_file)) != NULL) {
        num_course = strtol(line, NULL, 10);
        courselist = malloc(sizeof(Course) * num_course);
        while ((fgets(line, INPUT_BUFFER_SIZE, sourse_file)) != NULL) {
              strncpy(course_name, line, 6);
              strcpy(description, &line[7]);
	      description[strlen(description) - 1] = '\0';

              strcpy(courselist[numc * sizeof(Course)].code, course_name);

              courselist[numc * sizeof(Course)].description = malloc(strlen(description) + 1);
	      strcpy(courselist[numc * sizeof(Course)].description, description);

	      courselist[numc * sizeof(Course)].helped = 0;
	      courselist[numc * sizeof(Course)].bailed = 0;
	      courselist[numc * sizeof(Course)].wait_time = 0.0;
	      courselist[numc * sizeof(Course)].help_time= 0.0;
              courselist[numc * sizeof(Course)].head = NULL;
              courselist[numc * sizeof(Course)].tail = NULL;

              numc++;
        }
    }
    *courselist_ptr = courselist;

    error = fclose(sourse_file);
    if (error != 0) {
        perror("CloseError");
    }
    return num_course;
}
