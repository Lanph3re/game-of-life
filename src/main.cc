#include <iostream>
#include "../include/life-runner.h"

int main(int argc, char** argv) {
  // TODO: command line argument
  int row_length = 55;
  int col_length = 55;

  LifeRunner life_runner(row_length, col_length);
  life_runner.Run();

  return 0;
}
