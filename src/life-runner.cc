#include "../include/life-runner.h"

#define ESC 27
#define BACKSPACE 127
#define LEFT 'h'
#define DOWN 'j'
#define UP 'k'
#define RIGHT 'l'
#define X 'x'

#define LIFE_FIRST_ROW_OFFSET 2
#define LIFE_FIRST_COL_OFFSET 1
#define LIFE_ROW_HOP 1
#define LIFE_COL_HOP 3

#define CURSE_INIT(runner)                   \
  {                                          \
    signal(SIGINT, handler);                 \
    runner->is_curse_enabled = true;         \
    initscr();                               \
    cbreak();                                \
    noecho();                                \
    timeout(100);                            \
    runner->PrintLife();                     \
    RENDER_WITH_CMDLINE(runner, cmd_buffer_) \
  }

#define RENDER_WITH_CMDLINE(runner, buf) \
  {                                      \
    move(0, 0);                          \
    clrtoeol();                          \
    mvprintw(0, 0, ">> %s", (buf));      \
    refresh();                           \
  }

#define LOG(msg)                 \
  {                              \
    move(1, 0);                  \
    clrtoeol();                  \
    mvprintw(1, 0, "%s", (msg)); \
    refresh();                   \
  }

#define CLEAR_ERR() \
  {                 \
    move(1, 0);     \
    clrtoeol();     \
    refresh();      \
  }

void handler(int sig) {
  LOG("Type 'quit' to exit");
}

LifeRunner::LifeRunner(int row_length, int col_length)
    : is_curse_enabled(false),
      is_edit_mode(false),
      do_run_(false),
      cursor_(cmd_buffer_),
      game_of_life_(row_length, col_length) {
  memset(cmd_buffer_, 0, 256);
  game_of_life_.TestInit();
}

LifeRunner::~LifeRunner() {}

void LifeRunner::Run() {
  CURSE_INIT(this);
  std::thread render_thread([this]() { this->RunRenderThread(); });
  std::thread io_thread([this]() { this->RunIOThread(); });
  render_thread.detach();
  io_thread.join();
}

void LifeRunner::RunIOThread() {
  if (!is_curse_enabled)
    return;

  char ch;
  for (;;) {
    if ((ch = getch()) != ERR) {
      if (is_edit_mode) {
        int x, y;
        switch (ch) {
          case ESC:
            is_edit_mode = false;
            RENDER_WITH_CMDLINE(this, cmd_buffer_);
            continue;
          case LEFT:
            getyx(curscr, x, y);
            MoveCursor(x, y - LIFE_COL_HOP);
            break;
          case DOWN:
            getyx(curscr, x, y);
            MoveCursor(x + LIFE_ROW_HOP, y);
            break;
          case UP:
            getyx(curscr, x, y);
            MoveCursor(x - LIFE_ROW_HOP, y);
            break;
          case RIGHT:
            getyx(curscr, x, y);
            MoveCursor(x, y + LIFE_COL_HOP);
            break;
          case X:
            getyx(curscr, x, y);
            FlipCell(x, y);
            PrintLife();
            move(x, y);
            break;
        }
      } else {
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

        RENDER_WITH_CMDLINE(this, cmd_buffer_);
        if (is_edit_mode)
          move(LIFE_FIRST_ROW_OFFSET, LIFE_FIRST_COL_OFFSET);
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

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

bool LifeRunner::ProcessCommand() {
  std::string cmd(cmd_buffer_);

  if (cmd.empty()) {
    CLEAR_ERR();
    return true;
  }

  if (!cmd.compare("quit")) {
    endwin();
    return false;
  }

  if (!cmd.compare("start")) {
    if (!do_run_) {
      do_run_ = true;
      CLEAR_ERR();
    } else {
      LOG("Life is running");
    }

    return true;
  }

  if (!cmd.compare("stop")) {
    if (do_run_) {
      do_run_ = false;
      CLEAR_ERR();
    } else {
      LOG("Life is not running");
    }

    return true;
  }

  if (!cmd.compare("edit")) {
    if (do_run_) {
      LOG("Life is running");
    } else {
      is_edit_mode = true;
      CLEAR_ERR();
    }

    return true;
  }

  // TODO: create new life
  // TODO: edit the current life
  // TODO: load existing life
  // TODO: dump the current life
  LOG("Undefined command");

  return true;
}

void LifeRunner::PrintLife() {
  if (!is_curse_enabled)
    return;

  for (int row_idx = 0; row_idx < game_of_life_.GetRowLength(); ++row_idx) {
    for (int col_idx = 0; col_idx < game_of_life_.GetColLength(); ++col_idx) {
      mvprintw(row_idx + LIFE_FIRST_ROW_OFFSET, col_idx * 3,
               game_of_life_.IsAlive(row_idx, col_idx) ? " O " : " . ");
    }
  }
  refresh();
}

void LifeRunner::MoveCursor(int x, int y) {
  if (x < LIFE_FIRST_ROW_OFFSET)
    x += game_of_life_.GetRowLength();

  if (x >= LIFE_FIRST_ROW_OFFSET + game_of_life_.GetRowLength())
    x = LIFE_FIRST_ROW_OFFSET;

  if (y < 0)
    y += game_of_life_.GetColLength() * LIFE_COL_HOP;

  if (y >= game_of_life_.GetColLength() * LIFE_COL_HOP)
    y = LIFE_FIRST_COL_OFFSET;

  move(x, y);
}

void LifeRunner::FlipCell(int x, int y) {
  x -= LIFE_FIRST_ROW_OFFSET;
  y /= LIFE_COL_HOP;
  game_of_life_.FlipCell(x, y);
}
