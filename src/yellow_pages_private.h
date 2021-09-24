#pragma once

#include "database.pb.h"
#include "descriptions.h"
#include "utils.h"
#include "filters.h"

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <cstdint>

class YellowPagesPrivate {
public:
  YellowPagesPrivate() = default;

  YellowPagesPrivate(
      Descriptions::Database database
  );

  std::string Serialize() const;

  static YellowPagesPrivate Deserialize(const std::string& data);

  static bool NameMatch(const Descriptions::Company& company, const Filters::Names& names);

  static bool UrlMatch(const Descriptions::Company& company, const Filters::Urls& urls);

  static bool RubricMatch(const Descriptions::Company& company, const Filters::Rubrics& rubrics, const std::unordered_map<std::string, uint64_t>& mapToRubric);

  static bool DoesPhoneMatch(const Descriptions::Phone& object, const Filters::Phone& query);

  static bool PhoneMatch(const Descriptions::Company& company, const Filters::Phones& phones);

  static bool CompanyMatch(const Descriptions::Company& company,
      const Filters::Names& names, const Filters::Urls& urls,
      const Filters::Phones& phones, const Filters::Rubrics& rubrics,
      const std::unordered_map<std::string, uint64_t>& mapToRubric);

  static std::unordered_map<std::string, uint64_t> MakeRubricMap(const Descriptions::Database& db);

  Descriptions::Database database;
  std::unordered_map<std::string, uint64_t> mapToRubric;
  static const std::unordered_map<std::string, Descriptions::Phone::Type> mapToPhoneType;
};
