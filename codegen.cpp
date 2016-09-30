#include <iostream>
#include <vector>
#include <list>
#include <random>
#include <algorithm>
#include <bitset>
#include <memory>
#include <sstream>
#include <random>
#include "codegen.h"

size_t ham_dist(const std::bitset<NMAX>& l, const std::bitset<NMAX>& r) {
  return NMAX - (~(l ^ r)).count();
}

std::vector<std::shared_ptr<Vertex> > GenBinaryCodeGraph(size_t thr, size_t code_bits) {
  std::list<std::shared_ptr<Vertex> > vertexes(1 << code_bits);
  // Fill gray code table.
  auto iter = vertexes.begin();
  for (size_t idx = 0; idx < vertexes.size(); idx++, iter++) {
    iter->reset(new Vertex(std::bitset<NMAX>(idx), code_bits));
  }
  // Find adjacencies.
  for (auto iter1 = vertexes.begin(); iter1 != vertexes.end(); iter1++) {
    auto iter2 = iter1;
    for (iter2++; iter2 != vertexes.end(); iter2++) {
      auto val1 = *iter1;
      auto val2 = *iter2;
      std::string s1 = (std::string)(*val1);
      std::string s2 = (std::string)(*val2);
      size_t dist = ham_dist(val1->code, val2->code);
      if (dist >= thr) {
        val1->AppendAdj(VrxAdj(val2, dist));
        val2->AppendAdj(VrxAdj(val1, dist));
      }
    }
  }
  // Remove non connected vertexes.
  for (auto iter = vertexes.begin(); iter != vertexes.end(); ) {
    auto next = iter;
    next++;
    if ((*iter)->adjs.empty()) {
      vertexes.erase(iter);
    }
    iter = next;
  }
  // Convert from list to vector.
  std::vector<std::shared_ptr<Vertex> > vec2ret;
  vec2ret.assign(vertexes.begin(), vertexes.end());
  return vec2ret;
}

std::list<std::shared_ptr<Vertex> > FindSubset(std::shared_ptr<Vertex> pivot, size_t thr,
                                               std::list<std::shared_ptr<Vertex> > src) {
  std::list<std::shared_ptr<Vertex> > vertexes(src.begin(), src.end());
  // close located vertexes.
  for (auto iter = vertexes.begin(); iter != vertexes.end(); ) {
    auto next = iter;
    next++;
    if (ham_dist(pivot->code, (*iter)->code) < thr) {
      vertexes.erase(iter);
    }
    iter = next;
  }
  return vertexes;
}

std::list<std::shared_ptr<Vertex> > FindSubset0(std::shared_ptr<Vertex> pivot, size_t thr, std::list<VrxAdj> src) {
  std::list<std::shared_ptr<Vertex> > l2do;
  for (auto var : src) {
    l2do.push_back(var.vrx);
  }
  return FindSubset(pivot, thr, l2do);
}


std::vector<std::shared_ptr<Vertex> > generate_code(size_t seed, size_t code_bits) {
  const size_t DIST = 3;
  if (code_bits > NMAX) throw std::out_of_range("maximum code width exceeded");
  std::vector<std::shared_ptr<Vertex> > graph = GenBinaryCodeGraph(DIST, code_bits);
  // Generate code by decimation.
  std::default_random_engine gen(seed);
  std::uniform_int_distribution<size_t> idxgen(0, graph.size() - 1);
  // Start from random node
  auto elem = graph[idxgen(gen)];
  std::vector<std::shared_ptr<Vertex> > gcode;
  gcode.push_back(elem);
  gcode.push_back(gcode[0]->adjs.front().vrx);
  std::list<std::shared_ptr<Vertex> > ss = FindSubset0(gcode[1], DIST, gcode[0]->adjs);
  size_t idx = 2;
  while(!ss.empty()) {
    gcode.push_back(ss.front());
    ss = FindSubset(gcode[idx++], DIST, ss);
  }
  // Remove adjacencies because it isn't correct anymore.
  for (auto elem : gcode) {
    elem->adjs.clear();
  }

  return gcode;
}
