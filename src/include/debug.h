#include <stdio.h>
#include <stdlib.h>

// Define the log macro
#define LOG(namespace, message, ...) \
  do { \
    const char* env_var = getenv("SGI_" #namespace); \
    const char* env_var_all = getenv("SGI_ALL"); \
    if ((env_var != NULL && strcmp(env_var, "1") == 0) || (env_var_all != NULL && strcmp(env_var_all, "1") == 0)) { \
      fprintf(stderr, "[\033[32m" #namespace "\033[0m] " message "\n", ##__VA_ARGS__); \
    } \
  } while (0)

#define ERROR(namespace, message, ...) \
  do { \
    const char* env_var = getenv("SGI_" #namespace); \
    const char* env_var_all = getenv("SGI_ALL"); \
    if ((env_var != NULL && strcmp(env_var, "1") == 0) || (env_var_all != NULL && strcmp(env_var_all, "1") == 0)) { \
      fprintf(stderr, "[\033[31m" #namespace "\033[0m] " message "\n", ##__VA_ARGS__); \
    } \
  } while (0)

#ifndef NDEBUG

// Assert macro definition
#define ASSERT(condition, message, ...) \
  do { \
    const char* env_var = getenv("SGI_DISABLE_ASSERTS"); \
    if (env_var == NULL || strcmp(env_var, "0") == 0) { \
      if (!(condition)) { \
        fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", #condition, __FILE__, __LINE__); \
        fprintf(stderr, message "\n", ##__VA_ARGS__); \
        abort(); \
      } \
    } \
  } while (0)

#else
#define ASSERT(condition, message, ...) do { } while (0)
#endif

