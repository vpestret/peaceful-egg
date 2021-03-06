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

#ifndef _CODEGEN_H_
#define _CODEGEN_H_

const size_t NMAX = 6;

class Vertex;

class VrxAdj {
public:
  VrxAdj(std::shared_ptr<Vertex> _vrx, size_t _dst) : vrx(_vrx), dst(_dst) {}
  std::shared_ptr<Vertex> vrx;
  size_t dst;
};

const unsigned used_Unused = 0;

class PortConn {
public:
  PortConn() : from_code(-1), from_port(-1) {}
  size_t from_code;
  size_t from_port;
};

class PortUsage {
public:
  PortUsage() : used(used_Unused) {}
  std::vector<PortConn> conn;
  unsigned used;
};

class Vertex {
public:
  std::bitset<NMAX> code;
  std::list<VrxAdj> adjs;
  size_t code_bits;
  std::vector<std::bitset<NMAX> > sp1;
  std::vector<PortUsage> sp1_pu;
  std::unordered_map<std::bitset<NMAX>, std::vector<size_t>> sp2;
  unsigned used;
  Vertex(const std::bitset<NMAX>& _code, size_t _code_bits) :
    code(_code), code_bits(_code_bits), used(used_Unused) {}
  void PrepareSheres();

  void AppendAdj(const VrxAdj& ca) {
    auto iter = adjs.begin();
    for (; iter != adjs.end() && ca.dst > iter->dst; iter++);
    adjs.insert(iter, ca);
  }
  static std::string to_string(std::bitset<NMAX> code, size_t code_bits) {
    std::string s2r;
    for (size_t idx = 0; idx < code_bits; idx ++) {
      s2r += code[code_bits - idx - 1] ? "1" : "0";
    }
    return s2r;
  }
  operator std::string() {
    return to_string(code, code_bits);
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
    std::stringstream ss;
    ss << (this->used != used_Unused ? "*" : "  ");
    if (this->used != used_Unused) {
      ss << this->used;
    }
    s2r += ss.str() + "{";
    for (size_t idx = 0; idx < this->code_bits; idx++) {
      std::stringstream ss;
      ss << to_string(sp1[idx], code_bits) << (sp1_pu[idx].used != used_Unused ? "*" : "  ");
      if (sp1_pu[idx].used != used_Unused) {
        ss << sp1_pu[idx].used;
      }
      ss << "(";
      for (const auto& co: sp1_pu[idx].conn) {
        if (co.from_port == (size_t)-1)
          ss << co.from_code << " ";
        else
          ss << co.from_code << ":" << co.from_port << " ";
      }
      ss << ")";
      s2r += ss.str() + " ";
    }
    s2r += "}";
    return s2r;
  }
};

bool mark_sp_used(std::vector<std::shared_ptr<Vertex> >& code_vrx, const std::bitset<NMAX>& scode, unsigned used_tag);
bool mark_link_used(std::vector<std::shared_ptr<Vertex> >& code_vrx, std::shared_ptr<Vertex> vrx,
                    const std::bitset<NMAX>& scode, unsigned used_tag);

std::vector<std::shared_ptr<Vertex> > generate_code_d2(size_t seed, size_t code_bits);
std::vector<std::shared_ptr<Vertex> > generate_code_d3(size_t seed, size_t code_bits);

enum XSecType{SP1xSP1, SP1xSP2};

void intersect_code_spheres(std::vector<std::shared_ptr<Vertex> >& code, XSecType xsectype);

void clear_tag(std::vector<std::shared_ptr<Vertex> >& code, unsigned used_tag);

#endif // _CODEGEN_H_
