#include "UEFError.h"

UEFError::UEFError(const std::string &ErrorDescr,
                       const std::string &ErrorLocation,
                       int Svr)
: errorDescription_(ErrorDescr), errorLocation_(ErrorLocation), severity_(Svr) {}

std::string UEFError::GetDescription() {
    return "*** " + errorDescription_ + " ***";
}

std::string UEFError::GetLocation() {
  return severity_to_string() + " in " + errorLocation_;
}

std::string UEFError::severity_to_string() {
  std::string s;
  switch (severity_) {
    case WARNING:
      s = "WARNING";
      break;

    case SERIOUS:
      s = "SERIOUS ERROR";
      break;

    case FATAL:
      s = "FATAL ERROR";
      break;
  }
  return s;
}


