#include <iostream>
#include <vector>
#include <list>
#include <random>
#include <algorithm>
#include <bitset>
#include <memory>
#include <sstream>
#include <random>

const size_t NMAX = 6;

class Vertex;

class VrxAdj {
public:
  VrxAdj(std::shared_ptr<Vertex> _vrx, size_t _dst) : vrx(_vrx), dst(_dst) {}
  std::shared_ptr<Vertex> vrx;
  size_t dst;
};

class Vertex {
public:
  std::bitset<NMAX> code;
  std::list<VrxAdj> adjs;
  size_t code_bits;
  Vertex(const std::bitset<NMAX>& _code, size_t _code_bits) :
    code(_code), code_bits(_code_bits) {}
  void AppendAdj(const VrxAdj& ca) {
    auto iter = adjs.begin();
    for (; iter != adjs.end() && ca.dst > iter->dst; iter++);
    adjs.insert(iter, ca);
  }
  operator std::string() {
    std::string s2r;
    for (size_t idx = 0; idx < code_bits; idx ++) {
      s2r += code[code_bits - idx - 1] ? "1" : "0";
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

std::vector<std::shared_ptr<Vertex> > generate_code(size_t seed, size_t code_bits);
