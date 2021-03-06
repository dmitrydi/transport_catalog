#include "requests.h"
#include "transport_router.h"

#include <vector>

using namespace std;

namespace Requests {

  Json::Dict Stop::Process(const TransportCatalog& db) const {
    const auto* stop = db.GetStop(name);
    Json::Dict dict;
    if (!stop) {
      dict["error_message"] = Json::Node("not found"s);
    } else {
      Json::Array bus_nodes;
      bus_nodes.reserve(stop->bus_names.size());
      for (const auto& bus_name : stop->bus_names) {
        bus_nodes.emplace_back(bus_name);
      }
      dict["buses"] = Json::Node(move(bus_nodes));
    }
    return dict;
  }

  Json::Dict Bus::Process(const TransportCatalog& db) const {
    const auto* bus = db.GetBus(name);
    Json::Dict dict;
    if (!bus) {
      dict["error_message"] = Json::Node("not found"s);
    } else {
      dict = {
          {"stop_count", Json::Node(static_cast<int>(bus->stop_count))},
          {"unique_stop_count", Json::Node(static_cast<int>(bus->unique_stop_count))},
          {"route_length", Json::Node(static_cast<int>(bus->road_route_length))},
          {"curvature", Json::Node(bus->road_route_length / bus->geo_route_length)},
      };
    }
    return dict;
  }

  struct RouteItemResponseBuilder {
    Json::Dict operator()(const TransportRouter::RouteInfo::BusItem& bus_item) const {
      return Json::Dict{
          {"type", Json::Node("Bus"s)},
          {"bus", Json::Node(bus_item.bus_name)},
          {"time", Json::Node(bus_item.time)},
          {"span_count", Json::Node(static_cast<int>(bus_item.span_count))}
      };
    }
    Json::Dict operator()(const TransportRouter::RouteInfo::WaitItem& wait_item) const {
      return Json::Dict{
          {"type", Json::Node("Wait"s)},
          {"stop_name", Json::Node(wait_item.stop_name)},
          {"time", Json::Node(wait_item.time)},
      };
    }
    Json::Dict operator()(const TransportRouter::RouteInfo::WalkItem& walk_item) const {
      return Json::Dict{
        {"type", Json::Node("Walk"s)},
        {"stop_from", Json::Node(walk_item.stop_from)},
        {"to_company", Json::Node(walk_item.company_name)},
        {"time", Json::Node(walk_item.time)}
      };
    }
  };

  Json::Dict Route::Process(const TransportCatalog& db) const {
    Json::Dict dict;
    const auto route = db.FindRoute(stop_from, stop_to);
    if (!route) {
      dict["error_message"] = Json::Node("not found"s);
    } else {
      dict["total_time"] = Json::Node(route->total_time);
      Json::Array items;
      items.reserve(route->items.size());
      for (const auto& item : route->items) {
        items.push_back(visit(RouteItemResponseBuilder{}, item));
      }

      dict["items"] = move(items);

      dict["map"] = Json::Node(db.RenderRoute(*route));
    }

    return dict;
  }

  Json::Dict Map::Process(const TransportCatalog& db) const {
    return Json::Dict{
        {"map", Json::Node(db.RenderMap())},
    };
  }

  Json::Dict Companies::Process(const TransportCatalog& db) const {
    Json::Dict dict;
    Json::Array items;
    for (auto& name: db.FindCompanies(filter)) {
      items.push_back({move(name)});
    }
    dict["companies"] = move(items);
    return dict;
  }

  Json::Dict RouteToCompany::Process(const TransportCatalog& db) const {
    Json::Dict dict;
    map<double, TransportRouter::RouteInfo> responses;
    vector<string> companies;
    const auto search_result = db.FindCompanies(companies_filter);
    for (auto& company_name: search_result) {
      auto route = db.FindRoute(stop_from, company_name);
      if (route)
        responses[route->total_time] = *move(route);
    }
    if (responses.empty()) {
      dict["error_message"] = Json::Node("not found"s);
    } else {
      const auto& best_result = prev(responses.end())->second;
      dict["total_time"] = best_result.total_time;
      Json::Array items;
      items.reserve(best_result.items.size());
      for (const auto& item : best_result.items) {
        items.push_back(visit(RouteItemResponseBuilder{}, item));
      }

      dict["items"] = move(items);

      dict["map"] = Json::Node(db.RenderRoute(best_result));
    }
    return dict;
  }

  variant<Stop, Bus, Route, Map, Companies, RouteToCompany> Read(const Json::Dict& attrs) {
    const string& type = attrs.at("type").AsString();
    if (type == "Bus") {
      return Bus{attrs.at("name").AsString()};
    } else if (type == "Stop") {
      return Stop{attrs.at("name").AsString()};
    } else if (type == "Route") {
      return Route{attrs.at("from").AsString(), attrs.at("to").AsString()};
    } else if (type == "Map") {
      return Map{};
    } else if (type == "FindCompanies"){
      return Companies{Filters::Filter::ParseFrom(attrs)};
    } else {
      return RouteToCompany{attrs.at("from").AsString(), Filters::Filter::ParseFrom(attrs.at("filter").AsMap())};
    }
  }

  Json::Array ProcessAll(const TransportCatalog& db, const Json::Array& requests) {
    Json::Array responses;
    responses.reserve(requests.size());
    for (const Json::Node& request_node : requests) {
      Json::Dict dict = visit([&db](const auto& request) {
                                return request.Process(db);
                              },
                              Requests::Read(request_node.AsMap()));
      dict["request_id"] = Json::Node(request_node.AsMap().at("id").AsInt());
      responses.push_back(Json::Node(dict));
    }
    return responses;
  }

}
