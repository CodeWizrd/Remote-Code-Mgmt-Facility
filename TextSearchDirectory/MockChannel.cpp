/////////////////////////////////////////////////////////////////////////////
// MockChannel.cpp - Creates & Manages Mock channel and sender			   //
//	v 2.0									                               //
//-------------------------------------------------------------------------//
// - build as DLL to show how C++\CLI client can use native code channel   //
// - MockChannel reads from sendQ and writes to recvQ                      //
//-------------------------------------------------------------------------//
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015               //
// All rights granted provided this copyright notice is retained           //
//-------------------------------------------------------------------------//
// Application: OOD Projects #3, #4                                        //
// Platform:    Visual Studio 2013, Dell Inspiron 7520 , Win 8.1           //
// Modified by: Sampath Toragaravalli Janardhan, Syracuse University       //
//				(315)664-8206;  sampathjanardhan@gmail.com			       //
/////////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package provides:
*  - functionality to create & manage a mock channel.
*  - the sender functionality for the clients.
*  - the message functionality localized in the DLL
*  
*
*  Required Files:
*  ---------------
*  MockChannel.h, Display.cpp
*  FileSystem.cpp, Sockets.cpp
*
*  Build Command:
*  --------------
*  devenv MPComm.sln /rebuild debug
*
*  Maintenance History:
*  --------------------
*  ver 2.0 : 28 Apr 15
*  - modified Sender & added Message
*  ver 1.1 : 30 Mar 15
*  - minor modification to comments, above
*  ver 1.0 : 30 Mar 15
*  - first release
*/
#define IN_DLL
#include "MockChannel.h"
#include "Cpp11-BlockingQueue.h"
#include "../Sockets/Sockets.h"
#include "../Display/Display.h"
#include "../FileSystem/FileSystem.h"
#include <string>
#include <thread>
#include <iostream>
using namespace std;
using namespace MPComm;
using namespace FileSystem;
using BQueue = BlockingQueue < SMessage >;


class MessageMC
{
public:
	using attribPair = pair < string, string >;
	using attributes = vector < attribPair >;

	MessageMC();
	MessageMC(const string& command);
	MessageMC(const string& command, const attributes& attribs);
	MessageMC(const attributes& attribs, const string& command);

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
	~MessageMC() {}

private:
	string _command;
	attributes _attribs;
	string errorMsg, terminator = "\n", _body;
};

using bQueue = BlockingQueue < MessageMC >;

//----< Default constructor  >---------------------------------------------
MessageMC::MessageMC()
{
	_command = "command : ";
}

//----< constructor to initialize parameters >------------------------------
MessageMC::MessageMC(const string& command)// : _command(command)
{
	_command = "command : " + command;
}

//----< constructor to initialize parameters >------------------------------
MessageMC::MessageMC(const string& command, const attributes& attribs) : _command(command), _attribs(attribs)
{

}

//----< constructor to initialize parameters >------------------------------
MessageMC::MessageMC(const attributes& attribs, const string& command) : _command(command), _attribs(attribs)
{

}

//----< add command >-------------------------------------------------------
bool MessageMC::addCommand(string command)
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
bool MessageMC::addAttribute(string name, string value)
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
bool MessageMC::removeAttribute(string name)
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
bool MessageMC::removeAllAttribs()
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
bool MessageMC::addBody(const string& body)
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
string MessageMC::attributeName(const string& attribute)
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
string MessageMC::attributeValue(const string& attribute)
{
	string value = attribute;
	if (attribute.find(" : ") != string::npos)
	{
		size_t pos = attribute.find(" : ");
		value = attribute.substr(pos + 3);
	}
	return value;
}

//----< Dsiplay header >----------------------------------------------------
void MessageMC::print()
{

	cout << "\n " << _command << "\n";
	for (auto iter = _attribs.begin(); iter != _attribs.end(); ++iter)
	{
		cout << " " << iter->first << " : " << iter->second << "\n";
	}
	cout << " \n";
}

//----< Search for a message attribute >------------------------------------
string MessageMC::searchAttributes(const string& attributeName)
{
	string attributeValue = "";
	attributes::iterator iter;
	for (iter = _attribs.begin(); iter != _attribs.end(); ++iter)
		if (iter->first.compare(attributeName) == 0)
			attributeValue = iter->second;
	return attributeValue;
}

//----< Display message attributes >----------------------------------------
void MessageMC::printAttributes()
{
	cout << "\n";
	for (auto iter = _attribs.begin(); iter != _attribs.end(); ++iter)
	{
		cout << " " << iter->first << " : " << iter->second << "\n";
	}
	cout << " \n";
}


/////////////////////////////////////////////////////////////////////////////
// Sendr class
// - accepts messages from client for consumption by MockChannel
//
std::mutex ioLock;

class SenderMC : public ISendr
{
public:
	void postMessage(const SMessage& msg);
	BQueue& queue();
	SenderMC();
	SenderMC(string ip, int port);

	void handleString();
	void handleFileUpload(string filename);
	void handleFileDownload(string filename);
	void sendString(string message);
	void buildMessage();
	void buildMessage(const string & msg);
	bool sendMessage();
	void sendMessage(const string & messsage);
	void sendFile(File bufferIn, string filename);
	void sendFile(File buffer);
	bool connect(string ip, int port);
	void textSearch(const string& pattern, const string& keyword);
	void fileSearch(const string& pattern, const string& path);
private:
	BQueue sendQ_;
	bQueue senderQ;
	string _ip;
	int _port;
	size_t bodyLength, senderPort, receiverPort;
	string _command, _body, senderIP, receiverIP, _filename;
	string _filePattern, _path, _searchString;
	SocketSystem sS;
	SocketConnecter sC;
	bool socketConnected;
	// private helper functions
	void processAddAttributes(MessageMC& msg, const string& str);
};

BQueue& SenderMC::queue() { return sendQ_; }

//----< default constructor for the sender >--------------------------------
SenderMC::SenderMC() : socketConnected(false), senderIP("localhost"), senderPort(9070), receiverIP("localhost"), 
receiverPort(9080), _command("TEST")
{
	_body = "Hello World! \n CSE 687 - Object Oriented Design (Spring 2015)";
	bodyLength = _body.size();
}

//----< constructor for the sender >----------------------------------------
SenderMC::SenderMC(string ip, int port) : _ip(ip), _port(port)
{
	
}

//----< connect to the socket >---------------------------------------------
bool SenderMC::connect(string ip, int port)
{
	socketConnected = sC.connect(ip, port);
	return socketConnected;
}

//----< posts a terminator terminated string to connected socket >-----------
void SenderMC::postMessage(const SMessage& msg)
{
	if (socketConnected)
	{
		buildMessage(msg);
		sendMessage();
	}
	else
		Verbose::show("Not connected to any server or client!", always);

	if (sC.waitForData(100, 10))
	{
		while (true)
		{
			std::string str = sC.recvString();
			if (str.size() == 0 || str == "ACK")
			{
				Verbose::show("Acknowledgement Received", always);
				break;
			}
			
		}
	}
	else
	{
		Verbose::show("client wait for server timed-out", always);
	}

}

//----< test string handling - server acknowledges client sent string >------
void SenderMC::handleString()
{
	MessageMC msg;
	buildMessage();
	sendMessage();
}

//----< test file uploading - server acknowledges client sent string >-------
void SenderMC::handleFileUpload(string filename)
{
	_command = "FILE_UPLOAD";
	string file, directory;

	File bufferIn(filename);
	Verbose::show(FileSystem::Directory::getCurrentDirectory());
	bufferIn.open(File::in, File::binary);
	if (!bufferIn.isGood())
	{
		std::cout << "\n  could not open \"" << filename << "\" for reading";
		return;
	}
	else
	{
		std::cout << "\n  opening: \"" << filename << "\" for reading";
	}

	sendFile(bufferIn, filename);
}

//----< costructs a message from the given string >--------------------------
void SenderMC::buildMessage(const string & msg)
{
	MessageMC message;
	size_t pos = msg.find("\n");
	string command = msg.substr(0, pos);
	message.clearCommand();
	message.addCommand(command);
	size_t pos2 = msg.find("\n\n");
	string attributesBulk = msg.substr(pos + 1, pos2 - pos);
	processAddAttributes(message, attributesBulk);
	message.terminateHeader();
	string body = msg.substr(pos2 + 2, string::npos);
	message.addBody(body);
	senderQ.enQ(message);
}

//----< processes a message header and extracts attributes >----------------
void SenderMC::processAddAttributes(MessageMC& message, const string & str)
{
	string temp, name, value;
	size_t start = 0, length = 0, colon = 0;
	while (!(start > str.length()))
	{
		length = str.find("\n", start);
		temp = str.substr(start, length);
		colon = temp.find(":");
		if (colon != string::npos)
		{
			name = temp.substr(0, colon);
			value = temp.substr(colon + 1, string::npos);
			message.addAttribute(name, value);
		}
		start += length + 1;
		if (temp.compare("\n") == 0)
			break;
	}
}

//----< costructs a message header and a message body (if present) >---------
void SenderMC::buildMessage()
{
	MessageMC message;
	message.addCommand(_command);
	message.addAttribute("sender IP", senderIP);
	message.addAttribute("sender Port", to_string(senderPort));
	message.addAttribute("receiver IP", receiverIP);
	message.addAttribute("receiver Port", to_string(receiverPort));
	
	if (_command == "FILE_UPLOAD")
	{
		message.addAttribute("Content Length", to_string(bodyLength));
		message.addAttribute("FileName", _filename);
		message.terminateHeader();
	}
	else if (_command == "FILE_DOWNLOAD")
	{
		message.addAttribute("Content Length", 0);
		message.addAttribute("FileName", _filename);
		message.terminateHeader();
	}
	else if (_command == "TEXT_SEARCH")
	{
		message.addAttribute("Content Length", 0);
		message.addAttribute("pattern", _filePattern);
		message.addAttribute("searchstring", _searchString);
		message.terminateHeader();
	}
	else if (_command == "FILE_SEARCH")
	{
		message.addAttribute("Content Length", 0);
		message.addAttribute("pattern", _filePattern);
		message.addAttribute("path", _path);
		message.terminateHeader();
	}
	else
	{
		bodyLength = _body.size();
		message.addAttribute("Content Length", to_string(bodyLength));
		message.terminateHeader();
		message.addBody(_body);
	}
	senderQ.enQ(message);
}

//----< sends the file to the appropriate socket >---------------------------
void SenderMC::sendFile(File buffer, string filename)
{
	_command = "FILE_UPLOAD";
	_filename = filename;
	senderIP = "localhost";
	senderPort = 9070;
	receiverIP = "localhost";
	receiverPort = 9080;
	//bodyLength = 
	Verbose::show("Sending File to Receiver", always);
	sendFile(buffer);
}

//----< sends the file through a buffer of set capacity >--------------------
void SenderMC::sendFile(File bufferIn)
{
	const int BufCapacity = 1024;
	Socket::byte buffer[BufCapacity];
	int total = 0;
	while (true)
	{
		for (int i = 0; i < BufCapacity; i++)
			buffer[i] = NULL;
		bodyLength = bufferIn.getBuffer(BufCapacity, buffer);
		buildMessage();
		sendMessage();
		sC.send(bodyLength, buffer);
		for (int i = 0; i < BufCapacity; i++)
			buffer[i] = NULL;
		if (bodyLength < BufCapacity || !bufferIn.isGood())
		{
			bufferIn.close();
			break;
		}
	}
}

//----< sends a terminator terminated string to given socket >---------------
bool SenderMC::sendMessage()
{
	MessageMC message = senderQ.deQ();
	if (message.Attributes().size() == 0 && message.command().length() == 0) return false;
	else
	{
		sC.sendString(message.command());
		MessageMC::attributes::iterator iter;
		MessageMC::attributes mAttributes = message.Attributes();
		for (iter = mAttributes.begin(); iter != mAttributes.end(); ++iter)
			sC.sendString(iter->first + " : " + iter->second);
		sC.sendString(message.terminateHeader());

		string body = message.body();
		if (body.length() > 0)
			sC.sendString(body);
		return true;
	}
}

//----< sends a terminator terminated string to given socket >---------------
void SenderMC::sendMessage(const string & message)
{
	sC.sendString(message);
}

//----< test file downloading - server acknowledges client sent string >-----
void SenderMC::handleFileDownload(string filename)
{
	string file, directory;
	MessageMC message;
	message.clearCommand();
	_command = "FILE_DOWNLOAD";
	_filename = filename;
	buildMessage();
	sendMessage();
}

//----< searches for a given regular expression pattern >--------------------
void SenderMC::textSearch(const string& pattern, const string& keyword)
{
	//if (_command.size() == 0)
	_command = "TEXT_SEARCH";
	_filePattern = pattern;
	_searchString = keyword;
	bodyLength = 0;
	buildMessage();
	sendMessage();
}

//----< searches for a file from a regex pattern >---------------------------
void SenderMC::fileSearch(const string& pattern, const string& path)
{
	if (_command.size() == 0)
		_command = "FILE_SEARCH";
	_filePattern = pattern;
	_path = path;
	bodyLength = 0;
	buildMessage();
	sendMessage();
}


/////////////////////////////////////////////////////////////////////////////
// Recvr class
// - accepts messages from MockChanel for consumption by client
//
class Recvr : public IRecvr
{
public:
  SMessage getMessage();
  BQueue& queue();
private:
  BQueue recvQ_;
};

//----< receive message from Sender >--------------------------------------
SMessage Recvr::getMessage()
{
  return recvQ_.deQ();
}


BQueue& Recvr::queue()
{
  return recvQ_;
}
/////////////////////////////////////////////////////////////////////////////
// MockChannel class
// - reads messages from Sendr and writes messages to Recvr
//
class MockChannel : public IMockChannel
{
public:
  MockChannel(ISendr* pSendr, IRecvr* pRecvr);
  void start();
  void stop();
private:
  std::thread thread_;
  ISendr* pISendr_;
  IRecvr* pIRecvr_;
  bool stop_ = false;
};

//----< pass pointers to Sender and Receiver >-------------------------------
MockChannel::MockChannel(ISendr* pSendr, IRecvr* pRecvr) : pISendr_(pSendr), pIRecvr_(pRecvr) {}

//----< creates thread to read from sendQ and echo back to the recvQ >-------
void MockChannel::start()
{
  std::cout << "\n  MockChannel starting up";
  thread_ = std::thread(
    [this] {
	SenderMC* pSendr = dynamic_cast<SenderMC*>(pISendr_);
    Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);
    if (pSendr == nullptr || pRecvr == nullptr)
    {
      std::cout << "\n  failed to start Mock Channel\n\n";
      return;
    }
    BQueue& sendQ = pSendr->queue();
    BQueue& recvQ = pRecvr->queue();
    while (!stop_)
    {
      std::cout << "\n  channel deQing message";
      SMessage msg = sendQ.deQ();  // will block here so send quit message when stopping
      std::cout << "\n  channel enQing message";
      recvQ.enQ(msg);
    }
    std::cout << "\n  Server stopping\n\n";
  });
}

//----< signal server thread to stop >---------------------------------------
void MockChannel::stop() { stop_ = true; }

//----< factory functions >--------------------------------------------------
//----< create sender >------------------------------------------------------
ISendr* ObjectFactory::createSendr() { /*return new Sendr;*/ return new SenderMC; }

//----< create receiver >----------------------------------------------------
IRecvr* ObjectFactory::createRecvr() { return new Recvr; }

//----< create mock channel >-------------------------------------------------
IMockChannel* ObjectFactory::createMockChannel(ISendr* pISendr, IRecvr* pIRecvr) 
{ 
  return new MockChannel(pISendr, pIRecvr); 
}

//----< test stub  >----------------------------------------------------------
#ifdef TEST_MOCKCHANNEL

//----< test stub >----------------------------------------------------------

int main()
{
  ObjectFactory objFact;
  ISendr* pSendr = objFact.createSendr();
  IRecvr* pRecvr = objFact.createRecvr();
  IMockChannel* pMockChannel = objFact.createMockChannel(pSendr, pRecvr);
  pMockChannel->start();
  pSendr->postMessage("Hello World");
  pSendr->postMessage("CSE687 - Object Oriented Design");
  string filename = "abc.txt";
  pSendr->handleFileUpload(filename);
  string downloadfile = "def.txt";
  pSendr->handleFileDownload(downloadfile);
  MessageMC msg = pRecvr->getMessage();
  msg = pRecvr->getMessage();
  pSendr->postMessage("stopping");
  msg = pRecvr->getMessage();
  pMockChannel->stop();
  pSendr->postMessage("quit");
  std::cin.get();
}
#endif
