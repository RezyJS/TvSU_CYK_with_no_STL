#ifndef GRAMMAR_H
#define GRAMMAR_H

#include "utils.h"

// Узел хеш-таблицы для терминальных правил (A → a)
struct TermRule
{
  char *key;          // Терминал (a)
  StringArray values; // Список нетерминалов (A)
  TermRule *next;     // Указатель на следующий узел в цепочке
};

// Хеш-таблица для терминальных правил
struct TermRulesTable
{
  TermRule **buckets; // Массив корзин
  int bucket_count;   // Количество корзин

  TermRulesTable(int size = 16);                   // Конструктор
  ~TermRulesTable();                               // Деструктор
  void insert(const char *key, const char *value); // Добавление правила
  StringArray *get(const char *key);               // Получение списка нетерминалов
};

// Узел хеш-таблицы для нетерминальных правил (A → B C)
struct NonTermRule
{
  char *B;            // Левый нетерминал
  char *C;            // Правый нетерминал
  StringArray values; // Список нетерминалов (A)
  NonTermRule *next;  // Указатель на следующий узел
};

// Хеш-таблица для нетерминальных правил
struct NonTermRulesTable
{
  NonTermRule **buckets;
  int bucket_count;

  NonTermRulesTable(int size = 16);
  ~NonTermRulesTable();
  void insert(const char *B, const char *C, const char *value);
  StringArray *get(const char *B, const char *C);
};

extern TermRulesTable term_rules;       // Глобальная таблица терминальных правил
extern NonTermRulesTable nonterm_rules; // Глобальная таблица нетерминальных правил
extern char start_symbol[32];           // Стартовый символ грамматики

// Читает грамматику из файла и заполняет таблицы
bool read_grammar(const char *filename);

#endif // GRAMMAR_H