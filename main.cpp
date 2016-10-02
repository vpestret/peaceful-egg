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

const size_t code_bits = 6;

int main(int argc, char* argv[]) {

  std::vector<std::shared_ptr<Vertex> > gcode = generate_code(0, code_bits);

  std::cout << "Generated code : ";
  for (const auto& vrx : gcode) {
    std::cout << (std::string)(*vrx) << " ";
  }
  std::cout << std::endl;

  intersect_code_spheres(gcode);

  std::cout << "Close spheres\n";
  for (const auto& vrx : gcode) {
    std::cout << vrx->string_w_sp1() << "\n";
  }
  std::cout << std::endl;

  auto maplayers = generate_map_from_code(gcode);

  std::cout << "Layers built\n";
  size_t layer_idx = 0;
  for (const auto& layer : maplayers) {
    std::cout << "Layer " << layer_idx << ": ";
    for (const auto& code : layer) {
      std::cout << Vertex::to_string(code, code_bits) << " ";
    }
    std::cout << "\n";
  }
  std::cout << std::endl;

  std::getchar();

  return 0;
}
