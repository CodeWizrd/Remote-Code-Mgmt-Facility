/////////////////////////////////////////////////////////////////////////////
// Receiver.cpp -   Package for Receiver					     		   //
//					Creates & initiates a Receiver						   //
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
*  Package Operations:
*  -------------------
*  This package provides tests for Receiver:
*  - receiving byte buffers
*  - receiving strings
*  - receiving files
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
*  ver 1.2 : 28 Apr 15
*  - added timestamping on received files
*  - Improved processing on message receive
*  ver 1.1 : 30 Mar 15
*  - minor modification to comments, above
*  ver 1.0 : 30 Mar 15
*  - first release
*/
#include "Receiver.h"
#include <algorithm>
#include "../MockChannel/MockChannel.h"
using namespace MPComm;

//----< default constructor for the receiver >------------------------------
Receiver::Receiver()
{
	
}

//----< receiver thread starts from here >----------------------------------
void Receiver::operator()(Socket& socket_)
{
	rcvMessage(socket_);
	Verbose::show("", always);
}

//----< receive the message from client >-----------------------------------
bool Receiver::rcvMessage(Socket& socket_)
{
	while (true)
	{
		Message msg; 
		string message = socket_.recvString();
		string msgInLCase = message;
		transform(msgInLCase.begin(), msgInLCase.end(), msgInLCase.begin(), ::tolower);
		if (message.length() > 0)
		{
			if (msgInLCase.compare("stop") == 0)
			{
				Verbose::show("STOP command received. Exiting");
				return true;
			}
			if (msgInLCase.find("command") != string::npos)
			{
				msg.clearCommand();
				msg.addCommand(msg.attributeValue(message));
				rcvAttributes(socket_, msg);
				if (msgInLCase.find("test") != string::npos || msgInLCase.find("message") != string::npos)
				{
					string body = socket_.recvString();
					msg.addBody(body); 
				}
			}
			receiverQ.enQ(msg);
			processMessage(socket_);
		}
	}
	return true;
}

//----< retreive attributes from the message >------------------------------
void Receiver::rcvAttributes(Socket & socket_, Message & msg)
{
	while (true)
	{
		string message = socket_.recvString();
		if (message.compare("\n") == 0)
			break;
		addAttribute(message, msg);
	}
}

//----< add attributes from the message >-----------------------------------
void Receiver::addAttribute(const string & attribute, Message & msg)
{
	string name = msg.attributeName(attribute);
	string value = msg.attributeValue(attribute);
	bool result = msg.addAttribute(name, value);
}

//----< Get current date/time, format is YYYY-MM-DD.HH:mm:ss >--------------
const std::string currentDateTime() 
{
	char buf[80];
	struct tm newtime;
	__time64_t long_time;
	_time64(&long_time);
	_localtime64_s(&newtime, &long_time);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &newtime);
	return buf;
}

//----< process the message from client >-----------------------------------
bool Receiver::processMessage(Socket & socket_) {
	Message message = receiverQ.deQ();
	string command = message.attributeValue(message.command());
	if (command.find("FILE_UPLOAD") != string::npos) {
		int bodylen = atoi(message.searchAttributes("Body Length").c_str());
		string fileName = message.searchAttributes("FileName");
		fileHandler(socket_, bodylen);
		if (bodylen < 1024) 		{
			Verbose::show("Accepting File Contents", always);
			Verbose::show("File Name - " + fileName, always);
			message.printAttributes();
			if (fileName == "") fileName = "uploaded.txt";
			string file, directory;
			size_t pos = fileName.find_last_of("\\");
			directory = fileName.substr(0, pos + 1);
			file = fileName.substr(pos + 1, string::npos);
			fileName = /*currentDateTime() +*/ "Server_" + file;
			cout << "\n fileName: " << fileName;
			writeToFile(fileName);
		}
	}
	else if (command.find("FILE_DOWNLOAD") != string::npos) {
		string fileName = message.searchAttributes("FileName");
	}
	else if (command.find("TEXT_SEARCH") != string::npos) 	{
		textSearch(message);
		if (searchResult.empty())
			socket_.sendString("The query did not return any matches!");
		else
			socket_.sendString(searchResult);
		Verbose::show(searchResult, always);
		searchResult.clear();
	}
	else if (command.find("FILE_SEARCH") != string::npos) 	{
	}
	else  {
		message.print();
	}
	message.removeAllAttribs();
	socket_.sendString("ACK");
	return false;
}

//----< handles the file receiving from client >----------------------------
bool Receiver::fileHandler(Socket & socket_, int bodylen)
{
	const int BufLen = 1024;
	Socket::byte buffer[BufLen];
	bool ok;

	//cout << "\n bodyLen: " << bodylen << "\n";

	if (socket_.bytesWaiting() == 0)
	{
		Verbose::show("no bytes waiting", always);
		return false;
}
	ok = socket_.recv(bodylen, buffer);
	if (socket_ == INVALID_SOCKET || !ok)
	{
		Verbose::show("invalid transfer", always);
		return false;
	}
	std::string temp;
	for (int i = 0; i < bodylen; ++i)
		temp += buffer[i];

	fileContents += temp;
	return true;
}

//----< writes file contents to a file>-------------------------------------
bool Receiver::writeToFile(string fileName)
{
	try
	{
		//FileSystem::Directory::setCurrentDirectory("..//Data");
		string currDir = FileSystem::Directory::getCurrentDirectory();
		ofstream myfile;
		myfile.open(fileName);
		myfile << fileContents;
		myfile.close();
		Verbose::show("", always);
		Verbose::show("File written to " + currDir + "\\" + fileName, always);

		return true;
}
	catch (std::exception& ex)
	{
		Verbose::show("  Exception caught:", always);
		Verbose::show(std::string("\n  ") + ex.what() + "\n\n");
		return false;
	}
}

//----< searches specified string in a given file pattern >-----------------
void Receiver::textSearch(Message & msg)
{
	string filePattern = msg.searchAttributes("pattern");
	string searchString = msg.searchAttributes("searchstring");

	FileSystem::Directory::setCurrentDirectory("..//TextSearchDirectory");
	string currDir = FileSystem::Directory::getCurrentDirectory();
	Verbose::show("\n Text Search request received.", always);
	Verbose::show("Searching for files in directory" + currDir, always);
	Verbose::show("Search text " + searchString, always);
	Verbose::show("Pattern " + filePattern, always);
	vector<string> fileVec = Directory::getFiles(currDir, filePattern);
	vector<string>::iterator it;

	for (it = fileVec.begin(); it != fileVec.end(); ++it)
	{
		string filename = *it;
		string path = currDir + "\\" + filename;
		File file(path);
		file.open(File::in);
		if (file.isGood())
		{
			string contents = file.readAll();
			int found = -1;
			if (contents.find(searchString) != string::npos)
			{
				found = contents.find("keyword");
				searchResult += "Found in " + filename + " at position " + toString(found) + " ";
			}
}
		file.close();
	}
}

//----< Test module >-------------------------------------------------------
#ifdef TEST_RECEIVER
// Both sender & reciver need to run simultaneously for the test to work.
int main()
{
	Verbose::show("Testing Receiver Module", always);
	Receiver receiver;
	SocketSystem sockSys;
	SocketListener sockLis(9080, Socket::IP6);

	// runs on a separate thread
	sockLis.start((receiver)); 
	// The receiver runs till the socket closes
	Verbose::show("Receiver socket connection closing", always);
	sockLis.shutDown();
	sockLis.close();
}

#endif