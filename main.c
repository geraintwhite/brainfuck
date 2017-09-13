#include <stdio.h>

#define FAILURE          1
#define SUCCESS          0

#define PROGRAM_SIZE     4096
#define STACK_SIZE       512
#define DATA_SIZE        65535

#define STACK_PUSH(item) (stack[stack_pointer++] = item)
#define STACK_POP()      (stack[--stack_pointer])
#define STACK_EMPTY()    (stack_pointer == 0)
#define STACK_FULL()     (stack_pointer == STACK_SIZE)

struct instruction {
  int operator;
  int operand;
};

static struct instruction program[PROGRAM_SIZE];
static int stack[STACK_SIZE];
static int stack_pointer = 0;


int
compile(FILE *fp)
{
  int program_counter = 0;
  int jump_to;
  char c;

  while ((c = getc(fp)) != EOF && program_counter < PROGRAM_SIZE)
  {
    switch (c)
    {
      case '>':
      case '<':
      case '+':
      case '-':
      case '.':
      case ',':
        program[program_counter].operator = c;
        break;
      case '[':
        if (STACK_FULL()) return FAILURE;
        program[program_counter].operator = c;
        STACK_PUSH(program_counter);
        break;
      case ']':
        if (STACK_EMPTY()) return FAILURE;
        jump_to = STACK_POP();
        program[program_counter].operator = c;
        program[program_counter].operand = jump_to;
        program[jump_to].operand = program_counter;
        break;
      default:
        --program_counter;
        break;
    }
    ++program_counter;
  }

  if (!STACK_EMPTY() || program_counter == PROGRAM_SIZE)
  {
    return FAILURE;
  }

  program[program_counter].operator = -1;

  return SUCCESS;
}

int
execute()
{
  int program_counter = 0;
  int data[DATA_SIZE] = {0};
  int *ptr = data;

  while (program[program_counter].operator != -1 && ptr)
  {
    switch (program[program_counter].operator)
    {
      case '>': ++ptr; break;
      case '<': --ptr; break;
      case '+': ++*ptr; break;
      case '-': --*ptr; break;
      case '.': putchar(*ptr); break;
      case ',': *ptr = getchar(); break;
      case '[':
        if (!*ptr) program_counter = program[program_counter].operand;
        break;
      case ']':
        if (*ptr) program_counter = program[program_counter].operand;
        break;
      default: return FAILURE;
    }
    ++program_counter;
  }

  return ptr ? SUCCESS : FAILURE;
}

int
main(int argc, char * argv[])
{
  int result;
  FILE *fp;

  if (argc != 2 || (fp = fopen(argv[1], "r")) == NULL)
  {
    fprintf(stderr, "Usage: %s filename\n", argv[0]);
    return FAILURE;
  }

  result = compile(fp);
  fclose(fp);

  if (result == SUCCESS)
  {
    result = execute();
  }

  return result;
}
