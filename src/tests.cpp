
#include "tests.h"
#include <fstream>

using namespace std;

void TestAll() {
  TestRunner tr;

  RUN_TEST(tr, Descriptions::TestAddress);
  RUN_TEST(tr, Descriptions::TestName);
  RUN_TEST(tr, Descriptions::TestPhone);
  RUN_TEST(tr, Descriptions::TestUrl);
  RUN_TEST(tr, Descriptions::TestWorkingTime);
  RUN_TEST(tr, Descriptions::TestNearbyStop);
  RUN_TEST(tr, Descriptions::TestCompany);
  RUN_TEST(tr, Descriptions::TestRubric);
  RUN_TEST(tr, Descriptions::TestDatabase);

  RUN_TEST(tr, Filters::TestPhoneParsing);
  RUN_TEST(tr, Filters::TestFilterParsing);

  RUN_TEST(tr, Requests::TestCompanies);

  RUN_TEST(tr, MapToRubric);
  RUN_TEST(tr, NameMatch);
  RUN_TEST(tr, DoesPhoneMatch);
  RUN_TEST(tr, UrlMatch);
  RUN_TEST(tr, RubricMatch);
  RUN_TEST(tr, DoesPhoneMatch);

  RUN_TEST(tr, TestRandomInt);

  RUN_TEST(tr, Filters::TestGenerateNamesFilter);
  RUN_TEST(tr, Filters::TestGenerateUrlsFilter);
  RUN_TEST(tr, Filters::TestGenerateRubricsFilter);
  RUN_TEST(tr, Filters::TestGeneratePhonesFilter);

  RUN_TEST(tr, Descriptions::TestGenerateRubricsMap);
  RUN_TEST(tr, Descriptions::TestMakeNameFromFilter);
  RUN_TEST(tr, Descriptions::TestMakeUrlFromFilter);
  RUN_TEST(tr, Descriptions::TestMakeRubricFromFilter);
  RUN_TEST(tr, Descriptions::TestMakePhoneFromFilter);

  RUN_TEST(tr, Descriptions::TestFillNames);
  RUN_TEST(tr, Descriptions::TestFillUrls);
  RUN_TEST(tr, Descriptions::TestFillPhones);
  RUN_TEST(tr, Descriptions::TestFillRubrics);

  RUN_TEST(tr, Descriptions::TestGenerateCompany);

  RUN_TEST(tr, TestGenerateDataset);

  RUN_TEST(tr, TestPhoneToOstream);
  RUN_TEST(tr, TestCompanyToOstream);

  RUN_TEST(tr, TestDatabaseToOstream);
  RUN_TEST(tr, TestMain);
  RUN_TEST(tr, DoesPhoneMatch2);

}

