#include "data_generator.h"

using namespace std;

int RandomInt(const int min, const int max) {
  assert(max>=min);
  return min + rand() % ((max-min)+1);
}

string RandomString(const size_t min_length, const size_t max_length) {
  std::string tmp_s;
  static const char alphanum[] =
      "0123456789 "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  int len = RandomInt(min_length, max_length);
  tmp_s.reserve(len);
  for (int i = 0; i < len; ++i)
      tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
  return tmp_s;
}

std::vector<int> UniqueRandomSample(const int n_min, const int n_max, const int sample_count) {
  assert((n_max - n_min + 1) >= sample_count);
  vector<int> _result(n_max - n_min + 1);
  iota(_result.begin(), _result.end(), n_min);
  random_shuffle(_result.begin(), _result.end());
  return {_result.begin(), _result.begin() + sample_count};
}

std::string PureRandomString(const size_t min_length, const size_t max_length) {
  std::string tmp_s;
  static const char alphanum[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  int len = RandomInt(min_length, max_length);
  tmp_s.reserve(len);
  for (int i = 0; i < len; ++i)
      tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
  return tmp_s;
}

//Filters::mapIntToStringPhoneType

namespace Filters {
  Names GenerateNamesFilter(const int size) {
    Names result;
    result.reserve(size);
    for (int i = 0; i != size; ++i) {
      result.push_back(RandomString(MIN_NAME_LENGTH, MAX_NAME_LENGTH));
    }
    return result;
  }
  Urls GenerateUrlsFilter(const int size) {
    Urls result;
    result.reserve(size);
    for (int i = 0; i != size; ++i) {
      result.push_back(RandomString(MIN_URL_LENGTH, MAX_URL_LENGTH));
    }
    return result;
  }
  Rubrics GenerateRubricsFilter(const int size) {
    Rubrics result;
    result.reserve(size);
    for (int i = 0; i != size; ++i) {
      result.push_back(RandomString(MIN_RUBRIC_LENGTH, MAX_RUBRIC_LENGTH));
    }
    return result;
  }
  Phones GeneratePhonesFilter(const int size) {
    Phones result;
    result.reserve(size);
    for (int i = 0; i != size; ++i) {
      bool empty = true;
      bool has_type;
      bool has_country_code;
      bool has_local_code;
      bool has_number;
      bool has_extension;
      while (empty) {
        has_type = RandomInt(0,1);
        has_country_code = RandomInt(0,1);
        has_local_code = RandomInt(0,1);
        has_number = RandomInt(0,1);
        has_extension = RandomInt(0,1);
        empty = !(has_type || has_country_code || has_local_code || has_number || has_extension);
      };
      Phone phone;
      if (has_type) { // has type
        phone.type = mapIntToStringPhoneType.at(RandomInt(0,1));
      }
      if (has_country_code) // has country_code
        phone.country_code = PureRandomString(1, 3);
      if (has_local_code) // has local_code
        phone.local_code = PureRandomString(3,4);
      if (has_number) // has number
        phone.number = PureRandomString(MIN_NUMBER_LENGTH, MAX_NUMBER_LENGTH);
      if (has_extension) // has extension
        phone.extension = PureRandomString(2,4);
      result.push_back(move(phone));
    }
    return result;
  }
} // ~Filters

namespace Descriptions {
	std::unordered_map<uint64_t, Rubric> GenerateRubricsMap(const int size, const Filters::Rubrics& required_rubrics) {
	  int final_size = size;
	  std::unordered_map<uint64_t, Rubric> result;
	  result.reserve(final_size);
	  for (int i = 0; i != final_size; ++i) {
	    if (i < required_rubrics.size())
	      result[i] = Rubric{.name=required_rubrics[i]};
	    else
	      result[i] = Rubric{.name=RandomString(MIN_RUBRIC_LENGTH, MAX_RUBRIC_LENGTH)};
	  }
	  return result;
	}

    Name MakeNameFromFilter(const std::string& name_filter, const bool pass) {
      if (pass) {
        return Name{.value = name_filter, .type=Name::Type::MAIN};
      }
      else {
        return Name{.value = (name_filter+"_"), .type=static_cast<Name::Type>(RandomInt(0,2))};
      }
    }

    Url MakeUrlFromFilter(const std::string& url_filter, const bool pass) {
      if (pass) {
        return Url{.value=url_filter};
      }
      else {
        return Url{.value = (url_filter+"_")};
      }
    }

    uint64_t MakeRubricFromFilter(const Filters::Rubrics& rubric_filter, const std::unordered_map<uint64_t, Rubric>& rubric_map, const bool pass, std::optional<int> idx) {
      std::unordered_map<string, uint64_t> inverse_map;
      for (const auto& [num, rubric]: rubric_map)
        inverse_map[rubric.name] = num;
      int filter_size = rubric_filter.size();
      if (pass) {
        if (idx) {
          assert(filter_size > *idx);
          return inverse_map.at(rubric_filter[*idx]);
        }
        else {
          assert(filter_size>0);
          return inverse_map.at(rubric_filter[RandomInt(0, filter_size-1)]);
        }
      } else {
        assert(rubric_map.size()>rubric_filter.size());
        vector<uint64_t> candidates;
        candidates.reserve(rubric_map.size());
        for (const auto& [k, v]: rubric_map)
          if (find(rubric_filter.begin(), rubric_filter.end(), v.name) == rubric_filter.end())
            candidates.push_back(k);
        return candidates[RandomInt(0, candidates.size()-1)];
      }
    }

    Phone MakePhoneFromFilter(const Filters::Phone& filt_phone, const bool pass) {
      Phone phone;
      if (pass) {
        if (!filt_phone.type.empty()) {
          phone.type = mapStrToPhoneType.at(filt_phone.type);
        }
        else {
          if (RandomInt(0,1)) // phone may have it's type if filter type is empty
            phone.type = static_cast<Phone::Type>(RandomInt(0,1));
        }
        if (!filt_phone.country_code.empty()) {
          phone.country_code = filt_phone.country_code;
        } else {
          phone.country_code = RandomInt(0,1) ? PureRandomString(1,3) : "";
        }
        if (!filt_phone.country_code.empty() || !filt_phone.local_code.empty()) {
          phone.local_code = filt_phone.local_code;
        } else {
          phone.local_code = RandomInt(0,1) ? PureRandomString(3,4) : "";
        }
        phone.number = filt_phone.number;

        if (!filt_phone.extension.empty()) {
          phone.extension = filt_phone.extension;
        } else {
          phone.extension = RandomInt(0,1) ? PureRandomString(2,4) : "";
        }
        return phone;
      }
      else {
        phone.type = static_cast<Phone::Type>(RandomInt(0,1));
        phone.country_code = filt_phone.country_code + "not";
        phone.local_code = filt_phone.local_code + "not";
        phone.number = filt_phone.number + "not";
        phone.extension = filt_phone.extension + "not";
        return phone;
      }
    }

	void FillNames(std::vector<Name>& names, const Filters::Names& names_filter, const int max_names, const bool will_pass_filter) {
      names.reserve(max_names);
      if (names_filter.empty()) {
        int cntr = RandomInt(1, max_names);
        for (int i = 0; i < cntr; ++i) // at least one name should be present
          names.push_back(Name{.value = RandomString(MIN_NAME_LENGTH, MAX_NAME_LENGTH), .type=static_cast<Name::Type>(RandomInt(0,2))}); // Random Names
      } else {
        int cntr = RandomInt(1, max_names);
        for (int i = 0; i < cntr; ++i) {
          names.push_back(MakeNameFromFilter(names_filter[RandomInt(0, names_filter.size()-1)], false)); // First all names will not pass
        }
        if (will_pass_filter) {
          int cntr2 = RandomInt(1, names.size());
          for (int j = 0; j != cntr2; ++j) { // then some names will be changed to pass
            names[j] = MakeNameFromFilter(names_filter[RandomInt(0, names_filter.size()-1)], true);
          }
        }
      }
      int main_cntr = 0;
      for (const auto& name: names)
        if (name.type == Name::Type::MAIN)
          ++main_cntr;
      if (!main_cntr)
        names[0].type = Name::Type::MAIN; // ASSURE AS LEAST ONE NAME IS MAIN
	}

	void FillUrls(std::vector<Url>& urls, const Filters::Urls& urls_filter, const int max_urls, const bool will_pass_filter) {
	  urls.reserve(max_urls);
	  if (urls_filter.empty()) {
	    int cntr = RandomInt(0, max_urls);
	    for (int i = 0; i != cntr; ++i)
	      urls.push_back(Url{.value = RandomString(MIN_URL_LENGTH, MAX_URL_LENGTH)});
	  } else {
	    int cntr = RandomInt(0, max_urls);
	    for (int i = 0; i != cntr; ++i)
	      urls.push_back(MakeUrlFromFilter(urls_filter[RandomInt(0, urls_filter.size()-1)], false));
	    if (will_pass_filter) {
	      if (urls.empty())
	        urls.push_back(MakeUrlFromFilter(urls_filter[RandomInt(0, urls_filter.size()-1)], true));
	      else {
	        int cntr2 = RandomInt(1, urls.size());
            for (int j = 0; j != cntr2; ++j) { // then some urls will be changed to pass
              urls[j] = MakeUrlFromFilter(urls_filter[RandomInt(0, urls_filter.size()-1)], true);
            }
	      }
        }
      }
	}

	void FillPhones(std::vector<Phone>& phones, const Filters::Phones& phones_filter, const int max_phones, const bool will_pass_filter) {
      phones.reserve(max_phones);
      if (phones_filter.empty()) {
        int cntr = RandomInt(0, max_phones);
        for (int i = 0; i != cntr; ++i)
          phones.push_back(
              Phone{
                .type = static_cast<Phone::Type>(RandomInt(0,1)),
                .country_code = PureRandomString(0,3),
                .local_code = PureRandomString(0,4),
                .number = PureRandomString(0, MAX_NUMBER_LENGTH),
                .extension = PureRandomString(0,4)
              }
          );
      } else {
        int cntr = RandomInt(0, max_phones);
        for (int i = 0; i != cntr; ++i)
          phones.push_back(MakePhoneFromFilter(phones_filter[RandomInt(0, phones_filter.size()-1)], false));
        if (will_pass_filter) {
          if (phones.empty())
            phones.push_back(MakePhoneFromFilter(phones_filter[RandomInt(0, phones_filter.size()-1)], true));
          else {
            int cntr2 = RandomInt(1, phones.size());
            for (int j = 0; j != cntr2; ++j) { // then some names will be changed to pass
              phones[j] = MakePhoneFromFilter(phones_filter[RandomInt(0, phones_filter.size()-1)], true);
            }
          }
        }
      }
	}

	void FillRubrics(std::vector<uint64_t>& rubrics,
	   const Filters::Rubrics& rubrics_filter,
	   const std::unordered_map<uint64_t, Rubric>& rubrics_map,
	   const int max_rubrics,
	   const bool will_pass_filter) {
	  rubrics.reserve(max_rubrics);
	  std::vector<uint64_t> map_keys;
	  map_keys.reserve(rubrics_map.size());
	  for (const auto& [k,v]: rubrics_map)
	    map_keys.push_back(k);
	  if (rubrics_filter.empty()) {
	    int cntr = RandomInt(0, max_rubrics);
	    for (int i = 0; i != cntr; ++i) // at least one rubris should be present
	      rubrics.push_back(map_keys[RandomInt(0, map_keys.size()-1)]);
	  } else {
	    int cntr = RandomInt(0, max_rubrics);
	    for (int i = 0; i != cntr; ++i)
	      rubrics.push_back(MakeRubricFromFilter(rubrics_filter, rubrics_map, false));
	    if (will_pass_filter) {
	      if (rubrics.empty()) {
	        rubrics.push_back(MakeRubricFromFilter(rubrics_filter, rubrics_map, true));
	      }
	      else {
            int cntr2 = RandomInt(1, rubrics.size());
            for (int j = 0; j != cntr2; ++j) {
              rubrics[j] = MakeRubricFromFilter(rubrics_filter, rubrics_map, true);
            }
	      }
	    }
	  }
	}

	Company GenerateCompany(
        bool will_pass_filter,
        const std::unordered_map<uint64_t, Rubric>& rubric_map,
        const Filters::Names& names_filter,
        const Filters::Urls& urls_filter,
        const Filters::Phones& phones_filter,
        const Filters::Rubrics& rubrics_filter,
        const int max_names,
        const int max_urls,
        const int max_phones,
        const int max_rubrics
	) {
	  Company company;
	  FillNames(company.names, names_filter, max_names, will_pass_filter);
	  FillUrls(company.urls, urls_filter, max_urls, will_pass_filter);
	  FillPhones(company.phones, phones_filter, max_phones, will_pass_filter);
	  FillRubrics(company.rubrics, rubrics_filter, rubric_map, max_rubrics, will_pass_filter);
	  return company;
	}
} // ~Descriptions

TestDataset GenerateTestDataset(
	const int total_companies,
	const int expected_pass_companies,
	const int names_filter_size,
	const int urls_filter_size,
	const int rubrics_filter_size,
	const int phones_filter_size,
    const int max_names,
    const int max_urls,
    const int max_phones,
    const int max_rubrics
	) {
  /*
    Filters::Filter filter;
    Descriptions::Database databse;
    std::vector<Descriptions::Name> expected_names;
   */
  srand(time(nullptr));
  TestDataset result;
  result.filter.names = Filters::GenerateNamesFilter(names_filter_size);
  result.filter.urls = Filters::GenerateUrlsFilter(urls_filter_size);
  result.filter.phones = Filters::GeneratePhonesFilter(phones_filter_size);
  result.filter.rubrics = Filters::GenerateRubricsFilter(rubrics_filter_size);
  result.database.rubrics = Descriptions::GenerateRubricsMap(rubrics_filter_size*2+1, result.filter.rubrics);
  result.database.companies.reserve(total_companies);
  unordered_set<int> pass_idx;
  while(pass_idx.size() < expected_pass_companies)
    pass_idx.insert(RandomInt(0, total_companies-1));
  for (int i = 0; i < total_companies; ++i) {
      result.database.companies.push_back(Descriptions::GenerateCompany(
          (pass_idx.find(i) != pass_idx.end()),
          result.database.rubrics,
          result.filter.names,
          result.filter.urls,
          result.filter.phones,
          result.filter.rubrics,
          max_names,
          max_urls,
          max_phones,
          max_rubrics
          ));
  }
  result.expected_names.reserve(expected_pass_companies);
  for (int i = 0; i < total_companies; ++i) {
    if (pass_idx.find(i) != pass_idx.end())
      result.expected_names.push_back(result.database.companies[i].GetMainName());
  }
  return result;
}
