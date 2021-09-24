#include "yellow_pages_private.h"

using namespace std;

string YellowPagesPrivate::Serialize() const {
  YellowPages::Database proto;
  database.Serialize(proto);
  return proto.SerializeAsString();
}

YellowPagesPrivate YellowPagesPrivate::Deserialize(const string& data) {
  YellowPages::Database proto;
  assert(proto.ParseFromString(data));
  YellowPagesPrivate db = Descriptions::Database::Deserialize(proto);
  db.mapToRubric = MakeRubricMap(db.database);
  return db;
}

YellowPagesPrivate::YellowPagesPrivate(
    Descriptions::Database db
): database(move(db)), mapToRubric(MakeRubricMap(database)) {}

const std::unordered_map<std::string, Descriptions::Phone::Type> YellowPagesPrivate::mapToPhoneType = {
    {"PHONE", Descriptions::Phone::Type::PHONE},
    {"FAX", Descriptions::Phone::Type::FAX}
};

std::unordered_map<std::string, uint64_t> YellowPagesPrivate::MakeRubricMap(const Descriptions::Database& db) {
  std::unordered_map<std::string, uint64_t> result;
  result.reserve(db.rubrics.size());
  for (const auto& [k, rub]: db.rubrics)
    result[rub.name] = k;
  return result;
}

bool YellowPagesPrivate::NameMatch(const Descriptions::Company& company, const Filters::Names& names) {
  if (names.empty()) return true;
  for (const auto& name: names)
    for (const auto& cn: company.names)
      if (cn.value == name) return true;
  return false;
}

bool YellowPagesPrivate::UrlMatch(const Descriptions::Company& company, const Filters::Urls& urls) {
  if (urls.empty()) return true;
  for (const auto& url: urls)
    for (const auto& cu: company.urls)
      if (cu.value == url) return true;
  return false;
}

bool YellowPagesPrivate::RubricMatch(const Descriptions::Company& company, const Filters::Rubrics& rubrics, const std::unordered_map<std::string, uint64_t>& mapToRubric) {
  if (rubrics.empty()) return true;
  for (const auto& rubric: rubrics)
    for (const auto& cr: company.rubrics)
      if (cr == mapToRubric.at(rubric))
        return true;
  return false;
}

bool YellowPagesPrivate::DoesPhoneMatch(const Descriptions::Phone& object, const Filters::Phone& query_phone) {
  if (!query_phone.extension.empty() && (query_phone.extension != object.extension))
    return false;
  if (!query_phone.type.empty() && (query_phone.type != object.get_type()))
    return false;
  if (!query_phone.country_code.empty() && (query_phone.country_code != object.country_code)) {
    return false;
  }
  if (
      (!query_phone.local_code.empty() || !query_phone.country_code.empty())
      && (query_phone.local_code != object.local_code)
  ) {
    return false;
  }
  return query_phone.number == object.number;
}

bool YellowPagesPrivate::PhoneMatch(const Descriptions::Company& company,
    const Filters::Phones& phones) {
  if (phones.empty()) return true;
  for (const auto& phone: phones)
    for (const auto& cph: company.phones)
      if (DoesPhoneMatch(cph, phone))
        return true;
  return false;

}

bool YellowPagesPrivate::CompanyMatch(const Descriptions::Company& company,
      const Filters::Names& names, const Filters::Urls& urls,
      const Filters::Phones& phones,
      const Filters::Rubrics& rubrics,
      const std::unordered_map<std::string, uint64_t>& mapToRubric) {
  return NameMatch(company, names) && UrlMatch(company, urls) && PhoneMatch(company, phones) && RubricMatch(company, rubrics, mapToRubric);
}
