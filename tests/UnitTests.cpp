/** This file is part of BabyMINDdaq software package. This software
 * package is designed for internal use for the Baby MIND detector
 * collaboration and is tailored for this use primarily.
 *
 * BabyMINDdaq is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BabyMINDdaq is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BabyMINDdaq.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  \author   Yordan Karadzhov <Yordan.Karadzhov \at cern.ch>
 *            University of Geneva
 *
 *  \created  Jan 2017
 */

// C++
#include <iostream>

// CppUnit
#include <cppunit/TestCase.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>

// libufec
#include "TestLibUfec.h"


CPPUNIT_TEST_SUITE_REGISTRATION(TestLibUfec);

int main(int argc, char* argv[]) {

//   SetErrorHdlr();

  using namespace CppUnit;

  // Informs test-listener about testresults.
  CPPUNIT_NS::TestResult testresult;

  // Register listener for collecting the test-results.
  CPPUNIT_NS::TestResultCollector collectedresults;
  testresult.addListener (&collectedresults);

  // Register listener for per-test progress output.
  CPPUNIT_NS::BriefTestProgressListener progress;
  testresult.addListener (&progress);

  // Insert test-suite at test-runner by registry.
  CPPUNIT_NS::TestRunner testrunner;
  testrunner.addTest (CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest ());
  testrunner.run(testresult);

  // Output results in compiler-format.
  CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults, std::cerr);
  compileroutputter.write ();

  // Output XML for Jenkins CPPunit plugin.
//   std::ofstream xmlFileOut("Mic11_cppUnitTests.xml");
//   XmlOutputter xmlOut(&collectedresults, xmlFileOut);
//   xmlOut.write();

  // Return 0 if tests were successful.
  bool success = collectedresults.wasSuccessful();
  return  success ? 0 : 1;
}
