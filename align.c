#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

void remove_trailing_newline(char **);
char add_trailing_newline(char **);

const char FAILURE = -1;

enum trim_result {
  TRIM_RESULT_FAILED = FAILURE,
  TRIM_RESULT_SAME,
  TRIM_RESULT_TRUNC,
};

typedef enum trim_result TRIM_RESULT;

TRIM_RESULT trim_line(char **, size_t, size_t);

enum pad_result {
  PAD_RESULT_FAILED = FAILURE,
  PAD_RESULT_SAME,
  PAD_RESULT_PADDED,
};

typedef enum pad_result PAD_RESULT;

PAD_RESULT pad_line(char **, size_t, char *, char *, size_t);

static int opt_debug                = 0;
static int opt_remove_empty_lines   = 0;
static int opt_pad_length           = 0;
static char *opt_pad_string         = " ";

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
    }
  }
}

int main(int argc, char **argv) {
  prepare_options(argc, argv);

  size_t line_count = 0;

  while (1) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read = getline(&line, &len, stdin);

    if (opt_remove_empty_lines && read == 1) {
      free(line);
      continue;
    }

    if (opt_pad_length > 0) {
      remove_trailing_newline(&line);

      PAD_RESULT pad_result = pad_line(&line, len, NULL, opt_pad_string, opt_pad_length);
      add_trailing_newline(&line);
    }

    if (line) {
      printf("%s", line);
      free(line);
    }

    if (opt_debug) {
      printf("len: %lu\n", len);
      printf("read: %zd\n", read);
    }


    if (read == -1) {
      if (opt_debug) {
        printf("end of input");
      }

      break;
    } else {
      line_count++;
    }
  }

  return 0;
}

PAD_RESULT pad_line(
    char **line,
    size_t length,
    char *delimiter,
    char *replacement,
    size_t pad_length
    ) {
  char trim_successs = trim_line(line, length, pad_length);

  switch (trim_successs) {
    case TRIM_RESULT_SAME:
      {
        size_t new_size = strlen(*line) + strlen(replacement);
        char *new_buffer = realloc(*line, new_size);

        if (new_buffer) {
          if (new_buffer != *line) {
            free(*line);
            *line = new_buffer;
          }

          strcat(*line, replacement);
          return PAD_RESULT_PADDED;
        } else {
          return PAD_RESULT_FAILED;
        }
      }
      break;

    case TRIM_RESULT_TRUNC:
      return PAD_RESULT_SAME;

    default:
      return PAD_RESULT_FAILED;
  }
}

TRIM_RESULT trim_line(char **line, size_t current_length, size_t trimmed_length) {
  if (current_length <= trimmed_length) {
    return TRIM_RESULT_SAME;
  }

  char *new_line = realloc(*line, trimmed_length + sizeof(char));

  if (new_line) {
    if (new_line != *line) {
      free(*line);
      *line = new_line;
    }

    new_line[trimmed_length] = '\0';
    return TRIM_RESULT_TRUNC;
  } else {
    return TRIM_RESULT_FAILED;
  }
}

void remove_trailing_newline(char **str) {
  if (!*str) {
    return;
  }

  size_t len = strlen(*str);
  if (len > 0 && *str[len - 1] == '\n') {
    *str[--len] = '\0';
  }
}

char add_trailing_newline(char **str) {
  if (!*str) {
    return 0;
  }

  size_t len = strlen(*str);

  if (len > 0) {
    char *new_line = "\n";
    char *new_buf = realloc(*str, len + strlen(new_line));

    if (new_buf) {
      if (new_buf != *str) {
        free(*str);
        *str = new_buf;
      }

      strcat(*str, new_line);
      return 0;
    } else {
      return FAILURE;
    }
  }

  return 0;
}
