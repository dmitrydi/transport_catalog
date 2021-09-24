#include "test_descriptions.h"

using namespace std;

namespace Descriptions {
  void TestAddress() {
    { // Test Parsing
      istringstream iss(
          R"({
              "formatted": "test formatted",
               "coords": {
                   "lat": "43.573226",
                   "lon": "39.742947"
                },
              "components": [{"value": "Moscow"}, {"value": "SPb"}]
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Address addr = Address::ParseFrom(input_map);
      ASSERT_EQUAL(addr.formatted, "test formatted");
      ASSERT_EQUAL(addr.components.size(), 2);
      ASSERT_EQUAL(addr.components[0].value, "Moscow");
      ASSERT_EQUAL(addr.components[1].value, "SPb");
      ASSERT(addr.coords);
      ASSERT_CLOSE(addr.coords->latitude, 43.573226);
      ASSERT_CLOSE(addr.coords->longitude, 39.742947);
      ASSERT(addr.comment.empty());
    }
    { // Test Save and Load
      istringstream iss(
          R"({
              "formatted": "test formatted",
               "coords": {
                   "lat": "43.573226",
                   "lon": "39.742947"
                },
              "components": [{"value": "Moscow"}, {"value": "SPb"}]
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Address addr = Address::ParseFrom(input_map);
      YellowPages::Address proto;
      addr.Serialize(proto);
      const Address cmp_addr = Address::Deserialize(proto);
      ASSERT_EQUAL(addr.comment, cmp_addr.comment);
      ASSERT_EQUAL(addr.formatted, cmp_addr.formatted);
      ASSERT_CLOSE(addr.coords->latitude, cmp_addr.coords->latitude);
      ASSERT_CLOSE(addr.coords->longitude, addr.coords->longitude);
      ASSERT_EQUAL(addr.components.size(), cmp_addr.components.size());
      int c = addr.components.size();
      for (int i = 0; i < c; ++i)
        ASSERT_EQUAL(addr.components[i].value, cmp_addr.components[i].value)
    }
  }

  void TestName() {
    // Test Parsing
    {
      istringstream iss(
          R"({
              "value": "Name 1"
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Name name = Name::ParseFrom(input_map);
      ASSERT_EQUAL(name.value, "Name 1");
      ASSERT_EQUAL(name.type, Name::Type::MAIN);
    }
    {
      istringstream iss(
          R"({
              "value": "Name 1",
              "type": "MAIN"
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Name name = Name::ParseFrom(input_map);
      ASSERT_EQUAL(name.value, "Name 1");
      ASSERT_EQUAL(name.type, Name::Type::MAIN);
    }
    {
      istringstream iss(
          R"({
              "value": "Name 1",
              "type": "SHORT"
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Name name = Name::ParseFrom(input_map);
      ASSERT_EQUAL(name.value, "Name 1");
      ASSERT_EQUAL(name.type, Name::Type::SHORT);
    }
    {
      istringstream iss(
          R"({
              "value": "Name 1",
              "type": "SYNONYM"
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Name name = Name::ParseFrom(input_map);
      ASSERT_EQUAL(name.value, "Name 1");
      ASSERT_EQUAL(name.type, Name::Type::SYNONYM);
    }
    {
      istringstream iss(
          R"({
              "value": "Name 1",
              "type": "SPECIAL"
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      try {
        Name name = Name::ParseFrom(input_map);
      } catch (Name::UnknownTypeError&) {

      }
    }
    // ~Test Parsing

    // Lest Save & Load
    {
      istringstream iss(
          R"({
              "value": "Name 1"
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Name name = Name::ParseFrom(input_map);
      YellowPages::Name proto;
      name.Serialize(proto);
      Name loaded = Name::Deserialize(proto);
      ASSERT_EQUAL(loaded.value, "Name 1");
      ASSERT_EQUAL(loaded.type, Name::Type::MAIN);
    }
    {
      istringstream iss(
          R"({
              "value": "Name 1",
              "type": "MAIN"
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Name name = Name::ParseFrom(input_map);
      YellowPages::Name proto;
      name.Serialize(proto);
      Name loaded = Name::Deserialize(proto);
      ASSERT_EQUAL(loaded.value, "Name 1");
      ASSERT_EQUAL(loaded.type, Name::Type::MAIN);
    }
    {
      istringstream iss(
          R"({
              "value": "Name 1",
              "type": "SHORT"
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Name name = Name::ParseFrom(input_map);
      YellowPages::Name proto;
      name.Serialize(proto);
      Name loaded = Name::Deserialize(proto);
      ASSERT_EQUAL(loaded.value, "Name 1");
      ASSERT_EQUAL(loaded.type, Name::Type::SHORT);
    }
    {
      istringstream iss(
          R"({
              "value": "Name 1",
              "type": "SYNONYM"
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Name name = Name::ParseFrom(input_map);
      YellowPages::Name proto;
      name.Serialize(proto);
      Name loaded = Name::Deserialize(proto);
      ASSERT_EQUAL(loaded.value, "Name 1");
      ASSERT_EQUAL(loaded.type, Name::Type::SYNONYM);
    }
  }
  void TestPhone() {
    // Test Parsing
    {
      istringstream iss(
          R"({
                "type": "PHONE",
                "country_code": "7",
                "local_code": "862",
                "number": "2671842"
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Phone phone = Phone::ParseFrom(input_map);
      ASSERT_EQUAL(phone.type, Phone::Type::PHONE);
      ASSERT_EQUAL(phone.country_code, "7");
      ASSERT_EQUAL(phone.local_code, "862");
      ASSERT_EQUAL(phone.number, "2671842");
    }
    {
      istringstream iss(
          R"({
                "type": "FAX",
                "country_code": "7",
                "local_code": "862",
                "number": "2671842",
                "extension": "333",
                "description": "description"
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Phone phone = Phone::ParseFrom(input_map);
      ASSERT_EQUAL(phone.type, Phone::Type::FAX);
      ASSERT_EQUAL(phone.country_code, "7");
      ASSERT_EQUAL(phone.local_code, "862");
      ASSERT_EQUAL(phone.number, "2671842");
      ASSERT_EQUAL(phone.extension, "333");
      ASSERT_EQUAL(phone.description, "description")
    }
    {
      istringstream iss(
          R"({
                "country_code": "7",
                "local_code": "862",
                "number": "2671842"
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Phone phone = Phone::ParseFrom(input_map);
      ASSERT(!phone.type);
      ASSERT_EQUAL(phone.country_code, "7");
      ASSERT_EQUAL(phone.local_code, "862");
      ASSERT_EQUAL(phone.number, "2671842");
    } // ~ Test Parsing
    {
      istringstream iss(
          R"({
                "country_code": "7",
                "local_code": "862",
                "number": "2671842"
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Phone phone = Phone::ParseFrom(input_map);
      YellowPages::Phone proto;
      phone.Serialize(proto);
      Phone loaded_phone = Phone::Deserialize(proto);
      //ASSERT(!loaded_phone.type);
      ASSERT_EQUAL(phone.country_code, loaded_phone.country_code);
      ASSERT_EQUAL(phone.local_code, loaded_phone.local_code);
      ASSERT_EQUAL(phone.number, loaded_phone.number);
    }
    {
      istringstream iss(
          R"({
                "type": "FAX",
                "country_code": "7",
                "local_code": "862",
                "number": "2671842",
                "extension": "333",
                "description": "description"
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Phone phone = Phone::ParseFrom(input_map);
      YellowPages::Phone proto;
      phone.Serialize(proto);
      Phone loaded_phone = Phone::Deserialize(proto);
      ASSERT_EQUAL(phone.type, loaded_phone.type);
      ASSERT_EQUAL(phone.country_code, loaded_phone.country_code);
      ASSERT_EQUAL(phone.local_code, loaded_phone.local_code);
      ASSERT_EQUAL(phone.number, loaded_phone.number);
      ASSERT_EQUAL(phone.extension, loaded_phone.extension);
      ASSERT_EQUAL(phone.description, loaded_phone.description);
    }
    {
      istringstream iss(
          R"({
                "type": "PHONE",
                "country_code": "7",
                "local_code": "862",
                "number": "2671842",
                "extension": "333",
                "description": "description"
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Phone phone = Phone::ParseFrom(input_map);
      YellowPages::Phone proto;
      phone.Serialize(proto);
      Phone loaded_phone = Phone::Deserialize(proto);
      ASSERT_EQUAL(phone.type, loaded_phone.type);
      ASSERT_EQUAL(phone.country_code, loaded_phone.country_code);
      ASSERT_EQUAL(phone.local_code, loaded_phone.local_code);
      ASSERT_EQUAL(phone.number, loaded_phone.number);
      ASSERT_EQUAL(phone.extension, loaded_phone.extension);
      ASSERT_EQUAL(phone.description, loaded_phone.description);
    }
  }
  void TestUrl() {
    {
      istringstream iss(
          R"({
              "value": "www.example.com"
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Url url = Url::ParseFrom(input_map);
      ASSERT_EQUAL(url.value, "www.example.com");
    }
    {
        istringstream iss(
            R"({
                "value": "http://dendrarium.ru"
              })"
            );
        const auto input_doc = Json::Load(iss);
        const auto& input_map = input_doc.GetRoot().AsMap();
        Url url = Url::ParseFrom(input_map);
        YellowPages::Url proto;
        url.Serialize(proto);
        Url loaded_url = Url::Deserialize(proto);
        ASSERT_EQUAL(url.value, loaded_url.value);
    }
  }
  void TestWorkingTime() {
    // Test Parsing
    {
      istringstream iss(
          R"({
              "day": "THURSDAY",
              "minutes_from": 123,
              "minutes_to": 567
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      WorkingTimeInterval interval = WorkingTimeInterval::ParseFrom(input_map);
      ASSERT(interval.day == WorkingTimeInterval::Day::THURSDAY);
      ASSERT_EQUAL(interval.minutes_from, 123);
      ASSERT_EQUAL(interval.minutes_to, 567);
    }
    // Test Save and Load
    {
      istringstream iss(
          R"({
              "day": "THURSDAY",
              "minutes_from": 123,
              "minutes_to": 567
            })"
          );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      WorkingTimeInterval interval = WorkingTimeInterval::ParseFrom(input_map);
      YellowPages::WorkingTimeInterval proto;
      interval.Serialize(proto);
      WorkingTimeInterval loaded_interval = WorkingTimeInterval::Deserialize(proto);
      ASSERT(interval.day == loaded_interval.day);
      ASSERT_EQUAL(interval.minutes_from, loaded_interval.minutes_from);
      ASSERT_EQUAL(interval.minutes_to, loaded_interval.minutes_to);
    }
  }
  void TestNearbyStop() {
    {
      istringstream iss(
                R"({
                      "name": "Пансионат Светлана",
                      "meters": 580
                  })"
                );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      NearbyStop ns = NearbyStop::ParseFrom(input_map);
      ASSERT_EQUAL(ns.meters, 580);
      ASSERT_EQUAL(ns.name, "Пансионат Светлана");
    }
    {
      istringstream iss(
                R"({
                      "name": "Пансионат Светлана",
                      "meters": 580
                  })"
                );
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      NearbyStop ns = NearbyStop::ParseFrom(input_map);
      YellowPages::NearbyStop proto;
      ns.Serialize(proto);
      NearbyStop loaded_ns = NearbyStop::Deserialize(proto);
      ASSERT_EQUAL(ns.name, loaded_ns.name);
      ASSERT_EQUAL(ns.meters, loaded_ns.meters);
    }
  }
  void TestCompany() {
    {
      istringstream iss(ReadFileData("../test_inputs/test_company_parsing.txt"));
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Company company = Company::ParseFrom(input_map);
      ASSERT(company.address);
      ASSERT_CLOSE(company.address->coords->latitude, 43.573226);
      ASSERT_CLOSE(company.address->coords->longitude, 39.742947);
      ASSERT_EQUAL(company.names.size(), 1);
      ASSERT_EQUAL(company.names[0].value, "Дендрарий");
      ASSERT(company.names[0].type == Name::Type::MAIN);
      ASSERT_EQUAL(company.urls.size(), 1);
      ASSERT_EQUAL(company.urls[0].value, "http://dendrarium.ru");
      ASSERT_EQUAL(company.rubrics.size(), 1);
      ASSERT_EQUAL(company.rubrics[0], 1);
      ASSERT_EQUAL(company.nearby_stops.size(),1);
      ASSERT_EQUAL(company.nearby_stops[0].name, "Цирк");
      ASSERT_EQUAL(company.nearby_stops[0].meters, 180);
      ASSERT_EQUAL(company.phones.size(),3);
      ASSERT_EQUAL(company.phones[0].number, "2671646");
      ASSERT(company.phones[0].type == Phone::Type::PHONE);
      ASSERT_EQUAL(company.phones[1].number, "2671842");
      ASSERT(company.phones[1].type == Phone::Type::FAX);
      //ASSERT(!company.phones[2].type);
    }
    // Save and Load
    {
      istringstream iss(ReadFileData("../test_inputs/test_company_save_load.txt"));
        const auto input_doc = Json::Load(iss);
        const auto& input_map = input_doc.GetRoot().AsMap();
        Company company = Company::ParseFrom(input_map);
        YellowPages::Company proto;
        company.Serialize(proto);
        Company loaded_company = Company::Deserialize(proto);
        int phones_count = company.phones.size();
        for (int i = 0; i < phones_count; ++i)
          ASSERT(company.phones[i] == loaded_company.phones[i]);
    }
  }
  void TestRubric() {
    // Test Parsing
    {
      istringstream iss(R"({
                "name": "Парк"
            })");
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Rubric rub = Rubric::ParseFrom(input_map);
      ASSERT_EQUAL(rub.name, "Парк");
      ASSERT_EQUAL(rub.keywords.size(), 0);
    }
    {
      istringstream iss(R"({
                "name": "Парк",
                "keywords": ["Лес", "Сквер"]
            })");
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Rubric rub = Rubric::ParseFrom(input_map);
      ASSERT_EQUAL(rub.name, "Парк");
      ASSERT_EQUAL(rub.keywords.size(), 2);
      vector<string> expected = {"Лес", "Сквер"};
      ASSERT_EQUAL(rub.keywords, expected);
    }
    // Test Save and Load
    {
      istringstream iss(R"({
                "name": "Парк"
            })");
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Rubric rub = Rubric::ParseFrom(input_map);
      YellowPages::Rubric proto;
      rub.Serialize(proto);
      Rubric rub2 = Rubric::Deserialize(proto);
      ASSERT_EQUAL(rub.name, rub2.name);
      ASSERT_EQUAL(rub.keywords, rub2.keywords);
    }
    {
      istringstream iss(R"({
                "name": "Парк",
                "keywords": ["Лес", "Сквер"]
            })");
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Rubric rub = Rubric::ParseFrom(input_map);
      YellowPages::Rubric proto;
      rub.Serialize(proto);
      Rubric rub2 = Rubric::Deserialize(proto);
      ASSERT_EQUAL(rub.name, rub2.name);
      ASSERT_EQUAL(rub.keywords, rub2.keywords);
    }
  }
  void TestDatabase() {
    // Test Parsing
    {
      {
        istringstream iss(ReadFileData("../test_inputs/test_database_parsing.txt"));
        const auto input_doc = Json::Load(iss);
        const auto& input_map = input_doc.GetRoot().AsMap();
        Database db = Database::ParseFrom(input_map);
        ASSERT_EQUAL(db.rubrics.size(), 2);
        std::unordered_map<size_t, Rubric> expected_rubrics;
        expected_rubrics[1] = Rubric{"Парк"};
        expected_rubrics[2] = Rubric{"Кино", {"Фильмы", "Сериалы"}};
        ASSERT(db.rubrics == expected_rubrics);
        ASSERT_EQUAL(db.companies.size(),2);
        Company company1{
          .address{{.coords{{43.573226, 39.742947}}}},
          .names{{"Дендрарий"}},
          .phones{
            Phone{
              .type = Phone::Type::FAX,
              .country_code{"7"},
              .local_code{"862"},
              .number{"2671646"}
            },
            Phone{
                .type = Phone::Type::PHONE,
                .country_code{"7"},
                .local_code{"862"},
                .number{"2671842"}
            },
            Phone{
                .country_code{"7"},
                .local_code{"862"},
                .number{"2671842"}
            }
          },
          .urls{{"http://dendrarium.ru"}},
          .rubrics{1},
          .nearby_stops{{"Цирк", 180}}
        };
        Company company2{
          .address{{.coords{{43.567998, 39.734131}}}},
          .names{{"им. Фрунзе"}},
          .rubrics{1},
          .nearby_stops{
            {"Пансионат Светлана", 580},
            {"Цирк", 700},
            {"Театральная", 1000}
          }
        };
        ASSERT(db.companies[0] == company1);
        ASSERT(db.companies[1] == company2);
      }
    } // ~Test Parsing
    // Test Save and Load
    {
      istringstream iss(ReadFileData("../test_inputs/test_database_save_load.txt"));
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Database db = Database::ParseFrom(input_map);
      YellowPages::Database proto;
      db.Serialize(proto);
      ofstream out_file("../tmp/testdb", ios::binary);
      proto.SerializeToOstream(&out_file);
      out_file.close();
      ifstream in_file("../tmp/testdb", ios::binary);
      YellowPages::Database proto2;
      proto2.ParseFromIstream(&in_file);
      in_file.close();
      Database db2 = Database::Deserialize(proto2);
      ASSERT_EQUAL(db.rubrics.size(),2);
      ASSERT_EQUAL(db2.rubrics.size(),2);
      ASSERT_EQUAL(db.companies.size(), db2.companies.size());
      for (size_t i = 0; i < db.companies.size(); ++i)
        ASSERT(db.companies[i] == db2.companies[i]);
      ASSERT_EQUAL(db.rubrics.size(), db2.rubrics.size());
      for (const auto& [k,v]: db.rubrics) {
        ASSERT(db2.rubrics.count(k));
        ASSERT(db.rubrics.at(k) == db2.rubrics.at(k));
      }
    }
  }
}
