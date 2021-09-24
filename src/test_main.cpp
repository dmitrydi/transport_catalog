#include "test_main.h"

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

void TestMain() {
    srand(time(nullptr));
	string dummy_data = ReadFileData("../test_inputs/data_less_yellow_pages.txt");
	assert(!dummy_data.empty());

	int cntr = 5000;
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
