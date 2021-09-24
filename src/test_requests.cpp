#include "test_requests.h"

using namespace std;

namespace Requests {
  void TestCompanies() {
    istringstream iss(ReadFileData("../test_inputs/test_requests.txt"));
    const auto input_doc = Json::Load(iss);
    const auto& input_map = input_doc.GetRoot().AsMap();
    vector<Companies> companies;
    for (const auto& node: input_map.at("stat_requests").AsArray()) {
      auto req = Read(node.AsMap());
      if (std::holds_alternative<Companies>(req))
        companies.push_back(std::get<Companies>(req));
    }
    ASSERT_EQUAL(companies.size(), 4);
    ASSERT_EQUAL(companies[0].filter.names, vector<string>{"им. Фрунзе"});
    ASSERT_EQUAL(companies[0].filter.urls, vector<string>{"www.park.ru"});
    ASSERT_EQUAL(companies[0].filter.rubrics, vector<string>{"Парк"});
    ASSERT_EQUAL(companies[0].filter.phones[0].type, "PHONE");
    ASSERT_EQUAL(companies[0].filter.phones[0].number, "2671646");
  }
} // ~Requests