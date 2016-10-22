#include <iostream>
#include <vector>
#include <list>
#include <random>
#include <algorithm>
#include <bitset>
#include <memory>
#include <sstream>
#include "codegen.h"
#include "mapgen.h"
#include <fstream>

const size_t rnd_seed = 0;
const size_t code_bits = 6;
const size_t num_levels2gen = 3;

int main(int argc, char* argv[]) {

  std::vector<std::shared_ptr<Vertex> > gcode = generate_code_d3(rnd_seed, code_bits);

  std::cout << "Generated code : ";
  for (const auto& vrx : gcode) {
    std::cout << (std::string)(*vrx) << " ";
  }
  std::cout << std::endl;

  intersect_code_spheres(gcode, SP1xSP2);

  std::cout << "Close spheres\n";
  for (const auto& vrx : gcode) {
    std::cout << vrx->string_w_sp1() << "\n";
  }
  std::cout << std::endl;

  auto levelsmap = generate_map_from_code(rnd_seed, gcode, num_levels2gen, SP1xSP2);

  std::cout << "Layers built\n";
  size_t layer_idx = 0;
  for (const auto& layer : levelsmap.levels) {
    std::cout << "Layer " << layer_idx << ": ";
    for (const auto& code : layer) {
      std::cout << Vertex::to_string(code, code_bits) << " ";
    }
    std::cout << "\n";
    layer_idx++;
  }
  std::cout << std::endl;

  std::ofstream ofs("labyrinth.v");
  generate_verilog_dnf(levelsmap, ofs);
  ofs.close();

  std::getchar();

  return 0;
}
