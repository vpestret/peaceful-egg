#include "codegen.h"

#ifndef _MAPGEN_H_
#define _MAPGEN_H_

class LevelsMap {
public:
  size_t start_idx;
  size_t code_bits;
  std::vector<std::vector<std::bitset<NMAX> > > levels;
};

LevelsMap generate_map_from_code(size_t seed, std::vector<std::shared_ptr<Vertex> >& code, unsigned n_levels);

void generate_verilog(const LevelsMap&, std::ostream&);

void generate_verilog_dnf(const LevelsMap& lm, std::ostream& ost);

#endif // _MAPGEN_H_
