#include "codegen.h"

#ifndef _MAPGEN_H_
#define _MAPGEN_H_

class LevelsMap {
public:
  size_t start_idx;
  std::vector<std::vector<std::bitset<NMAX> > > levels;
};

LevelsMap generate_map_from_code(size_t seed, std::vector<std::shared_ptr<Vertex> >& code, unsigned n_levels);

void generate_verilog(const LevelsMap&, std::ostream&);

#endif // _MAPGEN_H_
