#ifndef LIFE_RUNNER_H
#define LIFE_RUNNER_H

#include <ncurses.h>
#include <chrono>
#include <csignal>
#include <string>
#include <thread>
#include "../include/game-of-life.h"

#define CMD_BUFFER_SIZE 0x100

class LifeRunner {
 public:
  LifeRunner(int row_length, int col_length);
  ~LifeRunner();

  void Run();

 private:
  void RunIOThread();
  void RunRenderThread();
  bool ProcessCommand();

  void PrintLife();
  void MoveCursor(int x, int y);
  void FlipCell(int x, int y);

  bool is_curse_enabled;
  bool is_edit_mode;
  bool do_run_;
  char cmd_buffer_[CMD_BUFFER_SIZE];
  char* cursor_;

  GameOfLife game_of_life_;
};

#endif
