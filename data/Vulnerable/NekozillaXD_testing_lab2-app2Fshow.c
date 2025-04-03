/**
 * show.c -- реализует команду вывода хранимого текста на экран
 *
 * Copyright (c) 2017, Alexander Borodin <aborod@petrsu.ru>
 *
 * This code is licensed under a MIT-style license.
 */

#include "common.h"
#include "text/text.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void show_line(int index, char *contents, int cursor, void *data);

/**
 * Выводит содержимое указанного файла на экран
 */
void show(text txt) {
  /* Применяем функцию show_line к каждой строке текста */
  process_forward(txt, show_line, NULL);
}

static void show_line(int index, char *contents, int cursor, void *data) {
  assert(contents != NULL);

  UNUSED(index);
  UNUSED(cursor);
  UNUSED(data);

  /* Выводим строку на экран */
  char line[MAXLINE];
  char output_line[MAXLINE];
  strcpy(line, contents);

  if (cursor >= 0) {
    strncpy(output_line, line, cursor);
    output_line[cursor] = '|';
    strcpy(output_line + cursor + 1, line + cursor);
    printf("%s", output_line);
    if (output_line[strlen(output_line) - 1] != '\n')
      printf("\n");
  } else {
    printf("%s", line);
    if (line[strlen(line) - 1] != '\n')
      printf("\n");
  }
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 35,2;35,8

// 40,4;40,10

// 35,2;35,8

// 40,4;40,10

