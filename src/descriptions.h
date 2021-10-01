#pragma once

#include "json.h"
#include "sphere.h"

#include "descriptions.pb.h"
#include "database.pb.h"
#include "utils.h"

#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include <optional>
#include <cstdint>


namespace Descriptions {
  struct Company;

  struct Stop {
    std::string name;
    Sphere::Point position;
    std::unordered_map<std::string, size_t> distances;
    bool is_company_stop = false;

    static Stop ParseFrom(const Json::Dict& attrs);
    static Stop ParseFrom(const Company& company);
  };

  size_t ComputeStopsDistance(const Stop& lhs, const Stop& rhs);

  struct Bus {
    std::string name;
    std::vector<std::string> stops;
    std::vector<std::string> endpoints;
    bool is_virtual = false;

    static Bus ParseFrom(const Json::Dict& attrs);

    void Serialize(TCProto::BusDescription& proto) const;
    static Bus Deserialize(const TCProto::BusDescription& proto);
  };

  using InputQuery = std::variant<Stop, Bus>;

  std::vector<InputQuery> ReadDescriptions(const Json::Array& nodes);

  template <typename Object>
  using Dict = std::map<std::string, const Object*>;

  using StopsDict = Dict<Stop>;
  using BusesDict = Dict<Bus>;
}

namespace Descriptions {
  struct AddressComponent {
    std::string value;
    enum class Type {
      COUNTRY,
      REGION,
      CITY,
      STREET,
      HOUSE
    };

    static AddressComponent ParseFrom(const Json::Dict& attrs);

    void Serialize(YellowPages::AddressComponent& proto) const;
    static AddressComponent Deserialize(const YellowPages::AddressComponent& proto);

    bool operator==(const AddressComponent& other) const;
  };

  struct Address {
    std::string formatted;
    std::vector<AddressComponent> components;
    std::optional<Sphere::Point> coords;
    std::string comment;

    static Address ParseFrom(const Json::Dict& attrs);

    void Serialize(YellowPages::Address& proto) const;
    static Address Deserialize(const YellowPages::Address& proto);

    bool operator==(const Address& other) const;
  };

  struct Name {
    std::string value;
    enum class Type {
      MAIN = 0,
      SYNONYM = 1,
      SHORT = 2
    };
    Type type;

    static Name ParseFrom(const Json::Dict& attrs);

    void Serialize(YellowPages::Name& proto) const;
    static Name Deserialize(const YellowPages::Name& proto);
    struct UnknownTypeError {};

    bool operator==(const Name& other) const;
  };

  struct Phone {
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

    std::string get_type() const;

    static Phone ParseFrom(const Json::Dict& attrs);

    void Serialize(YellowPages::Phone& proto) const;
    static Phone Deserialize(const YellowPages::Phone& proto);

    bool operator==(const Phone& other) const;
  };

  struct Url {
    std::string value;

    static Url ParseFrom(const Json::Dict& attrs);

    void Serialize(YellowPages::Url& proto) const;
    static Url Deserialize(const YellowPages::Url& proto);

    bool operator==(const Url& other) const;
  };

  struct WorkingTimeInterval {
    enum class Day {
      EVERYDAY = 0,
      MONDAY = 1,
      TUESDAY = 2,
      WEDNESDAY = 3,
      THURSDAY = 4,
      FRIDAY = 5,
      SATURDAY = 6,
      SUNDAY = 7
    };

    static const std::unordered_map<std::string, Day> mapDay;

    Day day;
    int32_t minutes_from;
    int32_t minutes_to;

    static WorkingTimeInterval ParseFrom(const Json::Dict& attrs);

    void Serialize(YellowPages::WorkingTimeInterval& proto) const;
    static WorkingTimeInterval Deserialize(const YellowPages::WorkingTimeInterval& proto);

    bool operator==(const WorkingTimeInterval& other) const;
  };

  struct WorkingTime {
    std::string formatted;
    std::vector<WorkingTimeInterval> intervals;

    static WorkingTime ParseFrom(const Json::Dict& attrs);

    void Serialize(YellowPages::WorkingTime& proto) const;
    static WorkingTime Deserialize(const YellowPages::WorkingTime& proto);

    bool operator==(const WorkingTime& other) const;
  };

  struct NearbyStop {
    std::string name;
    uint32_t meters;

    static NearbyStop ParseFrom(const Json::Dict& attrs);

    void Serialize(YellowPages::NearbyStop& proto) const;
    static NearbyStop Deserialize(const YellowPages::NearbyStop& proto);

    bool operator==(const NearbyStop& other) const;
  };

  struct Company {
    std::optional<Address> address;
    std::vector<Name> names;
    std::vector<Phone> phones;
    std::vector<Url> urls;
    std::vector<uint64_t> rubrics;
    std::optional<WorkingTime> working_time;
    std::vector<NearbyStop> nearby_stops;

    static Company ParseFrom(const Json::Dict& attrs);

    void Serialize(YellowPages::Company& proto) const;
    static Company Deserialize(const YellowPages::Company& proto);

    std::string GetMainName() const;

    bool operator==(const Company& other) const;

    void ToOstreamShort(std::ostream& os) const;
    static Company FromIstreamShort(std::istream& iss);

  };

  struct Rubric {
    std::string name;
    std::vector<std::string> keywords;

    static Rubric ParseFrom(const Json::Dict& attrs);

    void Serialize(YellowPages::Rubric& proto) const;
    static Rubric Deserialize(const YellowPages::Rubric& proto);

    bool operator==(const Rubric& other) const;
  };

  struct Database {
    std::vector<Company> companies;
    std::unordered_map<uint64_t, Rubric> rubrics;

    static Database ParseFrom(const Json::Dict& attrs);

    void Serialize(YellowPages::Database& proto) const;
    static Database Deserialize(const YellowPages::Database& proto);
  };

  template <typename T>
  bool operator==(const std::vector<T>& v1, const std::vector<T>& v2) {
    if (v1.size() != v2.size()) return false;
    size_t cnt = v1.size();
    for (size_t i = 0; i < cnt; ++i)
      if (!(v1[i] == v2[i])) return false;
    return true;
  }

  template <typename K, typename V>
  bool operator==(const std::unordered_map<K,V>& map1, const std::unordered_map<K, V>& map2) {
    if (map1.size() != map2.size()) return false;
    for (const auto& [k,v]: map1) {
      if (!map2.count(k)) {
        return false;
      } else {
        if (!(map1.at(k) == map2.at(k))) return false;
      }
    }
    return true;
  }

  Database ReadDatabase(const Json::Dict& input);

  std::ostream& operator << (std::ostream& os, const Descriptions::Name::Type& type);
  std::ostream& operator << (std::ostream& os, const Descriptions::Phone::Type& type);

  std::ostream& operator<<(std::ostream& os, const Descriptions::Phone& phone);
  std::ostream& operator<<(std::ostream& os, const Descriptions::Address& addr);
  std::ostream& operator<<(std::ostream& os, const Descriptions::Name& name);
  std::ostream& operator<<(std::ostream& os, const Descriptions::Url& url);
  std::ostream& operator<<(std::ostream& os, const Descriptions::WorkingTime& wt);
  std::ostream& operator<<(std::ostream& os, const Descriptions::NearbyStop& ns);
  std::ostream& operator<<(std::ostream& os, const Descriptions::Rubric& rubric);

  std::ostream& operator<<(std::ostream& os, const Descriptions::Company& company);

  std::ostream& operator<<(std::ostream& os, const Descriptions::Database& db);
}


