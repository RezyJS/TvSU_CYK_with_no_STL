#ifndef UTILS_H
#define UTILS_H

// Хеш-функция для строки (алгоритм djb2)
unsigned long hash_string(const char *str);

// Хеш-функция для пары строк
unsigned long hash_pair(const char *B, const char *C);

// Аналог strlen из стандартной библиотеки
int my_strlen(const char *s);

// Аналог strcmp
int my_strcmp(const char *a, const char *b);

// Аналог strcpy
void my_strcpy(char *dest, const char *src);

// Создает копию строки в динамической памяти
char *my_strdup(const char *s);

// Проверяет, является ли символ пробельным
bool my_isspace(char c);

// Удаляет пробелы из строки
void trim_spaces(char *str);

// Динамический массив строк
struct StringArray
{
  char **data;  // Указатель на массив строк
  int capacity; // Вместимость массива
  int size;     // Текущее количество элементов

  StringArray();                   // Конструктор
  ~StringArray();                  // Деструктор
  void push_back(const char *str); // Добавление элемента
};

// Разбивает строку на токены по пробелам
StringArray split(const char *str);

#endif // UTILS_H