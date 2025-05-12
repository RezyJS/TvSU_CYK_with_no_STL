#ifndef CYK_H
#define CYK_H

#include "utils.h"

// Структура для хранения информации о разбиении правила (A → B C)
struct Backtrace
{
  char *B;       // Левый дочерний нетерминал
  char *C;       // Правый дочерний нетерминал
  int split_pos; // Позиция разделения подстроки (k)
};

// Ячейка таблицы CYK. Хранит нетерминалы и обратные ссылки для восстановления вывода.
struct Cell
{
  StringArray symbols; // Список нетерминалов, порождающих подстроку
  Backtrace **traces;  // Массив указателей на Backtrace для каждого нетерминала
};

// Основная функция алгоритма CYK. Проверяет, выводима ли строка в грамматике.
bool cyk(const char *word, StringArray &derivation);

#endif // CYK_H