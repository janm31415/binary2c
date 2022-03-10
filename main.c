#include <stdio.h>
#include <math.h>

static char arr[4] = { 0x23, 0x45, 0x11, 0x11 };

int ipow(int base, int exp)
  {
  int result = 1;
  for (;;)
    {
    if (exp & 1)
      result *= base;
    exp >>= 1;
    if (!exp)
      break;
    base *= base;
    }

  return result;
  }

int main(int argc, char** argv)
  {
  if (argc < 3)
    {
    printf("Usage: binary2c <input.bin> <output.c>\n");
    return -1;
    }
  FILE* in = fopen(argv[1], "rb");
  if (!in)
    {
    printf("Usage: binary2c <input.bin> <output.c>\n");
    printf("The input file %s is invalid\n", argv[1]);
    return -1;
    }
  FILE* out = fopen(argv[2], "wb");
  if (!out)
    {
    fclose(in);
    printf("Usage: binary2c <input.bin> <output.c>\n");
    printf("The output file %s cannot be opened\n", argv[2]);
    return -1;
    }

#define HEADER_SIZE 80
  unsigned char header[HEADER_SIZE];
  for (int i = 0; i < HEADER_SIZE; ++i)
    header[i] = ' ';
  header[HEADER_SIZE - 1] = '\n';
  fwrite((void*)header, 1, HEADER_SIZE, out);

  unsigned char hex[5];
  hex[0] = '0';
  hex[1] = 'x';
  hex[2] = 'z';
  hex[3] = 'z';
  hex[4] = ',';

  int count = 0;
  const int elements_per_line = 16;

  int end_of_file = feof(in);

  while (!end_of_file)
    {
    unsigned char ch;
    fread((void*)&ch, 1, 1, in);   
    end_of_file = feof(in);
    int h1 = ((int)ch >> 4) & 0x0f;
    if (h1 < 10)
      hex[2] = (unsigned char)((int)'0' + h1);
    else
      hex[2] = (unsigned char)((int)'A' + h1 - 10);
    int h2 = (int)ch & 0x0f;
    if (h2 < 10)
      hex[3] = (unsigned char)((int)'0' + h2);
    else
      hex[3] = (unsigned char)((int)'A' + h2 - 10);

    fwrite((void*)hex, 1, 5-end_of_file, out);
    ++count;
    if (count%elements_per_line == 0)
      {
      ch = '\n';
      fwrite(&ch, 1, 1, out);
      }
    }
  
  hex[0] = '}';
  hex[1] = ';';
  fwrite((void*)hex, 1, 2, out);

  fseek(out, 0, SEEK_SET);
  header[0] = 's';
  header[1] = 't';
  header[2] = 'a';
  header[3] = 't';
  header[4] = 'i';
  header[5] = 'c';
  header[6] = ' ';
  header[7] = 'c';
  header[8] = 'h';
  header[9] = 'a';
  header[10] = 'r';
  header[11] = ' ';
  header[12] = 'a';
  header[13] = 'r';
  header[14] = 'r';
  header[15] = '[';

  int next_index = 16;
  int first_nonzero_digit_encountered = 0;
  for (int j = 9; j >= 0; --j)
    {
    int base = ipow(10, j);
    int digit = count / base;
    count = count % base;
    if (digit)
      {
      first_nonzero_digit_encountered = 1;
      header[next_index++] = (unsigned char)(digit + (int)'0');
      }
    else if (first_nonzero_digit_encountered)
      {
      header[next_index++] = '0';
      }
    }

  
  header[next_index++] = ']';
  header[next_index++] = ' ';
  header[next_index++] = '=';
  header[next_index++] = ' ';
  header[next_index++] = '{';
  fwrite((void*)header, 1, HEADER_SIZE, out);

  fclose(in);
  fclose(out);
  return 0;
  }
