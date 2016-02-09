/////////////////////////////////////////////////////////////////////////////
// Message.cpp - Message builder/deconstructor package      			   //
//					Creates & parses a message object					   //
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
*  Package Operations:
*  -------------------
*  This package provides tests for Message:
*  - construct messages
*  - deconstruct messages
*  - add/remove attributes & body
*
*  Required Files:
*  ---------------
*  Sockets.h, Sockets.cpp Message.cpp
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
#include "Message.h"
using namespace MPComm;

//----< Default constructor  >---------------------------------------------
Message::Message() 
{
	_command = "command : ";
}

//----< constructor to initialize parameters >------------------------------
Message::Message(const string& command)// : _command(command)
{
	_command = "command:" + command;
}

//----< constructor to initialize parameters >------------------------------
Message::Message(const string& command, const attributes& attribs) : _command(command), _attribs(attribs)
{

}

//----< constructor to initialize parameters >------------------------------
Message::Message(const attributes& attribs, const string& command) : _command(command), _attribs(attribs)
{

}

//----< add command >-------------------------------------------------------
bool Message::addCommand(string command) 
{
	try
	{
		_command += command;
	}
	catch (exception e)
	{
		return false;
	}
	return true;
}

//----< add attributes >----------------------------------------------------
bool Message::addAttribute(string name, string value)
{
	if (name == "" && value == "")
	{
		errorMsg = "Cannot add Empty attribute!";
		return false;
	}
	else if (name == "")
	{
		errorMsg = "Cannot add attribute with only value!";
		return false;
	}
	else if (value == "")
	{
		errorMsg = "Cannot add attribute with only name!";
		return false;
	}

	try
	{
		attribPair attribute;
		attribute.first = name;
		attribute.second = value;
		_attribs.push_back(attribute);
		return true;
	}
	catch (exception e)
	{
		errorMsg = "Exception raised when pushing attribute pair onto the vector!";
		return false;
	}
	
}

//----< remove  attributes >------------------------------------------------
bool Message::removeAttribute(string name)
{
	if (name == "")
	{
		errorMsg = "Cannot add attribute with only value!";
		return false;
	}
	try
	{
		bool flag = false;
		for (auto iter = _attribs.begin(); iter != _attribs.end(); ++iter)
		{
			if (iter->first == name)
			{
				_attribs.erase(iter);
				return true;
			}
		}
		errorMsg = "Attribute \"" + name + " \" not found!";
		return false;
	}
	catch (exception e)
	{
		errorMsg = "Exception raised when removing attribute pair from the vector!";
		return false;
	}
}

//----< remove all attributes >---------------------------------------------
bool Message::removeAllAttribs()
{
	try
	{
		_attribs.clear();
	}
	catch (exception e)
	{
		errorMsg = "Exception raised when removing attribute pair from the vector!";
		return false;
	}
	return true;
}

//----< add  body to message >----------------------------------------------
bool Message::addBody(const string& body)
{
	try
	{
		_body = body;
		return true;
	}
	catch (exception e)
	{
		return false;
	}
}

//----< Extract attribute name >--------------------------------------------
string Message::attributeName(const string& attribute)
{
	string name;
	if (attribute.find(" : ") != string::npos)
	{
		size_t pos = attribute.find(" : ");
		name = attribute.substr(0, pos);
	}
	return name;
}

//----< Extract attribute value >-------------------------------------------
string Message::attributeValue(const string& attribute)
{
	string value = attribute;
	if (attribute.find(" : ") != string::npos)
	{
		size_t pos = attribute.find(" : ");
		if (pos!=string::npos)
			value = attribute.substr(pos + 3);
		else
		{
			pos = attribute.find(":");
			value = attribute.substr(pos + 1);
		}
	}
	return value;
}

//----< Dsiplay header >----------------------------------------------------
void Message::print()
{
	if (!(_command == "command:" || _command == "command : "))
	{
		cout << "\n " << _command << "\n";
		for (auto iter = _attribs.begin(); iter != _attribs.end(); ++iter)
		{
			cout << " " << iter->first << " : " << iter->second << "\n";
		}
		cout << " " << _body << "\n";
	}
}

//----< Search for a message attribute >------------------------------------
string Message::searchAttributes(const string& attributeName)
{
	string attributeValue = "";
	attributes::iterator iter;
	for (iter = _attribs.begin(); iter != _attribs.end(); ++iter)
		if (iter->first.compare(attributeName) == 0)
			attributeValue = iter->second;
	return attributeValue;
}

//----< Display message attributes >----------------------------------------
void Message::printAttributes()
{
	cout << "\n";
	for (auto iter = _attribs.begin(); iter != _attribs.end(); ++iter)
	{
		cout << " " << iter->first << " : " << iter->second << "\n";
	}
	cout << " \n";
}

//----< Test module >-------------------------------------------------------
#ifdef TEST_MESSAGE
int main()
{
	Message message;
	message.addCommand("TEST_MESSAGE");
	message.addAttribute("sender IP", "localhost");
	message.addAttribute("sender Port", to_string(9070));
	message.addAttribute("receiver IP", "localhost");
	message.addAttribute("receiver Port", to_string(9080));
	message.addAttribute("Body Length", to_string(50));
	if (message.command() == "FILE_UPLOAD")
	{
		message.addAttribute("FileName", "abc.txt");
		message.terminateHeader();
	}
	else
	{
		message.terminateHeader();
		message.addBody("body");
	}
}
#endif
