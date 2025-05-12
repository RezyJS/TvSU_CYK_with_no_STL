#include "cyk.h"
#include "grammar.h"
#include "utils.h"
#include <cstdlib>

// Создает таблицу CYK размером n x (n+1). Инициализирует ячейки пустыми значениями.
// n + 1 используется для более интуитивного и красивого обращения к
// символу без сдвига "-1". [0] столбец не используется
static Cell **create_cyk_table(int n)
{
  Cell **table = (Cell **)malloc(n * sizeof(Cell *));
  for (int i = 0; i < n; ++i)
  {
    table[i] = (Cell *)malloc((n + 1) * sizeof(Cell));
    for (int j = 0; j <= n; ++j)
    {
      table[i][j].symbols = StringArray(); // Инициализация пустым массивом
      table[i][j].traces = nullptr;        // Изначально следов нет
    }
  }
  return table;
}

// Проверяет, содержится ли символ `symbol` в массиве строк `arr`.
static bool contains(const StringArray &arr, const char *symbol)
{
  for (int i = 0; i < arr.size; ++i)
  {
    if (my_strcmp(arr.data[i], symbol) == 0)
    {
      return true;
    }
  }
  return false;
}

// Рекурсивно строит вывод, начиная с нетерминала A в позиции (i, l).
static void build_derivation(int i, int l, const char *A, Cell **bt, const char *word, StringArray &derivation)
{
  if (l == 1)
  {
    // Базовый случай: терминальное правило (A → a)
    char buf[64];
    int pos = 0;
    // Формируем строку вида "A -> a"
    for (int idx = 0; A[idx] != '\0' && pos < 60; ++idx)
    {
      buf[pos++] = A[idx];
    }
    buf[pos++] = ' ';
    buf[pos++] = '-';
    buf[pos++] = '>';
    buf[pos++] = ' ';
    buf[pos++] = word[i]; // Добавляем терминал
    buf[pos] = '\0';
    derivation.push_back(buf);
    return;
  }

  // Ищем Backtrace для текущего нетерминала A
  Backtrace *trace = nullptr;
  for (int k = 0; k < bt[i][l].symbols.size; ++k)
  {
    if (my_strcmp(bt[i][l].symbols.data[k], A) == 0)
    {
      trace = bt[i][l].traces[k];
      break;
    }
  }
  if (!trace)
    return; // Если след не найден, завершаемся

  // Формируем правило вида "A -> B C"
  char rule[128];
  int pos = 0;
  // Добавляем левую часть правила (A)
  for (int idx = 0; A[idx] != '\0' && pos < 120; ++idx)
  {
    rule[pos++] = A[idx];
  }
  rule[pos++] = ' ';
  rule[pos++] = '-';
  rule[pos++] = '>';
  rule[pos++] = ' ';
  // Добавляем правые части (B и C)
  for (int idx = 0; trace->B[idx] != '\0' && pos < 120; ++idx)
  {
    rule[pos++] = trace->B[idx];
  }
  rule[pos++] = ' ';
  for (int idx = 0; trace->C[idx] != '\0' && pos < 120; ++idx)
  {
    rule[pos++] = trace->C[idx];
  }
  rule[pos] = '\0';
  derivation.push_back(rule);

  // Рекурсивно обрабатываем дочерние нетерминалы B и C
  build_derivation(i, trace->split_pos, trace->B, bt, word, derivation);
  build_derivation(i + trace->split_pos, l - trace->split_pos, trace->C, bt, word, derivation);
}

// Алгоритм CYK. Возвращает true, если слово выводимо, и заполняет `derivation`.
bool cyk(const char *word, StringArray &derivation)
{
  int n = my_strlen(word);
  if (n == 0)
    return false;

  Cell **table = create_cyk_table(n); // Создаем таблицу

  // Заполнение таблицы для подстрок длины 1
  for (int i = 0; i < n; ++i)
  {
    char symbol[2] = {word[i], '\0'};
    StringArray *A_list = term_rules.get(symbol); // Нетерминалы, порождающие word[i]
    if (A_list)
    {
      for (int k = 0; k < A_list->size; ++k)
      {
        table[i][1].symbols.push_back(A_list->data[k]);
        // Создаем Backtrace для терминального правила (нет дочерних B и C)
        Backtrace *bt = (Backtrace *)malloc(sizeof(Backtrace));
        bt->B = nullptr;
        bt->C = nullptr;
        bt->split_pos = 0;
        // Расширяем массив traces и добавляем новый Backtrace
        table[i][1].traces = (Backtrace **)realloc(
            table[i][1].traces,
            table[i][1].symbols.size * sizeof(Backtrace *));
        table[i][1].traces[table[i][1].symbols.size - 1] = bt;
      }
    }
  }

  // Заполнение таблицы для подстрок длины > 1
  for (int len = 2; len <= n; ++len)
  {
    for (int i = 0; i <= n - len; ++i)
    {
      for (int k = 1; k < len; ++k)
      { // Перебираем все возможные точки разбиения
        int j = i + k;
        Cell &left = table[i][k];        // Левая подстрока [i, i+k)
        Cell &right = table[j][len - k]; // Правая подстрока [j, j + len - k)

        // Перебираем все пары (B, C) из левой и правой подстрок
        for (int b = 0; b < left.symbols.size; ++b)
        {
          for (int c = 0; c < right.symbols.size; ++c)
          {
            // Ищем правила вида A → B C
            StringArray *A_list = nonterm_rules.get(
                left.symbols.data[b],
                right.symbols.data[c]);
            if (A_list)
            {
              for (int a = 0; a < A_list->size; ++a)
              {
                const char *A = A_list->data[a];
                if (!contains(table[i][len].symbols, A))
                {
                  table[i][len].symbols.push_back(A);
                  // Создаем Backtrace для правила A → B C
                  Backtrace *bt = (Backtrace *)malloc(sizeof(Backtrace));
                  bt->B = my_strdup(left.symbols.data[b]);
                  bt->C = my_strdup(right.symbols.data[c]);
                  bt->split_pos = k;
                  // Расширяем массив traces
                  table[i][len].traces = (Backtrace **)realloc(
                      table[i][len].traces,
                      table[i][len].symbols.size * sizeof(Backtrace *));
                  table[i][len].traces[table[i][len].symbols.size - 1] = bt;
                }
              }
            }
          }
        }
      }
    }
  }

  // Проверяем, содержится ли стартовый символ в table[0][n]
  bool result = contains(table[0][n].symbols, start_symbol);
  if (result)
  {
    build_derivation(0, n, start_symbol, table, word, derivation);
  }

  // Освобождаем память таблицы
  for (int i = 0; i < n; ++i)
  {
    for (int j = 0; j <= n; ++j)
    {
      for (int k = 0; k < table[i][j].symbols.size; ++k)
      {
        free(table[i][j].symbols.data[k]);
        if (table[i][j].traces && table[i][j].traces[k])
        {
          free(table[i][j].traces[k]->B);
          free(table[i][j].traces[k]->C);
          free(table[i][j].traces[k]);
        }
      }
      free(table[i][j].symbols.data);
      free(table[i][j].traces);
    }
    free(table[i]);
  }
  free(table);

  return result;
}