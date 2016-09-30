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

size_t ham_dist(const std::bitset<N>& l, const std::bitset<N>& r) {
  return N - (~(l ^ r)).count();
}

std::vector<std::shared_ptr<Vertex> > GenGrayCodeGraph(size_t thr) {
  std::list<std::shared_ptr<Vertex> > vertexes(1 << N);
  // Fill gray code table.
  auto iter = vertexes.begin();
  for (size_t idx = 0; idx < vertexes.size(); idx ++) {
    std::bitset<N> num(idx);
    Vertex* pv = new Vertex;
    pv->code = num ^ (num >> 1);
    iter->reset(pv);
    iter++;
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

std::list<std::shared_ptr<Vertex> > FindSubset(std::shared_ptr<Vertex> pivot, size_t thr, std::list<std::shared_ptr<Vertex> > src) {
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


std::vector<std::shared_ptr<Vertex> > generate_code(size_t seed) {
  const size_t DIST = 3;
  std::vector<std::shared_ptr<Vertex> > graph = GenGrayCodeGraph(DIST);

  //std::cout << "Adjacency graph\n";
  //for (auto vrx : graph) {
  //  std::cout << vrx->string_w_adjs() << std::endl;
  //}

  //for (auto elem : graph) {

  std::default_random_engine gen(seed);
  std::uniform_int_distribution<size_t> idxgen(0, graph.size() - 1);
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

//  }

  //std::cout << "Generated code : ";
  //for (auto vrx : gcode) {
  //  std::cout << (std::string)(*vrx) << " ";
  //}
  //std::cout << std::endl;

  return gcode;
}
