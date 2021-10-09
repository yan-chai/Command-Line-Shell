/**
 * @file
 *
 * Contains shell jobs data structures and retrieval functions.
 */

#ifndef _JOBS_H_
#define _JOBS_H_

void jobs_init();
void jobs_destroy();
void jobs_add(char *item);
void jobs_dele(char *item);
void jobs_print();

#endif