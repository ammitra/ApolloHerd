#include <iostream>
#include <array>
#include <memory>

#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

#include <msgpack.hpp>

#include "swatch/action/Board.hpp"
#include "swatch/action/Command.hpp"
#include "swatch/action/Device.hpp"
#include "swatch/action/DeviceStub.hpp"
#include "swatch/action/StateMachine.hpp"
#include "swatch/core/Factory.hpp"
#include "swatch/core/utilities.hpp"
#include "swatch/examples/utilities.hpp"
#include "swatch/server/Server.hpp"
#include "swatch/server/utilities.hpp"


using namespace swatch;
using namespace swatch::action;
using namespace swatch::core;



int main (int argc, char* argv[] )
{
  if (argc == 1) {
    std::cout << "ERROR! Missing argument: path to YAML config file" << std::endl;
    return 1;
  }
  else if (argc > 2) {
    std::cout << "ERROR! Only one argument expected." << std::endl;
  }
  else if ((strcmp(argv[1], "-h") == 0) or (strcmp(argv[1], "--help") == 0)) {
    std::cout << "usage: " << argv[0] << " YAML_CONFIG_FILE_PATH" << std::endl;
    return 0;
  }

  std::cout << "Parsing config file" << std::endl;
  const AppConfig cfg = parseConfigFile(argv[1]);

  std::cout << "Loading libraries" << std::endl;
  loadLibraries(cfg.libraries, boost::filesystem::path(argv[1]).parent_path());

  std::cout << "Constructing board & device objects" << std::endl;
  Board board(cfg.devices);

  std::cout << std::endl;
  printCommandSummary(board);

  std::cout << std::endl << "Starting server on port 12345" << std::endl;
  server::Server srv(board, 12345);
  srv.start();

  return 0;
}

