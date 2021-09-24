#include "test_yellow_pages_private.h"

using namespace std;

void MapToRubric() {
    {
      istringstream iss(ReadFileData("../test_inputs/test_tc_save_load.txt"));
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      const YellowPagesPrivate db(
            Descriptions::ReadDatabase(input_map.at("yellow_pages").AsMap())
        );
      std::unordered_map<std::string, size_t> expected_map = {{"Парк", 1}};
      ASSERT_EQUAL(db.mapToRubric, expected_map);
    }
    {
      istringstream iss(ReadFileData("../test_inputs/test_tc_save_load.txt"));
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      const YellowPagesPrivate db(
            Descriptions::ReadDatabase(input_map.at("yellow_pages").AsMap())
        );
      const string& file_name = input_map.at("serialization_settings").AsMap().at("file").AsString();
      ofstream file(file_name, ios::binary);
      file << db.Serialize();
      file.close();

      const auto db2 = YellowPagesPrivate::Deserialize(ReadFileData(file_name));

      std::unordered_map<std::string, size_t> expected_map = {{"Парк", 1}};
      ASSERT_EQUAL(db2.mapToRubric, expected_map);
    }
  }

  void NameMatch() {
    istringstream iss(ReadFileData("../test_inputs/test_tc_name_match.txt"));
    const auto input_doc = Json::Load(iss);
    const auto& input_map = input_doc.GetRoot().AsMap();
    const YellowPagesPrivate db(
          Descriptions::ReadDatabase(input_map.at("yellow_pages").AsMap())
      );
    const auto& companies = db.database.companies;
    { // Empty filter
      Filters::Names name_filter{};
      for (const auto& company: companies) {
        ASSERT(db.NameMatch(company, name_filter));
      }
    }
    { // Basic match
      Filters::Names name_filter{"Дендрарий", "им. Фрунзе"};
      for (const auto& company: companies) {
        ASSERT(db.NameMatch(company, name_filter));
      }
    }
    { // Synonym match
      Filters::Names name_filter{"Дендрарий 2"};
      vector<Descriptions::Company> expected;
      for (const auto& company: companies)
        if (db.NameMatch(company, name_filter))
          expected.push_back(company);
      ASSERT_EQUAL(expected.size(),1);
      for (const auto& name: expected[0].names) {
        if (name.type == Descriptions::Name::Type::MAIN)
          ASSERT_EQUAL(name.value, "Дендрарий");
      }
    }
    { // Short name match
      Filters::Names name_filter{"Дендрарий 3"};
      vector<Descriptions::Company> expected;
      for (const auto& company: companies)
        if (db.NameMatch(company, name_filter))
          expected.push_back(company);
      ASSERT_EQUAL(expected.size(),1);
      for (const auto& name: expected[0].names) {
        if (name.type == Descriptions::Name::Type::MAIN)
          ASSERT_EQUAL(name.value, "Дендрарий");
      }
    }
  }

  void UrlMatch() {
    istringstream iss(ReadFileData("../test_inputs/test_tc_url_match.txt"));
    const auto input_doc = Json::Load(iss);
    const auto& input_map = input_doc.GetRoot().AsMap();
    const YellowPagesPrivate tester(
          Descriptions::ReadDatabase(input_map.at("yellow_pages").AsMap())
      );
    const auto& companies = tester.database.companies;
    { // Empty filter
      Filters::Urls url_filter{};
      for (const auto& company: companies) {
        ASSERT(tester.UrlMatch(company, url_filter));
      }
    }
    { // All companies
      Filters::Urls url_filter{"www.dendrarium.ru", "www.frunze.ru"};
      for (const auto& company: companies) {
        ASSERT(tester.UrlMatch(company, url_filter));
      }
    }
    { // One company
      Filters::Urls url_filter{"www.dendrarium.ru"};
      for (const auto& company: companies) {
        if (tester.UrlMatch(company, url_filter))
          ASSERT(company.names[0].value == "Дендрарий");
      }
    }
    {// No companies
      Filters::Urls url_filter{"www.nonexistent1.ru", "nonexistent.ru"};
      for (const auto& company: companies)
        ASSERT(!tester.UrlMatch(company, url_filter));
    }
  }

  void RubricMatch() {
    {
      istringstream iss(ReadFileData("../test_inputs/test_tc_rubric_match.txt"));
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      const YellowPagesPrivate tester(
            Descriptions::ReadDatabase(input_map.at("yellow_pages").AsMap())
        );
      const auto& companies = tester.database.companies;
      { // Empty filter
        Filters::Rubrics rubrics_filter{};
        for (const auto& company: companies) {
          ASSERT(tester.RubricMatch(company, rubrics_filter, tester.mapToRubric));
        }
      }
      { // All companies
        Filters::Rubrics rubrics_filter{"Парк", "Стадион"};
        for (const auto& company: companies) {
          ASSERT(tester.RubricMatch(company, rubrics_filter, tester.mapToRubric));
        }
      }
      { // No companies
        Filters::Rubrics rubrics_filter{"Бар"};
        for (const auto& company: companies) {
          ASSERT(!tester.RubricMatch(company, rubrics_filter, tester.mapToRubric));
        }
      }
      { // One company
        Filters::Rubrics rubrics_filter{"Парк"};
        for (const auto& company: companies) {
          if (tester.RubricMatch(company, rubrics_filter, tester.mapToRubric))
            ASSERT(company.names[0].value == "Дендрарий");
        }
      }
      { // All companies
        Filters::Rubrics rubrics_filter{"Стадион"};
        for (const auto& company: companies) {
          ASSERT(tester.RubricMatch(company, rubrics_filter, tester.mapToRubric));
        }
      }
    }
    { // After Save-Load
      istringstream iss(ReadFileData("../test_inputs/test_tc_rubric_match.txt"));
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      const YellowPagesPrivate _tester(
            Descriptions::ReadDatabase(input_map.at("yellow_pages").AsMap())
        );

      const string& file_name = input_map.at("serialization_settings").AsMap().at("file").AsString();
      ofstream file(file_name, ios::binary);
      file << _tester.Serialize();
      file.close();

      const auto tester = YellowPagesPrivate::Deserialize(ReadFileData(file_name));


      const auto& companies = tester.database.companies;
      { // Empty filter
        Filters::Rubrics rubrics_filter{};
        for (const auto& company: companies) {
          ASSERT(tester.RubricMatch(company, rubrics_filter, tester.mapToRubric));
        }
      }
      { // All companies
        Filters::Rubrics rubrics_filter{"Парк", "Стадион"};
        for (const auto& company: companies) {
          ASSERT(tester.RubricMatch(company, rubrics_filter, tester.mapToRubric));
        }
      }
      { // No companies
        Filters::Rubrics rubrics_filter{"Бар"};
        for (const auto& company: companies) {
          ASSERT(!tester.RubricMatch(company, rubrics_filter, tester.mapToRubric));
        }
      }
      { // One company
        Filters::Rubrics rubrics_filter{"Парк"};
        for (const auto& company: companies) {
          if (tester.RubricMatch(company, rubrics_filter, tester.mapToRubric))
            ASSERT(company.names[0].value == "Дендрарий");
        }
      }
      { // All companies
        Filters::Rubrics rubrics_filter{"Стадион"};
        for (const auto& company: companies) {
          ASSERT(tester.RubricMatch(company, rubrics_filter, tester.mapToRubric));
        }
      }

    }
  }
  void DoesPhoneMatch2() {
    { // #1
      istringstream iss(R"({"country_code": "ehBymMPIfyUDvKD4Y", "extension": "Z6cN k8nDjc8rwwG"})");
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Descriptions::Phone phone = Descriptions::Phone::ParseFrom(input_map);
      Filters::Phone filter = Filters::Phone::ParseFrom(input_map);
      {
        ASSERT(YellowPagesPrivate::DoesPhoneMatch(phone, filter));
      }
      {
        Descriptions::Phone test_phone{
          .type = Descriptions::Phone::Type::PHONE,
          .country_code="ehBymMPIfyUDvKD4Y",
          .extension="Z6cN k8nDjc8rwwG"
        };
        ASSERT(YellowPagesPrivate::DoesPhoneMatch(test_phone, filter));
      }
      {
        Descriptions::Phone test_phone{
          .type = Descriptions::Phone::Type::FAX,
          .country_code="ehBymMPIfyUDvKD4Y",
          .extension="Z6cN k8nDjc8rwwG"
        };
        ASSERT(YellowPagesPrivate::DoesPhoneMatch(test_phone, filter));
      }
      {
        Descriptions::Phone test_phone{
          .country_code="ehBymMPIfyUDvKD4Y",
          .local_code = "any",
          .extension="Z6cN k8nDjc8rwwG"
        };
        ASSERT(!YellowPagesPrivate::DoesPhoneMatch(test_phone, filter));
      }
    }
    {
      istringstream iss(R"({"type": "PHONE"})");
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Descriptions::Phone phone = Descriptions::Phone::ParseFrom(input_map);
      Filters::Phone filter = Filters::Phone::ParseFrom(input_map);
      {
        ASSERT(YellowPagesPrivate::DoesPhoneMatch(phone, filter));
      }
      {
        Descriptions::Phone test_phone{
          .type = Descriptions::Phone::Type::PHONE,
          .country_code="ehBymMPIfyUDvKD4Y",
          .extension="Z6cN k8nDjc8rwwG"
        };
        ASSERT(YellowPagesPrivate::DoesPhoneMatch(test_phone, filter));
      }
    }
  }

  void DoesPhoneMatch() {
    /*
     По шаблону {"type": "PHONE", "number": "3633659"} могут найтись следующие номера телефонов:
    {"type": "PHONE", "country_code": "7", "local_code": "812", "number": "3633659"};
    {"type": "PHONE", "local_code": "812", "number": "3633659"};
    {"type": "PHONE", "country_code": "7", "number": "3633659"};
    {"type": "PHONE", "number": "3633659"}.
    А такие не найдутся:
    {"country_code": "7", "local_code": "812", "number": "3633659"} (потому что шаблон требует типа PHONE);
    {"type": "PHONE", "number": "3633658"} (потому что номер неверный);
    {"type": "PHONE"} (потому что номера нет, а он требуется шаблоном).
    По шаблону же {"type": "PHONE"} найдутся лишь номера телефонов, по какому-то недоразумению не содержащие number и при этом имеющие тип PHONE.
     */

    istringstream iss(ReadFileData("../test_inputs/test_tc_save_load.txt"));
    const auto input_doc = Json::Load(iss);
    const auto& input_map = input_doc.GetRoot().AsMap();
    const YellowPagesPrivate _tt(
          Descriptions::ReadDatabase(input_map.at("yellow_pages").AsMap())
      );

    const string& file_name = input_map.at("serialization_settings").AsMap().at("file").AsString();
    ofstream file(file_name, ios::binary);
    file << _tt.Serialize();
    file.close();

    const auto tt = YellowPagesPrivate::Deserialize(ReadFileData(file_name));


    { // Full phone
      Descriptions::Phone phone{
        .type = Descriptions::Phone::Type::PHONE,
        .country_code{"7"},
        .local_code{"876"},
        .number{"111111"},
        .extension{"095"}
      };

      { // Full match
        Filters::Phone query{
          .type{"PHONE"},
          .country_code{"7"},
          .local_code{"876"},
          .number{"111111"},
          .extension{"095"}
        };
        ASSERT(tt.DoesPhoneMatch(phone, query));
      }
      { // No type match
        Filters::Phone query{
          .country_code{"7"},
          .local_code{"876"},
          .number{"111111"},
          .extension{"095"}
        };
        ASSERT(tt.DoesPhoneMatch(phone, query));
      }
      { // Type Mismatch
        Filters::Phone query{
          .type{"FAX"},
          .country_code{"7"},
          .local_code{"876"},
          .number{"111111"},
          .extension{"095"}
        };
        ASSERT(!tt.DoesPhoneMatch(phone, query));
      }
      { // No extension match
        Filters::Phone query{
          .type{"PHONE"},
          .country_code{"7"},
          .local_code{"876"},
          .number{"111111"},
        };
        ASSERT(tt.DoesPhoneMatch(phone, query));
      }
      { // No number
        Filters::Phone query{
          .type{"PHONE"},
          .country_code{"7"},
          .local_code{"876"},
          .extension{"095"}
        };
        ASSERT(!tt.DoesPhoneMatch(phone, query));
      }
      { // No local code with country code present
        Filters::Phone query{
          .type{"PHONE"},
          .country_code{"7"},
          .number{"111111"},
          .extension{"095"}
        };
        ASSERT(!tt.DoesPhoneMatch(phone, query));
      }
      { // No country code
        Filters::Phone query{
          .type{"PHONE"},
          .local_code{"876"},
          .number{"111111"},
          .extension{"095"}
        };
        ASSERT(tt.DoesPhoneMatch(phone, query));
      }
      { // No country code mismatch
        Filters::Phone query{
          .type{"PHONE"},
          .local_code{"8776"},
          .number{"111111"},
          .extension{"095"}
        };
        ASSERT(!tt.DoesPhoneMatch(phone, query));
      }
      { // No local code, no country code
        Filters::Phone query{
          .type{"PHONE"},
          .number{"111111"},
          .extension{"095"}
        };
        ASSERT(tt.DoesPhoneMatch(phone, query));
      }
    } // ~Full phone
    { // Inverse check
      /*
      std::string type;
      std::string country_code;
      std::string local_code;
      std::string number;
      std::string extension;
       */
        Filters::Phone query{
          .type{"PHONE"},
          .number{"3633659"}
        };
        Descriptions::Phone ph1{
            .type = Descriptions::Phone::Type::PHONE,
            .country_code{"7"},
            .local_code{"812"},
            .number{"3633659"}
        };
        Descriptions::Phone ph2{
          .type = Descriptions::Phone::Type::PHONE,
          .local_code{"812"},
          .number{"3633659"}
        };
        Descriptions::Phone ph3{
          .type = Descriptions::Phone::Type::PHONE,
          .country_code{"7"},
          .number{"3633659"}
        };
        Descriptions::Phone ph4{
          .type = Descriptions::Phone::Type::PHONE,
          .number{"3633659"}
        };
        ASSERT(tt.DoesPhoneMatch(ph1, query));
        ASSERT(tt.DoesPhoneMatch(ph2, query));
        ASSERT(tt.DoesPhoneMatch(ph3, query));
        ASSERT(tt.DoesPhoneMatch(ph4, query));

        Descriptions::Phone ph5{
          .country_code{"7"},
          .local_code{"812"},
          .number{"3633659"}
        };

        Descriptions::Phone ph6{
          .type = Descriptions::Phone::Type::PHONE,
          .number{"3633658"}
        };

        Descriptions::Phone ph7{
          .type = Descriptions::Phone::Type::PHONE,
        };

        ASSERT(!tt.DoesPhoneMatch(ph5, query));
        ASSERT(!tt.DoesPhoneMatch(ph6, query));
        ASSERT(!tt.DoesPhoneMatch(ph7, query));
     }
    { // case only phone type
      Filters::Phone query{
        .type{"PHONE"}
      };
      Descriptions::Phone ph1{
          .type = Descriptions::Phone::Type::PHONE,
          .country_code{"7"},
          .local_code{"812"},
          .extension{"000"}
      };
      Descriptions::Phone ph2{
          .type = Descriptions::Phone::Type::PHONE,
          .country_code{"7"},
          .local_code{"812"},
      };
      Descriptions::Phone ph3{
          .type = Descriptions::Phone::Type::PHONE,
          .local_code{"812"}
      };
      Descriptions::Phone ph4{
          .type = Descriptions::Phone::Type::PHONE,
      };
      ASSERT(tt.DoesPhoneMatch(ph1, query));
      ASSERT(tt.DoesPhoneMatch(ph2, query));
      ASSERT(tt.DoesPhoneMatch(ph3, query));
      ASSERT(tt.DoesPhoneMatch(ph3, query));
    }
  } // ~DoesPhoneMatch
