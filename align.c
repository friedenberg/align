#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>

const char FAILURE = -1;

errno_t strtrunc_s(char **, size_t, size_t, char);

static int opt_debug                = 0;
static int opt_remove_empty_lines   = 0;
static int opt_pad_length           = 0;
static char *opt_pad_string         = " ";

void ldebug(char *str) {
  if (opt_debug) {
    printf("%s\n", str);
  }
}

void prepare_options(int argc, char **argv) {
  static struct option long_options[] = {
    {"debug",               no_argument,        &opt_debug,              1},
    {"remove-empty-lines",  no_argument,        &opt_remove_empty_lines, 1},
    {"width",               required_argument,  NULL,                    'w'},
    {"pad-string",          required_argument,  NULL,                    'p'},
    {0, 0, 0, 0}
  };

  while (1) {
    int c;
    int option_index = 0;

    c = getopt_long(argc, argv, "", long_options, &option_index);

    if (c == -1) {
      break;
    }

    switch (c) {
      case 0:
        break;

      case 'w':
        {
          opt_pad_length = atoi(optarg);
        }
        break;

      case 'p':
        {
          opt_pad_string = optarg;
        }
        break;

      default:
        exit(1);
        break;
    }
  }
}

int main(int argc, char **argv) {
  prepare_options(argc, argv);

  size_t line_count = 0;

  while (1) {
    if (opt_debug) {
      printf("for line no %lu\n", line_count);
    }

    char *line = NULL;
    size_t len = 0;

    ldebug("reading");

    ssize_t read = getline(&line, &len, stdin);

    if (opt_debug) {
      printf("read length (with null): %lu\n", len);
      printf("read length (without null): %lu\n", read);
      printf("read str: %s\n", line);
    }

    if (opt_remove_empty_lines && read == 1) {
      ldebug("line was empty, skipping");

      free(line);
      line = NULL;
      continue;
    }

    errno_t opresult = strtrunc_s(&line, read, opt_pad_length, opt_pad_string[0]);

    if (line) {
      printf("%s\n", line);
      free(line);
    }

    if (read == -1) {
      ldebug("end of input");

      break;
    } else {
      line_count++;
    }
  }

  return 0;
}

errno_t strtrunc_s(char **str, size_t len, size_t trunc_len, char pad_char) {
  char last_char = (*str)[len - 1];

  if (opt_debug) {
    printf("beginning truncation for str: \nlength: %lu\ntrunc_len: %lu\nstr: %s", len, trunc_len, *str);
  }

  ldebug("checking for trailing newlines");
  //remove trailing new lines, if they exist
  if (last_char == '\n') {
    ldebug("removing trailing newline");

    len--;
    (*str)[len] = '\0';
  }

  ldebug("checking for trunc_length");

  if (trunc_len == 0) {
    return 0;
  }

  ldebug("reallocating");

  char *new_str = realloc(*str, trunc_len * sizeof(char));

  //we couldn't reallocate memory so bye girl bye
  if (new_str == NULL) {
    ldebug("unable to reallocate memory");
    return -1;
  }

  ldebug("comparing strings");

  if (new_str != *str) {
    free(*str);
    *str = new_str;
  }

  (*str)[trunc_len] = '\0';

  ldebug("checking for length less than trunc_length");

  if (len < trunc_len) {
    ldebug("length less than trunc_length, not modifying");

    for (size_t i = len; i < trunc_len; i++) {
      (*str)[i] = pad_char;
    }

    (*str)[trunc_len] = '\0';
  } else {
    ldebug("searching end for spaces");
    for (size_t i = trunc_len; i > 0; i--) {
      if (isspace((*str)[i])) {
        (*str)[i] = pad_char;
      } else {
        break;
      }
    }
  }

  return 0;
}
