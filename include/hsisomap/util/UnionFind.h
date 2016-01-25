//
// Created by Can on 1/25/16.
//

#ifndef HSISOMAP_UNIONFIND_H
#define HSISOMAP_UNIONFIND_H

#include <numeric>
#include <vector>
#include "../typedefs.h"

HSISOMAP_NAMESPACE_BEGIN


class UnionFind 
{
 public:
  UnionFind(Index count) : count_(count), id_(count, 0), sz_(count, 1) {
    std::iota(id_.begin(), id_.end(), 0);
  }
  Index count() { return count_; }
  Index Find(Index p) {
    while (p != id_[p]) p = id_[p];
    return p;
  }
  bool connected(Index p, Index q) { return Find(p) == Find(q); }
  void Connect(Index p, Index q) {
    Index root_p = Find(p);
    Index root_q = Find(q);
    if (root_p == root_q) return;
    if (sz_[root_p] < sz_[root_q]) {
      id_[root_p] = root_q;
      sz_[root_q] += sz_[root_p];
    } else {
      id_[root_q] = root_p;
      sz_[root_p] += sz_[root_q];
    }
    count_--;
  }
 private:
  std::vector<Index> id_;
  std::vector<Index> sz_;
  Index count_;
};


HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_UNIONFIND_H
