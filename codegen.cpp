#include "codegen.h"

size_t ham_dist(const std::bitset<NMAX>& l, const std::bitset<NMAX>& r) {
  return NMAX - (~(l ^ r)).count();
}

std::vector<std::shared_ptr<Vertex> > GenBinaryCodeGraph(size_t thr, size_t code_bits) {
  std::list<std::shared_ptr<Vertex> > vertexes(1 << code_bits);
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
    l2do.emplace_back(var.vrx);
  }
  return FindSubset(pivot, thr, l2do);
}

std::vector<std::shared_ptr<Vertex> > generate_code_d2(size_t seed, size_t code_bits) {
  std::vector<std::shared_ptr<Vertex> > gcode(1 << (code_bits - 1));
  // Fill gray code table.
  auto iter = gcode.begin();
  for(size_t num = 0; num < (1 << code_bits); num += 2, iter++) {
    iter->reset(new Vertex(std::bitset<NMAX>(num ^ (num >> 1)), code_bits));
  }
  return gcode;
}

std::vector<std::shared_ptr<Vertex> > generate_code_d3(size_t seed, size_t code_bits) {
  const size_t DIST = 3;
  if (code_bits > NMAX) throw std::out_of_range("maximum code width exceeded");
  std::vector<std::shared_ptr<Vertex> > graph = GenBinaryCodeGraph(DIST, code_bits);
  // Generate code by decimation.
  std::default_random_engine gen(seed);
  std::uniform_int_distribution<size_t> idxgen(0, graph.size() - 1);
  // Start from random node
  auto elem = graph[idxgen(gen)];
  std::vector<std::shared_ptr<Vertex> > gcode;
  gcode.emplace_back(elem);
  gcode.emplace_back(gcode[0]->adjs.front().vrx);
  std::list<std::shared_ptr<Vertex> > ss = FindSubset0(gcode[1], DIST, gcode[0]->adjs);
  size_t idx = 2;
  while(!ss.empty()) {
    gcode.emplace_back(ss.front());
    ss = FindSubset(gcode[idx++], DIST, ss);
  }
  // Remove adjacencies because it isn't correct anymore.
  for (auto elem : gcode) {
    elem->adjs.clear();
  }

  return gcode;
}

void Vertex::PrepareSheres() {
  for (size_t idx1 = 0; idx1 < this->code_bits; idx1++) {
    this->sp1.emplace_back(this->code ^ std::bitset<NMAX>(1 << idx1));
    this->sp1_pu.emplace_back(PortUsage());
    for (size_t idx2 = 0; idx2 < this->code_bits; idx2++) {
      if (idx1 != idx2) {
        auto cti = this->code ^ std::bitset<NMAX>((1 << idx1) | (1 << idx2));
        if (this->sp2.find(cti) == this->sp2.end()) {
          this->sp2[cti] = std::vector<size_t>();
        }
        this->sp2[cti].emplace_back(idx1);
      }
    }
  }
}

void intersect_code_spheres(std::vector<std::shared_ptr<Vertex> >& code, XSecType xsectype) {
  // Initialize spheres.
  for (auto iter1 = code.begin(); iter1 != code.end(); iter1++) {
    (*iter1)->PrepareSheres();
  }
  // Find intersections.
  for (auto iter1 = code.begin(); iter1 != code.end(); iter1++) { // sp 1
    if (xsectype == SP1xSP2) {
      size_t idx2 = 0;
      for (auto iter2 = code.begin(); iter2 != code.end(); iter2++, idx2++) { // sp 2
        if (iter1 != iter2) {
          auto val1 = *iter1;
          auto val2 = *iter2;
          std::string s1 = (std::string)(*val1);
          std::string s2 = (std::string)(*val2);
          // cycle over all sp1
          for (size_t isp1_v1 = 0; isp1_v1 < val1->sp1.size(); isp1_v1++) {
            for (auto& psp2_v2: val2->sp2) {
              if (psp2_v2.first == val1->sp1[isp1_v1]) {
                for(auto  port : psp2_v2.second) {
                  val1->sp1_pu[isp1_v1].conn.emplace_back(PortConn());
                  val1->sp1_pu[isp1_v1].conn.back().from_code = idx2;
                  val1->sp1_pu[isp1_v1].conn.back().from_port = port;
                }
              }
            }
          }
        }
      }
    } else if (xsectype == SP1xSP1) {
      size_t idx2 = 0;
      for (auto iter2 = code.begin(); iter2 != code.end(); iter2++, idx2++) {
        if (iter1 != iter2) {
          auto val1 = *iter1;
          auto val2 = *iter2;
          std::string s1 = (std::string)(*val1);
          std::string s2 = (std::string)(*val2);
          // cycle over all sp1
          for (size_t isp1_v1 = 0; isp1_v1 < val1->sp1.size(); isp1_v1++) {
            for (size_t isp1_v2 = 0; isp1_v2 < val2->sp1.size(); isp1_v2++) {
              if (val2->sp1[isp1_v2] == val1->sp1[isp1_v1]) {
                val1->sp1_pu[isp1_v1].conn.emplace_back(PortConn());
                val1->sp1_pu[isp1_v1].conn.back().from_code = idx2;
                val1->sp1_pu[isp1_v1].conn.back().from_port = -1;
              }
            }
          }
        }
      }
    } else {
      std::ostringstream oss;
      oss << xsectype;
      throw std::runtime_error("unknown intersect type" + oss.str());
    }
  }
}

void clear_tag(std::vector<std::shared_ptr<Vertex> >& code, unsigned used_tag) {
  for (const auto& vrx : code) {
    if (vrx->used == used_tag) vrx->used = used_Unused;
    for (size_t idx = 0; idx < vrx->sp1_pu.size(); idx++) {
      if (vrx->sp1_pu[idx].used == used_tag) vrx->sp1_pu[idx].used = used_Unused;
    }
  }
}
