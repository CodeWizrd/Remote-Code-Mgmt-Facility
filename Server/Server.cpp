/////////////////////////////////////////////////////////////////////////////
// ServerTest.cpp - Test passing strings, blocks, and buffers              //
// Ver 1.1                                                                 //
// Jim Fawcett, CSE#687 - Object Oriented Design, Spring 2015              //
// CST 4-187, Syracuse University, 315 443-3948, jfawcett@twcny.rr.com     //
//-------------------------------------------------------------------------//
// Jim Fawcett (c) copyright 2015                                          //
// All rights granted provided this copyright notice is retained           //
//-------------------------------------------------------------------------//
// Application: Demo for Project #3, Communication Channel                 //
// Platform:    Dell 2720, Win Pro 8.1, Visual Studio 2013                 //
/////////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package provides tests for Sockets-4.5:
*  - passing byte buffers
*  - passing strings
*
*  Required Files:
*  ---------------
*  ClientTest.cpp, ServerTest.cpp
*  Sockets.h, Sockets.cpp, AppHelpers.h, AppHelpers.cpp
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

#include "Server.h"
using namespace MPComm;

//----< Default constructor  >-----------------------------------------------
ClientHandler::ClientHandler()
{
	
}

//----< start client handling >----------------------------------------------
Socket& ClientHandler::start()
{
	SocketSystem sockSys;
	SocketListener sockLis(9080, Socket::IP6);
	sockLis.start((receiver));
	cin.get();
	return sockLis;
}

//----< shutdown seerver  >--------------------------------------------------
void ClientHandler::shutdown(Socket & socket_)
{
	Verbose::show("Receiver socket connection closing", always);
	socket_.shutDown();
	socket_.close();
}

//----< test stub starts here >----------------------------------------------
struct Cosmetic
{
  ~Cosmetic()
  {
    std::cout << "\n  press key to exit: ";
    std::cin.get();
    std::cout << "\n\n";
  }
} aGlobalForCosmeticAction;

#ifdef TEST_SERVER
//----< demonstration >------------------------------------------------------
int main()
{
  title("Testing Socket Server", '=');

  try
  {
    Verbose v(true);
    SocketSystem ss;
    SocketListener sl(9080, Socket::IP6);
    ClientHandler cp;
    sl.start(cp);

    Verbose::show("press key to exit: ", always);
    std::cout.flush();
    std::cin.get();
  }
  catch (std::exception& ex)
  {
    Verbose::show("  Exception caught:", always);
    Verbose::show(std::string("\n  ") + ex.what() + "\n\n");
  }
}

#endif

