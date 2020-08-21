#ifndef GAME_OF_LIFE_H_
#define GAME_OF_LIFE_H_

#include <algorithm>
#include <iostream>
#include <memory>
#include <set>
#include <utility>
#include <vector>

class GameOfLife {
 public:
  GameOfLife(int width, int height);
  ~GameOfLife();

  int GetRowLength();
  int GetColLength();

  void NextGeneration();
  void TestInit(void);

 private:
  friend class LifeRunner;

  int GetSafeIndex(int i, int length);
  std::pair<int, int> GetNeighbor(const std::pair<int, int>& cell, int i);

  bool IsAlive(int row_idx, int col_idx);
  bool IsAlive(const std::pair<int, int>& cell);
  size_t GetLiveNeighbors(const std::pair<int, int>& cell);

  bool IsUnderpopulated(const std::pair<int, int>& cell);
  bool IsSurvived(const std::pair<int, int>& cell);
  bool IsOverpopulated(const std::pair<int, int>& cell);
  bool IsRevived(const std::pair<int, int>& cell);

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
  std::set<std::pair<int, int> > live_;
  std::set<std::pair<int, int> > inspect_;
  std::set<std::pair<int, int> > pending_;

  std::vector<std::vector<bool> > board_;
};

#endif
