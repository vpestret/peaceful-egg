#include "codegen.h"

#ifndef _MAPGEN_H_
#define _MAPGEN_H_

std::vector<std::vector<std::bitset<NMAX> > > generate_map_from_code(std::vector<std::shared_ptr<Vertex> >& code);

#endif // _MAPGEN_H_
