#ifndef WINDOW_H
#define WINDOW_H
///////////////////////////////////////////////////////////////////////////
// Window.h - C++\CLI implementation of WPF Application                  //
//          - Demo for CSE 687 Project #4                                //
// ver 2.0                                                               //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015             //
// Modified by: Sampath Toragaravalli Janardhan							 //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package demonstrates how to build a C++\CLI WPF application.  It 
*  provides one class, WPFCppCliDemo, derived from System::Windows::Window
*  that is compiled with the /clr option to run in the Common Language
*  Runtime, and another class MockChannel written in native C++ and compiled
*  as a DLL with no Common Language Runtime support.
*
*  The window class hosts, in its window, a tab control with three views, two
*  of which are provided with functionality that you may need for Project #4.
*  It loads the DLL holding MockChannel.  MockChannel hosts a send queue, a
*  receive queue, and a C++11 thread that reads from the send queue and writes
*  the deQ'd message to the receive queue.
*
*  The Client can post a message to the MockChannel's send queue.  It hosts
*  a receive thread that reads the receive queue and dispatches any message
*  read to a ListBox in the Client's FileList tab.  So this Demo simulates
*  operations you will need to execute for Project #4.
*
*  Required Files:
*  ---------------
*  Window.h, Window.cpp, MochChannel.h, MochChannel.cpp,
*  Cpp11-BlockingQueue.h, Cpp11-BlockingQueue.cpp
*
*  Build Command:
*  --------------
*  devenv CppCli-WPF-App.sln
*  - this builds C++\CLI client application and native mock channel DLL
*
*  Maintenance History:
*  --------------------
*  ver 2.0.1 : 28 Apr 15
*  - added GUI components relevant to Project 4
*  ver 2.0 : 15 Apr 2015
*  - completed message passing demo with moch channel
*  - added FileBrowserDialog to show files in a selected directory
*  ver 1.0 : 13 Apr 2015
*  - incomplete demo with GUI but not connected to mock channel
*/
/*
* Create C++/CLI Console Application
* Right-click project > Properties > Common Language Runtime Support > /clr
* Right-click project > Add > References
*   add references to :
*     System
*     System.Windows.Presentation
*     WindowsBase
*     PresentatioCore
*     PresentationFramework
*/
using namespace System;
using namespace System::Text;
using namespace System::Windows;
using namespace System::Windows::Input;
using namespace System::Windows::Markup;
using namespace System::Windows::Media;                   // TextBlock formatting
using namespace System::Windows::Controls;                // TabControl
using namespace System::Windows::Controls::Primitives;    // StatusBar
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Windows::Threading;
using namespace System::ComponentModel;

#include "../MockChannel/MockChannel.h"
#include <iostream>

namespace CppCliWindows
{
	using namespace MPComm;
  ref class WPFCppCliDemo : Window
  {
    // MockChannel references

    ISendr* pSendr_;
    IRecvr* pRecvr_;
    IMockChannel* pChann_;

    // Controls for Window

    DockPanel^ hDockPanel = gcnew DockPanel();      // support docking statusbar at bottom
    Grid^ hGrid = gcnew Grid();                    
    TabControl^ hTabControl = gcnew TabControl();
    TabItem^ hSendMessageTab = gcnew TabItem();
    TabItem^ hFileListTab = gcnew TabItem();
    TabItem^ hConnectTab = gcnew TabItem();
	TabItem^ hFileUploadTab = gcnew TabItem();
	TabItem^ hFileDownloadTab = gcnew TabItem();
	TabItem^ hTextSearchTab = gcnew TabItem();
	TabItem^ hFileSearchTab = gcnew TabItem();
    StatusBar^ hStatusBar = gcnew StatusBar();
    StatusBarItem^ hStatusBarItem = gcnew StatusBarItem();
    TextBlock^ hStatus = gcnew TextBlock();

	// Controls for Connection View

	Grid^ hConnectionGrid = gcnew Grid();
	Button^ hConnectButton = gcnew Button();
	TextBox^ hIPAddrTextbox = gcnew TextBox();
	TextBox^ hPortNumTextbox = gcnew TextBox();
	Grid^ hConnectionGrid2 = gcnew Grid();
	TextBlock^ hIPAddrTextBlock = gcnew TextBlock();
	TextBlock^ hPortNumTextBlock = gcnew TextBlock();

    // Controls for SendMessage View

    Grid^ hSendMessageGrid = gcnew Grid();
    Button^ hSendButton = gcnew Button();
    Button^ hClearButton = gcnew Button();
    TextBlock^ hTextBlock1 = gcnew TextBlock();
    ScrollViewer^ hScrollViewer1 = gcnew ScrollViewer();
    StackPanel^ hStackPanel1 = gcnew StackPanel();

    String^ msgText 
      = "Command:TestMessage\n"   // command
      + "Sender:localhost@9070\n"  // send address
      + "Reciever:localhost@9080\n"  // receive address
      + "Content-length:44\n"     // body length attribute
      + "\n"                      // end header
      + "Hello World! CSE687 - Object Oriented Design";  // message body

    // Controls for FileListView View

    Grid^ hFileListGrid = gcnew Grid();
    Forms::FolderBrowserDialog^ hFolderBrowserDialog = gcnew Forms::FolderBrowserDialog();
    ListBox^ hListBox = gcnew ListBox();
    Button^ hFolderBrowseButton = gcnew Button();
    Grid^ hGrid2 = gcnew Grid();

	// Controls for File Upload View

	Grid^ hFileUploadGrid = gcnew Grid();
	Forms::FolderBrowserDialog^ hUploadBrowserDialog = gcnew Forms::FolderBrowserDialog();
	ListBox^ hUploadBox = gcnew ListBox();
	Button^ hUploadBrowseButton = gcnew Button();
	Button^ hUploadFileButton = gcnew Button();
	Grid^ hFileUploadGrid2 = gcnew Grid();
	StackPanel^ hUploadStackPanel = gcnew StackPanel();

	// Controls for File Download View

	Grid^ hFileDownloadGrid = gcnew Grid();
	Forms::FolderBrowserDialog^ hDownloadBrowserDialog = gcnew Forms::FolderBrowserDialog();
	ListBox^ hDownloadBox = gcnew ListBox();
	Button^ hDownloadBrowseButton = gcnew Button();
	Button^ hDownloadFileButton = gcnew Button();
	Grid^ hFileDownloadGrid2 = gcnew Grid();
	StackPanel^ hDownloadStackPanel = gcnew StackPanel();

	// Controls for Text Search View
	Grid^ hTextSearchGrid = gcnew Grid();
	Button^ hTextSearchButton = gcnew Button();
	TextBox^ hPatternTextbox = gcnew TextBox();
	TextBox^ hSearchstringTextbox = gcnew TextBox();
	Grid^ hTextSearchGrid2 = gcnew Grid();
	TextBlock^ hPatternTextBlock = gcnew TextBlock();
	TextBlock^ hSearchstringBlock = gcnew TextBlock();

    // receive thread

    Thread^ recvThread;

  public:
    WPFCppCliDemo();
    ~WPFCppCliDemo();

    void setUpStatusBar();
    void setUpTabControl();
    void setUpSendMessageView();
    void setUpFileListView();
    void setUpConnectionView();
	void continueSetupConnectionView();
	void setUpFileUploadView();
	void setUpFileDownloadView();
	void setUpTextSearchView();
	void continuesetUpTextSearchView();
	void setUpFileSearchView();

    void sendMessage(Object^ obj, RoutedEventArgs^ args);
    void addText(String^ msg);
    void getMessage();
    void clear(Object^ sender, RoutedEventArgs^ args);
    void browseForFolder(Object^ sender, RoutedEventArgs^ args);
	void uploadBrowseFolder(Object^ sender, RoutedEventArgs^ args);
	void uploadFile(Object^ sender, RoutedEventArgs^ args);
	void downloadBrowseFolder(Object^ sender, RoutedEventArgs^ args);
	void downloadFile(Object^ sender, RoutedEventArgs^ args);
    void OnLoaded(Object^ sender, RoutedEventArgs^ args);
    void Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args);
	void connect(Object^ obj, RoutedEventArgs^ args);
	void textSearch(Object^ obj, RoutedEventArgs^ args);
  private:
    std::string toStdString(String^ pStr);
    String^ toSystemString(std::string& str);
    void setTextBlockProperties();
    void setButtonsProperties();
  };
}


#endif
