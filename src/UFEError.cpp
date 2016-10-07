#include "UFEError.h"

UFEError::UFEError(const std::string &ErrorDescr,
                       const std::string &ErrorLocation,
                       int Svr)
: errorDescription_(ErrorDescr), errorLocation_(ErrorLocation), severity_(Svr) {}

std::string UFEError::GetDescription() {
    return "*** " + errorDescription_ + " ***";
}

std::string UFEError::GetLocation() {
  return severity_to_string() + " in " + errorLocation_;
}

std::string UFEError::severity_to_string() {
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


