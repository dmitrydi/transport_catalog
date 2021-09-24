#pragma once

#include <iostream>

#include "test_runner.h"
#include "descriptions.h"
#include "json.h"
#include "requests.h"
#include "transport_catalog.h"
#include "utils.h"

#include "test_descriptions.h"
#include "test_filters.h"
#include "test_requests.h"
#include "test_yellow_pages_private.h"
#include "test_data_generator.h"
#include "test_ostreams.h"
#include "test_main.h"

//std::ostream& operator<<(std::ostream& os, const Descriptions::Phone& phone);
//std::ostream& operator<<(std::ostream& os, const Descriptions::Address& addr);
//std::ostream& operator<<(std::ostream& os, const Descriptions::Name& name);
//std::ostream& operator<<(std::ostream& os, const Descriptions::Url& url);
//std::ostream& operator<<(std::ostream& os, const Descriptions::WorkingTime& wt);
//std::ostream& operator<<(std::ostream& os, const Descriptions::NearbyStop& ns);
//std::ostream& operator<<(std::ostream& os, const Descriptions::Rubric& rubric);
//
//std::ostream& operator<<(std::ostream& os, const Descriptions::Company& company);
//
//std::ostream& operator<<(std::ostream& os, const Descriptions::Database& db);
//
//std::ostream& operator<<(std::ostream& os, const Filters::Phone& phone);

void TestTCSaveLoad();

void TestIn1();

void TestAll();
