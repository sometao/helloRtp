#pragma once
#include "httplib.h"
#include <iostream>
#include <string>
namespace httpUtils {

using std::string;
using std::cout;
using std::endl;

namespace contentType {
  extern const char* html;
  extern const char* plain;
  extern const char* json;
}


using namespace httplib;

using FrontHandler = std::function<bool(const Request&, Response&)>;

using Handler = Server::Handler;

Handler insertFrontAction(const FrontHandler front, const Handler action);

Handler JsonReqAction1(Handler action);

bool jsonCheck(const Request& req, Response& res);

Handler JsonReqAction(Handler action);

Handler baseAction(const string& actionName, Handler action);

}  // namespace httplib
