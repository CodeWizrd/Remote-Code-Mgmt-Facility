/////////////////////////////////////////////////////////////////////////
// Display.cpp - I/O and Conversion helpers                            //
// ver 2.0                                                             //
//---------------------------------------------------------------------//
// Jim Fawcett (c) copyright 2015                                      //
// All rights granted provided this copyright notice is retained       //
//---------------------------------------------------------------------//
// Application: OOD Projects #3, #4                                    //
// Platform:    Visual Studio 2013, Dell Inspiron 7520 , Win 8.1       //
// Modified by: Sampath Toragaravalli Janardhan, Syracuse University   //
//				(315)664-8206;  sampathjanardhan@gmail.com			   //
/////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package provides functionality to display content onto console
*
*  Required Files:
*  ---------------
*  Display.h
*
*  Build Command:
*  --------------
*  devenv MPComm.sln /rebuild debug
*
*  Maintenance History:
*  --------------------
*  ver 1.1 : 30 Mar 15
*  - minor modification to comments, above
*  ver 1.0 : 30 Mar 15
*  - first release
*/
#include "Display.h"
#include <string>
#include <iostream>

using namespace MPComm;

bool MPComm::Verbose::showState_ = false;
const bool MPComm::always = true;

std::mutex mtx;
std::unique_lock<std::mutex> iolock(mtx, std::defer_lock);

void MPComm::title(const std::string& msg, char underline)
{
  iolock.lock();
  std::cout << "\n  " << msg;
  std::cout << "\n " << std::string(msg.size() + 2, underline);
  iolock.unlock();
}

void MPComm::putLine(const std::string& msg)
{
  iolock.lock();
  std::cout << "\n" + msg; 
  iolock.unlock();
}

MPComm::Verbose::Verbose(bool showState)
{ 
  showState_ = showState; 
  if (showState)
    putLine("  Verbose mode turned on");
}

void MPComm::Verbose::show(const std::string& msg, bool always)
{
  if (always == true || showState_ == true)
  {
    std::string temp = std::string("\n  ") + msg;
    iolock.lock();
    std::cout << temp;
    std::cout.flush();
    iolock.unlock();
  }
}

#ifdef TEST_DISPLAY

int main()
{
	title("TEST DISPLAY");
	Verbose v(true);
	Verbose::show("\n\nTest Message");
	putLine("Put line  message\n");
}

#endif
