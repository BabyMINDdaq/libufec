#ifndef UEF_ERROR_H
#define UEF_ERROR_H 1

// C++
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <exception>


class UEFError {

 public:
  enum {
    WARNING=0,
    SERIOUS=1,
    FATAL=2
  };

  UEFError(const std::string &ErrorDescr = "",
           const std::string &ErrorLocation = "",
           const int Svr = SERIOUS );

  // Default destructor has nothing to do
  // except be declared virtual.
  virtual ~UEFError() {}

  std::string GetDescription();
  std::string GetLocation();

private:
  std::string severity_to_string();

  std::string errorDescription_;
  std::string errorLocation_;
  int severity_;

};

#endif

