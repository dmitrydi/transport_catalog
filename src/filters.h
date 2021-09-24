#pragma once

#include "json.h"
#include "utils.h"
#include <string>
#include <vector>
#include <iostream>

namespace Filters{
  struct Phone {
    std::string type = "";
    std::string country_code = "";
    std::string local_code = "";
    std::string number = "";
    std::string extension = "";
    bool is_empty() const;
    static Phone ParseFrom(const Json::Dict& input);
  };

  using Names = std::vector<std::string>;
  using Urls = std::vector<std::string>;
  using Rubrics = std::vector<std::string>;
  using Phones = std::vector<Phone>;

  struct Filter {
    Names names;
    Urls urls;
    Rubrics rubrics;
    Phones phones;

    static Filter ParseFrom(const Json::Dict& input_map);
  };

  std::ostream& operator<<(std::ostream& os, const Filters::Phone& phone);
} // ~Filters

//std::ostream& operator<<(std::ostream& os, const Filters::Phone& phone);
