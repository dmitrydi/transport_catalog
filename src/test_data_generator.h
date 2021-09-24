#pragma once

#include "data_generator.h"
#include "test_runner.h"
#include "json.h"
#include "utils.h"
#include "yellow_pages_private.h"

#include <iostream>
#include <fstream>

void TestRandomInt();

namespace Filters {
  void TestGenerateNamesFilter();
  void TestGenerateUrlsFilter();
  void TestGenerateRubricsFilter();
  void TestGeneratePhonesFilter();
} // ~Filters

namespace Descriptions {
  void TestGenerateRubricsMap();
  void TestMakeNameFromFilter();
  void TestMakeUrlFromFilter();
  void TestMakeRubricFromFilter();
  void TestMakePhoneFromFilter();

  void TestFillNames();
  void TestFillUrls();
  void TestFillPhones();
  void TestFillRubrics();

  void TestGenerateCompany();
} // ~Descriptions

void TestGenerateDataset();
