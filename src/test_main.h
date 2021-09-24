#pragma once

#include "transport_catalog.h"
#include "data_generator.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include "utils.h"
#include "json.h"
#include "test_runner.h"


std::istringstream MakeBaseInput(const std::string& dummy_data, const Descriptions::Database& database);

std::istringstream MakeProcessInput(const std::string& serialized_db_filename, const Filters::Filter& f);

void TestMain();

void TestFilters();

