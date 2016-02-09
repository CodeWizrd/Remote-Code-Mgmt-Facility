#ifndef MESSAGE_H
#define MESSAGE_H
/////////////////////////////////////////////////////////////////////////////
// Message.h	  - The message builder module							   //
//					Builds & parses messages to/from strings.			   //
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
The module is responsible for encoding & decoding messages.

Used by the client to construct a message header and/or body.

Used by the server to deconstruct attributes & contents from a message.


Public Interface
----------------
Message Message();
Message Message(const string& command);
Message Message(const string& command, const attributes& attribs);
Message Message(const attributes& attribs, const string& command);
Message addCommand(string command);
Message addAttribute(string name, string value);
Message removeAttribute(string name);
Message removeAllAttribs();
Message addBody(const string& body);


Required Files
--------------
Message.h, Message.cpp

Build Command
-------------
cl /D TEST_MESSAGE Message.cpp


Maintenance History
-------------------

ver 1.0:
- First implementation of the Message builder module
*/
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <exception>

namespace MPComm
{
	using namespace std;
	class Message
	{
	public:
		using attribPair = pair < string, string >;
		using attributes = vector < attribPair >;

		Message();
		Message(const string& command);
		Message(const string& command, const attributes& attribs);
		Message(const attributes& attribs, const string& command);

		bool addCommand(string command);
		void clearCommand() { _command.erase(); }
		bool addAttribute(string name, string value);
		bool removeAttribute(string name);
		bool removeAllAttribs();
		bool addBody(const string& body);

		//public helper functions
		string command() { return _command; }
		attributes Attributes() { return _attribs; }
		string attributeValue(const string& attribute);
		string attributeName(const string& attribute);
		string terminateHeader() { return terminator; }
		string body() { return _body; }
		void print();
		string searchAttributes(const string& attributeName);
		void printAttributes();
		~Message() {}
		
	private:
		string _command;
		attributes _attribs;
		string errorMsg, terminator = "\n", _body;
	};

}

#endif