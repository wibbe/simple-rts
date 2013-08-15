
#include "config.h"
#include "tcl.h"

#include <iostream>
#include <map>

namespace tcl {

  // -- Externs

  double calculateExpr(std::string const& str);

  // -- Utils --

  void split(std::string const& input, std::string const& delims, std::vector<std::string> & result)
  {
    std::string::size_type lastPos = input.find_first_not_of(delims, 0);
    std::string::size_type pos = input.find_first_of(delims, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
      result.push_back(input.substr(lastPos, pos - lastPos));

      lastPos = input.find_first_not_of(delims, pos);
      pos = input.find_first_of(delims, lastPos);
    }
  }

  // -- CallFrame --

  typedef std::map<std::string, std::string> VariableMap;

  struct CallFrame
  {
    CallFrame()
      : result("")
    { }

    void set(std::string const& name, std::string const& value)
    {
      variables[name] = value;
    }

    std::string get(std::string const& name)
    {
      VariableMap::const_iterator result = variables.find(name);
      return result == variables.end() ? "" : result->second;
    }

    bool get(std::string const& name, std::string & value) const
    {
      VariableMap::const_iterator result = variables.find(name);
      if (result == variables.end())
        return false;
      value = result->second;
      return true;
    }

    VariableMap variables;
    std::string result;
  };

  // -- Parser --

  enum Token
  {
    EndOfLine,
    EndOfFile,
    Separator,
    String,
    Variable,
    Escaped,
    Command,
    Append,
    Error
  };

  static const char * tokenAsReadable[] = {
    "EndOfLine",
    "EndOfFile",
    "Separator",
    "String",
    "Variable",
    "Escaped",
    "Command",
    "Append",
    "Error"
  };

  struct Parser
  {
    Parser(std::string const& code)
      : code(code),
        value(""),
        token(EndOfLine),
        current(code.empty() ? 0 : code[0]),
        pos(0),
        insideString(false)
    { }

    bool next();
    void inc() { if (pos < code.size()) current = code[++pos]; }
    bool eof() const { return len() <= 0; }
    size_t len() const { return code.size() - pos; }

    std::string code;
    std::string value;
    Token token;
    char current;
    size_t pos;
    bool insideString;
  };

  inline bool isSeparator(char t)
  {
    return t == ' ' || t == '\t' || t == '\n';
  }

  static bool parseSeparator(Parser * p)
  {
    p->value = "";

    while (true)
    {
      if (isSeparator(p->current))
        p->inc();
      else
        break;
    }
    p->token = Separator;
    return true;
  }

  static bool parseBracet(Parser * p)
  {
    int level = 1;
    p->inc();
    p->value = "";

    while (true)
    {
      if (p->len() >= 2 && p->current == '\\')
      {
        p->inc();
        p->value += p->current;
      }
      else if (p->len() == 0 || p->current == '}')
      {
        level--;

        if (level == 0 || p->len() == 0)
        {
          if (!p->eof())
            p->inc();

          p->token = String;
          return true;
        }
      }
      else if (p->current == '{')
      {
        level++;
      }

      p->value += p->current;
      p->inc();
    }

    return true;
  }

  static bool parseComment(Parser * p)
  {
    while (!p->eof() && p->current != '\n')
      p->inc();
    return true;
  }

  static bool parseCommand(Parser * p)
  {
    int outerLevel = 1;
    int innerLevel = 0;

    p->inc();
    p->value = "";

    while (true)
    {
      if (p->eof())
        break;
      else if (p->current == '[' && innerLevel == 0)
      {
        outerLevel++;
      }
      else if (p->current == ']' && innerLevel == 0)
      {
        if (--outerLevel == 0)
          break;
      }
      else if (p->current == '\\')
      {
        p->inc();
      }
      else if (p->current == '{')
      {
        innerLevel++;
      }
      else if (p->current == '}')
      {
        if (innerLevel > 0)
          innerLevel--;
      }

      p->value += p->current;
      p->inc();
    }

    if (p->current == ']')
      p->inc();

    p->token = Command;
    return true;
  }

  static bool parseEndOfLine(Parser * p)
  {
    while (isSeparator(p->current) || p->current == ';')
      p->inc();

    p->token = EndOfLine;
    return true;
  }

  static bool parseVariable(Parser * p)
  {
    p->value = "";
    p->inc(); // eat $

    while (true)
    {
      if ((p->current >= 'a' && p->current <= 'z') || (p->current >= 'A' && p->current <= 'Z') || (p->current >= '0' && p->current <= '9'))
      {
        p->value += p->current;
        p->inc();
      }
      else
        break;
    }

    if (p->value.empty()) // This was just a single character string "$"
    {
      p->value = "$";
      p->token = String;
    }
    else
      p->token = Variable;

    return true;
  }

  static bool parseString(Parser * p)
  {
    const bool newWord = (p->token == Separator || p->token == EndOfLine || p->token == String);

    if (newWord && p->current == '{')
      return parseBracet(p);
    else if (newWord && p->current == '"')
    {
      p->insideString = true;
      p->inc();
    }

    p->value = "";

    while (true)
    {
      if (p->eof())
      {
        p->token = Escaped;
        return true;
      }

      switch (p->current)
      {
        case '\\':
          if (p->len() >= 2)
          {
            p->value += p->current;
            p->inc();
          }
          break;

        case '$': case '[':
          p->token = Escaped;
          return true;

        case ' ': case '\t': case '\n': case ';':
          if (!p->insideString)
          {
            p->token = Escaped;
            return true;
          }
          break;

        case '"':
          if (p->insideString)
          {
            p->inc();
            p->token = Escaped;
            p->insideString = false;
            return true;
          }
          break;
      }

      p->value += p->current;
      p->inc();
    }

    return String;
  }

  bool Parser::next()
  {
    value = "";
    if (pos == code.size())
    {
      token = EndOfFile;
      return true;
    }

    while (true)
    {
      switch (current)
      {
        case ' ': case '\t': case '\r':
          if (insideString)
            return parseString(this);
          else
            return parseSeparator(this);

        case '\n': case ';':
          if (insideString)
            return parseString(this);
          else
            return parseEndOfLine(this);

        case '$':
          return parseVariable(this);

        case '[':
          return parseCommand(this);

        case '#':
          parseComment(this);
          continue;

        default:
          return parseString(this);
      }
    }
  }

  // -- Globals --

  typedef std::map<std::string, Procedure *> ProcedureMap;
  typedef std::vector<std::string> ArgumentVector;
  struct ProcReg;

  namespace {
    ProcedureMap _procedures;
    std::vector<CallFrame> _frames;
    bool _debug = false;
    std::string _error = "";
    ProcReg * _procRegHead = NULL;
    ProcReg * _procRegTail = NULL;
  }

  inline CallFrame & currentFrame()
  {
    return _frames.back();
  }

  struct ProcReg
  {
    ProcReg(const char * name, Procedure * proc)
      : name(name),
        proc(proc),
        next(NULL)
    {
      if (_procRegHead)
      {
        _procRegTail->next = this;
        _procRegTail = this;
      }
      else
      {
        _procRegHead = this;
        _procRegTail = this;
      }
    }

    const char * name;
    Procedure * proc;
    ProcReg * next;
  };

  // -- Interface --

  void init()
  {
    reset();
  }

  void shutdown()
  {
    _procedures.clear();
  }

  void reset()
  {
    _procedures.clear();
    _frames.clear();

    ProcReg * it = _procRegHead;
    for (; it; it = it->next)
      _procedures.insert(std::make_pair(std::string(it->name), it->proc));

    _frames.push_back(CallFrame());
  }

  ReturnCode evaluate(std::string const& code)
  {
    Parser parser(code);

    currentFrame().result = "";
    std::vector<std::string> args;

    while (true)
    {
      Token previousToken = parser.token;
      if (!parser.next())
        return RET_ERROR;

      if (_debug)
        std::cout << "Token: " << tokenAsReadable[parser.token] << " = '" << parser.value << "'" << std::endl;

      std::string value = parser.value;
      if (parser.token == Variable)
      {
        std::string var;
        if (currentFrame().get(value, var))
          currentFrame().result = value = var;
        else
          return _reportError("Could not locate variable '" + value + "'");
      }
      else if (parser.token == Command)
      {
        if (!evaluate(parser.value))
          return RET_ERROR;

        value = currentFrame().result;
      }
      else if (parser.token == Separator)
      {
        previousToken = parser.token;
        continue;
      }

      if (parser.token == EndOfLine || parser.token == EndOfFile)
      {
        if (_debug)
        {
          std::cout << "Evaluating: ";

          for (size_t i = 0; i < args.size(); ++i)
            std::cout << args[i] << ",";
          std::cout << std::endl;
        }

        if (args.size() >= 1)
        {
          ProcedureMap::iterator it = _procedures.find(args[0]);
          if (it == _procedures.end())
            return _reportError("Could not find procedure '" + args[0] + "'");

          Procedure * proc = it->second;

          ReturnCode retCode = proc->call(args);
          if (retCode != RET_OK)
            return retCode;
        }

        args.clear();
      }

      if (!value.empty())
      {
        if (previousToken == Separator || previousToken == EndOfLine)
        {
          args.push_back(value);
        }
        else
        {
          if (args.empty())
            args.push_back(value);
          else
            args.back() += value;
        }
      }

      if (parser.token == EndOfFile)
        break;
    }

    return RET_OK;
  }

  bool _registerProc(const char * name, Procedure * function)
  {
    new ProcReg(name, function);
    return true;
  }

  ReturnCode _arityError(std::string const& command)
  {
    return _reportError("Wrong number of arguments to procedure '" + command + "'");
  }

  ReturnCode _reportError(std::string const& error)
  {
    _error = error;
    fprintf(stderr, "TCL ERROR: %s\n", error.c_str());
    return RET_ERROR;
  }

  void _return(std::string const& value)
  {
    currentFrame().result = value;
  }

  // -- Build in functions --

  #define BUILT_IN(name) \
    struct BuiltIn_##name : public Procedure { ReturnCode call(ArgumentVector const& args); }; \
    namespace { bool __dummy##name = _registerProc(#name, new BuiltIn_##name); } \
    ReturnCode BuiltIn_##name::call(ArgumentVector const& args)

  struct TclProc :public Procedure
  {
    ReturnCode call(ArgumentVector const& args)
    {
      if ((args.size() - 1) != arguments.size())
        return _reportError("Procedure '" + args[0] + "' called with wrong number of arguments");

      _frames.push_back(CallFrame());

      // Setup arguments
      for (size_t i = 0, len = arguments.size(); i < len; ++i)
        currentFrame().set(arguments[i], args[i + 1]);

      ReturnCode retCode = evaluate(body);
      std::string result = currentFrame().result;
      _frames.pop_back();
      currentFrame().result = result;

      return retCode;
    };

    std::string body;
    ArgumentVector arguments;
  };

  BUILT_IN(proc)
  {
    if (args.size() != 4)
      return _arityError(args[0]);

    if (_procedures.find(args[1]) != _procedures.end())
      return _reportError("Procedure of name '" + args[1] + "' already exists");

    TclProc * procData = new TclProc;
    procData->body = args[3];
    split(args[2], " \t", procData->arguments);

    _procedures.insert(std::make_pair(args[1], procData));
    return RET_OK;
  }

  BUILT_IN(puts)
  {
    if (args.size() > 1)
      for (size_t i = 1, len = args.size(); i < len; ++i)
        std::cout << args[i] << (((i - 1) == len) ? "" : " ");

    std::cout << std::endl;
    return RET_OK;
  }

  BUILT_IN(set)
  {
    const size_t len = args.size();

    if (len == 2)
    {
      return currentFrame().get(args[1], currentFrame().result) ? RET_OK : RET_ERROR;
    }
    else if (len == 3)
    {
      currentFrame().result = args[2];
      currentFrame().set(args[1], args[2]);
      return RET_OK;
    }
    else
      return _arityError(args[0]);
  }

  BUILT_IN(expr)
  {
    if (args.size() == 1)
      return _arityError(args[0]);

    std::string str;
    for (size_t i = 1; i < args.size(); ++i)
      str += args[i];

    _error = "";
    double result = calculateExpr(str);
    char buf[128];
    snprintf(buf, 128, "%f", result);
    currentFrame().result = std::string(buf);
    return _error.empty() ? RET_OK : RET_ERROR;
  }

  BUILT_IN(if)
  {
    if (args.size() != 3 && args.size() != 5)
      return _arityError(args[0]);

    double result = calculateExpr(args[1]);

    if (result > 0.0)
      return evaluate(args[2]);
    else if (args.size() == 5)
      return evaluate(args[4]);

    return RET_OK;
  }

  BUILT_IN(return)
  {
    if (args.size() != 2)
      return _arityError(args[0]);

    currentFrame().result = args[1];
    return RET_RETURN;
  }

  BUILT_IN(error)
  {
    if (args.size() != 2)
      return _arityError(args[0]);
    return _reportError(args[1]);
  }

  BUILT_IN(eval)
  {
    if (args.size() != 2)
      return _arityError(args[0]);

    std::string str;
    for (size_t i = 1; i < args.size(); ++i)
      str += args[i] + " ";

    return evaluate(str);
  }

  BUILT_IN(while)
  {
    if (args.size() != 3)
      return _arityError(args[0]);

    const std::string check = "expr " + args[1];

    while (true)
    {
      ReturnCode retCode = evaluate(check);
      if (retCode != RET_OK)
        return retCode;

      if (std::atof(currentFrame().result.c_str()) > 0.0)
      {
        retCode = evaluate(args[2]);
        if (retCode == RET_OK || retCode == RET_CONTINUE)
          continue;
        else if (retCode == RET_BREAK)
          return RET_OK;
        else
          return retCode;
      }
      else
      {
        return RET_OK;
      }
    }
  }

  BUILT_IN(incr)
  {
    if (args.size() != 2 && args.size() != 3)
      return _arityError(args[0]);

    std::string var;
    if (!currentFrame().get(args[1], var))
      return _reportError("Could not find variable '" + args[1] + "'");

    int inc = 1;
    int value = std::atoi(var.c_str());

    if (args.size() == 3)
      inc = std::atoi(args[2].c_str());

    char buf[32];
    snprintf(buf, 32, "%d", value + inc);
    currentFrame().set(args[1], std::string(buf));
    currentFrame().result = std::string(buf);
    return RET_OK;
  }

  BUILT_IN(break)
  {
    return RET_BREAK;
  }

  BUILT_IN(continue)
  {
    return RET_CONTINUE;
  }

  #undef BUILT_IN
}