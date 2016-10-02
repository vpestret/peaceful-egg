#include "codegen.h"


std::vector<std::vector<std::bitset<NMAX> > > generate_map_from_code(std::vector<std::shared_ptr<Vertex> >& code) {
  std::vector<std::vector<std::bitset<NMAX> > > map2ret;
  std::vector<std::bitset<NMAX> > level0;
  // Selecting start index.
  size_t curr_idx = 0; // here
  std::cout << "Start vertex " << curr_idx << "-" << code[curr_idx]->code << " ";
  unsigned used_tag = 1;
  level0.push_back(code[curr_idx]->code);
  code[curr_idx]->used = used_tag;
  // Vertex based routing.
  // Form index vector of not used vertexes reachable from current.
  std::set<size_t> possibles;
  for (size_t port_idx = 0; port_idx < code[curr_idx]->code_bits; port_idx++) {
    for (const auto& co : code[curr_idx]->sp1_pu[port_idx].conn) {
      if (code[co.from_code]->used == used_Unused) {
        possibles.insert(co.from_code);
      }
    }
  }
  // Print possibles.
  std::cout << "Possible target vertexes: ";
  for (const auto& coidx : possibles) {
    std::cout << coidx << "-" << code[coidx]->code << " ";
  }
  std::cout << "\n";
  // Select target index.
  size_t tgt_idx = *possibles.begin(); // here
  std::cout << "Target vertex " << tgt_idx << "-" << code[tgt_idx]->code << "\n";
  // Find a route.
  size_t curr_port = code[curr_idx]->sp1.size(); // this value means not found.
  size_t tgt_port = code[tgt_idx]->sp1.size(); // this value means not found.
  // Cycle over target ports.
  for (size_t port_idx = 0; port_idx < code[tgt_idx]->code_bits; port_idx++) {
    for (const auto& co : code[tgt_idx]->sp1_pu[port_idx].conn) {
      if (co.from_code == curr_idx && code[tgt_idx]->sp1_pu[port_idx].used == used_Unused &&
          code[curr_idx]->sp1_pu[co.from_port].used == used_Unused) {
        curr_port = co.from_port;
        tgt_port = port_idx;
        port_idx = code[tgt_idx]->code_bits; // exiting outer loop this way.
        break;
      }
    }
  }
  if (curr_port != code[curr_idx]->sp1.size()) {
    // Route found make a connection.
    std::cout << "Route from " << curr_idx << ":" << curr_port << "-" << code[curr_idx]->sp1[curr_port] << " ";
    std::cout << "to " << tgt_idx << ":" << tgt_port << "-" << code[tgt_idx]->sp1[tgt_port] << "\n";
  }

  map2ret.push_back(level0);

  return map2ret;
}
