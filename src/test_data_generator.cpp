#include "test_data_generator.h"

using namespace std;

void TestRandomInt() {
  srand(time(nullptr));
  int min = 1;
  int max = 10;
  int samples = 10000;
  bool min_faced = false;
  bool max_faced = false;
  for (int i = 0; i != samples; ++i) {
    int random_num = RandomInt(min, max);
    ASSERT(((random_num >= min)&&(random_num <= max)));
    if (random_num == min) min_faced = true;
    if (random_num == max) max_faced = true;
  }
  ASSERT(min_faced && max_faced); // almost certainly

}

bool IsPureString(const string& str) {
  return str.find(" ") == str.npos;
}

namespace Filters {
  void TestGenerateNamesFilter() {
    srand(time(nullptr));
    int size = 100;
    Names names = GenerateNamesFilter(size);
    ASSERT_EQUAL(names.size(), size);
    for (const auto& name: names)
      ASSERT((name.size() >= MIN_NAME_LENGTH) && (name.size() <= MAX_NAME_LENGTH));
  }
  void TestGenerateUrlsFilter() {
    srand(time(nullptr));
    int size = 100;
    Urls urls = GenerateUrlsFilter(size);
    ASSERT_EQUAL(urls.size(), size);
    for (const auto& url: urls)
      ASSERT((url.size() >= MIN_URL_LENGTH) && (url.size() <= MAX_URL_LENGTH));
  }
  void TestGenerateRubricsFilter() {
    srand(time(nullptr));
    int size = 100;
    Rubrics rubrics = GenerateRubricsFilter(size);
    ASSERT_EQUAL(rubrics.size(), size);
    for (const auto& rubric: rubrics)
      ASSERT((rubric.size() >= MIN_RUBRIC_LENGTH) && (rubric.size() <= MAX_RUBRIC_LENGTH));
  }
  void TestGeneratePhonesFilter() {
    srand(time(nullptr));
    int size = 100;
    Phones phones = GeneratePhonesFilter(size);
    ASSERT_EQUAL(phones.size(), size);
    for (const auto& phone: phones) {
      ASSERT(!phone.is_empty());
      ASSERT(phone.type.empty() || phone.type=="PHONE" || phone.type == "FAX");
      ASSERT(IsPureString(phone.local_code));
      ASSERT(IsPureString(phone.country_code));
      ASSERT(IsPureString(phone.number));
      ASSERT(IsPureString(phone.extension));
    }
  }
} // ~Filters

namespace Descriptions {
  void TestGenerateRubricsMap() {
    srand(time(nullptr));
    { // Test correct size
      int size = 100;
      int filter_size = 50;
      Filters::Rubrics filter = Filters::GenerateRubricsFilter(filter_size);
      std::unordered_map<uint64_t, Rubric> rub_map = GenerateRubricsMap(size, filter);
      ASSERT_EQUAL(rub_map.size(), size);
    }
    { // Test correct size
      int size = 50;
      int filter_size = 100;
      Filters::Rubrics filter = Filters::GenerateRubricsFilter(filter_size);
      std::unordered_map<uint64_t, Rubric> rub_map = GenerateRubricsMap(size, filter);
      ASSERT_EQUAL(rub_map.size(), size);
    }
    {
      int size = 100;
      int filter_size = 50;
      Filters::Rubrics filter = Filters::GenerateRubricsFilter(filter_size);
      std::unordered_map<uint64_t, Rubric> rub_map = GenerateRubricsMap(size, filter);
      int hits = 0;
      for (const auto& [k,v]: rub_map)
        if (find(filter.begin(), filter.end(), v.name) != filter.end())
          ++hits;
      ASSERT_EQUAL(hits, filter_size);
    }
    {
      int size = 50;
      int filter_size = 100;
      Filters::Rubrics filter = Filters::GenerateRubricsFilter(filter_size);
      std::unordered_map<uint64_t, Rubric> rub_map = GenerateRubricsMap(size, filter);
      int hits = 0;
      for (const auto& [k,v]: rub_map)
        if (find(filter.begin(), filter.end(), v.name) != filter.end())
          ++hits;
      ASSERT_EQUAL(hits, size);
    }
  }

  void TestMakeNameFromFilter() {
    srand(time(nullptr));
    int size = 100;
    Filters::Names names_filter = Filters::GenerateNamesFilter(size);
    for (const auto& filter: names_filter) {
      ASSERT(YellowPagesPrivate::NameMatch(Company{.names={MakeNameFromFilter(filter, true)}}, names_filter));
    }
    for (const auto& filter: names_filter) {
      ASSERT(!YellowPagesPrivate::NameMatch(Company{.names={MakeNameFromFilter(filter, false)}}, names_filter));
    }
  }
  void TestMakeUrlFromFilter() {
    srand(time(nullptr));
    int size = 100;
    Filters::Urls urls_filter = Filters::GenerateUrlsFilter(size);
    for (const auto& filter: urls_filter) {
      ASSERT(YellowPagesPrivate::UrlMatch(Company{.urls={MakeUrlFromFilter(filter, true)}}, urls_filter));
    }
    for (const auto& filter: urls_filter) {
      ASSERT(!YellowPagesPrivate::UrlMatch(Company{.urls={MakeUrlFromFilter(filter, false)}}, urls_filter));
    }
  }
  void TestMakeRubricFromFilter() {
    int size = 100;
    int filter_size = 50;
    Filters::Rubrics rubrics_filter = Filters::GenerateRubricsFilter(filter_size);
    std::unordered_map<uint64_t, Rubric> rub_map = GenerateRubricsMap(size, rubrics_filter);
    std::unordered_map<string, uint64_t> inverseMap;
    for (const auto& [k,v]: rub_map)
      inverseMap[v.name] = k;
    { // check all members of rubric_filter
      for (int i = 0; i < rubrics_filter.size(); ++i) {
        ASSERT(YellowPagesPrivate::RubricMatch(Company{.rubrics={MakeRubricFromFilter(rubrics_filter, rub_map, true, i)}}, rubrics_filter, inverseMap));
      }
      for (int i = 0; i < rubrics_filter.size(); ++i) {
        ASSERT(!YellowPagesPrivate::RubricMatch(Company{.rubrics={MakeRubricFromFilter(rubrics_filter, rub_map, false, i)}}, rubrics_filter, inverseMap));
      }
    }
    { // random rubrics
      for (int i = 0; i < 1000; ++i) {
        ASSERT(YellowPagesPrivate::RubricMatch(Company{.rubrics={MakeRubricFromFilter(rubrics_filter, rub_map, true)}}, rubrics_filter, inverseMap));
      }
      for (int i = 0; i < 1000; ++i) {
        ASSERT(!YellowPagesPrivate::RubricMatch(Company{.rubrics={MakeRubricFromFilter(rubrics_filter, rub_map, false)}}, rubrics_filter, inverseMap));
      }
    }
  }
  void TestMakePhoneFromFilter() {
    srand(time(nullptr));
    int size = 100;
    Filters::Phones phones_filter = Filters::GeneratePhonesFilter(size);
    for (const auto& filter: phones_filter) {
      ASSERT(YellowPagesPrivate::PhoneMatch(Company{.phones={MakePhoneFromFilter(filter, true)}}, phones_filter));
    }
    for (const auto& filter: phones_filter) {
      Phone ph = MakePhoneFromFilter(filter, false);
      ASSERT(!YellowPagesPrivate::PhoneMatch(Company{.phones={ph}}, phones_filter));
    }
  }

  void TestFillNames() {
    //void FillNames(std::vector<Name>& names, const Filters::Names& names_filter, const int max_names, const bool will_pass_filter)
    srand(time(nullptr));
    int size = 100;
    Filters::Names filter = Filters::GenerateNamesFilter(size);
    int max_names = 20;
    int runs = 5000;
    { // empty filter
      Filters::Names empty_filter = {};
      for (int i = 0; i < runs; ++i) {
        vector<Name> names;
        FillNames(names, empty_filter, max_names, true);
        ASSERT(names.size() >= 1 && names.size() <= max_names);
        ASSERT(YellowPagesPrivate::NameMatch(Company{.names=names}, empty_filter));
        int main_cntr = 0;
        for (const auto& name:names)
          if (name.type == Name::Type::MAIN)
            ++main_cntr;
        ASSERT(main_cntr);
      }
    }
    { // non-empty filter
      for (int i = 0; i < runs; ++i) {
        vector<Name> names;
        FillNames(names, filter, max_names, true);
        ASSERT(names.size() >= 1 && names.size() <= max_names);
        ASSERT(YellowPagesPrivate::NameMatch(Company{.names=names}, filter));
        int main_cntr = 0;
        for (const auto& name:names)
          if (name.type == Name::Type::MAIN)
            ++main_cntr;
        ASSERT(main_cntr);
      }
    }
    { // empty filter with false condition
      Filters::Names empty_filter = {};
      for (int i = 0; i < runs; ++i) {
        vector<Name> names;
        FillNames(names, empty_filter, max_names, false);
        ASSERT(names.size() >= 1 && names.size() <= max_names);
        ASSERT(YellowPagesPrivate::NameMatch(Company{.names=names}, empty_filter));
        int main_cntr = 0;
        for (const auto& name:names)
          if (name.type == Name::Type::MAIN)
            ++main_cntr;
        ASSERT(main_cntr);
      }
    }
    { // non-empty filter with false condition
      for (int i = 0; i < runs; ++i) {
        vector<Name> names;
        int max_names = 20;
        FillNames(names, filter, max_names, false);
        ASSERT(names.size() >= 1 && names.size() <= max_names);
        ASSERT(!YellowPagesPrivate::NameMatch(Company{.names=names}, filter));
        int main_cntr = 0;
        for (const auto& name:names)
          if (name.type == Name::Type::MAIN)
            ++main_cntr;
        ASSERT(main_cntr);
      }
    }
  }
  void TestFillUrls() {
    // void FillUrls(std::vector<Url>& urls, const Filters::Urls& urls_filter, const int max_urls, const bool will_pass_filter)
    srand(time(nullptr));
    int size = 100;
    Filters::Urls filter = Filters::GenerateUrlsFilter(size);
    int max_urls = 20;
    int runs = 5000;
    { // empty filter
      vector<Url> urls;
      Filters::Urls empty_filter = {};
      FillUrls(urls, empty_filter, max_urls, true);
      ASSERT(urls.size() >= 0 && urls.size() <= max_urls);
      ASSERT(YellowPagesPrivate::UrlMatch(Company{.urls=urls}, empty_filter));
    }
    { // non-empty filter
      for (int i = 0; i < runs; ++i) {
        vector<Url> urls;
        FillUrls(urls, filter, max_urls, true);
        ASSERT(urls.size() >= 1 && urls.size() <= max_urls); // at least one url for match
        ASSERT(YellowPagesPrivate::UrlMatch(Company{.urls=urls}, filter));
      }
    }
    { // empty filter with false condition
      vector<Url> urls;
      Filters::Urls empty_filter = {};
      FillUrls(urls, empty_filter, max_urls, false);
      ASSERT(urls.size() >= 0 && urls.size() <= max_urls);
      ASSERT(YellowPagesPrivate::UrlMatch(Company{.urls=urls}, empty_filter));
    }
    { // non-empty filter with false condition
      for (int i = 0; i < runs; ++i) {
        vector<Url> urls;
        FillUrls(urls, filter, max_urls, false);
        ASSERT(urls.size() >= 0 && urls.size() <= max_urls); // may have no urls
        ASSERT(!YellowPagesPrivate::UrlMatch(Company{.urls=urls}, filter));
      }
    }
  }
  void TestFillPhones() {
    // void FillPhones(std::vector<Phone>& phones, const Filters::Phones& phones_filter, const int max_phones, const bool will_pass_filter)
    srand(time(nullptr));
    int size = 100;
    Filters::Phones filter = Filters::GeneratePhonesFilter(size);
    int max_phones = 20;
    int runs = 5000;
    { // empty filter
      vector<Phone> phones;
      Filters::Phones empty_filter = {};
      FillPhones(phones, empty_filter, max_phones, true);
      ASSERT(phones.size() >= 0 && phones.size() <= max_phones);
      ASSERT(YellowPagesPrivate::PhoneMatch(Company{.phones=phones}, empty_filter));
    }
    { // non-empty filter
      for (int i = 0; i < runs; ++i) {
        vector<Phone> phones;
        FillPhones(phones, filter, max_phones, true);
        ASSERT(phones.size() >= 1 && phones.size() <= max_phones); // at least one phone for match
        ASSERT(YellowPagesPrivate::PhoneMatch(Company{.phones=phones}, filter));
      }
    }
    { // empty filter with false condition
      vector<Phone> phones;
      Filters::Phones empty_filter = {};
      FillPhones(phones, empty_filter, max_phones, false);
      ASSERT(phones.size() >= 0 && phones.size() <= max_phones);
      ASSERT(YellowPagesPrivate::PhoneMatch(Company{.phones=phones}, empty_filter));
    }
    { // non-empty filter with false condition
      for (int i = 0; i < runs; ++i) {
        vector<Phone> phones;
        FillPhones(phones, filter, max_phones, false);
        ASSERT(phones.size() >= 0 && phones.size() <= max_phones); // may have no phones
        ASSERT(!YellowPagesPrivate::PhoneMatch(Company{.phones=phones}, filter));
      }
    }
  }
  void TestFillRubrics() {
    /*
     * void FillRubrics(std::vector<uint64_t>& rubrics,
       const Filters::Rubrics& rubrics_filter,
       const std::unordered_map<uint64_t, Rubric>& rubrics_map,
       const int max_rubrics,
       const bool will_pass_filter)
     */
    srand(time(nullptr));
    int filter_size = 100;
    int rubrics_size = 200;
    Filters::Rubrics filter = Filters::GenerateRubricsFilter(filter_size);
    int max_rubrics = 20;
    int runs = 100;
    std::unordered_map<uint64_t, Rubric> rubrics_map = GenerateRubricsMap(rubrics_size, filter);
    std::unordered_map<string, uint64_t> inverse_map;
    for (const auto& [k,v]: rubrics_map)
      inverse_map[v.name] = k;
    { // empty filter
      vector<uint64_t> rubrics;
      Filters::Rubrics empty_filter = {};
      FillRubrics(rubrics, empty_filter, rubrics_map, max_rubrics, true);
      ASSERT(rubrics.size() >= 0 && rubrics.size() <= max_rubrics);
      ASSERT(YellowPagesPrivate::RubricMatch(Company{.rubrics=rubrics}, empty_filter, inverse_map));
    }
    { // non-empty filter
      for (int i = 0; i < runs; ++i) {
        vector<uint64_t> rubrics;
        FillRubrics(rubrics, filter, rubrics_map, max_rubrics, true);
        ASSERT(rubrics.size() >= 0 && rubrics.size() <= max_rubrics); // at least one rubric for match
        ASSERT(YellowPagesPrivate::RubricMatch(Company{.rubrics=rubrics}, filter, inverse_map));
      }
    }
    { // empty filter false condition
      vector<uint64_t> rubrics;
      Filters::Rubrics empty_filter = {};
      FillRubrics(rubrics, empty_filter, rubrics_map, max_rubrics, false);
      ASSERT(rubrics.size() >= 0 && rubrics.size() <= max_rubrics);
      ASSERT(YellowPagesPrivate::RubricMatch(Company{.rubrics=rubrics}, empty_filter, inverse_map));
    }
    { // non-empty filter false condition
      for (int i = 0; i < runs; ++i) {
        vector<uint64_t> rubrics;
        FillRubrics(rubrics, filter, rubrics_map, max_rubrics, false);
        ASSERT(rubrics.size() >= 0 && rubrics.size() <= max_rubrics); // at least one rubric for match
        ASSERT(!YellowPagesPrivate::RubricMatch(Company{.rubrics=rubrics}, filter, inverse_map));
      }
    }
  }

  void TestGenerateCompany() {
    srand(time(nullptr));
    int size = 20;
    int map_size = 30;
    const int max_names = 20;
    const int max_urls = 20;
    const int max_phones = 20;
    const int max_rubrics = 20;
    const Filters::Names names_filter = Filters::GenerateNamesFilter(size);
    const Filters::Urls urls_filter = Filters::GenerateUrlsFilter(size);
    const Filters::Phones phones_filter = Filters::GeneratePhonesFilter(size);
    const Filters::Rubrics rubrics_filter = Filters::GenerateRubricsFilter(size);
    const std::unordered_map<uint64_t, Rubric>& rubric_map = GenerateRubricsMap(map_size, rubrics_filter);
    std::unordered_map<string, uint64_t> inverse_map;
    for (const auto& [k,v]: rubric_map)
      inverse_map[v.name] = k;
    int runs = 1000;
    for (int i = 0; i < runs; ++i) {
      Company company = GenerateCompany(true, rubric_map, names_filter, urls_filter, phones_filter, rubrics_filter, max_names, max_urls, max_phones, max_rubrics);
      ASSERT(YellowPagesPrivate::CompanyMatch(company, names_filter, urls_filter, phones_filter, rubrics_filter, inverse_map));
    }
    for (int i = 0; i < runs; ++i) {
      Company company = GenerateCompany(false, rubric_map, names_filter, urls_filter, phones_filter, rubrics_filter, max_names, max_urls, max_phones, max_rubrics);
      ASSERT(!YellowPagesPrivate::CompanyMatch(company, names_filter, urls_filter, phones_filter, rubrics_filter, inverse_map));
    }
    for (int i = 0; i < runs; ++i) {
      Company company = GenerateCompany(false, rubric_map, {}, {}, {}, {}, max_names, max_urls, max_phones, max_rubrics);
      ASSERT(YellowPagesPrivate::CompanyMatch(company, {}, {}, {}, {}, inverse_map));
    }
    for (int i = 0; i < runs; ++i) {
      Company company = GenerateCompany(true, rubric_map, {}, {}, {}, {}, max_names, max_urls, max_phones, max_rubrics);
      ASSERT(YellowPagesPrivate::CompanyMatch(company, {}, {}, {}, {}, inverse_map));
    }
  }
} // ~Descriptions

void TestGenerateDataset() {
  const int total_companies = 100;
  const int expected_pass_companies = 47;
  const int names_filter_size = 10;
  const int urls_filter_size = 10;
  const int rubrics_filter_size = 10;
  const int phones_filter_size = 10;
  const int max_names = 5;
  const int max_urls = 5;
  const int max_phones = 5;
  const int max_rubrics =5;
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
  ASSERT_EQUAL(dataset.database.companies.size(), total_companies);
  ASSERT_EQUAL(dataset.database.rubrics.size(), 2*rubrics_filter_size + 1);
  ASSERT_EQUAL(dataset.expected_names.size(), expected_pass_companies);
  {
    const auto inverseMap = YellowPagesPrivate::MakeRubricMap(dataset.database);
    for (const auto& company: dataset.database.companies) {
      if (YellowPagesPrivate::CompanyMatch(company, dataset.filter.names, dataset.filter.urls, dataset.filter.phones, dataset.filter.rubrics, inverseMap)) {
        ASSERT(find(dataset.expected_names.begin(), dataset.expected_names.end(), company.GetMainName()) != dataset.expected_names.end());
      } else {
        ASSERT(find(dataset.expected_names.begin(), dataset.expected_names.end(), company.GetMainName()) == dataset.expected_names.end());
      }
  }
  }
}
