#ifndef TOYOS_PATH_H
#define TOYOS_PATH_H

/*
 * Единый модуль разбора путей Toy OS v62.
 *
 * Модуль не знает ничего о FAT16, диске и каталогах. Он отвечает только за
 * синтаксис и канонизацию пути. Благодаря этому файловый код ядра не должен
 * содержать собственные копии алгоритма обработки '.', '..' и '/'.
 */

int path_is_absolute(const char *path);
int path_normalize(const char *cwd, const char *path, char *out, unsigned int cap);
int path_join(const char *left, const char *right, char *out, unsigned int cap);
int path_parent(const char *path, char *out, unsigned int cap);
int path_basename(const char *path, char *out, unsigned int cap);

#endif
