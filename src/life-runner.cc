#include "../include/life-runner.h"

#define BACKSPACE 127

#define CURSE_INIT(runner)           \
  {                                  \
    runner->is_curse_enabled = true; \
    initscr();                       \
    cbreak();                        \
    noecho();                        \
    timeout(100);                    \
    runner->PrintLife();             \
  }

#define RENDER_WITH_CMDLINE(runner, buf) \
  {                                      \
    if (!runner->is_curse_enabled)       \
      return;                            \
                                         \
    move(0, 0);                          \
    clrtoeol();                          \
    mvprintw(0, 0, ">> %s", (buf));      \
    refresh();                           \
  }

LifeRunner::LifeRunner(int row_length, int col_length)
    : is_curse_enabled(false),
      do_run_(false),
      cursor_(cmd_buffer_),
      game_of_life_(row_length, col_length) {
  memset(cmd_buffer_, 0, 256);
  game_of_life_.TestInit();
}

LifeRunner::~LifeRunner() {}

void LifeRunner::PrintLife() {
  if (!is_curse_enabled)
    return;

#define LIFE_FIRST_ROW_OFFSET 2
  for (int row_idx = 0; row_idx < game_of_life_.row_length_; ++row_idx) {
    for (int col_idx = 0; col_idx < game_of_life_.col_length_; ++col_idx) {
      mvprintw(row_idx + LIFE_FIRST_ROW_OFFSET, col_idx * 3,
               game_of_life_.IsAlive(row_idx, col_idx) ? " O " : " . ");
    }
  }
  refresh();
}

void LifeRunner::Run() {
  CURSE_INIT(this);
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
      game_of_life_.NextGeneration();
      PrintLife();
    }

    RENDER_WITH_CMDLINE(this, cmd_buffer_);
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
