#ifndef PARSE3TUPLE_H
#define PARSE3TUPLE_H

/*
 * $Id: parse3tuple.h,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#define TUPLE3_MAX_LENGTH 1024
#define TAG_MAX_LENGTH 96
#define TYPE_MAX_LENGTH 32
#define VALUE_MAX_LENGTH TUPLE3_MAX_LENGTH - TAG_MAX_LENGTH - TYPE_MAX_LENGTH

/*
 * parse3tuple() parses file fname line by line. Each line is expected
 * to have 3 fields, <tag>-<type>-<value>, separated by blanks or tabs.
 * Lines starting with '#' are comments. Empty lines are ignored.
 * The value field contains a number of characters, even field separators
 * like blanks and tabs, up to the end of the line.
 * User of this utility provides a call back function with 3 arguments
 * (pointers to the tag, type and value strings) to validate and use the
 * data.
 * usage example: if (!parse3tuple ("cs.cfg", processCsConfigData))
 *                    exit (0);
 */
extern bool parse3tuple (const char*, void (*f) (char*, char*, char*));

extern int strConst2i (const char*str, const char* strTable[], const int tableSize);

/* PARSE3TUPLE_H  */
#endif
