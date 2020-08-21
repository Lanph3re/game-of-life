#ifndef GAME_OF_LIFE_H_
#define GAME_OF_LIFE_H_

#include <algorithm>
#include <ctime>
#include <iostream>
#include <memory>
#include <set>
#include <utility>
#include <vector>

using Cell = std::pair<int, int>;

class GameOfLife {
 public:
  GameOfLife(int width, int height);
  ~GameOfLife();

  void RandomInit(void);

  int GetRowLength();
  int GetColLength();
  int GetSafeIndex(int i, int length);

  void FlipCell(const Cell& cell);
  void FlipCell(int row_idx, int col_idx);

  void NextGeneration();

 private:
  friend class LifeRunner;

  bool IsAlive(int row_idx, int col_idx);
  bool IsAlive(const Cell& cell);

  Cell GetNeighbor(const Cell& cell, int i);
  size_t GetLiveNeighbors(const Cell& cell);

  bool IsUnderpopulated(const Cell& cell);
  bool IsSurvived(const Cell& cell);
  bool IsOverpopulated(const Cell& cell);
  bool IsRevived(const Cell& cell);

  int row_length_;
  int col_length_;

  static constexpr size_t kUnderpopulation = 2;
  static constexpr size_t kSurvival = 3;
  static constexpr size_t kOverpopulation = 3;
  static constexpr size_t kRevival = 3;

  static constexpr int kNumNeighbors = 8;
  static constexpr int kNeighborIndices[] = {0, 1, 2, 3, 4, 5, 6, 7};
  static constexpr int kNeighborDiffs[][2] = {
      {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

  // These variables used for calculate next generation
  // live_ contains the corordinates of live cells for fast lookup
  // inspect_ has all live cells plus their neighbor cells
  // pending_ stores pending changes to be applied to next generation
  std::set<Cell> live_;
  std::set<Cell> inspect_;
  std::set<Cell> pending_;

  std::vector<std::vector<bool> > board_;
};

#endif
