
#include "input.h"
#include "tcl.h"

#include <map>

namespace input
{
  struct Command
  {
    std::string downCmd;
    std::string upCmd;
  };

  namespace {
    std::map<std::string, Command> _inputCommands;
    int _mouseX = 0;
    int _mouseY = 0;
  }

  void init()
  {
  }

  void shutdown()
  {
    _inputCommands.clear();
  }

  void key(std::string const& name, bool down)
  {
    std::map<std::string, Command>::iterator result = _inputCommands.find(name);
    if (result == _inputCommands.end())
      return;

    Command & cmd = result->second;
    if (down && !cmd.downCmd.empty())
      tcl::evaluate(cmd.downCmd);
    else if (!down && !cmd.upCmd.empty())
      tcl::evaluate(cmd.upCmd);
  }

  void setMousePos(int x, int y)
  {
    _mouseX = x;
    _mouseY = y;
  }

  int32_t mouseX()
  {
    return _mouseX;
  }

  int32_t mouseY()
  {
    return _mouseY;
  }

  static void bind(std::string const& key, std::string const& down)
  {
    Command cmd;
    cmd.downCmd = down;
    cmd.upCmd = "";

    _inputCommands[key] = cmd;
  }

  static void bind2(std::string const& key, std::string const& down, std::string const& up)
  {
    Command cmd;
    cmd.downCmd = down;
    cmd.upCmd = up;

    _inputCommands[key] = cmd;
  }

  PROC("input::bind", bind);
  PROC("input::bind2", bind2);
  PROC("input::mouseX", mouseX);
  PROC("input::mouseY", mouseY);

}