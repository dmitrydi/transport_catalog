#include "test_ostreams.h"
#include "test_data_generator.h"

using namespace std;
using namespace Descriptions;

void TestPhoneToOstream() {
  { // test empty
    Descriptions::Phone phone;
    ostringstream os;
    os << phone;
    istringstream iss(os.str());
    const auto input_doc = Json::Load(iss);
    const auto& input_map = input_doc.GetRoot().AsMap();
    Descriptions::Phone phone2 = Phone::ParseFrom(input_map);
    ASSERT(phone==phone2);
  }
  { // test type
    {
      Descriptions::Phone phone;
      phone.type = Phone::Type::PHONE;
      ostringstream os;
      os << phone;
      istringstream iss(os.str());
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Descriptions::Phone phone2 = Phone::ParseFrom(input_map);
      ASSERT(phone==phone2);
    }
    {
      Descriptions::Phone phone;
      phone.type = Phone::Type::FAX;
      ostringstream os;
      os << phone;
      istringstream iss(os.str());
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Descriptions::Phone phone2 = Phone::ParseFrom(input_map);
      ASSERT(phone==phone2);
    }
  }
  { // test country_code
      Descriptions::Phone phone;
      phone.country_code = "123435";
      ostringstream os;
      os << phone;
      istringstream iss(os.str());
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Descriptions::Phone phone2 = Phone::ParseFrom(input_map);
      ASSERT(phone==phone2);
  }
  { // test local_code
      Descriptions::Phone phone;
      phone.local_code = "123435";
      ostringstream os;
      os << phone;
      istringstream iss(os.str());
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Descriptions::Phone phone2 = Phone::ParseFrom(input_map);
      ASSERT(phone==phone2);
  }
  { // test number
      Descriptions::Phone phone;
      phone.number = "123435";
      ostringstream os;
      os << phone;
      istringstream iss(os.str());
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Descriptions::Phone phone2 = Phone::ParseFrom(input_map);
      ASSERT(phone==phone2);
  }
  { // test extension
      Descriptions::Phone phone;
      phone.extension = "123435";
      ostringstream os;
      os << phone;
      istringstream iss(os.str());
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();
      Descriptions::Phone phone2 = Phone::ParseFrom(input_map);
      ASSERT(phone==phone2);
  }
  { // test all
    Descriptions::Phone phone;
    phone.type = Phone::Type::FAX;
    phone.country_code = "country";
    phone.local_code = "local";
    phone.number = "number";
    phone.extension = "extension";
    ostringstream os;
    os << phone;
    istringstream iss(os.str());
    const auto input_doc = Json::Load(iss);
    const auto& input_map = input_doc.GetRoot().AsMap();
    Descriptions::Phone phone2 = Phone::ParseFrom(input_map);
    ASSERT_EQUAL((*phone.type), (*phone2.type));
    ASSERT_EQUAL(phone.country_code, phone2.country_code);
    ASSERT_EQUAL(phone.local_code, phone2.local_code);
    ASSERT_EQUAL(phone.number, phone2.number);
    ASSERT_EQUAL(phone.extension, phone2.extension);
  }
}
void TestCompanyToOstream() {
  Company company;
  /*
  std::optional<Address> address;
  std::vector<Name> names;
  std::vector<Phone> phones;
  std::vector<Url> urls;
  std::vector<uint64_t> rubrics;
  std::optional<WorkingTime> working_time;
  */
  srand(time(nullptr));
  int runs = 1000;
  for (int j = 0; j < runs; ++j) {
    if (RandomInt(0,1))
      company.address->coords={1.234, 0.567};
    for (int i = 0; i < 3; ++i)
      company.names.push_back(Name{.value=RandomString(3,20), .type=static_cast<Name::Type>(RandomInt(0,2))});
    if (RandomInt(0,1)) {
      for (int i = 0; i < 5; ++i)
        company.phones.push_back(
                        Phone{
                            .type=static_cast<Phone::Type>(RandomInt(0,1)),
                            .country_code = RandomString(0,6),
                            .local_code = RandomString(0,6),
                            .number = RandomString(0,10),
                            .extension = RandomString(0,5)
                          }
                       );
    }
    if (RandomInt(0,1)) {
      for (int i = 0; i < 3; ++i)
        company.urls.push_back({.value=RandomString(0,10)});
    }
    if (RandomInt(0,1)) {
      for (int i = 0; i < 5; ++i)
        company.rubrics.push_back(i);
    }
    if (RandomInt(0,1)) {
      for (int i = 0; i < 3; ++i)
        company.nearby_stops.push_back(NearbyStop{.name=RandomString(3,10), .meters=static_cast<uint32_t>(RandomInt(10,1000))});
    }
    ostringstream os;
    os << company;
    //cerr << company;
    istringstream iss(os.str());
    const auto input_doc = Json::Load(iss);
    const auto& input_map = input_doc.GetRoot().AsMap();
    Company company2 = Company::ParseFrom(input_map);
    ASSERT(company==company2);
  }
}

void TestCompanyToShort() {
  const int total_companies = 1000;
  const int expected_pass_companies = 367;
  const int names_filter_size = 10;
  const int urls_filter_size = 10;
  const int rubrics_filter_size = 10;
  const int phones_filter_size = 10;
  const int max_names = 20;
  const int max_urls = 20;
  const int max_phones = 20;
  const int max_rubrics = 20;
  auto dataset = GenerateTestDataset(
      total_companies,
      expected_pass_companies,
      names_filter_size,
      urls_filter_size,
      rubrics_filter_size,
      phones_filter_size,
      max_names,
      max_urls,
      max_phones,
      max_rubrics
      );
  ostringstream oss;
  for (const auto& cp: dataset.database.companies) {
    //cp.ToOstreamShort(cerr);
    cp.ToOstreamShort(oss);
  }
  vector<Company> from_iss;
  from_iss.reserve(total_companies);
  istringstream iss(oss.str());
  string dum;
  while(getline(iss, dum, ';')) {
    istringstream _iss_(dum);
    from_iss.push_back(Company::FromIstreamShort(_iss_));
  }
  for (int i = 0; i < total_companies; ++i)
    ASSERT(dataset.database.companies[i]==from_iss[i]);
}

void TestDatabaseToOstream() {
  istringstream iss(R"({
        "rubrics": {
            "1": {
                "name": "Парк"
            }
        },
        "companies": [
            {
                "names": [
                    {
                        "value": "Дендрарий"
                    }
                ],
                "urls": [
                    {
                        "value": "http://dendrarium.ru"
                    }
                ],
                "rubrics": [
                    1
                ],
                "address": {
                    "coords": {
                        "lat": "43.573226",
                        "lon": "39.742947"
                    }
                },
                "nearby_stops": [
                    {
                        "name": "Цирк",
                        "meters": 180
                    }
                ],
                "phones": [
                    {
                        "type": "PHONE",
                        "country_code": "7",
                        "local_code": "862",
                        "number": "2671646"
                    },
                    {
                        "type": "PHONE",
                        "country_code": "7",
                        "local_code": "862",
                        "number": "2671842"
                    }
                ]
            },
            {
                "names": [
                    {
                        "value": "им. Фрунзе"
                    }
                ],
                "rubrics": [
                    1
                ],
                "address": {
                    "coords": {
                        "lat": "43.567998",
                        "lon": "39.734131"
                    }
                },
                "nearby_stops": [
                    {
                        "name": "Пансионат Светлана",
                        "meters": 580
                    },
                    {
                        "name": "Цирк",
                        "meters": 700
                    },
                    {
                        "name": "Театральная",
                        "meters": 1000
                    }
                ]
            }
        ]
    })");
  const auto input_doc = Json::Load(iss);
  const auto& input_map = input_doc.GetRoot().AsMap();
  Database db = Database::ParseFrom(input_map);
  ostringstream os;
  os << db;
  istringstream iss2(os.str());
  const auto input_doc2 = Json::Load(iss2);
  const auto& input_map2 = input_doc2.GetRoot().AsMap();
  Database db2 = Database::ParseFrom(input_map2);
  ASSERT(Descriptions::operator ==(db.companies, db.companies));
  ASSERT(db.rubrics==db2.rubrics);
}
