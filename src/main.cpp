#include "descriptions.h"
#include "json.h"
#include "requests.h"
#include "transport_catalog.h"
#include "tests.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <string_view>

using namespace std;

int main(int argc, const char* argv[]) {

  if (argc != 2) {
    cerr << "Usage: transport_catalog_part_r [make_base|process_requests|run_tests]\n";
    return 5;
  }

  const string_view mode(argv[1]);

  if (mode == "run_tests") {
    TestAll();
    return 0;
  }

  const auto input_doc = Json::Load(cin);
  const auto& input_map = input_doc.GetRoot().AsMap();

  if (mode == "process_requests") {

    const string& file_name = input_map.at("serialization_settings").AsMap().at("file").AsString();
    const auto db = TransportCatalog::Deserialize(ReadFileData(file_name));

    Json::PrintValue(
      Requests::ProcessAll(db, input_map.at("stat_requests").AsArray()),
      cout
    );
    cout << endl;

  } else if (mode == "make_base") {

    const TransportCatalog db(
        Descriptions::ReadDescriptions(input_map.at("base_requests").AsArray()),
        input_map.at("routing_settings").AsMap(),
        input_map.at("render_settings").AsMap(),
        Descriptions::ReadDatabase(input_map.at("yellow_pages").AsMap())
    );

    const string& file_name = input_map.at("serialization_settings").AsMap().at("file").AsString();
    ofstream file(file_name, ios::binary);
    file << db.Serialize();

  } else {
    cout << "Unnown mode" << endl;
  }

  return 0;
}
