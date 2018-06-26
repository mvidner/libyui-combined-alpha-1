
#define BOOST_TEST_MODULE YQPkgServiceList_tests
#include <boost/test/unit_test.hpp>

#include "YQPkgServiceList.h"

#include <QApplication>
#include <zypp/RepoManager.h>
#include <zypp/Pathname.h>
#include <zypp/TmpPath.h>
#include <zypp/base/Logger.h>

#include "KeyRingTestReceiver.h"

// disable libzypp signature checking to load repodata signed by unknown key
KeyRingTestReceiver keyring_callbacks;
KeyRingTestSignalReceiver receiver;

struct MyGlobalFixture {
  // initialization before running any test
  void setup() {
      keyring_callbacks.answerAcceptKey(zypp::KeyRingReport::KEY_TRUST_TEMPORARILY);
      keyring_callbacks.answerAcceptVerFailed(true);
      keyring_callbacks.answerAcceptUnknownKey(true);
  }
  // cleanup after all tests are finished
  void teardown() {
  }
};

BOOST_TEST_GLOBAL_FIXTURE( MyGlobalFixture );

BOOST_AUTO_TEST_CASE( no_service_present )
{
    // Qt requires to have an QApplication created before creating any QWidget
    int argc = 0;
    QApplication qapp(argc, nullptr);

    YQPkgServiceList service_list(nullptr);
    // no libzypp service => empty list
    BOOST_CHECK( service_list.topLevelItemCount() == 0 );
}

BOOST_AUTO_TEST_CASE( one_service_present )
{
    // Qt requires to have an QApplication created before creating any QWidget
    int argc = 0;
    QApplication qapp(argc, nullptr);

    // save the repomanager data to a temporary directory
    zypp::filesystem::TmpDir tmpCachePath;
    zypp::RepoManagerOptions opts( zypp::RepoManagerOptions::makeTestSetup( tmpCachePath ) ) ;
    zypp::RepoManager manager(opts);

    zypp::RepoInfo repo;
    repo.setAlias("test_repo");
    repo.setService("test_service");
    repo.setBaseUrl( (zypp::Pathname(TESTS_SRC_DIR) / "/data/repo").asDirUrl() );

    // load metadata and build the cache
    manager.buildCache(repo);
    manager.loadFromCache(repo);
    
    YQPkgServiceList service_list(nullptr);
    // the list contains just one service
    BOOST_CHECK( service_list.topLevelItemCount() == 1 );
}
