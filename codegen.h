#include <iostream>
#include <vector>
#include <list>
#include <random>
#include <algorithm>
#include <bitset>
#include <memory>
#include <sstream>
#include <random>

const size_t N = 6;

class Vertex;

class VrxAdj {
public:
  VrxAdj(std::shared_ptr<Vertex> _vrx, size_t _dst) : vrx(_vrx), dst(_dst) {}
  std::shared_ptr<Vertex> vrx;
  size_t dst;
};

class Vertex {
public:
  std::bitset<N> code;
  std::list<VrxAdj> adjs;
  void AppendAdj(const VrxAdj& ca) {
    auto iter = adjs.begin();
    for (; iter != adjs.end() && ca.dst > iter->dst; iter++);
    adjs.insert(iter, ca);
  }
  operator std::string() {
    std::string s2r;
    for (size_t idx = 0; idx < code.size(); idx ++) {
      s2r += code[code.size() - idx - 1] ? "1" : "0";
    }
    return s2r;
  }
  std::string string_w_adjs() {
    std::string s2r = (std::string)(*this);
    s2r += " {";
    for(auto p : adjs) {
      std::stringstream ss;
      ss << p.dst;
      s2r += (std::string)(*p.vrx) + "(" + ss.str() + ") ";
    }
    s2r += "}";
    return s2r;
  }
};

std::vector<std::shared_ptr<Vertex> > generate_code(size_t seed);
