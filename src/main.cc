#include <iostream>
#include <memory>
#include "../include/life-runner.h"

int main(int argc, char** argv) {
  // TODO: command line argument
  int row_length = 55;
  int col_length = 55;

  std::unique_ptr<LifeRunner> life_runner =
      std::make_unique<LifeRunner>(row_length, col_length);

  { life_runner->Run(); }

  return 0;
}
