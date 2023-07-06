#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define Nx 3840
#define Ny 2160
int a[Ny][Nx] = {0};
int tmp[Ny][Nx] = {0};
int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

void readPattern(char* filename) {
  FILE* file = fopen(filename, "r");
  char line[256];
  int x = -1, y = -1;

  int row, col;
  for (int i = 0; fgets(line, 256, file) != NULL; ++i) {
    if (i == 0) {
      continue;
    } else if (i == 1) {
      sscanf(line, "x = %d, y = %d", &x, &y);
      row = (Ny - y) / 2;
      col = (Nx - x) / 2;
    } else {
      // printf("%s", line);
      int len = strlen(line);
      int run_count = 1, tag = 0, cur = 0;
      while (cur < len) {
        if ('0' <= line[cur] && line[cur] <= '9') {
          sscanf(line + cur, "%d", &run_count);
          // printf("run count %d\n", run_count);
          while ((cur < len) && ('0' <= line[cur] && line[cur] <= '9')) {
            // printf("cur %d %c \n", cur, line[cur]);
            cur++;
          }
        } else if (line[cur] == 'b' || line[cur] == 'o') {
          tag = line[cur];
          for (int k = 0; k < run_count; ++k) {
            if (tag == 'o') a[row][col + k] = 1;
          }
          col += run_count;
          run_count = 1;
          cur++;
        } else if (line[cur] == '$') {
          for (int k = 0; k < run_count; ++k) {
            row++;
          }
          col = (Nx - x) / 2;
          run_count = 1;
          cur++;

        } else if (line[cur] == '\n') {
          cur++;
        } else if (line[cur] == '!') {
          return;
        }
      }
    }
  }
}

void runConwayLifeGame(int max_iter) {
  for (int iter = 0; iter < max_iter; ++iter) {
    printf("Iter %d...\n", iter);
    for (int i = 1; i < Ny - 1; ++i) {
      for (int j = 1; j < Nx - 1; ++j) {
        int cnt = 0;
        for (int k = 0; k < 8; ++k) {
          int ii = i + dy[k];
          int jj = j + dx[k];
          if (a[ii][jj] == 1) {
            cnt++;
          }
        }
        if (a[i][j] == 1) {
          if (cnt == 2 || cnt == 3) {
            tmp[i][j] = 1;
          } else {
            tmp[i][j] = 0;
          }
        } else {  // a[i][j] == 0
          if (cnt == 3) {
            tmp[i][j] = 1;
          } else {
            tmp[i][j] = 0;
          }
        }
      }
    }
    for (int i = 0; i < Ny; ++i) {
      for (int j = 0; j < Nx; ++j) {
        a[i][j] = tmp[i][j];
      }
    }
    #ifdef DEBUG
    char filename[10];
    sprintf(filename, "./output/iter%d", iter);
    FILE* f = fopen(filename, "w");
    for (int i = 0; i < Ny; ++i) {
      for (int j = 0; j < Nx; ++j) {
        fprintf(f, "%d ", tmp[i][j]);
      }
      fprintf(f, "\n");
    }
    fclose(f);
    #endif
  }

  FILE* f = fopen("test_output", "wb");
  #ifdef BINARY_OUT
  fwrite(a, Nx*Ny, sizeof(int), f);
  #else
  char* str = (char*)malloc((Nx*(Ny+1)+1)*sizeof(char));
  for (int i = 0; i < Ny; ++i) {
    int j;
    for (j = 0; j < Nx; ++j) {
      str[i*(Ny+1)+j] = a[i][j] + '0';
    }
    str[i*(Ny+1)+j] = '\n';
  }
  str[Nx*(Ny+1)] = 0;
  fwrite(str, Nx*(Ny+1), sizeof(char), f);
  free(str);
  #endif
  fclose(f);
}

int main(int argc, char** argv) {
  // for (int i = 0; i < N; ++i) {
    // for (int j = 0; j < N; ++j) {
      // if ((i - N/2) * (i - N/2) + (j - N/2) * (j - N/2) <= N/4 * N/4 && rand()%10 >4)
        // a[i][j] = 1;
    // }
  // }
  readPattern("test_pattern");

  struct timeval start;
  struct timeval end;
  gettimeofday (&start, NULL);

  runConwayLifeGame(1000);

  gettimeofday (&end, NULL);
  printf("%lf ms\n", ((end.tv_sec-start.tv_sec)*1000+(end.tv_usec-start.tv_usec)/1000.0));
}
