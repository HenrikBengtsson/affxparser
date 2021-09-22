////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License 
// (version 2.1) as published by the Free Software Foundation.
// 
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
//
////////////////////////////////////////////////////////////////


#include <util/md5sum.h>
//
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
//
using namespace std;
using namespace affx;

const char* self_test_data[]={
  // input, output
  "","d41d8cd98f00b204e9800998ecf8427e",
  "a","0cc175b9c0f1b6a831c399e269772661",
  "The quick brown fox jumps over the lazy dog","9e107d9d372bb6826bd81d3542a419d6",
  "The quick brown fox jumps over the lazy cog","1055d3e698d289f2af8663725127bd4b",
  NULL, NULL
};

void
self_test()
{
  md5sum md5sum;
  std::string sum;
  const char** dataptr;

  dataptr=self_test_data;

  while (*dataptr!=NULL) {
    // generate the hashes.  (we could have used "ofString()")
    md5sum.init();
    md5sum.update(*dataptr);
    md5sum.final(sum);
    //
    printf("%-15s: \"%s\"\n%15s: %32s\n%15s: %32s\n\n",
           "String",    *dataptr,
           "Reference", *(dataptr+1),
           "Computed",  sum.c_str());
    // computed MD5 should equal reference
    assert(sum==*(dataptr+1));
    // step input and reference.
    dataptr+=2;
  }
}

//////////

void nbo_test_int(unsigned int val,const std::string& ref)
{
  md5sum md5sum;
  std::string sum;
  
  md5sum.init();
  md5sum.update_nbo(val);
  md5sum.final(sum);
 
  printf("0x%08x -> %32s\n",val,sum.c_str());
  assert(sum==ref);
}

void
nbo_test()
{
  md5sum md5sum;
  std::string sum;

  //
  nbo_test_int(     0,"f1d3ff8443297732862df21dc4e57262");
  nbo_test_int(     1,"f1450306517624a57eafbbf8ed995985");
  nbo_test_int(123456,"d747074027ce566f5dd8697337091787");
}


//
int
main(int argc,char* argv[])
{
  if (argc==1) {
    self_test();
    nbo_test();
  }
  else {
    for (int a=1;a<argc;a++) {
      md5sum md5sum;
      std::string sum;
      md5sum.ofFile(argv[a],sum);
      printf("MD5(%s) = %s\n",argv[a],sum.c_str());
    }
  }
  //
  printf("ok.\n");
}
