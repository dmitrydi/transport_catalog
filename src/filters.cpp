#include "filters.h"

using namespace std;

namespace Filters {
  bool Phone::is_empty() const {
    return type.empty() && country_code.empty() && local_code.empty() && number.empty() && extension.empty();
  }

  Phone Phone::ParseFrom(const Json::Dict& input) {
    /*
    std::string country_code;
    std::string local_code;
    std::string number;
    std::string extension;
     */
    Phone result;
    if (input.count("type"))
      result.type = input.at("type").AsString();
    if (input.count("country_code"))
      result.country_code = input.at("country_code").AsString();
    if (input.count("local_code"))
      result.local_code = input.at("local_code").AsString();
    if (input.count("number"))
      result.number = input.at("number").AsString();
    if (input.count("extension"))
      result.extension = input.at("extension").AsString();
    return result;
  }

  Filter Filter::ParseFrom(const Json::Dict& input) {

    Filter result;

    if (input.count("names")) {
      result.names.reserve(input.at("names").AsArray().size());
      for (const auto& name: input.at("names").AsArray())
        result.names.push_back(name.AsString());
    }
    if (input.count("urls")) {
      result.urls.reserve(input.at("urls").AsArray().size());
      for (const auto& url: input.at("urls").AsArray())
        result.urls.push_back(url.AsString());
    }
    if (input.count("rubrics")) {
      result.rubrics.reserve(input.at("rubrics").AsArray().size());
      for (const auto& rub: input.at("rubrics").AsArray())
        result.rubrics.push_back(rub.AsString());
    }
    if (input.count("phones")) {
      result.phones.reserve(input.at("phones").AsArray().size());
      for (const auto& phone: input.at("phones").AsArray())
        result.phones.push_back(Phone::ParseFrom(phone.AsMap()));
    }
    return result;
  }

} //~Filters

namespace Filters {

std::ostream& operator<<(std::ostream& os, const Filters::Phone& phone) {
  /*
       std::string type;
    std::string country_code;
    std::string local_code;
    std::string number;
    std::string extension;
   */
  os << "{";
  bool first = true;
  if (!phone.type.empty()) {
    os << GuardedString("type")  << ": " << GuardedString(phone.type);
    first = false;
  }
  if (!phone.country_code.empty()) {
    if (!first)
      os << ", ";
    first = false;
    os << GuardedString("country_code") << ": " << GuardedString(phone.country_code);
  }
  if (!phone.local_code.empty()) {
    if (!first)
          os << ", ";
        first = false;
    os << GuardedString("local_code") << ": " << GuardedString(phone.local_code);
  }

  if (!phone.number.empty()) {
    if (!first)
          os << ", ";
        first = false;
    os << GuardedString("number") <<": " << GuardedString(phone.number);
  }
  if (!phone.extension.empty()) {
    if (!first)
          os << ", ";
        first = false;
    os << GuardedString("extension") << ": " << GuardedString(phone.extension);
  }
  os << "}";
  return os;
}
}
