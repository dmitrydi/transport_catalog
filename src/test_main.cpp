#include "test_main.h"
#include "yellow_pages_private.h"

using namespace std;

istringstream MakeBaseInput(const string& dummy_data, const Descriptions::Database& database) {
  ostringstream os;
  os << "{\n";
  os << dummy_data << "\n";
  os << GuardedString("yellow_pages") << ": ";
  os << database;
  os << "\n}";
  return istringstream(os.str());
}


std::istringstream MakeProcessInput(const std::string& serialized_db_filename, const Filters::Filter& f) {
  ostringstream os;
  os << "{\n";
  os << GuardedString("serialization_settings") << ":  {\n";
  os << GuardedString("file") << ": " << GuardedString(serialized_db_filename) << "\n},\n";
  os << GuardedString("stat_requests") << ": [\n{\n";
  os << GuardedString("id") << ": " << RandomInt(0,10) << ",\n";
  os << GuardedString("type") << ": " << GuardedString("FindCompanies") << ",\n";
  bool first = true;
  if (!f.names.empty()) {
    os << GuardedString("names") << ": ";
    PrintStringVectorGuarded(os, f.names);
    first = false;
  }
  if (!f.urls.empty()) {
    if (!first)
      os << ",\n";
    os << GuardedString("urls") << ": ";
    PrintStringVectorGuarded(os, f.urls);
    first = false;
  }
  if (!f.phones.empty()) {
    if (!first)
      os << ",\n";
    os << GuardedString("phones") << ": ";
    PrintVector(os, f.phones);
    first = false;
  }
  if (!f.rubrics.empty()) {
    if (!first)
      os << ",\n";
    os << GuardedString("rubrics") << ": ";
    PrintStringVectorGuarded(os, f.rubrics);
    first = false;
  }
  os << "\n}\n]";
  os << "\n}";
  return istringstream{os.str()};
}


void TestFilters() {
  srand(time(nullptr));
  Filters::Rubrics rubrics_filter = {"HJkIw vB0eHRp02", "gJP4aP", "651mekbUJECUtEe0OPR", "yOUQHN7pt"};
  istringstream iss(R"([{"country_code": "ehBymMPIfyUDvKD4Y", "extension": "Z6cN k8nDjc8rwwG"},
  {"type": "FAX", "country_code": "E4d4AHzP0a3JoIycQNa", "local_code": "mtobCBKb7jAqeHgXjdN7y", "extension": "j8YRL8tlfZ"},
  {"type": "FAX", "country_code": "E", "local_code": "vPN1", "extension": "AwJSjBERw1ecMoNEUAvyE"},
  {"number": "bM"},
  {"country_code": "WA sO"},
  {"local_code": "1ngSFCu8uXGK"},
  {"country_code": "97k3hAaKpJJWMwtG0b", "local_code": "npTqPzZ2M5joD", "number": "G8krndjwwmiki8Pw0 X", "extension": "RN2a9Nauhhb"},
  {"number": "rOP2f4DbNeifr", "extension": "nMAuwZXvS60WAQghElGcrka6"},
  {"type": "PHONE"}])");
  Filters::Phones phones_filter;
  const auto input_doc = Json::Load(iss);
  const auto& input_array = input_doc.GetRoot().AsArray();
  for (const auto& node: input_array)
    phones_filter.push_back(Filters::Phone::ParseFrom(node.AsMap()));
  unordered_map<uint64_t, Descriptions::Rubric> rubrics_map = {
      {98, {"GLSkXF"}},
      {96, {"VCIswO0RVyM0a6gDt"}},
      {95, {"YXMqSkpur9bwrx6pmWPkX"}},
      {94, {"tGYDBZmMfpVIgqE3"}},
      {93, {"hBHHD0xJVTG9P m"}},
      {90, {"OQm5n"}},
      {89, {"URUKwxmTrDkxBfkp"}},
      {88, {"6ieh5gfAhdogogO2M"}},
      {87, {"vPMgHnuI"}},
      {84, {"5VDA7QiKRP2etNrGSRHZG34eU"}},
      {83, {"uvfBQyPQfejmLxFA6Mrf0"}},
      {79, {"OEr"}},
      {56, {"TAfTB7s"}},
      {53, {"d2N 4Vk7iXu2tvl4zztwKG"}},
      {52, {"zc68hrlM5"}},
      {46, {"wIN"}},
      {1, {"lLjRzUdZlit9"}},
      {54, {"X4umiPSo"}},
      {60, {"soiwbq5aM59fOA"}},
      {2, {"v"}},
      {61, {"2p4iRl"}},
      {59, {"lOtqlV2EI7"}},
      {3, {"ED74tudKS1z15PZK"}},
      {62, {"ploIUqkBfX9xDxfAVdTdUGx"}},
      {86, {"7tXDLVpyFj6g8nhO"}},
      {75, {"Byla5ytRnSn"}},
      {4, {"SM5"}},
      {64, {"Hzk"}},
      {65, {"8gOgfnONeTiqY0mZ9t"}},
      {91, {"Vvb8"}},
      {66, {"4sprHkIHYrjqGk5RlpI"}},
      {68, {"o9XlHoA6AX27Ipz4mHhSQ"}},
      {50, {"ngHgcCsO Vh"}},
      {70, {"F1"}},
      {72, {"e90hHHjpYBo"}},
      {78, {"FQs5YoF8cOvBu28E"}},
      {73, {"WPqgILhHPvWbwt79pt"}},
      {74, {"z4CfvIhivgymFio77CK0OZ0wd"}},
      {92, {"Q7g GBIkoNj6aSEwLXHAXR"}},
      {67, {"m2xCb2nwwLDpi3aReqO"}},
      {5, {"ClDKb"}},
      {7, {"ttF"}},
      {81, {"qdcnpK FeT5Gg7uANCxXjls"}},
      {80, {"5DjH4eu3geM0EslqY"}},
      {8, {"nYQofWP38"}},
      {47, {"RO"}},
      {10, {"oL8bEtFp94VL1Bc2X"}},
      {11, {"IzXNo"}},
      {85, {"N2xJyTgXg4Qf4"}},
      {77, {"jtgN1PWyOyuA7OdD9oor"}},
      {12, {"RWN9ZdnvB6Hkuu"}},
      {49, {"B"}},
      {13, {"8wm1BQnMZD0vAMEVC k"}},
      {63, {"BMMr9"}},
      {14, {"N7151vBM0YISHPv"}},
      {99, {"dRsbWl1FHINHeT3V"}},
      {16, {"6X4"}},
      {17, {"xO"}},
      {97, {"N9"}},
      {9, {"L1sxpCuV1CHfCVEzR8sMgdQh8"}},
      {18, {"MH"}},
      {20, {"UShqBLv4x3C"}},
      {21, {"glPX59oLBcOWLV2y54"}},
      {48, {"8Yh3oZzXv6xG"}},
      {22, {"pAfjiEbh4EJ"}},
      {23, {"n7NoBeio1pemovTx6"}},
      {24, {"4d4F1"}},
      {15, {"Da"}},
      {19, {"NbCt3YXC8zYTFL"}},
      {25, {"9lQ1"}},
      {26, {"f7k50iXr7cv07S"}},
      {27, {"rDktTDjjGdcz9"}},
      {82, {"por0GQXtk"}},
      {28, {"JX03AJl"}},
      {29, {"gJP4aP"}},
      {30, {"Fl"}},
      {31, {"M"}},
      {6, {"8EpS9OL"}},
      {32, {"H5NxJEJtMo2FucAz1K4XyAD"}},
      {58, {"yqMEQdxlnJELGtPv E6kkoV"}},
      {51, {"8YyBbmfuxTPMsgysCwW"}},
      {33, {"YsxUWR3jXJWCyeGFZbHoe"}},
      {0, {"n0zbE5u"}},
      {71, {"tYF241ZWEx4"}},
      {34, {"HJkIw vB0eHRp02"}},
      {35, {"E9H5SVZuBWKe"}},
      {36, {"3Hk"}},
      {37, {"U14MD46IXOlYmGl6"}},
      {38, {"3T4"}},
      {76, {"kVo"}},
      {69, {"nzPYdJEhsAgHoC"}},
      {39, {"S60GoIlAcVYAt ReQQ"}},
      {57, {"FxTTYvFNAHfQB"}},
      {40, {"jXdWktOQRHq2"}},
      {41, {"BYtFiJPfeLlZ0OCi5Fg"}},
      {42, {"GT"}},
      {43, {"651mekbUJECUtEe0OPR"}},
      {55, {"m"}},
      {44, {"yOUQHN7pt"}},
      {45, {"IvI"}}
  };
  unordered_map<string, uint64_t> inv_map;
  for (const auto& [k,v]: rubrics_map)
    inv_map[v.name] = k;
  for (const auto& rub: rubrics_filter)
    ASSERT(inv_map.count(rub));

  Descriptions::Database db;
  db.rubrics = rubrics_map;
  int max_companies = 5000;
  const int max_names = 10;
  const int max_urls = 10;
  const int max_phones = 10;
  const int max_rubrics = 10;
  const int expected_pass = 418;
  int passed = 0;
  vector<string> expected_names;
  for (int i = 0; i < max_companies; ++i) {
    Descriptions::Company company;
    bool will_pass = static_cast<bool>(RandomInt(0,1));
    if (passed >= expected_pass)
      will_pass = false;
    FillNames(company.names, {}, max_names, will_pass);
    FillUrls(company.urls, {}, max_urls, will_pass);
    FillPhones(company.phones, phones_filter, max_phones, will_pass);
    FillRubrics(company.rubrics, rubrics_filter, rubrics_map, max_rubrics, will_pass);
    if (will_pass) {
      expected_names.push_back(company.GetMainName());
      ++passed;
    }
    db.companies.push_back(move(company));
  }
  vector<string> got_names;
  for (const auto& company: db.companies) {
    if (YellowPagesPrivate::CompanyMatch(company, {}, {}, phones_filter, rubrics_filter, inv_map)) {
      got_names.push_back(company.GetMainName());
    }
  }
  ASSERT_EQUAL(got_names.size(), expected_pass);
  ASSERT_EQUAL(expected_names, got_names);

}

void TestMain() {
    srand(time(nullptr));
	string dummy_data = ReadFileData("../test_inputs/data_less_yellow_pages.txt");
	assert(!dummy_data.empty());

	int cntr = 100;//5000;
	for (int i = 0; i < cntr; ++i) {
      const int total_companies = 5000;
      const int expected_pass_companies = RandomInt(0,439);
      const int names_filter_size = RandomInt(0,10);
      const int urls_filter_size = RandomInt(0,10);
      const int rubrics_filter_size = RandomInt(0,10);
      const int phones_filter_size = RandomInt(0,10);
      const int max_names = 10;
      const int max_urls = 10;
      const int max_phones = 10;
      const int max_rubrics = 10;

      cerr << "i=" << i << ", {expected_pass: " << expected_pass_companies << " names filter: " << names_filter_size << " urls filter: " << urls_filter_size;
      cerr << " rubrics filter: " << rubrics_filter_size << " phones filter: " << phones_filter_size << "}";
      TestDataset dataset = GenerateTestDataset(
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

      istringstream iss=MakeBaseInput(dummy_data, dataset.database);
      const auto input_doc = Json::Load(iss);
      const auto& input_map = input_doc.GetRoot().AsMap();

      { // make base
        const TransportCatalog db(
            Descriptions::ReadDescriptions(input_map.at("base_requests").AsArray()),
            input_map.at("routing_settings").AsMap(),
            input_map.at("render_settings").AsMap(),
            Descriptions::ReadDatabase(input_map.at("yellow_pages").AsMap())
        );

        const string& file_name = input_map.at("serialization_settings").AsMap().at("file").AsString();
        ofstream file(file_name, ios::binary);
        file << db.Serialize();
      }
      { // process requests
        istringstream riss= MakeProcessInput(input_map.at("serialization_settings").AsMap().at("file").AsString(), dataset.filter);//MakeBaseInput(dummy_data, dataset.database);
        const auto rinput_doc = Json::Load(riss);
        const auto& rinput_map = rinput_doc.GetRoot().AsMap();

        const string& file_name = rinput_map.at("serialization_settings").AsMap().at("file").AsString();
        const auto db = TransportCatalog::Deserialize(ReadFileData(file_name));

        const auto response = Requests::ProcessAll(db, rinput_map.at("stat_requests").AsArray())[0];
        vector<string> response_names;
        for (const auto& node: response.AsMap().at("companies").AsArray())
        {
          response_names.push_back(node.AsString());
        }
        if (!(response_names == dataset.expected_names)) {
          cerr << "Response names: " << endl;
          cerr << response_names << endl;
          cerr << "Expected names: " << dataset.expected_names << endl;
          cerr << "Filters:" << endl;
          cerr << "Names:" << endl << dataset.filter.names;
          cerr << "Urls:" << endl << dataset.filter.urls;
          cerr << "Phones:" << endl << dataset.filter.phones;
          cerr << "Rubrics:" << endl << dataset.filter.rubrics;
          throw;
        } else {
          cerr << " OK" << endl;
        }
        //ASSERT_EQUAL(response_names, dataset.expected_names);
      }
	}
}
