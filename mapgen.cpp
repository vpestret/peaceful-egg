#include "mapgen.h"

const unsigned one_level_route_length = 3;

std::vector<std::bitset<NMAX> > generate_path(std::default_random_engine gen,
                                              std::vector<std::shared_ptr<Vertex> >& code,
                                              size_t curr_idx,
                                              unsigned used_tag,
                                              unsigned route_length) {
  std::vector<std::bitset<NMAX> > level;

  level.push_back(code[curr_idx]->code);
  code[curr_idx]->used = used_tag;

  unsigned curr_length = 0;
  while (curr_length < route_length) {
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
    std::cout << "For source vertex " << curr_idx << "-" << code[curr_idx]->code << " ";
    std::cout << "Possible target vertexes: ";
    for (const auto& coidx : possibles) {
      std::cout << coidx << "-" << code[coidx]->code << " ";
    }
    std::cout << "\n";
    // Select target index.
    std::uniform_int_distribution<size_t> idxgen(0, possibles.size() - 1);
    size_t idx_in_pos = idxgen(gen);
    size_t tgt_idx = *possibles.begin();
    for (const auto& idx : possibles) {
      if (idx_in_pos-- == 0) {
        tgt_idx = idx;
      }
    }
    std::cout << "Target vertex " << tgt_idx << "-" << code[tgt_idx]->code << "\n";
    // Find a route.
    size_t curr_port = code[curr_idx]->sp1.size(); // this value means not found.
    size_t tgt_port = code[tgt_idx]->sp1.size(); // this value means not found.
    // Cycle over target ports to find unused pair to make a connection.
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
      // Link found make a connection.
      std::cout << "Route from " << curr_idx << ":" << curr_port << "-" << code[curr_idx]->sp1[curr_port] << " ";
      std::cout << "to " << tgt_idx << ":" << tgt_port << "-" << code[tgt_idx]->sp1[tgt_port] << "\n";
      level.push_back(code[curr_idx]->sp1[curr_port]);
      code[curr_idx]->sp1_pu[curr_port].used = used_tag;
      // Make also as used all other incoming ports.
      for (const auto& co : code[curr_idx]->sp1_pu[curr_port].conn) {
        code[co.from_code]->sp1_pu[co.from_port].used = used_tag;
      }
      level.push_back(code[tgt_idx]->sp1[tgt_port]);
      code[tgt_idx]->sp1_pu[tgt_port].used = used_tag;
      // Make also as used all other incoming ports.
      for (const auto& co : code[tgt_idx]->sp1_pu[tgt_port].conn) {
        code[co.from_code]->sp1_pu[co.from_port].used = used_tag;
      }
      // And finally add the target vertex to the used list.
      level.push_back(code[tgt_idx]->code);
      code[tgt_idx]->used = used_tag;
      // Debug dump examination.
      std::cout << "Used spheres\n";
      for (const auto& vrx : code) {
        std::cout << vrx->string_w_sp1() << "\n";
      }
      std::cout << std::endl;
      std::cout << "Path: ";
      for (const auto& c2p : level) {
        std::cout << Vertex::to_string(c2p, code[0]->code_bits) << " ";
      }
      std::cout << "\n";
      // Debug dump examination pause.
      std::getchar();
      // Got to next iteration.
      curr_length++;
      curr_idx = tgt_idx;
    } else {
      std::cout << "No route found, exiting at length " << curr_length << "\n";
      break;
    }
  }

  // Before return remove last vertex from level because it should be exit.
  level.pop_back();
  return level;
}

LevelsMap generate_map_from_code(size_t seed, std::vector<std::shared_ptr<Vertex> >& code, unsigned n_levels) {
  LevelsMap map2ret;
  // Generating level 0.
  std::default_random_engine gen(seed);
  std::uniform_int_distribution<size_t> idxgen(0, code.size() - 1);
  // Selecting start index.
  size_t curr_idx = idxgen(gen);
  std::cout << "Trying start vertex " << curr_idx << "-" << code[curr_idx]->code << "\n";
  unsigned used_tag = 1;
  if (code[curr_idx]->used != used_Unused) throw std::runtime_error("intended start code is already used");
  std::vector<std::bitset<NMAX> > level0 = generate_path(gen, code, curr_idx, used_tag, one_level_route_length);

  map2ret.levels.push_back(level0);

  return map2ret;
}
