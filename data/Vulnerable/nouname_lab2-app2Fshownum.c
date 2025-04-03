/**
 * show.c -- реализует команду вывода содержимого текста с нумерацией строк
 *
 * Copyright (c) 2018, Darya Madrakhimova <madrahim@petrsu.ru>
 *
 * This code is licensed under a MIT-style license.
 */

#include "common.h"
#include "text/text.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAGENTA "\x1b[35m"
#define RESET "\033[0m"

static void shownum_line(int index, char *contents, int cursor, void *data);

/**
 * Выводит содержимое с нумерацией строк
 */
void shownum(text txt) { process_forward(txt, shownum_line, NULL); }

static void shownum_line(int index, char *contents, int cursor, void *data) {
  assert(contents != NULL);

  UNUSED(data);
  UNUSED(cursor);

  char line[MAXLINE];
  char output_line[MAXLINE];
  strcpy(line, contents);

  /* Выводим строку и ее номер на экран */
  if (cursor >= 0) {
    strncpy(output_line, line, cursor);
    output_line[cursor] = '|';
    strcpy(output_line + cursor + 1, line + cursor);
    printf(MAGENTA "%d" RESET " %s", index + 1, output_line);
    if (output_line[strlen(output_line) - 1] != '\n')
      printf("\n");
  }
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 34,2;34,8

// 40,4;40,10

