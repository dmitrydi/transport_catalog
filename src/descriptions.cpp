#include "descriptions.h"

using namespace std;

namespace Descriptions {

  Stop Stop::ParseFrom(const Json::Dict& attrs) {
    Stop stop = {
        .name = attrs.at("name").AsString(),
        .position = {
            .latitude = attrs.at("latitude").AsDouble(),
            .longitude = attrs.at("longitude").AsDouble(),
        }
    };
    if (attrs.count("road_distances") > 0) {
      for (const auto& [neighbour_stop, distance_node] : attrs.at("road_distances").AsMap()) {
        stop.distances[neighbour_stop] = distance_node.AsInt();
      }
    }
    return stop;
  }

  Stop Stop::ParseFrom(const Company& company) {
    Stop stop;
    stop.name = company.GetMainName();
    stop.position = *(company.address->coords);
    for (const auto& ns: company.nearby_stops)
      stop.distances[ns.name] = ns.meters;
    stop.is_company_stop = true;
    return stop;
  }

  static vector<string> ParseStops(const Json::Array& stop_nodes, bool is_roundtrip) {
    vector<string> stops;
    stops.reserve(stop_nodes.size());
    for (const Json::Node& stop_node : stop_nodes) {
      stops.push_back(stop_node.AsString());
    }
    if (is_roundtrip || stops.size() <= 1) {
      return stops;
    }
    stops.reserve(stops.size() * 2 - 1);  // end stop is not repeated
    for (size_t stop_idx = stops.size() - 1; stop_idx > 0; --stop_idx) {
      stops.push_back(stops[stop_idx - 1]);
    }
    return stops;
  }

  size_t ComputeStopsDistance(const Stop& lhs, const Stop& rhs) {
    if (auto it = lhs.distances.find(rhs.name); it != lhs.distances.end()) {
      return it->second;
    } else {
      return rhs.distances.at(lhs.name);
    }
  }

  Bus Bus::ParseFrom(const Json::Dict& attrs) {
    const auto& name = attrs.at("name").AsString();
    const auto& stops = attrs.at("stops").AsArray();
    if (stops.empty()) {
      return Bus{.name = name};
    } else {
      Bus bus{
          .name = name,
          .stops = ParseStops(stops, attrs.at("is_roundtrip").AsBool()),
          .endpoints = {stops.front().AsString(), stops.back().AsString()}
      };
      if (bus.endpoints.back() == bus.endpoints.front()) {
        bus.endpoints.pop_back();
      }
      return bus;
    }
  }

  void Bus::Serialize(TCProto::BusDescription& proto) const {
    proto.set_name(name);
    for (const string& stop : stops) {
      proto.add_stops(stop);
    }
    for (const string& stop : endpoints) {
      proto.add_endpoints(stop);
    }
  }

  Bus Bus::Deserialize(const TCProto::BusDescription& proto) {
    Bus bus;
    bus.name = proto.name();

    bus.stops.reserve(proto.stops_size());
    for (const auto& stop : proto.stops()) {
      bus.stops.push_back(stop);
    }

    bus.endpoints.reserve(proto.endpoints_size());
    for (const auto& stop : proto.endpoints()) {
      bus.endpoints.push_back(stop);
    }

    return bus;
  }

  vector<InputQuery> ReadDescriptions(const Json::Array& nodes) {
    vector<InputQuery> result;
    result.reserve(nodes.size());

    for (const Json::Node& node : nodes) {
      const auto& node_dict = node.AsMap();
      if (node_dict.at("type").AsString() == "Bus") {
        result.push_back(Bus::ParseFrom(node_dict));
      } else {
        result.push_back(Stop::ParseFrom(node_dict));
      }
    }

    return result;
  }

}

// Yellow Pages

namespace Descriptions {
  // AddressComponent
  AddressComponent AddressComponent::ParseFrom(const Json::Dict& attrs) {
    AddressComponent component;
    if (attrs.count("value"))
      component.value = attrs.at("value").AsString();
    return component;
  }

  void AddressComponent::Serialize(YellowPages::AddressComponent& proto) const {
    proto.set_value(value);
  }

  AddressComponent AddressComponent::Deserialize(const YellowPages::AddressComponent& proto) {
    AddressComponent component;
    component.value = proto.value();
    return component;
  }

  bool AddressComponent::operator==(const AddressComponent& other) const {
    return value == other.value;
  }
  // ~AddressComponent

  // Address
  Address Address::ParseFrom(const Json::Dict& attrs) {
    /*
    std::string formatted;
    std::vector<AddressComponent> components;
    Sphere::Point coords;
    std::string comment;
     */
    Address addr;
    if (attrs.count("formatted"))
      addr.formatted = attrs.at("formatted").AsString();

    if (attrs.count("components")) {
      addr.components.reserve(attrs.at("components").AsArray().size());
      for (const auto& comp: attrs.at("components").AsArray())
        addr.components.push_back(AddressComponent::ParseFrom(comp.AsMap()));
    }

    if (attrs.count("coords")) {
      Sphere::Point crd;
      if (attrs.at("coords").AsMap().at("lat").IsString())
        crd.latitude = stod(attrs.at("coords").AsMap().at("lat").AsString());
      else
        crd.latitude = attrs.at("coords").AsMap().at("lat").AsDouble();
      if (attrs.at("coords").AsMap().at("lon").IsString())
        crd.longitude = stod(attrs.at("coords").AsMap().at("lon").AsString());
      else
        crd.longitude = attrs.at("coords").AsMap().at("lon").AsDouble();
      addr.coords = move(crd);
    }

    if (attrs.count("comment"))
      addr.comment = attrs.at("comment").AsString();

    return addr;
  }

  void Address::Serialize(YellowPages::Address& proto) const {
    proto.set_formatted(formatted);
    for (const auto& comp: components)
      comp.Serialize(*proto.add_components());
    if (coords) {
      proto.mutable_coords()->set_lat(coords->latitude);
      proto.mutable_coords()->set_lon(coords->longitude);
    }
    proto.set_comment(comment);
  }

  Address Address::Deserialize(const YellowPages::Address& proto) {
    Address result;
    result.formatted = proto.formatted();
    result.comment = proto.comment();
    if (proto.has_coords())
      result.coords = {proto.coords().lat(), proto.coords().lon()};
    int comp_sz = proto.components_size();
    result.components.reserve(comp_sz);
    for (int i = 0; i < comp_sz; ++i)
      result.components.push_back(AddressComponent::Deserialize(proto.components(i)));
    return result;
  }

  bool Address::operator==(const Address& other) const {
    /*
     *     std::string formatted;
    std::vector<AddressComponent> components;
    std::optional<Sphere::Point> coords;
    std::string comment;
     */
    if (formatted != other.formatted) return false;
    if (components.size() != other.components.size()) {
      return false;
    } else {
      size_t cnt = components.size();
      for (size_t i = 0; i < cnt; ++i)
        if (!(components[i] == other.components[i]))
          return false;
    }
    if (coords) {
      if (!other.coords) {
        return false;
      }
      else {
        if (!(*coords == *other.coords)) {
          return false;
        }
      }
    } else {
      if (other.coords) {
        return false;
      }
    }
    return comment == other.comment;
  }
  // ~ Address

  // Name
  Name Name::ParseFrom(const Json::Dict& attrs) {
    /*
         std::string value;
    enum class Type {
      MAIN = 0,
      SYNONYM = 1,
      SHORT = 2
    };
    Type type;
     */
    Name result;
    result.value = attrs.at("value").AsString();
    if (attrs.count("type")) {
      if (attrs.at("type").AsString() == "MAIN")
        result.type = Name::Type::MAIN;
      else if (attrs.at("type").AsString() == "SYNONYM")
        result.type = Name::Type::SYNONYM;
      else if (attrs.at("type").AsString() == "SHORT")
        result.type = Name::Type::SHORT;
      else
        throw UnknownTypeError();
    } else {
      result.type = Name::Type::MAIN;
    }
    return result;
  }

  void Name::Serialize(YellowPages::Name& proto) const {
    proto.set_value(value);
    proto.set_type(static_cast<YellowPages::Name::Type>(type));
  }

  Name Name::Deserialize(const YellowPages::Name& proto) {
    Name result;
    result.value = proto.value();
    result.type = static_cast<Name::Type>(proto.type());
    return result;
  }

  bool Name::operator==(const Name& other) const {
    return (value == other.value) && (type == other.type);
  }
  // ~Name

  // Phone
  Phone Phone::ParseFrom(const Json::Dict& attrs) {
   /*
    std::string formatted;
    enum class Type {
      PHONE = 0,
      FAX = 1
    };
    std::optional<Type> type;
    std::string country_code;
    std::string local_code;
    std::string number;
    std::string extension;
    std::string description;
    */
    Phone result;
    if (attrs.count("formatted"))
      result.formatted = attrs.at("formatted").AsString();
    if (attrs.count("type")) {
      if (attrs.at("type").AsString() == "PHONE")
        result.type = Type::PHONE;
      else if (attrs.at("type").AsString() == "FAX")
        result.type = Type::FAX;
      else
        throw;
    }
    if (attrs.count("country_code"))
      result.country_code = attrs.at("country_code").AsString();
    if (attrs.count("local_code"))
      result.local_code = attrs.at("local_code").AsString();
    if (attrs.count("number"))
      result.number = attrs.at("number").AsString();
    if (attrs.count("extension"))
      result.extension = attrs.at("extension").AsString();
    if (attrs.count("description"))
      result.description = attrs.at("description").AsString();
    return result;
  }

  string Phone::get_type() const {
    if (type) {
      if (type == Phone::Type::PHONE)
        return "PHONE";
      else
        return "FAX";
    } else {
      return {};
    }
    }

  void Phone::Serialize(YellowPages::Phone& proto) const {
    proto.set_formatted(formatted);
    if (type)
      proto.mutable_type()->set_type(static_cast<YellowPages::PhoneType::Type>(*type));
    proto.set_country_code(country_code);
    proto.set_local_code(local_code);
    proto.set_number(number);
    proto.set_extension(extension);
    proto.set_description(description);

  }

  Phone Phone::Deserialize(const YellowPages::Phone& proto) {
    Phone result;
    result.formatted = proto.formatted();
    if (proto.has_type())
      result.type = static_cast<Phone::Type>(proto.type().type());
    result.country_code = proto.country_code();
    result.local_code = proto.local_code();
    result.number = proto.number();
    result.extension = proto.extension();
    result.description = proto.description();
    return result;
  }

  bool Phone::operator==(const Phone& other) const {
    /*
     *     std::optional<Type> type;
    std::string country_code;
    std::string local_code;
    std::string number;
    std::string extension;
    std::string description;
     */
//    if (type) {
//      if (!other.type) return false;
//      if (*type != *other.type) return false;
//    } else {
//      if (other.type) return false;
//    }
    if (type != other.type) return false;
    if (country_code != other.country_code) return false;
    if (local_code != other.local_code) return false;
    if (number != other.number) return false;
    if (extension != other.extension) return false;
    if (description != other.description) return false;
    return true;
  }
  // ~ Phone

  // Url
  Url Url::ParseFrom(const Json::Dict& attrs) {
    //std::string value;
    return {attrs.at("value").AsString()};
  }

  void Url::Serialize(YellowPages::Url& proto) const {
    proto.set_value(value);
  }

  Url Url::Deserialize(const YellowPages::Url& proto) {
    return {proto.value()};
  }

  bool Url::operator==(const Url& other) const {
    return value == other.value;
  }
  // ~Url

  // WorkingTimeInterval
  const unordered_map<std::string, WorkingTimeInterval::Day> WorkingTimeInterval::mapDay = {
      {"EVERYDAY", WorkingTimeInterval::Day::EVERYDAY},
      {"MONDAY", WorkingTimeInterval::Day::MONDAY},
      {"TUESDAY", WorkingTimeInterval::Day::TUESDAY},
      {"WEDNESDAY", WorkingTimeInterval::Day::WEDNESDAY},
      {"THURSDAY", WorkingTimeInterval::Day::THURSDAY},
      {"FRIDAY", WorkingTimeInterval::Day::FRIDAY},
      {"SATURDAY", WorkingTimeInterval::Day::SATURDAY},
      {"SUNDAY", WorkingTimeInterval::Day::SUNDAY}
  };

  WorkingTimeInterval WorkingTimeInterval::ParseFrom(const Json::Dict& attrs) {
    /*
       Day day;
      int32_t minutes_from;
      int32_t minutes_to;
     */
    WorkingTimeInterval result;
    result.day = mapDay.at(attrs.at("day").AsString());
    result.minutes_from = attrs.at("minutes_from").AsInt();
    result.minutes_to = attrs.at("minutes_to").AsInt();
    return result;
  }

  void WorkingTimeInterval::Serialize(YellowPages::WorkingTimeInterval& proto) const {
    proto.set_day(static_cast<YellowPages::WorkingTimeInterval::Day>(day));
    proto.set_minutes_from(minutes_from);
    proto.set_minutes_to(minutes_to);
  }

  WorkingTimeInterval WorkingTimeInterval::Deserialize(const YellowPages::WorkingTimeInterval& proto) {
    WorkingTimeInterval result;
    result.day = static_cast<WorkingTimeInterval::Day>(proto.day());
    result.minutes_from = proto.minutes_from();
    result.minutes_to = proto.minutes_to();
    return result;
  }

  /*
   *     Day day;
    int32_t minutes_from;
    int32_t minutes_to;
   */
  bool WorkingTimeInterval::operator==(const WorkingTimeInterval& other) const {
    return (day == other.day) && (minutes_from == other.minutes_from) && (minutes_to == other.minutes_to);
  }
  // ~WorkingTimeInterval

  // Working Time
  WorkingTime WorkingTime::ParseFrom(const Json::Dict& attrs) {
    /*
    std::string formatted;
    std::vector<WorkingTimeInterval> intervals;
     */
    WorkingTime result;
    result.formatted = attrs.at("formatted").AsString();
    result.intervals.reserve(attrs.at("intervals").AsArray().size());
    for (const auto& interv: attrs.at("intervals").AsArray())
      result.intervals.push_back(WorkingTimeInterval::ParseFrom(interv.AsMap()));
    return result;
  }

  void WorkingTime::Serialize(YellowPages::WorkingTime& proto) const {
    proto.set_formatted(formatted);
    for (const auto& interv: intervals)
      interv.Serialize(*proto.add_intervals());
  }

  WorkingTime WorkingTime::Deserialize(const YellowPages::WorkingTime& proto) {
    WorkingTime result;
    result.formatted = proto.formatted();
    int c = proto.intervals_size();
    result.intervals.reserve(c);
    for (int i = 0; i < c; ++i)
      result.intervals.push_back(WorkingTimeInterval::Deserialize(proto.intervals(i)));
    return result;
  }

  bool WorkingTime::operator==(const WorkingTime& other) const {
    if (formatted != other.formatted)
      return false;
    if (intervals.size() != other.intervals.size()) {
      return false;
    }
    else {
      size_t cnt = intervals.size();
      for (size_t i = 0; i < cnt; ++i)
        if (!(intervals[i] == other.intervals[i]))
          return false;
    }
    return true;
  }
  // ~Working Time

  // Nearby Stop
  NearbyStop NearbyStop::ParseFrom(const Json::Dict& attrs) {
    //    std::string name;
    //    uint32_t meters;
    NearbyStop result;
    result.name = attrs.at("name").AsString();
    result.meters = static_cast<uint32_t>(attrs.at("meters").AsInt());
    return result;//{attrs.at("name").AsString(), attrs.at("meters").AsInt()};
  }

  void NearbyStop::Serialize(YellowPages::NearbyStop& proto) const {
    proto.set_name(name);
    proto.set_meters(meters);
  }

  NearbyStop NearbyStop::Deserialize(const YellowPages::NearbyStop& proto) {
    return {proto.name(), proto.meters()};
  }

  bool NearbyStop::operator==(const NearbyStop& other) const {
    return (name == other.name) && (meters == other.meters);
  }
  // ~Nearby Stop

  // Company
  Company Company::ParseFrom(const Json::Dict& attrs) {
    Company result;
    if (attrs.count("address"))
      result.address = Address::ParseFrom(attrs.at("address").AsMap());
    if (attrs.count("names")) {
      result.names.reserve(attrs.at("names").AsArray().size());
      for (const auto& name: attrs.at("names").AsArray())
        result.names.push_back(move(Name::ParseFrom(name.AsMap())));
    }
    if (attrs.count("phones")) {
      result.phones.reserve(attrs.at("phones").AsArray().size());
      for (const auto& phone: attrs.at("phones").AsArray())
        result.phones.push_back(move(Phone::ParseFrom(phone.AsMap())));
    }
    if (attrs.count("urls")) {
      result.urls.reserve(attrs.at("urls").AsArray().size());
      for (const auto& url: attrs.at("urls").AsArray())
        result.urls.push_back(move(Url::ParseFrom(url.AsMap())));
    }
    if (attrs.count("rubrics")) {
      result.rubrics.reserve(attrs.at("rubrics").AsArray().size());
      for (const auto& rub: attrs.at("rubrics").AsArray())
        result.rubrics.push_back(rub.AsInt());
    }
    if (attrs.count("working_time"))
      result.working_time = WorkingTime::ParseFrom(attrs.at("working_time").AsMap());
    if (attrs.count("nearby_stops")) {
      result.nearby_stops.reserve(attrs.at("nearby_stops").AsArray().size());
      for (const auto& ns: attrs.at("nearby_stops").AsArray())
        result.nearby_stops.push_back(NearbyStop::ParseFrom(ns.AsMap()));
    }
    return result;
  }

  string Company::GetMainName() const {
    for (const auto& name: names) {
      if (name.type == Name::Type::MAIN)
        return name.value;
    }
    throw;
  }

  void Company::Serialize(YellowPages::Company& proto) const {
    if (address)
      address->Serialize(*proto.mutable_address());
    for (const auto& name: names)
      name.Serialize(*proto.add_names());
    for (const auto& phone: phones)
      phone.Serialize(*proto.add_phones());
    for (const auto& url: urls)
      url.Serialize(*proto.add_urls());
    for (const auto rub: rubrics)
      proto.add_rubrics(rub);
    if (working_time)
      working_time->Serialize(*proto.mutable_working_time());
    for (const auto& ns: nearby_stops)
      ns.Serialize(*proto.add_nearby_stops());
  }
  Company Company::Deserialize(const YellowPages::Company& proto) {
    Company result;
    if (proto.has_address())
      result.address = Address::Deserialize(proto.address());
    result.names.reserve(proto.names_size());
    for (const auto& name: proto.names())
      result.names.push_back(Name::Deserialize(name));
    result.phones.reserve(proto.phones_size());
    for (const auto& phone: proto.phones())
      result.phones.push_back(Phone::Deserialize(phone));
    result.urls.reserve(proto.urls_size());
    for (const auto& url: proto.urls())
      result.urls.push_back(Url::Deserialize(url));
    result.rubrics.reserve(proto.rubrics_size());
    for (const auto& rub: proto.rubrics())
      result.rubrics.push_back(rub);
    if (proto.has_working_time())
      result.working_time = WorkingTime::Deserialize(proto.working_time());
    result.nearby_stops.reserve(proto.nearby_stops_size());
    for (const auto& ns: proto.nearby_stops())
      result.nearby_stops.push_back(NearbyStop::Deserialize(ns));
    return result;
  }

  bool Company::operator==(const Company& other) const {
    /*
    std::optional<Address> address;
    std::vector<Name> names;
    std::vector<Phone> phones;
    std::vector<Url> urls;
    std::vector<uint64_t> rubrics;
    std::optional<WorkingTime> working_time;
    std::vector<NearbyStop> nearby_stops;
     */
    if (address) {
      if (!other.address) return false;
      if (!(*address == *other.address)) return false;
    } else {
      if (other.address) return false;
    }
    if (!(names==other.names)) return false;
    if (!(phones==other.phones)) return false;
    if (!(urls == other.urls)) return false;
    if (!(rubrics == other.rubrics)) return false;
    if (working_time) {
      if (!other.working_time) return false;
      if (!(*working_time == *other.working_time)) return false;
    } else {
      if (other.working_time) return false;
    }
    return nearby_stops == other.nearby_stops;
  }

  void Company::ToOstreamShort(std::ostream& os) const {
    os << names.size() << ",";
    for (const auto& n: names) {
      if (n.type == Name::Type::MAIN)
        os<< "M" << ",";
      else if (n.type == Name::Type::SYNONYM)
        os << "S" << ",";
      else
        os << "s" << ",";
      os << n.value << ",";
    }
    os << phones.size() << ",";
    for (const auto& p: phones) {
      if (p.type) {
        if(*p.type == Phone::Type::PHONE) {
          os << "P" << ",";
        }
        else if(*p.type == Phone::Type::FAX) {
          os << "F" << ",";
        }
      } else {
        os << "_" << ",";
      }
      os << p.country_code << ",";
      os << p.local_code << ",";
      os << p.number << ",";
      os << p.extension << ",";
    }
    os << urls.size() << ",";
    for (const auto& url: urls)
      os << url.value << ",";
    os << rubrics.size() << ",";
    bool first = true;
    for (const auto r: rubrics) {
      if (!first)
        os << ",";
      first = false;
      os << r;
    }
    os << ";";
  }
  Company Company::FromIstreamShort(std::istream& iss) {
    /*
     *     std::optional<Address> address;
    std::vector<Name> names;
    std::vector<Phone> phones;
    std::vector<Url> urls;
    std::vector<uint64_t> rubrics;
    std::optional<WorkingTime> working_time;
    std::vector<NearbyStop> nearby_stops;
     */
    /*
     *     std::optional<Type> type;
  std::string country_code;
  std::string local_code;
  std::string number;
  std::string extension;
  std::string description;
     */
    const char del = ',';
    Company cp;
    string dum;
    int cnt;
    getline(iss, dum, del);
    cnt = stoi(dum);
    cp.names.reserve(cnt);

    for (int i = 0; i < cnt; ++i) {
      Name name;
      getline(iss, dum, del);
      if (dum == "M")
        name.type = Name::Type::MAIN;
      else if (dum == "s")
        name.type = Name::Type::SHORT;
      else if (dum == "S")
        name.type = Name::Type::SYNONYM;
      else
        throw;
      getline(iss, dum, del);
      name.value = dum;
      cp.names.push_back(move(name));
    }

    getline(iss, dum, del);
    cnt = stoi(dum);
    cp.phones.reserve(cnt);
    for (int i = 0; i < cnt; ++i) {
      Phone phone;
      getline(iss,dum,del);
      if (dum != "_") {
        if (dum == "P")
          phone.type = Phone::Type::PHONE;
        else if (dum == "F")
          phone.type = Phone::Type::FAX;
        else
          throw;
      }
      getline(iss, phone.country_code, del);
      getline(iss, phone.local_code, del);
      getline(iss, phone.number, del);
      getline(iss, phone.extension, del);
      cp.phones.push_back(move(phone));
    }

    getline(iss, dum, del);
    cnt = stoi(dum);
    cp.urls.reserve(cnt);
    for (int i = 0; i < cnt; ++i) {
      getline(iss, dum, del);
      cp.urls.push_back({dum});
    }

    getline(iss, dum, del);
    cnt = stoi(dum);
    cp.rubrics.reserve(cnt);
    for (int i = 0; i < cnt; ++i) {
      if (i < cnt - 1)
        getline(iss, dum, del);
      else
        getline(iss, dum, ';');
      cp.rubrics.push_back(stoi(dum));
    }

    return cp;
  }
  // ~Company

  // Rubric


  Rubric Rubric::ParseFrom(const Json::Dict& attrs) {
   /*
    std::string name;
    std::vector<std::string> keywords;
    */
    Rubric result;
    result.name = attrs.at("name").AsString();
    if (attrs.count("keywords")) {
      result.keywords.reserve(attrs.at("keywords").AsArray().size());
      for (const auto& kw: attrs.at("keywords").AsArray())
        result.keywords.push_back(kw.AsString());
    }
    return result;
  }

  void Rubric::Serialize(YellowPages::Rubric& proto) const {
    proto.set_name(name);
    for (const auto& kw: keywords)
      proto.add_keywords(kw);
  }
  Rubric Rubric::Deserialize(const YellowPages::Rubric& proto) {
    Rubric rubric;
    rubric.name = proto.name();
    rubric.keywords.reserve(proto.keywords_size());
    for (const auto& kw: proto.keywords())
      rubric.keywords.push_back(kw);
    return rubric;
  }

  bool Rubric::operator==(const Rubric& other) const {
    return (name == other.name) && (keywords == other.keywords);
  }
  // ~Rubric

  // Database
  Database Database::ParseFrom(const Json::Dict& attrs) {
    /*
       std::vector<Company> companies;
       std::unordered_map<size_t, Rubric> rubrics;
     */
    Database result;
    if (attrs.count("companies")) {
      result.companies.reserve(attrs.at("companies").AsArray().size());
      for (const auto& cmp: attrs.at("companies").AsArray())
        result.companies.push_back(move(Company::ParseFrom(cmp.AsMap())));
    }
    if (attrs.count("rubrics")) {
      for (const auto& [k, rub]: attrs.at("rubrics").AsMap()) {
        result.rubrics[stoi(k)] = move(Rubric::ParseFrom(rub.AsMap()));
      }
    }
    return result;
  }

  void Database::Serialize(YellowPages::Database& proto) const {
    for (const Company& comp: companies)
      comp.Serialize(*proto.add_companies());
    for (const auto& [k, rub]: rubrics) {
      rub.Serialize(proto.mutable_rubrics()->operator [](k));
    }
  }

  Database Database::Deserialize(const YellowPages::Database& proto) {
    Database db;
    db.companies.reserve(proto.companies_size());
    for (const auto& cmp: proto.companies())
      db.companies.push_back(Company::Deserialize(cmp));
    for (const auto& [k, rub]: proto.rubrics()) {
      db.rubrics[k] = Rubric::Deserialize(rub);
    }
    return db;
  }
  // ~Database

  Database ReadDatabase(const Json::Dict& input) {
    return Database::ParseFrom(input);
  }

} // ~ Descriptions

namespace Descriptions {
std::ostream& operator << (std::ostream& os, const Descriptions::Name::Type& type) {
  switch (type) {
  case Descriptions::Name::Type::MAIN:
    os << "MAIN";
    break;
  case Descriptions::Name::Type::SHORT:
    os << "SHORT";
    break;
  case Descriptions::Name::Type::SYNONYM:
    os << "SYNONYM";
    break;
  default:
    break;
  }
  return os;
}

std::ostream& operator << (std::ostream& os, const Descriptions::Phone::Type& type) {
  switch (type) {
  case Descriptions::Phone::Type::PHONE:
    os << "PHONE";
    break;
  case Descriptions::Phone::Type::FAX:
    os << "FAX";
    break;
  default:
    break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const Descriptions::Company& company) {
  /*
      std::optional<Address> address;
    std::vector<Name> names;
    std::vector<Phone> phones;
    std::vector<Url> urls;
    std::vector<size_t> rubrics;
    std::optional<WorkingTime> working_time;
    std::vector<NearbyStop> nearby_stops;
   */
  bool first = true;
  os << "{";
  if (company.address)
    os << "\n" << GuardedString("address") << ": " << *company.address;
  if (!company.names.empty()) {
    if (!first)
      os << ",";
    first = false;
    os << "\n" << GuardedString("names") << ": ";
    PrintVector(os, company.names);
  }
  if (!company.phones.empty()) {
    if (!first)
          os << ",";
        first = false;
    os << "\n" << GuardedString("phones") << ": ";
    PrintVector(os, company.phones);
  }
  if (!company.urls.empty()) {
    if (!first)
          os << ",";
        first = false;
    os << "\n" << GuardedString("urls") << ": ";
    PrintVector(os, company.urls);
  }
  if (!company.rubrics.empty()) {
    if (!first)
          os << ",";
        first = false;
    os << "\n" << GuardedString("rubrics") << ": ";
    PrintVector(os, company.rubrics);
  }
  if (!company.nearby_stops.empty()) {
    if (!first)
          os << ",";
        first = false;
    os << "\n" << GuardedString("nearby_stops") << ": ";
    PrintVector(os, company.nearby_stops);
  }
  os << "\n}";
  return os;
}


std::ostream& operator<<(std::ostream& os, const Descriptions::Phone& phone) {
  bool first = true;
  os << "{" ;
  if (phone.type) {
    os<< "\n" << "\"type\": ";
    if (*phone.type == Descriptions::Phone::Type::PHONE)
      os << "\"PHONE\"";
    else
      os << "\"FAX\"";
    first = false;
  };
  if (!phone.country_code.empty()) {
    if (!first) {
      os << ",";
    }
    first = false;
    os << "\n" << "\"country_code\": " << GuardedString(phone.country_code);
  }
  if (!phone.local_code.empty()) {
    if (!first) {
      os << ",";
    }
    first = false;
    os << "\n" << GuardedString("local_code") << ": " << GuardedString(phone.local_code);
  }
  if (!phone.number.empty()) {
    if (!first) {
      os << ",";
    }
    first = false;
    os << "\n" << GuardedString("number") << ": " << GuardedString(phone.number);
  }
  if (!phone.extension.empty()) {
    if (!first) {
      os << ",";
    }
    first = false;
    os << "\n" << GuardedString("extension") << ": " << GuardedString(phone.extension);
  }
  os << "\n}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Descriptions::Address& addr) {
  os << "{\n";
  if (addr.coords) {
    os << GuardedString("coords") << ": " << *addr.coords;
  }
  os << "\n}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Descriptions::Name& name) {
  os << "{\n";
  if (name.type != Descriptions::Name::Type::MAIN) {
    os << GuardedString("type") << ": ";
    if (name.type == Descriptions::Name::Type::SHORT)
      os << GuardedString("SHORT");
    else
      os << GuardedString("SYNONYM");
  } else {
    os << GuardedString("type") << ": \"MAIN\"";
  }
  os << ",\n";
  os << GuardedString("value") << ": " << GuardedString(name.value);
  os << "\n}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Descriptions::Url& url) {
  os << "{\n";
  os << GuardedString("value") << ": " << GuardedString(url.value);
  os << "\n}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Descriptions::WorkingTime& wt) {
  return os;
}

std::ostream& operator<<(std::ostream& os, const Descriptions::NearbyStop& ns) {
  os << "{\n";
  os << GuardedString("meters") << ": " << ns.meters << ",\n";
  os << GuardedString("name") << ": " << GuardedString(ns.name);
  os << "\n}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Descriptions::Database& db) {
  os << "{\n";
  os << GuardedString("rubrics") << ": {";
  // rubrics
  bool first = true;
  for (const auto& [k,v]: db.rubrics) {
    if (!first)
      os << ",";
    first = false;
    os << "\n";
    os << GuardedString(to_string(k)) << ": " << v;
  }
  os << "\n},\n";
  os << GuardedString("companies") << ": ";
  PrintVector(os, db.companies);
  os << "}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Descriptions::Rubric& rubric) {
  os << "{\n";
  os << GuardedString("name") << ": " << GuardedString(rubric.name);
  if (!rubric.keywords.empty()) {
    os << GuardedString("keywords") << ": ";
    PrintVector(os, rubric.keywords);
  }
  os << "\n}";
  return os;
}

}


