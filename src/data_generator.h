#pragma once

#include "descriptions.h"
#include "requests.h"
#include "filters.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <numeric>
#include <unordered_set>

const size_t MIN_NAME_LENGTH = 3;
const size_t MAX_NAME_LENGTH = 20;
const size_t MIN_URL_LENGTH = 5;
const size_t MAX_URL_LENGTH = 20;
const size_t MIN_RUBRIC_LENGTH = 5;
const size_t MAX_RUBRIC_LENGTH = 20;
const size_t MIN_NUMBER_LENGTH = 6;
const size_t MAX_NUMBER_LENGTH = 10;


int RandomInt(const int min, const int max);

std::string RandomString(const size_t min_length, const size_t max_length);

std::string PureRandomString(const size_t min_length, const size_t max_length);

std::vector<int> UniqueRandomSample(const int idx_min, const int idx_max, const int sample_count);


namespace Filters {
	Names GenerateNamesFilter(const int size);
	Urls GenerateUrlsFilter(const int size);
	Rubrics GenerateRubricsFilter(const int size);
	Phones GeneratePhonesFilter(const int size);

	static const std::unordered_map<int, std::string> mapIntToStringPhoneType = {{0, "PHONE"}, {1, "FAX"}};
}

namespace Descriptions {
	std::unordered_map<uint64_t, Rubric> GenerateRubricsMap(const int size, const Filters::Rubrics& required_rubrics);

	Name MakeNameFromFilter(const std::string& name_filter, const bool pass);
	Url MakeUrlFromFilter(const std::string& url_filter, const bool pass);
	uint64_t MakeRubricFromFilter(const Filters::Rubrics& filter, const std::unordered_map<uint64_t, Rubric>& rubric_map, const bool pass, std::optional<int> idx = std::nullopt);
	Phone MakePhoneFromFilter(const Filters::Phone& phone_filter, const bool pass);

	void FillNames(std::vector<Name>& names, const Filters::Names& names_filter, const int max_names, const bool will_pass_filter);
	void FillUrls(std::vector<Url>& urls, const Filters::Urls& urls_filter, const int max_urls, const bool will_pass_filter);
	void FillPhones(std::vector<Phone>& phones, const Filters::Phones& phones_filter, const int max_phones, const bool will_pass_filter);
	void FillRubrics(std::vector<uint64_t>& rubrics,
	       const Filters::Rubrics& rubrics_filter,
	       const std::unordered_map<uint64_t, Rubric>& rubrics_map,
	       const int max_rubrics,
	       const bool will_pass_filter);

	Company GenerateCompany(
		bool will_pass_filter,
		const std::unordered_map<uint64_t, Rubric>& rubric_map,
		const Filters::Names& names_filter,
		const Filters::Urls& urls_filter,
		const Filters::Phones& phones_filter,
		const Filters::Rubrics& rubrics_filter,
		const int max_names = 10,
		const int max_urls = 10,
		const int max_phones = 10,
		const int max_rubrics = 10
	);

	static const std::unordered_map<std::string, Phone::Type> mapStrToPhoneType = {{"PHONE", Phone::Type::PHONE}, {"FAX", Phone::Type::FAX}};
}

struct TestDataset {
	Filters::Filter filter;
	Descriptions::Database database;
	std::vector<std::string> expected_names;
};

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
	);
