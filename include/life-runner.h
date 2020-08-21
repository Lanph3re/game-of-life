#ifndef LIFE_RUNNER_H
#define LIFE_RUNNER_H

#include <curses.h>
#include <chrono>
#include <string>
#include <thread>
#include "../include/game-of-life.h"

#define CMD_BUFFER_SIZE 0x100

class LifeRunner {
 public:
  LifeRunner(int row_length, int col_length);
  ~LifeRunner();

  void Run();
  void PrintLife();

 private:
  void RunIOThread();
  void RunRenderThread();
  bool ProcessCommand();

  bool is_curse_enabled;
  bool do_run_;
  std::unique_ptr<GameOfLife> game_of_life_;
  char cmd_buffer_[CMD_BUFFER_SIZE];
  char* cursor_;
};

#endif
