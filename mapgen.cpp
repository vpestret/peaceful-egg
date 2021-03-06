#include "mapgen.h"

const unsigned one_level_route_length = 3;

std::vector<std::bitset<NMAX> > generate_path_d2(std::default_random_engine gen,
                                                 std::vector<std::shared_ptr<Vertex> >& code,
                                                 size_t& curr_idx,
                                                 unsigned used_tag,
                                                 unsigned route_length) {
  std::vector<std::bitset<NMAX> > level;

  level.push_back(code[curr_idx]->code);
  code[curr_idx]->used = used_tag;

  unsigned curr_length = 0;
  while (curr_length < route_length) {
    // Vertex based routing.
    // Form index vector of not used vertexes reachable from current.
    std::set<std::pair<size_t, size_t> > possibles;
    for (size_t port_idx = 0; port_idx < code[curr_idx]->code_bits; port_idx++) {
      if (code[curr_idx]->sp1_pu[port_idx].used != used_Unused) continue;
      for (const auto& co : code[curr_idx]->sp1_pu[port_idx].conn) {
        if (code[co.from_code]->used == used_Unused) {
          possibles.insert(std::make_pair(port_idx, co.from_code));
        }
      }
    }
    if (possibles.size() == 0) {
      std::cout << "No route found, exiting at length " << curr_length << "\n";
      break;
    }
    // Print possibles.
    std::cout << "For source vertex " << curr_idx << ":" << Vertex::to_string(code[curr_idx]->code, code[0]->code_bits) << " ";
    std::cout << "Possible links vertexes: ";
    for (const auto& pos : possibles) {
      std::cout << Vertex::to_string(code[curr_idx]->sp1[pos.first], code[0]->code_bits) << "->"
                << Vertex::to_string(code[pos.second]->code, code[0]->code_bits) << " ";
    }
    std::cout << "\n";
    // Select target index.
    std::uniform_int_distribution<size_t> tgtgen(0, possibles.size() - 1);
    size_t tgt_idx = 0;//tgtgen(gen);
    std::vector<std::pair<size_t, size_t> > tgt_idxes(possibles.begin(), possibles.end());
    auto& tgt_pos = tgt_idxes[tgt_idx];
    std::cout << "Target " << Vertex::to_string(code[curr_idx]->sp1[tgt_pos.first], code[0]->code_bits) << "->"
              << Vertex::to_string(code[tgt_pos.second]->code, code[0]->code_bits) << "\n";
    // Add current vertexes to path.
    level.push_back(code[curr_idx]->sp1[tgt_pos.first]);
    level.push_back(code[tgt_pos.second]->code);
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
    std::getchar();
    // Mark current as used and go to next.
    mark_sp_used(code, code[curr_idx]->code, used_tag);
    mark_link_used(code, code[curr_idx], code[curr_idx]->sp1[tgt_pos.first], used_tag);
    curr_length++;
    curr_idx = tgt_pos.second;
  }
  // Before return remove last vertex from level because it should be exit.
  level.pop_back();
  return level;
}

std::vector<std::bitset<NMAX> > generate_path_d3(std::default_random_engine gen,
                                                 std::vector<std::shared_ptr<Vertex> >& code,
                                                 size_t& curr_idx,
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
    std::vector<size_t> tgt_idxes(possibles.begin(), possibles.end());
    bool connection_made = false;
    for (auto tgt_idx : tgt_idxes) {
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
        connection_made = true;
        break; // leave cycle over tgt_idx
      } else {
        std::cout << "No link found for tgt_idx =  " << tgt_idx << "\n";
      }
    } // cycle over tgt_idx
    if (!connection_made) {
      std::cout << "No route found, exiting at length " << curr_length << "\n";
      break;
    }
  }
  // Before return remove last vertex from level because it should be exit.
  level.pop_back();
  return level;
}

LevelsMap generate_map_from_code(size_t seed, std::vector<std::shared_ptr<Vertex> >& code, unsigned n_levels,
                                 XSecType xsectype) {
  LevelsMap map2ret;
  // Generating level 0.
  std::default_random_engine gen(seed);
  std::uniform_int_distribution<size_t> idxgen(0, code.size() - 1);
  // Selecting start index.
  size_t curr_idx = idxgen(gen);
  std::cout << "Trying start vertex " << curr_idx << "-" << code[curr_idx]->code << "\n";
  unsigned used_tag = 1;
  if (code[curr_idx]->used != used_Unused) throw std::runtime_error("intended start code is already used");
  map2ret.start_idx = curr_idx;
  std::vector<std::bitset<NMAX> > level0;
  if (xsectype == SP1xSP2) {
    level0 = generate_path_d3(gen, code, curr_idx, used_tag, one_level_route_length);
  } else if (xsectype == SP1xSP1) {
    level0 = generate_path_d2(gen, code, curr_idx, used_tag, one_level_route_length);
  }
  map2ret.levels.push_back(level0);
  unsigned prev_used_tag = used_tag;
  used_tag = 3 - used_tag;
  for (int idx = 1; idx < n_levels; idx++) {
    // Remove previously used tag.
    clear_tag(code, used_tag);
    // Generate new level with new tag.
    std::vector<std::bitset<NMAX> > level;
    if (xsectype == SP1xSP2) {
      level = generate_path_d3(gen, code, curr_idx, used_tag, one_level_route_length);
    } else if (xsectype == SP1xSP1) {
      level = generate_path_d2(gen, code, curr_idx, used_tag, one_level_route_length);
    }
    if (level.empty()) break;
    map2ret.levels.push_back(level);
    prev_used_tag = used_tag;
    used_tag = 3 - used_tag;
  }

  map2ret.code_bits = code[0]->code_bits;
  return map2ret;
}

void generate_verilog(const LevelsMap& lm, std::ostream& ost) {
  ost << "module labyrinth (\n"
         "  A,  //  binary input\n"
         "  Q   //  output\n"
         ");\n";

  ost << "  input [" << lm.code_bits - 1 << ":0] A;\n";
  ost << "  output [" << lm.levels.size() - 1 << ":0] Q;\n";

  ost << "  wire [" << lm.levels.size() -1 << ":0] Q;\n";

  for (unsigned level_idx = 0; level_idx < lm.levels.size(); level_idx++) {
    auto& level = lm.levels[level_idx];
    ost << "  wire [" << level.size() - 1 << ":0] term" << level_idx << ";\n";
    for (unsigned term_idx = 0; term_idx < lm.levels[level_idx].size(); term_idx++) {
      ost << "  assign term" << level_idx << "[" << term_idx << "] = (A == 'b"
          << lm.levels[level_idx][term_idx] << ");\n";
    }
    ost << "  assign Q[" << level_idx << "] = (| term" << level_idx << ");\n";
  }

  ost << "endmodule\n";

}

std::string conv2term(std::bitset<NMAX> bs, size_t code_bits) {
  std::stringstream ss;
  for (size_t idx = 0; idx < code_bits; idx ++) {
    if (bs[code_bits - idx - 1]) {
      ss << " A" << (code_bits - idx - 1);
    } else {
      ss << "~A" << (code_bits - idx - 1);
    }
    if (idx != code_bits - 1) {
      ss << " & ";
    }
  }
  std::string s2r = ss.str();
  return s2r;
}

void generate_verilog_dnf(const LevelsMap& lm, std::ostream& ost) {

  std::vector<std::bitset<NMAX> > terms_vec;
  for (unsigned level_idx = 0; level_idx < lm.levels.size(); level_idx++) {
    for (unsigned term_idx = 0; term_idx < lm.levels[level_idx].size(); term_idx++) {
      auto& term = lm.levels[level_idx][term_idx];
      if (std::find(terms_vec.begin(), terms_vec.end(), term) == terms_vec.end()) {
        terms_vec.push_back(term);
      }
    }
  }

  ost << "module labyrinth (\n";
  for (size_t in_idx = 0; in_idx < lm.code_bits; in_idx++) {
    ost << "  A" << in_idx << ",  //  binary input\n";
  }
  for (size_t out_idx = 0; out_idx < lm.levels.size(); out_idx++) {
    if (out_idx != lm.levels.size() - 1) {
      ost << "  Q" << out_idx << ",   //  output\n";
    } else {
      ost << "  Q" << out_idx << "   //  output\n";
    }
  }
  ost << ");\n";

  for (size_t in_idx = 0; in_idx < lm.code_bits; in_idx++) {
    ost << "  input A" << in_idx << ";\n";
  }
  for (size_t out_idx = 0; out_idx < lm.levels.size(); out_idx++) {
    ost << "  output Q" << out_idx << ";\n";
  }

  // Print terms.
  for (unsigned term_idx = 0; term_idx < terms_vec.size(); term_idx++) {
    ost << "  wire term" << term_idx << ";\n";
  }
  for (unsigned term_idx = 0; term_idx < terms_vec.size(); term_idx++) {
    ost << "  assign term" << term_idx << " = "
        << conv2term(terms_vec[term_idx], lm.code_bits) << ";\n";
  }

  for (unsigned level_idx = 0; level_idx < lm.levels.size(); level_idx++) {
    ost << "  assign Q" << level_idx << " = 1'b0";
    for (unsigned term_idx = 0; term_idx < lm.levels[level_idx].size(); term_idx++) {
      // Find term.
      auto& term = lm.levels[level_idx][term_idx];
      size_t vec_idx = std::find(terms_vec.begin(), terms_vec.end(), term) - terms_vec.begin();

      ost << " | term" << vec_idx << "";
    }
    ost << ";\n";
  }

  ost << "endmodule\n";

}
