#ifndef RECEIVER_H
#define RECEIVER_H
/////////////////////////////////////////////////////////////////////////////
// Receiver.h	  - The receiver module						   			   //
//					Defines the behaviour of a receiver   				   //
//																		   //
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
Module Operations
-----------------
The module is  responsible for listening to and receiving data from a socket.

Used by the server (and / or other clients in peer-to-peer model).

COntains a blocking queue which it constantly listens to.

Provides functionality to write out uploaded file contents to a file.


Public Interface
----------------
Receiver Receiver();
Receiver createQ(BlockingQueue<Message>* pQ);
Receiver operator()(Socket& socket_);
Receiver rcvMessage(Socket& socket_);
Receiver fileHandler(Socket& socket_, int bodylen);


Required Files
--------------
Receiver.h, Receiver.cpp
BlockingQueue.h, BlockingQueue.cpp

Build Command
-------------
cl /D TEST_RECEIVER Receiver.cpp


Maintenance History
-------------------

ver 1.0:
- First implementation of the Receiver module
*/
#include <iostream>
#include "../MockChannel/Cpp11-BlockingQueue.h"
#include "../Message/Message.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Display/Display.h"

namespace MPComm
{
	using namespace std;
	using namespace FileSystem;
	class Receiver
	{
	public:
		Receiver();
		void createQ(BlockingQueue<Message>* pQ);
		void operator()(Socket& socket_);
		bool rcvMessage(Socket& socket_);
		bool fileHandler(Socket& socket_, int bodylen);

		void textSearch(Message & msg);
		void fileSearch(Message & msg);
	private:
		BlockingQueue<Message> receiverQ;
		string fileContents, searchResult;
		//helper functions
		bool Receiver::writeToFile(string fileName);
		void rcvAttributes(Socket& socket_, Message & msg);
		bool processMessage(Socket& socket_);
		void addAttribute(const string& attribute, Message & msg);
	};
}

#endif