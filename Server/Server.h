#ifndef SERVER_H
#define SERVER_H
/////////////////////////////////////////////////////////////////////////////
// Client.h -       The clienthandler package						   	   //
//					Defines the behaviour of a server   				   //
//																		   //
// ----------------------------------------------------------------------- //
// copyright © Sampath Janardhan, 2015                                     //
// All rights granted provided that this notice is retained                //
// ----------------------------------------------------------------------- //
// Language:    Visual C++, Visual Studio 2013                             //
// Platform:    Dell Inspiron 7520 										   //
// Application: OOD Project #4, 2015	                                   //
// Author:      Sampath Toragaravalli Janardhan, Syracuse University       //
//              (315) 664-8206, storagar@syr.edu                           //
/////////////////////////////////////////////////////////////////////////////
/*
Module Operations
-----------------
The module is  responsible for primary Server-side operations

Uses a receiver module which listens to the Client and receives messages

Provides functionality to deconstruct a Message object


Public Interface
----------------
ClientHandler ClientHandler;
ClientHandler handleBuffer(Socket& socket_);
ClientHandler handleString(Socket& socket_);
ClientHandler handleFile(Socket& socket_);
ClientHandler writeToFile(string filename, string fileContent);
ClientHandler start();
ClientHandler shutdown();


Required Files
--------------
Server.h, Server.cpp
Reciever.h, Reciever.cpp

Build Command
-------------
cl /D TEST_SERVER Server.cpp


Maintenance History
-------------------

ver 1.0: 30 Mar 15
- First implementation of the client package
*/
#include "../sockets/sockets.h"
#include "../Display/Display.h"
#include "../Message/Message.h"
#include "../Receiver/Receiver.h"
#include <iostream>
#include <fstream>
#include <exception>
#include <thread>
#include <mutex>

using namespace std;
namespace MPComm
{

	/////////////////////////////////////////////////////////////////////////////
	// Server's client handler class
	// - must be callable object so we've built as a functor
	// - passed to SocketListener::start(CallObject& co)
	// - Client handling thread starts by calling operator()
	class ClientHandler
	{
	public:
		ClientHandler();
		void operator()(Socket& socket_);
		bool handleString(Socket& socket_);
		bool handleBuffer(Socket& socket_);
		bool handleFile(Socket& socket_);
		bool writeToFile(string filename, string fileContent);
		Socket& start();
		void shutdown(Socket & si);
	private:
		Receiver receiver;
		thread *tReceiver;
	};
}

#endif
