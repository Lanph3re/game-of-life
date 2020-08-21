#include "../include/game-of-life.h"

GameOfLife::GameOfLife(int row_length, int col_length)
    : row_length_(row_length), col_length_(col_length), board_(row_length) {
  for (auto row = board_.begin(); row != board_.end(); ++row)
    row->resize(col_length_, false);
}

GameOfLife::~GameOfLife() {}

void GameOfLife::RandomInit(void) {
  static bool seed = false;
  if (!seed) {
    seed = true;
    srand(time(0));
  }

  for (int row_idx = 0; row_idx < GetRowLength(); ++row_idx) {
    for (int col_idx = 0; col_idx < GetColLength(); ++col_idx)
      board_[row_idx][col_idx] = false;
  }

  for (int i = 0; i < 300; ++i)
    FlipCell(rand() % GetRowLength(), rand() % GetColLength());
}

int GameOfLife::GetRowLength() {
  return row_length_;
}

int GameOfLife::GetColLength() {
  return col_length_;
}

void GameOfLife::NextGeneration() {
  inspect_.clear();
  pending_.clear();

  for (const auto live : live_) {
    inspect_.insert(std::make_pair(live.first, live.second));
    for (int neighbor_index : kNeighborIndices)
      inspect_.insert(GetNeighbor(live, neighbor_index));
  }

  for (auto inspect : inspect_) {
    if (IsSurvived(inspect))
      continue;

    if (IsUnderpopulated(inspect) || IsOverpopulated(inspect) ||
        IsRevived(inspect)) {
      pending_.insert(inspect);
    }
  }

  // Apply all pending changes to board
  for (auto pending : pending_)
    FlipCell(pending);
}

// Modular function
int GameOfLife::GetSafeIndex(int i, int length) {
  return i % length >= 0 ? (i % length) : (i + length);
}

void GameOfLife::FlipCell(const Cell& cell) {
  FlipCell(cell.first, cell.second);
}

void GameOfLife::FlipCell(int row_idx, int col_idx) {
  if (!IsAlive(row_idx, col_idx)) {
    live_.insert(std::make_pair(row_idx, col_idx));
  } else {
    live_.erase(std::make_pair(row_idx, col_idx));
  }
  board_[row_idx][col_idx] = !board_[row_idx][col_idx];
}

bool GameOfLife::IsAlive(int row_idx, int col_idx) {
  return board_[row_idx][col_idx];
}

using Cell = std::pair<int, int>;

bool GameOfLife::IsAlive(const Cell& cell) {
  return board_[cell.first][cell.second];
}

Cell GameOfLife::GetNeighbor(const Cell& cell, int i) {
  return std::make_pair(
      GetSafeIndex(cell.first + kNeighborDiffs[i][0], row_length_),
      GetSafeIndex(cell.second + kNeighborDiffs[i][1], col_length_));
}

size_t GameOfLife::GetLiveNeighbors(const Cell& cell) {
  return std::count_if(kNeighborIndices, kNeighborIndices + kNumNeighbors,
                       [this, &cell](int neighbor_index) {
                         return IsAlive(GetNeighbor(cell, neighbor_index));
                       });
}

bool GameOfLife::IsUnderpopulated(const Cell& cell) {
  return IsAlive(cell) && GetLiveNeighbors(cell) < kUnderpopulation;
}

bool GameOfLife::IsSurvived(const Cell& cell) {
  return IsAlive(cell) && GetLiveNeighbors(cell) >= kUnderpopulation &&
         GetLiveNeighbors(cell) <= kSurvival;
}

bool GameOfLife::IsOverpopulated(const Cell& cell) {
  return IsAlive(cell) && GetLiveNeighbors(cell) > kOverpopulation;
}

bool GameOfLife::IsRevived(const Cell& cell) {
  return !IsAlive(cell) && GetLiveNeighbors(cell) == kRevival;
}
