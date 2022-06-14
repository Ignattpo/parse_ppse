#include "parse.h"

#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

struct parser_t
{
  const char *s_package;
  u_int8_t *i_package;
  struct candidates_list candidates;
};

size_t hex_to_num(const char *s)
{
  char c;
  u_int8_t n = 0;

  errno = 0;
  for (int i = 0; i < 2; i++, s++)
  {
    if (*s >= '0' && *s <= '9')
    {
      n = (n << 4) | (size_t)(*s - '0');
    }
    else
    {
      c = *s & 0xDF;
      if (c >= 'A' && c <= 'F')
      {
        n = (n << 4) | (size_t)(c - 'A' + 10);
      }
      else
      {
        goto err;
      }
    }
  }
  return n;
err:
  errno = EINVAL;
  return 0;
}

static int init_i_package(struct parser_t *parser)
{
  size_t len_package = strlen(parser->s_package);
  u_int8_t *tmp = malloc(len_package * sizeof(u_int8_t));
  if (!tmp)
  {
    return -1;
  }

  parser->i_package = tmp;

  return 0;
}

static int convert_str_to_int(struct parser_t *parser)
{
  size_t len_package = strlen(parser->s_package);
  for (int i = 0; i < len_package / 2; i++)
  {
    parser->i_package[i] = hex_to_num(&parser->s_package[i * 2]);
  }
}

static int main_parse_arguments(int argc, char *argv[], struct parser_t *parser)
{
  static struct option long_options[] = {
      // аргументы
      {"ppsl", required_argument, 0, 'p'}, // package
      {0, 0, 0, 0}                         // конец аргументов
  };

  int c = -1;
  do
  {
    int option_index = 0;
    /* Получаем следующую опцию... */
    c = getopt_long(argc, argv, "p:", long_options, &option_index);
    if (c == -1)
    {
      break;
    }

    switch (c)
    {
    case 'p':
      if (!optarg)
      {
        break;
      }
      parser->s_package = strdup(optarg);
      if (parser->s_package == NULL)
      {
        goto aborting_strdup;
      }
      break;
    }
  } while (c != -1);

  return 0;

aborting_strdup:
  return -1;
}

int main(int argc, char *argv[])
{
  struct parser_t parser = {.s_package = NULL, .i_package = NULL};
  main_parse_arguments(argc, argv, &parser);
  init_i_package(&parser);
  convert_str_to_int(&parser);
  candidates_list_init(&parser.candidates);

  parse_package(parser.i_package, &parser.candidates);
  candidates_list_print(&parser.candidates);
  candidates_list_free(&parser.candidates);

  return 0;
}
