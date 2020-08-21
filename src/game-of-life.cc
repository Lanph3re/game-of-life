#include "../include/game-of-life.h"

GameOfLife::GameOfLife(int row_length, int col_length)
    : row_length_(row_length), col_length_(col_length) {
  board_.reset(std::move(new std::vector<std::vector<bool> >(row_length_)));
  for (auto row = board_->begin(); row != board_->end(); ++row)
    row->resize(col_length_, false);
}

GameOfLife::~GameOfLife() {}

int GameOfLife::GetRowLength() {
  return row_length_;
}

int GameOfLife::GetColLength() {
  return col_length_;
}

void GameOfLife::PrintBoard() {
#define LIFE_FIRST_ROW_OFFSET 2
  for (int row_idx = 0; row_idx < row_length_; ++row_idx) {
    for (int col_idx = 0; col_idx < col_length_; ++col_idx) {
      mvprintw(row_idx + LIFE_FIRST_ROW_OFFSET, col_idx * 3,
               board_->at(row_idx).at(col_idx) ? " O " : " . ");
    }
  }
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

    if (IsUnderpopulated(inspect) || IsOverpopulated(inspect)) {
      live_.erase(inspect);
      pending_.insert(inspect);
    }

    if (IsRevived(inspect)) {
      live_.insert(inspect);
      pending_.insert(inspect);
    }
  }

  // Apply all pending changes to board
  for (auto pending : pending_) {
    board_->at(pending.first).at(pending.second) =
        !board_->at(pending.first).at(pending.second);
  }
}

void GameOfLife::TestInit() {
  (*board_)[3][3] = true;
  (*board_)[3][4] = true;
  (*board_)[3][5] = true;
  (*board_)[3][6] = true;
  (*board_)[3][7] = true;
  (*board_)[3][8] = true;
  (*board_)[4][2] = true;
  (*board_)[4][8] = true;
  (*board_)[5][8] = true;
  (*board_)[6][2] = true;
  (*board_)[6][7] = true;
  (*board_)[7][4] = true;
  (*board_)[7][5] = true;

  live_.insert(std::make_pair(3, 3));
  live_.insert(std::make_pair(3, 4));
  live_.insert(std::make_pair(3, 5));
  live_.insert(std::make_pair(3, 6));
  live_.insert(std::make_pair(3, 7));
  live_.insert(std::make_pair(3, 8));
  live_.insert(std::make_pair(4, 2));
  live_.insert(std::make_pair(4, 8));
  live_.insert(std::make_pair(5, 8));
  live_.insert(std::make_pair(6, 2));
  live_.insert(std::make_pair(6, 7));
  live_.insert(std::make_pair(7, 4));
  live_.insert(std::make_pair(7, 5));
}

// Modular function
int GameOfLife::GetSafeIndex(int i, int length) {
  return i % length >= 0 ? (i % length) : (i + length);
}

bool GameOfLife::IsAlive(const std::pair<int, int>& cell) {
  return board_->at(cell.first).at(cell.second);
}

std::pair<int, int> GameOfLife::GetNeighbor(const std::pair<int, int>& cell,
                                            int i) {
  return std::make_pair(
      GetSafeIndex(cell.first + kNeighborDiffs[i][0], row_length_),
      GetSafeIndex(cell.second + kNeighborDiffs[i][1], col_length_));
}

size_t GameOfLife::GetLiveNeighbors(const std::pair<int, int>& cell) {
  return std::count_if(kNeighborIndices, kNeighborIndices + kNumNeighbors,
                       [this, &cell](int neighbor_index) {
                         return IsAlive(GetNeighbor(cell, neighbor_index));
                       });
}

bool GameOfLife::IsUnderpopulated(const std::pair<int, int>& cell) {
  return IsAlive(cell) && GetLiveNeighbors(cell) < kUnderpopulation;
}

bool GameOfLife::IsSurvived(const std::pair<int, int>& cell) {
  return IsAlive(cell) && GetLiveNeighbors(cell) >= kUnderpopulation &&
         GetLiveNeighbors(cell) <= kSurvival;
}

bool GameOfLife::IsOverpopulated(const std::pair<int, int>& cell) {
  return IsAlive(cell) && GetLiveNeighbors(cell) > kOverpopulation;
}

bool GameOfLife::IsRevived(const std::pair<int, int>& cell) {
  return !IsAlive(cell) && GetLiveNeighbors(cell) == kRevival;
}
