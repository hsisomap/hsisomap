//
// Created by Can on 1/19/16.
//

#ifndef HSISOMAP_VPTREE_H
#define HSISOMAP_VPTREE_H

#include <hsisomap/Matrix.h>
#include <queue>
#include <gsl/gsl_matrix_double.h>
#include "../typedefs.h"

HSISOMAP_NAMESPACE_BEGIN


template<typename T, double (*distance)(const T &, const T &)>
class VpTree {
 public:
  VpTree() : _root(0) { }

  ~VpTree() {
    delete _root;
  }

  void create(const std::vector<T> &items) {
    delete _root;
    _items = items;
    _root = buildFromPoints(0, (int) items.size());
  }

  void search(const T &target, int k, std::vector<T> *results,
              std::vector<double> *distances) {
    std::priority_queue<HeapItem> heap;

    _tau = std::numeric_limits<double>::max();
    search(_root, target, k, heap);

    results->clear();
    distances->clear();

    while (!heap.empty()) {
      results->push_back(_items[heap.top().index]);
      distances->push_back(heap.top().dist);
      heap.pop();
    }

    std::reverse(results->begin(), results->end());
    std::reverse(distances->begin(), distances->end());
  }

  void search_r(const T &target, double dist_rad, std::vector<T> *results, std::vector<double> *distances) {
    std::priority_queue<HeapItem> heap;

    search_r(_root, target, dist_rad, heap);

    results->clear();
    distances->clear();

    while (!heap.empty()) {
      results->push_back(_items[heap.top().index]);
      distances->push_back(heap.top().dist);
      heap.pop();
    }

    std::reverse(results->begin(), results->end());
    std::reverse(distances->begin(), distances->end());
  }
 private:
  std::vector<T> _items;
  double _tau;
  int _k_found;

  struct Node {
    int index;
    double threshold;
    Node *left;
    Node *right;

    Node() :
        index(0), threshold(0.), left(0), right(0) { }

    ~Node() {
      delete left;
      delete right;
    }
  } *_root;

  struct HeapItem {
    HeapItem(int index, double dist) :
        index(index), dist(dist) { }
    int index;
    double dist;
    bool operator<(const HeapItem &o) const {
      return dist < o.dist;
    }
  };

  struct DistanceComparator {
    const T &item;
    DistanceComparator(const T &item) : item(item) { }
    bool operator()(const T &a, const T &b) {
      return distance(item, a) < distance(item, b);
    }
  };

  Node *buildFromPoints(int lower, int upper) {
    if (upper == lower) {
      return NULL;
    }

    Node *node = new Node();
    node->index = lower;

    if (upper - lower > 1) {

      // choose an arbitrary point and move it to the start
      int i = (int) ((double) rand() / RAND_MAX * (upper - lower - 1)) + lower;
      std::swap(_items[lower], _items[i]);

      int median = (upper + lower) / 2;

      // partitian around the median distance
      std::nth_element(
          _items.begin() + lower + 1,
          _items.begin() + median,
          _items.begin() + upper,
          DistanceComparator(_items[lower]));

      // what was the median?
      node->threshold = distance(_items[lower], _items[median]);

      node->index = lower;
      node->left = buildFromPoints(lower + 1, median);
      node->right = buildFromPoints(median, upper);
    }

    return node;
  }

  void search(Node *node, const T &target, int k,
              std::priority_queue<HeapItem> &heap) {
    if (node == NULL) return;

    double dist = distance(_items[node->index], target);
    //printf("dist=%g tau=%gn", dist, _tau );

    if (dist < _tau) {
      if (heap.size() == k) heap.pop();
      heap.push(HeapItem(node->index, dist));
      if (heap.size() == k) _tau = heap.top().dist;
    }

    if (node->left == NULL && node->right == NULL) {
      return;
    }

    if (dist < node->threshold) {
//      if (dist - _tau <= node->threshold) {
        search(node->left, target, k, heap);
//      }

      if (dist + _tau >= node->threshold) {
        search(node->right, target, k, heap);
      }

    } else {
//      if (dist + _tau >= node->threshold) {
        search(node->right, target, k, heap);
//      }

      if (dist - _tau <= node->threshold) {
        search(node->left, target, k, heap);
      }
    }
  }

  void search_r(Node *node, const T &target, double dist_rad,
                std::priority_queue<HeapItem> &heap) {
    if (node == NULL) return;

    double dist = distance(_items[node->index], target);
    //printf("dist=%g tau=%gn", dist, _tau );

    if (dist <= dist_rad) {
//            if ( heap.size() == k ) heap.pop();
      heap.push(HeapItem(node->index, dist));
//            if ( heap.size() == k ) _tau = heap.top().dist;
    }

    if (node->left == NULL && node->right == NULL) {
      return;
    }

    if (dist < node->threshold) {
      if (dist - dist_rad <= node->threshold) {
        search_r(node->left, target, dist_rad, heap);
      }

      if (dist + dist_rad >= node->threshold) {
        search_r(node->right, target, dist_rad, heap);
      }

    } else {
      if (dist + dist_rad >= node->threshold) {
        search_r(node->right, target, dist_rad, heap);
      }

      if (dist - dist_rad <= node->threshold) {
        search_r(node->left, target, dist_rad, heap);
      }
    }
  }

};


struct PixelView {
  double *data;
  Index bands;
  Index index;
  PixelView(Index bands, Scalar *data, Index index) : bands(bands), data(data), index(index) { }
};

inline Scalar SquaredDistance(const PixelView& p1, const PixelView& p2) {
  Scalar val = 0;
  for (size_t i = 0; i < p1.bands; ++i) {
    val += (p1.data[i] - p2.data[i]) * (p1.data[i] - p2.data[i]);
  }
  return val;
}

inline std::vector<PixelView> CreatePixelViewsFromMatrix(const gsl::Matrix &matrix) {
  Index pixels = matrix.rows();
  Index bands = matrix.cols();
  std::vector<PixelView> result(pixels, PixelView(bands, NULL, 0));
  for (Index i = 0; i < pixels; ++i) {
    result[i].data = matrix.m_->data + i * bands;
    result[i].index = i;
  }
  return result;
}


HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_VPTREE_H
