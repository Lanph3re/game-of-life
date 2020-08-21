#include "../include/life-runner.h"

#define BACKSPACE 127

#define CURSE_INIT()             \
  {                              \
    initscr();                   \
    cbreak();                    \
    noecho();                    \
    timeout(100);                \
    game_of_life_->PrintBoard(); \
    refresh();                   \
  }

#define RENDER_WITH_CMDLINE(game_of_life, buf) \
  {                                            \
    move(0, 0);                                \
    clrtoeol();                                \
    mvprintw(0, 0, ">> %s", (buf));            \
    refresh();                                 \
  }

LifeRunner::LifeRunner(int row_length, int col_length)
    : do_run_(false), cursor_(cmd_buffer_) {
  game_of_life_.reset(std::move(new GameOfLife(row_length, col_length)));
  memset(cmd_buffer_, 0, 256);
  game_of_life_->TestInit();
}

LifeRunner::~LifeRunner() {}

void LifeRunner::Run() {
  CURSE_INIT();
  std::thread render_thread([this]() { this->RunRenderThread(); });
  std::thread io_thread([this]() { this->RunIOThread(); });
  render_thread.detach();
  io_thread.join();
}

void LifeRunner::RunIOThread() {
  char ch;
  for (;;) {
    if ((ch = getch()) != ERR) {
      if (ch == '\n') {
        if (!ProcessCommand())
          return;

        cursor_ = cmd_buffer_;
        *cursor_ = 0;
      } else if (ch == BACKSPACE) {
        if (cursor_ > cmd_buffer_)
          *--cursor_ = 0;
      } else if (cursor_ - cmd_buffer_ < sizeof(cmd_buffer_) - 1) {
        *cursor_++ = ch;
        *cursor_ = 0;
      }
    }
  }
}

void LifeRunner::RunRenderThread() {
  for (;;) {
    if (do_run_) {
      game_of_life_->PrintBoard();
      game_of_life_->NextGeneration();
    }

    RENDER_WITH_CMDLINE(game_of_life_, cmd_buffer_);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

bool LifeRunner::ProcessCommand() {
  std::string cmd(cmd_buffer_);
  if (!cmd.compare("quit")) {
    endwin();
    return false;
  }

  if (!cmd.compare("start")) {
    if (!do_run_)
      do_run_ = true;
    return true;
  }

  if (!cmd.compare("stop")) {
    if (do_run_)
      do_run_ = false;
    return true;
  }

  // TODO: create new life
  if (!cmd.compare("create")) {
    return true;
  }

  // TODO: edit the current life
  if (!cmd.compare("edit")) {
    return true;
  }

  // TODO: load existing life
  if (!cmd.compare("load")) {
    return true;
  }

  // TODO: dump the current life
  if (!cmd.compare("dump")) {
    return true;
  }

  return true;
}
