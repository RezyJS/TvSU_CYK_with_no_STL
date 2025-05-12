#include "utils.h"
#include <cstdlib>

// Хеширование строки (djb2)
unsigned long hash_string(const char *str)
{
  unsigned long h = 5381;
  while (*str)
  {
    h = ((h << 5) + h) + (unsigned char)(*str); // h * 33 + c
    ++str;
  }
  return h;
}

// Хеширование пары строк
unsigned long hash_pair(const char *B, const char *C)
{
  unsigned long h = 5381;
  while (*B)
  {
    h = ((h << 5) + h) + (unsigned char)(*B);
    ++B;
  }
  while (*C)
  {
    h = ((h << 5) + h) + (unsigned char)(*C);
    ++C;
  }
  return h;
}

// Длина строки
int my_strlen(const char *s)
{
  int len = 0;
  while (s[len])
    ++len;
  return len;
}

// Сравнение строк
int my_strcmp(const char *a, const char *b)
{
  while (*a && (*a == *b))
  {
    ++a;
    ++b;
  }
  return (unsigned char)*a - (unsigned char)*b;
}

// Копирование строки
void my_strcpy(char *dest, const char *src)
{
  // *src копируется в *dest
  // и мы получаем следующее значение
  // Если скопировали \0, то скопируем
  // а условие станет ложным и мы закончим
  while ((*dest++ = *src++))
    ;
}

// Копирование строки в динамическую память
char *my_strdup(const char *s)
{
  int len = my_strlen(s);
  char *p = (char *)malloc(len + 1);
  if (!p)
    return nullptr;
  my_strcpy(p, s);
  return p;
}

// Проверка на пробел
bool my_isspace(char c)
{
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

// Удаление пробелов из строки
void trim_spaces(char *str)
{
  char *dst = str;
  while (*str)
  {
    if (!my_isspace(*str))
    {
      *dst++ = *str;
    }
    ++str;
  }
  *dst = '\0';
}

// Конструктор StringArray
StringArray::StringArray() : data(nullptr), capacity(0), size(0) {}

// Деструктор StringArray
StringArray::~StringArray()
{
  for (int i = 0; i < size; ++i)
  {
    free(data[i]); // Освобождаем каждую строку
  }
  free(data); // Освобождаем массив указателей
}

// Добавление элемента в массив
void StringArray::push_back(const char *str)
{
  if (size >= capacity)
  {
    capacity = (capacity == 0) ? 4 : capacity * 2; // Увеличиваем вместимость
    char **new_data = (char **)malloc(capacity * sizeof(char *));
    for (int i = 0; i < size; ++i)
    {
      new_data[i] = data[i]; // Копируем старые указатели
    }
    free(data);
    data = new_data;
  }
  data[size++] = my_strdup(str); // Копируем строку и сохраняем указатель
}

// Разбиение строки на токены
StringArray split(const char *str)
{
  StringArray tokens;
  const char *start = str; // Начало текущего токена
  int pos = 0;
  while (true)
  {
    char c = str[pos];
    if (c == '\0' || my_isspace(c))
    {
      if (pos > 0 && !my_isspace(str[pos - 1]))
      { // Если токен не пуст
        int len = pos - (start - str);
        char *token = (char *)malloc(len + 1);
        for (int i = 0; i < len; ++i)
        {
          token[i] = start[i];
        }
        token[len] = '\0';
        tokens.push_back(token);
        free(token);
      }
      if (c == '\0')
        break;
      start = str + pos + 1; // Новый токен начинается после пробела
    }
    ++pos;
  }
  return tokens;
}