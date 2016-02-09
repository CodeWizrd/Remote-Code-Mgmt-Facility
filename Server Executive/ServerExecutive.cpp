/////////////////////////////////////////////////////////////////////////////
// ServerExecutive.cpp - Executive package for Server		     		   //
//					Creates & initiates a Server						   //
//					Calls the relevant functions to demonstrate a Server's //
//					functionality										   //
// ----------------------------------------------------------------------- //
// copyright © Sampath Janardhan, 2015                                     //
// All rights granted provided that this notice is retained                //
// ----------------------------------------------------------------------- //
// Language:    Visual C++, Visual Studio 2013                             //
// Platform:    Dell Inspiron 7520 										   //
// Application: OOD Project, 2015	                                       //
// Author:      Sampath Toragaravalli Janardhan, Syracuse University       //
//              (315) 664-8206, storagar@syr.edu                           //
/////////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package provides tests for Server operations:
*  - test receiving messages
*  - test receiving files
*
*  Required Files:
*  ---------------
*  Server.cpp, FileSystem.cpp
*  Sockets.h, Sockets.cpp Message.cpp
*
*  Build Command:
*  --------------
*  devenv MPComm.sln /rebuild debug
*
*  Maintenance History:
*  --------------------
*  ver 1.0 : 20 Apr 15
*  - first release
*/
#include <iostream>
#include <string>
#include "../Server/Server.h"
#include "../Display/Display.h"
#include "../FileSystem/FileSystem.h"
#include "../Receiver/Receiver.h"
#include "../Sockets/Sockets.h"
using namespace std;
using namespace MPComm;

int main(int argc, char* argv[])
{
	title("Server Executive listening at 9080", '=');

	try
	{
		Verbose v(false);
		ClientHandler cH;
		Socket& sock = cH.start();
		cH.shutdown(sock);
		std::cout.flush();
	}
	catch (std::exception& ex)
	{
		Verbose::show(" Error! ", always);
		Verbose::show(ex.what());
	}
}