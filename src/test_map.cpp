#include "test_map.h"
#include "utils.h"
#include "requests.h"
#include <fstream>

using namespace std;

void PrintOutputMap() {
  const string path = "../test_inputs/part_t_make_base.txt";

  istringstream iss(ReadFileData(path));

  const auto input_doc = Json::Load(iss);
  const auto& input_map = input_doc.GetRoot().AsMap();

  const TransportCatalog db(
          Descriptions::ReadDescriptions(input_map.at("base_requests").AsArray()),
          input_map.at("routing_settings").AsMap(),
          input_map.at("render_settings").AsMap(),
          Descriptions::ReadDatabase(input_map.at("yellow_pages").AsMap())
      );

  Requests::Map map;
  auto result = map.Process(db);

  const string path_out = "../test_out/map1.svg";

  ofstream fout(path_out);
  if (!fout)
    cerr << "Error uotput";
  fout << result.at("map").AsString();
}

void PrintRouteMap() {
  const string path = "../test_inputs/part_t_make_base.txt";

  istringstream iss(ReadFileData(path));

  const auto input_doc = Json::Load(iss);
  const auto& input_map = input_doc.GetRoot().AsMap();

  const TransportCatalog db(
          Descriptions::ReadDescriptions(input_map.at("base_requests").AsArray()),
          input_map.at("routing_settings").AsMap(),
          input_map.at("render_settings").AsMap(),
          Descriptions::ReadDatabase(input_map.at("yellow_pages").AsMap())
      );

  Requests::Route route{"Санаторий Салют", "Улица Лизы Чайкиной"};

  auto result = route.Process(db);

  const string path_out = "../test_out/map2.svg";

  ofstream fout(path_out);
  if (!fout)
    cerr << "Error otput";
  fout << result.at("map").AsString();
}

void PrintRouteToCompany() {
  const string path = "../test_inputs/part_t_make_base.txt";

  istringstream iss(ReadFileData(path));

  const auto input_doc = Json::Load(iss);
  const auto& input_map = input_doc.GetRoot().AsMap();

  const TransportCatalog db(
          Descriptions::ReadDescriptions(input_map.at("base_requests").AsArray()),
          input_map.at("routing_settings").AsMap(),
          input_map.at("render_settings").AsMap(),
          Descriptions::ReadDatabase(input_map.at("yellow_pages").AsMap())
      );
  Requests::RouteToCompany rtc;
  rtc.stop_from = "Санаторий Салют";
  rtc.companies_filter = Filters::Filter{.urls={"http://dendrarium.ru"}};

  auto result = rtc.Process(db);

  const string path_out = "../test_out/map31.svg";

  ofstream fout(path_out);
  if (!fout)
    cerr << "Error uotput";
  fout << result.at("map").AsString();
}


void PrintFromSaved() {
  {
    const string path = "../test_inputs/part_t_make_base.txt";

    istringstream iss(ReadFileData(path));
    const auto input_doc = Json::Load(iss);
    const auto& input_map = input_doc.GetRoot().AsMap();

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
  {
    const string path = "../test_inputs/part_t_make_base.txt";

    istringstream iss(ReadFileData(path));
    const auto input_doc = Json::Load(iss);
    const auto& input_map = input_doc.GetRoot().AsMap();

    const string& file_name = input_map.at("serialization_settings").AsMap().at("file").AsString();
    const auto db = TransportCatalog::Deserialize(ReadFileData(file_name));

    Requests::RouteToCompany rtc;
    rtc.stop_from = "Санаторий Салют";
    rtc.companies_filter = Filters::Filter{.urls={"http://dendrarium.ru"}};

    auto result = rtc.Process(db);

    const string path_out = "../test_out/map4.svg";

    ofstream fout(path_out);
    if (!fout)
      cerr << "Error uotput";
    fout << result.at("map").AsString();
  }

}
