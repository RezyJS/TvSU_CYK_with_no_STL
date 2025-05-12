#include "grammar.h"
#include "utils.h"
#include <cstdlib>
#include <cstdio>

TermRulesTable term_rules;       // Глобальный экземпляр таблицы терминальных правил
NonTermRulesTable nonterm_rules; // Глобальный экземпляр таблицы нетерминальных правил
char start_symbol[32] = {0};     // Стартовый символ

// Конструктор TermRulesTable
TermRulesTable::TermRulesTable(int size) : bucket_count(size)
{
  buckets = (TermRule **)malloc(size * sizeof(TermRule *));
  for (int i = 0; i < size; ++i)
  {
    buckets[i] = nullptr;
  }
}

// Деструктор TermRulesTable
TermRulesTable::~TermRulesTable()
{
  for (int i = 0; i < bucket_count; ++i)
  {
    TermRule *curr = buckets[i];
    while (curr)
    {
      TermRule *next = curr->next;
      free(curr->key);
      free(curr);
      curr = next;
    }
  }
  free(buckets);
}

// Вставка терминального правила (key → value)
void TermRulesTable::insert(const char *key, const char *value)
{
  unsigned long h = hash_string(key) % bucket_count;
  TermRule *curr = buckets[h];
  // Поиск существующего узла с таким же ключом
  while (curr)
  {
    if (my_strcmp(curr->key, key) == 0)
    {
      curr->values.push_back(value); // Добавляем значение в существующий узел
      return;
    }
    curr = curr->next;
  }
  // Создаем новый узел
  TermRule *new_node = (TermRule *)malloc(sizeof(TermRule));
  new_node->key = my_strdup(key);
  new_node->values = StringArray();
  new_node->values.push_back(value);
  new_node->next = buckets[h];
  buckets[h] = new_node;
}

// Получение списка нетерминалов по терминалу
StringArray *TermRulesTable::get(const char *key)
{
  unsigned long h = hash_string(key) % bucket_count;
  TermRule *curr = buckets[h];
  while (curr)
  {
    if (my_strcmp(curr->key, key) == 0)
    {
      return &curr->values;
    }
    curr = curr->next;
  }
  return nullptr;
}

// Конструктор NonTermRulesTable (аналогично TermRulesTable)
NonTermRulesTable::NonTermRulesTable(int size) : bucket_count(size)
{
  buckets = (NonTermRule **)malloc(size * sizeof(NonTermRule *));
  for (int i = 0; i < size; ++i)
  {
    buckets[i] = nullptr;
  }
}

// Деструктор NonTermRulesTable
NonTermRulesTable::~NonTermRulesTable()
{
  for (int i = 0; i < bucket_count; ++i)
  {
    NonTermRule *curr = buckets[i];
    while (curr)
    {
      NonTermRule *next = curr->next;
      free(curr->B);
      free(curr->C);
      free(curr);
      curr = next;
    }
  }
  free(buckets);
}

// Вставка нетерминального правила (B, C → value)
void NonTermRulesTable::insert(const char *B, const char *C, const char *value)
{
  unsigned long h = hash_pair(B, C) % bucket_count;
  NonTermRule *curr = buckets[h];
  // поиск существующего узла
  while (curr)
  {
    if (my_strcmp(curr->B, B) == 0 && my_strcmp(curr->C, C) == 0)
    {
      // Проверяем, не добавляем ли мы дубликат
      bool exists = false;
      for (int i = 0; i < curr->values.size; i++)
      {
        if (my_strcmp(curr->values.data[i], value) == 0)
        {
          exists = true;
          break;
        }
      }
      if (!exists)
      {
        curr->values.push_back(value);
      }
      return;
    }
    curr = curr->next;
  }
  // Создаем новый узел
  NonTermRule *new_node = (NonTermRule *)malloc(sizeof(NonTermRule));
  new_node->B = my_strdup(B);
  new_node->C = my_strdup(C);
  new_node->values = StringArray();
  new_node->values.push_back(value);
  new_node->next = buckets[h];
  buckets[h] = new_node;
}

// Получение списка нетерминалов по паре (B, C)
StringArray *NonTermRulesTable::get(const char *B, const char *C)
{
  unsigned long h = hash_pair(B, C) % bucket_count;
  NonTermRule *curr = buckets[h];
  while (curr)
  {
    if (my_strcmp(curr->B, B) == 0 && my_strcmp(curr->C, C) == 0)
    {
      return &curr->values;
    }
    curr = curr->next;
  }
  return nullptr;
}

// Чтение грамматики из файла
bool read_grammar(const char *filename)
{
  FILE *file = fopen(filename, "r");
  if (!file)
    return false;

  char line[256];
  bool first = true; // Для определения стартового символа (первое правило)
  while (fgets(line, sizeof(line), file))
  {
    int pos = 0;
    // Ищем разделитель "->"
    while (line[pos] && !(line[pos] == '-' && line[pos + 1] == '>'))
    {
      ++pos;
    }
    if (!line[pos])
      continue; // Пропускаем некорректные строки

    line[pos] = '\0'; // Разделяем строку на левую и правую части
    char *left = line;
    char *right = line + pos + 2; // Пропускаем "->"

    trim_spaces(left);              // Удаляем пробелы вокруг левой части
    StringArray rhs = split(right); // Разбиваем правую часть на токены

    if (rhs.size == 1)
    {
      // Терминальное правило: A -> a
      term_rules.insert(rhs.data[0], left);
    }
    else if (rhs.size == 2)
    {
      // Нетерминальное правило: A -> B C
      nonterm_rules.insert(rhs.data[0], rhs.data[1], left);
    }

    // Первое правило определяет стартовый символ
    if (first)
    {
      my_strcpy(start_symbol, left);
      first = false;
    }
  }

  fclose(file);
  return true;
}