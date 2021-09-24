#include "test_filters.h"

using namespace std;

namespace Filters {
  void TestPhoneParsing() {
    /*
    std::string type;
    std::string country_code;
    std::string local_code;
    std::string number;
    std::string extension;
     */
    {
      istringstream iss(R"({
                    "type": "PHONE",
                    "country_code": "7",
                    "local_code": "123",
                    "number": "2671646",
                    "extension": "0"
                })");
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Phone phone = Phone::ParseFrom(input_map);
      ASSERT_EQUAL(phone.type, "PHONE");
      ASSERT_EQUAL(phone.country_code, "7");
      ASSERT_EQUAL(phone.local_code, "123");
      ASSERT_EQUAL(phone.number, "2671646");
      ASSERT_EQUAL(phone.extension, "0");
    }
  }
  
  void TestFilterParsing() {
    istringstream iss (R"(        {
            "id": 162389115,
            "type": "FindCompanies",
            "names": [
                "Комсомольский", "Komsomolsky"
            ],
            "rubrics": [
                "Вокзал", "Vokzal", "Railway"
            ],
            "phones": [
                {
                    "type": "PHONE",
                    "number": "2671646"
                }
            ],
            "urls": ["www.1.ru", "www.2.ru"]
        })");
    const auto input_doc = Json::Load(iss);
    const auto& input_map = input_doc.GetRoot().AsMap();
    Filter filter = Filter::ParseFrom(input_map);
    ASSERT_EQUAL(filter.names, vector<string>({"Комсомольский", "Komsomolsky"}));
    ASSERT_EQUAL(filter.rubrics, vector<string>({"Вокзал", "Vokzal", "Railway"}));
    ASSERT_EQUAL(filter.urls, vector<string>({"www.1.ru", "www.2.ru"}));
    ASSERT_EQUAL(filter.phones.size(), 1);
    ASSERT_EQUAL(filter.phones[0].type, "PHONE");
    ASSERT_EQUAL(filter.phones[0].number, "2671646");
  }
} //~Filters