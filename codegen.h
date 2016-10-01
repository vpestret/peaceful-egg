#include <iostream>
#include <vector>
#include <list>
#include <random>
#include <algorithm>
#include <bitset>
#include <memory>
#include <sstream>
#include <random>
#include <unordered_map>
#include <set>

const size_t NMAX = 6;

class Vertex;

class VrxAdj {
public:
  VrxAdj(std::shared_ptr<Vertex> _vrx, size_t _dst) : vrx(_vrx), dst(_dst) {}
  std::shared_ptr<Vertex> vrx;
  size_t dst;
};

class PortUsage {
public:
  static const unsigned Unused = 0;
  PortUsage() : connected(false), from_code(-1), from_port(-1), used(Unused) {}
  bool connected;
  size_t from_code;
  size_t from_port;
  unsigned used;
};

class Vertex {
public:
  std::bitset<NMAX> code;
  std::list<VrxAdj> adjs;
  size_t code_bits;
  std::unordered_map<std::bitset<NMAX>, std::vector<PortUsage> > sp1;
  std::unordered_map<std::bitset<NMAX>, std::vector<size_t>> sp2;
  Vertex(const std::bitset<NMAX>& _code, size_t _code_bits) :
    code(_code), code_bits(_code_bits) {}
  void PrepareSheres(size_t no_one_value);

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
    for(const auto& p : adjs) {
      std::stringstream ss;
      ss << p.dst;
      s2r += (std::string)(*p.vrx) + "(" + ss.str() + ") ";
    }
    s2r += "}";
    return s2r;
  }
  std::string string_w_sp1() {
    std::string s2r = (std::string)(*this);
    s2r += " {";
    for (size_t idx1 = 0; idx1 < this->code_bits; idx1++) {
      auto key = this->code ^ std::bitset<NMAX>(1 << idx1);
      auto it = sp1.find(key);
      auto& p = *it;
      std::stringstream ss;
      ss << p.first << "(";
      for (const auto& pu: p.second) {
        if (pu.connected) {
          ss << pu.from_code << ":" << pu.from_port << " ";
        } else {
          ss << "NC";
        }
      }
      ss << ")";
      s2r += ss.str() + " ";
    }
    s2r += "}";
    return s2r;
  }
};

std::vector<std::shared_ptr<Vertex> > generate_code(size_t seed, size_t code_bits);

void intersect_code_spheres(std::vector<std::shared_ptr<Vertex> >& code);
