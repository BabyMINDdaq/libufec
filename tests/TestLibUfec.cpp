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

#include "TestLibUfec.h"

using namespace std;

extern ufe_context *ufe_context_handler;

void TestLibUfec::setUp() {
  
}

void TestLibUfec::tearDown() {
  
}

void TestLibUfec::TestContext() {
  // Test that the context is empty at start.
  CPPUNIT_ASSERT( ufe_context_handler == NULL );

  // Test default initialization.
  ufe_context *ctx_1 = NULL;
  ufe_default_context(&ctx_1);

  CPPUNIT_ASSERT( ctx_1 != NULL );
  CPPUNIT_ASSERT( ufe_context_handler == ctx_1 );
  CPPUNIT_ASSERT( ctx_1->verbose_ == 1 );
  CPPUNIT_ASSERT( ctx_1->readout_buffer_size_ == 1024*32 );
  CPPUNIT_ASSERT( ctx_1->readout_timeout_ == 100 );

  ctx_1->verbose_ = 4;

  // Test assignment.
  ufe_context *ctx_2 = ctx_1;
  CPPUNIT_ASSERT( ufe_context_handler == ctx_2 );
  CPPUNIT_ASSERT( ctx_2->verbose_ == 4 );
  CPPUNIT_ASSERT( ctx_2->readout_buffer_size_ == 1024*32 );
  CPPUNIT_ASSERT( ctx_2->readout_timeout_ == 100 );

  // Test reset to default.
  ufe_default_context(&ctx_2);
  CPPUNIT_ASSERT( ctx_1 != ctx_2 );
  CPPUNIT_ASSERT( ufe_context_handler == ctx_2 );
  CPPUNIT_ASSERT( ctx_2->verbose_ == 1 );
  CPPUNIT_ASSERT( ctx_2->readout_buffer_size_ == 1024*32 );
  CPPUNIT_ASSERT( ctx_2->readout_timeout_ == 100 );
}

void TestLibUfec::TestPrint() {
  ufe_context *ctx = NULL;
  ufe_default_context(&ctx);

  // Test Max verbose.
  ctx->verbose_ = 3;

  int r = 0;
  r = ufe_error_print("test");
  CPPUNIT_ASSERT( r == 4 );

#ifdef UFE_WARNING
  r = ufe_warning_print("test %i", 99);
  CPPUNIT_ASSERT( r == 7 );
#endif

#ifdef UFE_INFO
  r = ufe_info_print("test %i", 1);
  CPPUNIT_ASSERT( r == 6 );
#endif

#ifdef UFE_DEBUG
  r = ufe_debug_print("test %c", 'a');
  CPPUNIT_ASSERT( r == 6 );
#endif

  // Test no debug.
  ctx->verbose_ = 2;
  r = ufe_error_print("test");
  CPPUNIT_ASSERT( r == 4 );

#ifdef UFE_WARNING
  r = ufe_warning_print("test %i", 99);
  CPPUNIT_ASSERT( r == 7 );
#endif

#ifdef UFE_INFO
  r = ufe_info_print("test %i", 1);
  CPPUNIT_ASSERT( r == 6 );
#endif

  r = ufe_debug_print("test %c", 'a');
  CPPUNIT_ASSERT( r == 0 );


  // Test no debug and info.
  ctx->verbose_ = 1;
  r = ufe_error_print("test");
  CPPUNIT_ASSERT( r == 4 );

#ifdef UFE_WARNING
  r = ufe_warning_print("test %i", 99);
  CPPUNIT_ASSERT( r == 7 );
#endif

  r = ufe_info_print("test %i", 1);
  CPPUNIT_ASSERT( r == 0 );

  r = ufe_debug_print("test %c", 'a');
  CPPUNIT_ASSERT( r == 0 );


  // Test error only.
  ctx->verbose_ = 0;
  r = ufe_error_print("test");
  CPPUNIT_ASSERT( r == 4 );

  r = ufe_warning_print("test %i", 99);
  CPPUNIT_ASSERT( r == 0 );

  r = ufe_info_print("test %i", 1);
  CPPUNIT_ASSERT( r == 0 );

  r = ufe_debug_print("test %c", 'a');
  CPPUNIT_ASSERT( r == 0 );

  // Test mute.
  ctx->verbose_ = -1;
  r = ufe_error_print("test");
  CPPUNIT_ASSERT( r == 0 );

  r = ufe_warning_print("test %i", 99);
  CPPUNIT_ASSERT( r == 0 );

  r = ufe_info_print("test %i", 1);
  CPPUNIT_ASSERT( r == 0 );

  r = ufe_debug_print("test %c", 'a');
  CPPUNIT_ASSERT( r == 0 );

}




