/*
 **************************************************************************
 *                                                                        *
 *           General Purpose Hash Function Algorithms Test                *
 *                                                                        *
 * Author: Arash Partow - 2002                                            *
 * URL: http://www.partow.net                                             *
 * URL: http://www.partow.net/programming/hashfunctions/index.html        *
 *                                                                        *
 * Copyright notice:                                                      *
 * Free use of the General Purpose Hash Function Algorithms Library is    *
 * permitted under the guidelines and in accordance with the most current *
 * version of the Common Public License.                                  *
 * http://www.opensource.org/licenses/cpl1.0.php                          *
 *                                                                        *
 **************************************************************************
*/


#include <iostream>
#include <string>
#include <time.h>
#include "GeneralHashFunctions.h"

#define LOOP_TEST(hash_func) \
 {\
   clock_t t1, t2; \
   t1 = clock(); \
   for (long i=0; i < TEST_LOOP ; i++) { \
       hash_func(key); \
   } \
   t2 = clock(); \
   double duration = t2-t1; \
   duration /= CLOCKS_PER_SEC; \
   printf(#hash_func "%6.2f sec.\n", duration); \
}


int main(int argc, char* argv[])
{
   std::string key = "abcdefghijklmnopqrstuvwxyz1234567890";

   std::cout << "General Purpose Hash Function Algorithms Test" << std::endl;
   std::cout << "By Arash Partow - 2002        " << std::endl;
   std::cout << "Key: "                          << key           << std::endl;
   std::cout << " 1. RS-Hash Function Value:   " << RSHash(key)   << std::endl;
   std::cout << " 2. JS-Hash Function Value:   " << JSHash(key)   << std::endl;
   std::cout << " 3. PJW-Hash Function Value:  " << PJWHash(key)  << std::endl;
   std::cout << " 4. ELF-Hash Function Value:  " << ELFHash(key)  << std::endl;
   std::cout << " 5. BKDR-Hash Function Value: " << BKDRHash(key) << std::endl;
   std::cout << " 6. SDBM-Hash Function Value: " << SDBMHash(key) << std::endl;
   std::cout << " 7. DJB-Hash Function Value:  " << DJBHash(key)  << std::endl;
   std::cout << " 8. DEK-Hash Function Value:  " << DEKHash(key)  << std::endl;
   std::cout << " 9. FNV-Hash Function Value:  " << FNVHash(key)  << std::endl;
   std::cout << "10. BP-Hash Function Value:   " << BPHash(key)   << std::endl;
   std::cout << "11. AP-Hash Function Value:   " << APHash(key)   << std::endl;

   long TEST_LOOP = 50000000L;
   printf("benchmark test, perform %ld times \n", TEST_LOOP);
   LOOP_TEST(RSHash);
   LOOP_TEST(JSHash);
   LOOP_TEST(PJWHash);
   LOOP_TEST(ELFHash);
   LOOP_TEST(BKDRHash);
   LOOP_TEST(SDBMHash);
   LOOP_TEST(DJBHash);
   LOOP_TEST(DEKHash);
   LOOP_TEST(FNVHash);
   LOOP_TEST(BPHash);
   LOOP_TEST(APHash);
   return true;
}
