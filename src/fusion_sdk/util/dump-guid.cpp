////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This program is free software; you can redistribute it and/or modify 
// it under the terms of the GNU General Public License (version 2) as 
// published by the Free Software Foundation.
// 
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License 
// along with this program;if not, write to the 
// 
// Free Software Foundation, Inc., 
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
////////////////////////////////////////////////////////////////

#include <unistd.h>
#include "Guid.h"

int
main(int argc,char* argv[])
{
  //
  printf("time =%10d\n",(int)time(NULL) );
  printf("pid  =%10d\n",(int)getpid()   );
  printf("host =%10d\n",(int)gethostid());
  affxutil::Guid guidmaker;
  affxutil::GuidType guid;  

  int n = 5;

  if(argc>1) { n = atoi(argv[1]); }

  for (int i=0;i<n;i++) {
    guid=guidmaker.GenerateNewGuid();
    printf("%2d : %s\n",i,guid.c_str());
  }
}

  
