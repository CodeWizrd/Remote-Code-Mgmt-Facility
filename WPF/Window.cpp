///////////////////////////////////////////////////////////////////////////
// Window.cpp - C++\CLI implementation of WPF Application                //
//          - Demo for CSE 687 Project #4                                //
// ver 2.0                                                               //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015             //
// Modified by: Sampath Toragaravalli Janardhan							 //
///////////////////////////////////////////////////////////////////////////
/*
*  To run as a Windows Application:
*  - Set Project Properties > Linker > System > Subsystem to Windows
*  - Comment out int main(...) at bottom
*  - Uncomment int _stdcall WinMain() at bottom
*  To run as a Console Application:
*  - Set Project Properties > Linker > System > Subsytem to Console
*  - Uncomment int main(...) at bottom
*  - Comment out int _stdcall WinMain() at bottom
*
*  Package Operations:
*  -------------------
*  This package provides a GUI client in a socket conneciton:
*  - Connect to a server socket
*  - Send messages to the connected socket
*  - Upload files to the server
*  - Download files from the server
*
*  Required Files:
*  ---------------
*  MockChannel.dll
*  BlockingQueue.h, BlockingQueue.cpp
*
*  Build Command:
*  --------------
*  devenv MPComm.sln /rebuild debug
*
*  Maintenance History:
*  --------------------
*  ver 1.2 : 28 Apr 15
*  - setup the GUI for Project 4
*  ver 1.1 : 30 Mar 15
*  - minor modification to comments, above
*  ver 1.0 : 30 Mar 15
*  - first release
*/
#include "Window.h"
using namespace CppCliWindows;
using namespace MPComm;
using namespace std;

WPFCppCliDemo::WPFCppCliDemo()
{
  // set up channel
  ObjectFactory* pObjFact = new ObjectFactory;
  pSendr_ = pObjFact->createSendr();
  pRecvr_ = pObjFact->createRecvr();
  pChann_ = pObjFact->createMockChannel(pSendr_, pRecvr_);
  pChann_->start();
  delete pObjFact;
  // client's receive thread
  recvThread = gcnew Thread(gcnew ThreadStart(this, &WPFCppCliDemo::getMessage));
  recvThread->Start();
  // set event handlers
  this->Loaded += 
    gcnew System::Windows::RoutedEventHandler(this, &WPFCppCliDemo::OnLoaded);
  this->Closing += 
    gcnew CancelEventHandler(this, &WPFCppCliDemo::Unloading);
  hSendButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::sendMessage);
  hClearButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::clear);
  hFolderBrowseButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browseForFolder);
  hConnectButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::connect);
  hUploadBrowseButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::uploadBrowseFolder);
  hUploadFileButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::uploadFile);
  hDownloadBrowseButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::downloadBrowseFolder);
  hDownloadFileButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::downloadFile);
  hTextSearchButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::textSearch);

  // set Window properties
  this->Title = "WPF C++/CLI Demo";
  this->Width = 800;
  this->Height = 600;
  // attach dock panel to Window
  this->Content = hDockPanel;
  hDockPanel->Children->Add(hStatusBar);
  hDockPanel->SetDock(hStatusBar, Dock::Bottom);
  hDockPanel->Children->Add(hGrid);

  // setup Window controls and views
  setUpTabControl();
  setUpStatusBar();
  setUpSendMessageView();
  setUpConnectionView();
  setUpFileUploadView();
  setUpFileDownloadView();
  setUpTextSearchView();
  setUpFileSearchView();
}

//----< Default destructor  >-----------------------------------------------
WPFCppCliDemo::~WPFCppCliDemo()
{
  delete pChann_;
  delete pSendr_;
  delete pRecvr_;
}

//----< Set up the status bar of the display window  >-----------------------
void WPFCppCliDemo::setUpStatusBar()
{
  hStatusBar->Items->Add(hStatusBarItem);
  hStatus->Text = "Status displayed in Status bar";
  //status->FontWeight = FontWeights::Bold;
  hStatusBarItem->Content = hStatus;
  hStatusBar->Padding = Thickness(10, 2, 10, 2);
}

//----< Set up the various tabs in the display window  >---------------------
void WPFCppCliDemo::setUpTabControl()
{
  hGrid->Children->Add(hTabControl);
  hSendMessageTab->Header = "Send Message";
  hFileListTab->Header = "File List";
  hConnectTab->Header = "Connect";
  hFileUploadTab->Header = "File Upload";
  hFileDownloadTab->Header = "File Download";
  hTextSearchTab->Header = "Search for Text";
  hFileSearchTab->Header = "Search for File";
  hTabControl->Items->Add(hConnectTab);
  hTabControl->Items->Add(hSendMessageTab);
  hTabControl->Items->Add(hFileUploadTab);
  hTabControl->Items->Add(hTextSearchTab);
  hTabControl->Items->Add(hFileDownloadTab);
  hTabControl->Items->Add(hFileSearchTab);
  //hTabControl->Items->Add(hFileListTab);
  
  
}

//----< Set up the text display in send Message tab  >-----------------------
void WPFCppCliDemo::setTextBlockProperties()
{
  RowDefinition^ hRow1Def = gcnew RowDefinition();
  hSendMessageGrid->RowDefinitions->Add(hRow1Def);
  Border^ hBorder1 = gcnew Border();
  hBorder1->BorderThickness = Thickness(1);
  hBorder1->BorderBrush = Brushes::Black;
  hBorder1->Child = hTextBlock1;
  hTextBlock1->Padding = Thickness(15);
  hTextBlock1->Text = "";
  hTextBlock1->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
  hTextBlock1->FontWeight = FontWeights::Bold;
  hTextBlock1->FontSize = 16;
  hScrollViewer1->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
  hScrollViewer1->Content = hBorder1;
  hSendMessageGrid->SetRow(hScrollViewer1, 0);
  hSendMessageGrid->Children->Add(hScrollViewer1);
}

//----< Set up the buttons in send Message tab  >----------------------------
void WPFCppCliDemo::setButtonsProperties()
{
  RowDefinition^ hRow2Def = gcnew RowDefinition();
  hRow2Def->Height = GridLength(75);
  hSendMessageGrid->RowDefinitions->Add(hRow2Def);
  hSendButton->Content = "Send Message";
  Border^ hBorder2 = gcnew Border();
  hBorder2->Width = 120;
  hBorder2->Height = 30;
  hBorder2->BorderThickness = Thickness(1);
  hBorder2->BorderBrush = Brushes::Black;
  hClearButton->Content = "Clear";
  hBorder2->Child = hSendButton;
  Border^ hBorder3 = gcnew Border();
  hBorder3->Width = 120;
  hBorder3->Height = 30;
  hBorder3->BorderThickness = Thickness(1);
  hBorder3->BorderBrush = Brushes::Black;
  hBorder3->Child = hClearButton;
  hStackPanel1->Children->Add(hBorder2);
  TextBlock^ hSpacer = gcnew TextBlock();
  hSpacer->Width = 10;
  hStackPanel1->Children->Add(hSpacer);
  hStackPanel1->Children->Add(hBorder3);
  hStackPanel1->Orientation = Orientation::Horizontal;
  hStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
  hSendMessageGrid->SetRow(hStackPanel1, 1);
  hSendMessageGrid->Children->Add(hStackPanel1);
}

//----< Set up the send Message tab  >---------------------------------------
void WPFCppCliDemo::setUpSendMessageView()
{
  Console::Write("\n  setting up sendMessage view");
  hSendMessageGrid->Margin = Thickness(20);
  hSendMessageTab->Content = hSendMessageGrid;

  setTextBlockProperties();
  setButtonsProperties();
}

//----< Convert System-string to std::string  >------------------------------
std::string WPFCppCliDemo::toStdString(String^ pStr)
{
  std::string dst;
  for (int i = 0; i < pStr->Length; ++i)
    dst += (char)pStr[i];
  return dst;
}

//----< event handler that sends the message on button click  >--------------
void WPFCppCliDemo::sendMessage(Object^ obj, RoutedEventArgs^ args)
{
	Console::Write("\n\n  Sending message");
	pSendr_->postMessage(toStdString(msgText));
	hStatus->Text = "Sent message";
	// test display in sendmessage box
	String^ sMsg = msgText;
	array<String^>^ argstemp = gcnew array<String^>(1);
	argstemp[0] = sMsg;

	Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addText);
	Dispatcher->Invoke(act, argstemp);  // must call addText on main UI thread
}

//----< event handler that connects to given port & IP  >--------------------
void WPFCppCliDemo::connect(Object^ obj, RoutedEventArgs^ args)
{

	string ip, sPort;
	int port;
	ip = toStdString(hIPAddrTextbox->Text);
	sPort = toStdString(hPortNumTextbox->Text);
	port = atoi(sPort.c_str());
	bool success = pSendr_->connect(ip, port);
	if (success)
	{
		string status = "\n IP: " + ip + "\t Port: " + sPort + "\n Connected";
		Console::Write("Connected");
		hStatus->Text = "Connected ";
	}
	else
	{
		hStatus->Text = " Connect Failed";
	}

}

//----< Convert std::string to System-string  >------------------------------
String^ WPFCppCliDemo::toSystemString(std::string& str)
{
  StringBuilder^ pStr = gcnew StringBuilder();
  for (size_t i = 0; i < str.size(); ++i)
    pStr->Append((Char)str[i]);
  return pStr->ToString();
}

//----< dsiplay text received from getMessage  >-----------------------------
void WPFCppCliDemo::addText(String^ msg)
{
  hTextBlock1->Text += msg + "\n\n";
}

//----< listener that receives messages from server >------------------------
void WPFCppCliDemo::getMessage()
{
  // recvThread runs this function

  while (true)
  {
    std::cout << "\n  receive thread calling getMessage()";
    std::string msg = pRecvr_->getMessage();
    String^ sMsg = toSystemString(msg);
    array<String^>^ args = gcnew array<String^>(1);
    args[0] = sMsg;

    Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addText);
    Dispatcher->Invoke(act, args);  // must call addText on main UI thread
  }
}

//----< clears display on send message tab >---------------------------------
void WPFCppCliDemo::clear(Object^ sender, RoutedEventArgs^ args)
{
  Console::Write("\n  cleared message text");
  hStatus->Text = "Cleared message";
  hTextBlock1->Text = "";
}

//----< set up the file list tab >-------------------------------------------
void WPFCppCliDemo::setUpFileListView()
{
  Console::Write("\n  setting up FileList view");
  hFileListGrid->Margin = Thickness(20);
  hFileListTab->Content = hFileListGrid;
  RowDefinition^ hRow1Def = gcnew RowDefinition();
  //hRow1Def->Height = GridLength(75);
  hFileListGrid->RowDefinitions->Add(hRow1Def);
  Border^ hBorder1 = gcnew Border();
  hBorder1->BorderThickness = Thickness(1);
  hBorder1->BorderBrush = Brushes::Black;
  hBorder1->Child = hListBox;
  hFileListGrid->SetRow(hBorder1, 0);
  hFileListGrid->Children->Add(hBorder1);

  RowDefinition^ hRow2Def = gcnew RowDefinition();
  hRow2Def->Height = GridLength(75);
  hFileListGrid->RowDefinitions->Add(hRow2Def);
  hFolderBrowseButton->Content = "Select Directory";
  hFolderBrowseButton->Height = 30;
  hFolderBrowseButton->Width = 120;
  hFolderBrowseButton->BorderThickness = Thickness(2);
  hFolderBrowseButton->BorderBrush = Brushes::Black;
  hFileListGrid->SetRow(hFolderBrowseButton, 1);
  hFileListGrid->Children->Add(hFolderBrowseButton);

  hFolderBrowserDialog->ShowNewFolderButton = false;
  hFolderBrowserDialog->SelectedPath = System::IO::Directory::GetCurrentDirectory();
}

//----< event handler that browses for directories in file list tab >--------
void WPFCppCliDemo::browseForFolder(Object^ sender, RoutedEventArgs^ args)
{
  std::cout << "\n  Browsing for folder";
  hListBox->Items->Clear();
  System::Windows::Forms::DialogResult result;
  result = hFolderBrowserDialog->ShowDialog();
  if (result == System::Windows::Forms::DialogResult::OK)
  {
    String^ path = hFolderBrowserDialog->SelectedPath;
    std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
    array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.*");
    for (int i = 0; i < files->Length; ++i)
      hListBox->Items->Add(files[i]);
    array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
    for (int i = 0; i < dirs->Length; ++i)
      hListBox->Items->Add(L"<> " + dirs[i]);
  }
}

//----< event handler that browses for directories in upload file tab >------
void WPFCppCliDemo::uploadBrowseFolder(Object^ sender, RoutedEventArgs^ args)
{
	hStatus->Text = "";
	std::cout << "\n  Browsing for file to upload";
	hUploadBox->Items->Clear();
	System::Windows::Forms::DialogResult uploadResult;
	uploadResult = hUploadBrowserDialog->ShowDialog();
	if (uploadResult == System::Windows::Forms::DialogResult::OK)
	{
		String^ path = hUploadBrowserDialog->SelectedPath;
		std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
		array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.*");
		for (int i = 0; i < files->Length; ++i)
			hUploadBox->Items->Add(files[i]);
		array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
		for (int i = 0; i < dirs->Length; ++i)
			hUploadBox->Items->Add(L"<> " + dirs[i]);
	}
}

//----< event handler that uploads selected file to server >-----------------
void WPFCppCliDemo::uploadFile(Object^ sender, RoutedEventArgs^ args)
{
	hStatus->Text = "";
	if (hUploadBox->SelectedIndex != -1)
	{
		String^ file = hUploadBox->SelectedItem->ToString();
		string filename = toStdString(file);
		pSendr_->handleFileUpload(filename);
		hStatus->Text = "File uploaded";
	}
	else
	{
		hStatus->Text = "Please select a File first!";
	}
}

//----< event handler that browses for directories in download file tab >----
void WPFCppCliDemo::downloadBrowseFolder(Object^ sender, RoutedEventArgs^ args)
{
	hStatus->Text = "";
	std::cout << "\n  Browsing for file to download";
	hDownloadBox->Items->Clear();
	System::Windows::Forms::DialogResult downloadResult;
	downloadResult = hDownloadBrowserDialog->ShowDialog();
	if (downloadResult == System::Windows::Forms::DialogResult::OK)
	{
		String^ path = hDownloadBrowserDialog->SelectedPath;
		std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
		array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.*");
		for (int i = 0; i < files->Length; ++i)
			hDownloadBox->Items->Add(files[i]);
		array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
		for (int i = 0; i < dirs->Length; ++i)
			hDownloadBox->Items->Add(L"<> " + dirs[i]);
	}
}

//----< event handler that downloads selected file to server >---------------
void WPFCppCliDemo::downloadFile(Object^ sender, RoutedEventArgs^ args)
{
	hStatus->Text = "";
	if (hDownloadBox->SelectedIndex != -1)
	{
		String^ file = hDownloadBox->SelectedItem->ToString();
		string filename = toStdString(file);
		//pSendr_->handleFileUpload(filename);
		//hStatus->Text = "File uploaded";
	}
	else
	{
		hStatus->Text = "Please select a File first!";
	}
}

//----< set up the Upload file tab >------------------------------------------
void WPFCppCliDemo::setUpFileUploadView()
{
	Console::Write("\n  setting up 'File Upload' view");
	
	hFileUploadGrid->Margin = Thickness(20);
	hFileUploadTab->Content = hFileUploadGrid;
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	//hRow3Def->Height = GridLength(75);
	hFileUploadGrid->RowDefinitions->Add(hRow3Def);
	Border^ hBorder3 = gcnew Border();
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hUploadBox;
	hFileUploadGrid->SetRow(hBorder3, 0);
	hFileUploadGrid->Children->Add(hBorder3);


	RowDefinition^ hRow4Def = gcnew RowDefinition();
	hRow4Def->Height = GridLength(75);
	hFileUploadGrid->RowDefinitions->Add(hRow4Def);

	hUploadBrowseButton->Content = "Select Directory";
	hUploadBrowseButton->Height = 30;
	hUploadBrowseButton->Width = 120;
	hUploadBrowseButton->BorderThickness = Thickness(2);
	hUploadBrowseButton->BorderBrush = Brushes::Black;

	hUploadFileButton->Content = "Upload File";
	hUploadFileButton->Height = 30;
	hUploadFileButton->Width = 120;
	hUploadFileButton->BorderThickness = Thickness(2);
	hUploadFileButton->BorderBrush = Brushes::Black;

	hUploadStackPanel->Children->Add(hUploadBrowseButton);
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 10;
	hUploadStackPanel->Children->Add(hSpacer);
	hUploadStackPanel->Children->Add(hUploadFileButton);
	hUploadStackPanel->Orientation = Orientation::Horizontal;
	hUploadStackPanel->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hFileUploadGrid->SetRow(hUploadStackPanel, 1);
	hFileUploadGrid->Children->Add(hUploadStackPanel);

	hUploadBrowserDialog->ShowNewFolderButton = false;
	hUploadBrowserDialog->SelectedPath = System::IO::Directory::GetCurrentDirectory();
	
}

//----< set up the Download file tab >----------------------------------------
void WPFCppCliDemo::setUpFileDownloadView()
{
	Console::Write("\n  setting up 'File Upload' view");

	hFileDownloadGrid->Margin = Thickness(20);
	hFileDownloadTab->Content = hFileDownloadGrid;
	RowDefinition^ hRowDefDl1 = gcnew RowDefinition();
	//hRow3Def->Height = GridLength(75);
	hFileDownloadGrid->RowDefinitions->Add(hRowDefDl1);
	Border^ hBorder3 = gcnew Border();
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hDownloadBox;
	hFileDownloadGrid->SetRow(hBorder3, 0);
	hFileDownloadGrid->Children->Add(hBorder3);


	RowDefinition^ hRowDefDl2 = gcnew RowDefinition();
	hRowDefDl2->Height = GridLength(75);
	hFileDownloadGrid->RowDefinitions->Add(hRowDefDl2);

	hDownloadBrowseButton->Content = "Select Directory";
	hDownloadBrowseButton->Height = 30;
	hDownloadBrowseButton->Width = 120;
	hDownloadBrowseButton->BorderThickness = Thickness(2);
	hDownloadBrowseButton->BorderBrush = Brushes::Black;

	hDownloadFileButton->Content = "Download File";
	hDownloadFileButton->Height = 30;
	hDownloadFileButton->Width = 120;
	hDownloadFileButton->BorderThickness = Thickness(2);
	hDownloadFileButton->BorderBrush = Brushes::Black;

	hDownloadStackPanel->Children->Add(hDownloadBrowseButton);
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 10;
	hDownloadStackPanel->Children->Add(hSpacer);
	hDownloadStackPanel->Children->Add(hDownloadFileButton);
	hDownloadStackPanel->Orientation = Orientation::Horizontal;
	hDownloadStackPanel->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hFileDownloadGrid->SetRow(hDownloadStackPanel, 1);
	hFileDownloadGrid->Children->Add(hDownloadStackPanel);

	hDownloadBrowserDialog->ShowNewFolderButton = false;
	hDownloadBrowserDialog->SelectedPath = System::IO::Directory::GetCurrentDirectory();

}

//----< set up the Connect tab >----------------------------------------------
void WPFCppCliDemo::setUpConnectionView()
{
  Console::Write("\n  setting up Connection view");
  hConnectionGrid->Margin = Thickness(20);
  hConnectTab->Content = hConnectionGrid;
  RowDefinition^ hRowDefConn1 = gcnew RowDefinition();
  hRowDefConn1->Height = GridLength(25);
  ColumnDefinition^ hColDefConn1 = gcnew ColumnDefinition();
  hColDefConn1->Width = GridLength(75);
  ColumnDefinition^ hColDefConn2 = gcnew ColumnDefinition();
  hColDefConn2->Width = GridLength(75);
  hIPAddrTextBlock->Text = "IP Address";
  hIPAddrTextBlock->IsEnabled = true;
  hPortNumTextBlock->Text = "Port Number";
  hPortNumTextBlock->IsEnabled = true;
  hConnectionGrid->RowDefinitions->Add(hRowDefConn1);
  hConnectionGrid->SetRow(hIPAddrTextbox, 0);
  hConnectionGrid->SetRow(hIPAddrTextBlock, 0);
  hConnectionGrid->ColumnDefinitions->Add(hColDefConn1);
  hConnectionGrid->SetColumn(hIPAddrTextBlock, 0);
  hConnectionGrid->Children->Add(hIPAddrTextBlock);
  hConnectionGrid->ColumnDefinitions->Add(hColDefConn2);
  hConnectionGrid->SetColumn(hIPAddrTextbox, 1);
  hConnectionGrid->Children->Add(hIPAddrTextbox);
  hIPAddrTextbox->Text = "localhost";
  hPortNumTextbox->Text = "9080";
  RowDefinition^ hRowDefConnBlank1 = gcnew RowDefinition();
  hRowDefConnBlank1->Height = GridLength(25);
  hConnectionGrid->RowDefinitions->Add(hRowDefConnBlank1);
  RowDefinition^ hRowDefConn2 = gcnew RowDefinition();
  hRowDefConn2->Height = GridLength(25);
  ColumnDefinition^ hColDefConn3 = gcnew ColumnDefinition();
  hColDefConn3->Width = GridLength(75);
  ColumnDefinition^ hColDefConn4 = gcnew ColumnDefinition();
  hColDefConn4->Width = GridLength(75);
  hConnectionGrid->RowDefinitions->Add(hRowDefConn2);
  hConnectionGrid->ColumnDefinitions->Add(hColDefConn3);
  hConnectionGrid->SetRow(hPortNumTextbox, 2);
  hConnectionGrid->SetColumn(hPortNumTextBlock, 0);
  hConnectionGrid->SetRow(hPortNumTextBlock, 2);
  hConnectionGrid->Children->Add(hPortNumTextBlock);
  hConnectionGrid->ColumnDefinitions->Add(hColDefConn4);
  hConnectionGrid->SetColumn(hPortNumTextbox, 1);
  hConnectionGrid->Children->Add(hPortNumTextbox);
  continueSetupConnectionView();
}

void WPFCppCliDemo::continueSetupConnectionView()
{
	// add a blank line
	RowDefinition^ hRowDefConnBlank2 = gcnew RowDefinition();
	hRowDefConnBlank2->Height = GridLength(25);
	hConnectionGrid->RowDefinitions->Add(hRowDefConnBlank2);

	// add button
	RowDefinition^ hRowDefConn3 = gcnew RowDefinition();
	hRowDefConn3->Height = GridLength(25);
	ColumnDefinition^ hColDefConn5 = gcnew ColumnDefinition();
	//hColDefConn5->Width = GridLength(150);
	hConnectionGrid->RowDefinitions->Add(hRowDefConn3);
	hConnectionGrid->SetRow(hConnectButton, 4);
	hConnectionGrid->ColumnDefinitions->Add(hColDefConn5);
	hConnectButton->Content = "Connect";
	hConnectButton->Height = 25;
	hConnectButton->Width = 75;
	hConnectButton->BorderThickness = Thickness(2);
	hConnectButton->BorderBrush = Brushes::Black;
	hConnectionGrid->SetColumn(hConnectButton, 1);
	hConnectionGrid->Children->Add(hConnectButton);
}

//----< set up the Text Search tab >------------------------------------------
void WPFCppCliDemo::setUpTextSearchView()
{
	hTextSearchGrid->Margin = Thickness(20);
	hTextSearchTab->Content = hTextSearchGrid;
	RowDefinition^ hRowDefText1 = gcnew RowDefinition(); 
	hRowDefText1->Height = GridLength(25);
	ColumnDefinition^ hColDefText1 = gcnew ColumnDefinition(); 
	hColDefText1->Width = GridLength(75);
	ColumnDefinition^ hColDefText2 = gcnew ColumnDefinition(); 
	hColDefText2->Width = GridLength(75);
	hPatternTextBlock->Text = "Pattern"; 
	hSearchstringBlock->Text = "Search String";
	hTextSearchGrid->RowDefinitions->Add(hRowDefText1); 
	hTextSearchGrid->SetRow(hPatternTextbox, 0);
	hTextSearchGrid->SetRow(hPatternTextBlock, 0); 	
	hTextSearchGrid->ColumnDefinitions->Add(hColDefText1);
	hTextSearchGrid->SetColumn(hPatternTextBlock, 0); 	
	hTextSearchGrid->Children->Add(hPatternTextBlock);
	hTextSearchGrid->ColumnDefinitions->Add(hColDefText2);
	hTextSearchGrid->SetColumn(hPatternTextbox, 1); 
	hTextSearchGrid->Children->Add(hPatternTextbox);
	hPatternTextbox->Text = "";
	hSearchstringTextbox->Text = "socket";
	RowDefinition^ hRowDefTextBlank1 = gcnew RowDefinition(); 
	hRowDefTextBlank1->Height = GridLength(25);
	hTextSearchGrid->RowDefinitions->Add(hRowDefTextBlank1);
	RowDefinition^ hRowDefText2 = gcnew RowDefinition(); 
	hRowDefText2->Height = GridLength(25);
	ColumnDefinition^ hColDefText3 = gcnew ColumnDefinition(); 
	hColDefText3->Width = GridLength(75);
	ColumnDefinition^ hColDefText4 = gcnew ColumnDefinition(); 
	hColDefText4->Width = GridLength(75);
	hTextSearchGrid->RowDefinitions->Add(hRowDefText2);
	hTextSearchGrid->ColumnDefinitions->Add(hColDefText3);
	hTextSearchGrid->SetRow(hSearchstringTextbox, 2);
	hTextSearchGrid->SetColumn(hSearchstringBlock, 0);
	hTextSearchGrid->SetRow(hSearchstringBlock, 2);
	hTextSearchGrid->Children->Add(hSearchstringBlock);
	hTextSearchGrid->ColumnDefinitions->Add(hColDefText4);
	hTextSearchGrid->SetColumn(hSearchstringTextbox, 1);
	hTextSearchGrid->Children->Add(hSearchstringTextbox);
	continuesetUpTextSearchView();
}

void WPFCppCliDemo::continuesetUpTextSearchView()
{
	// add a blank line
	RowDefinition^ hRowDefTextBlank2 = gcnew RowDefinition();
	hRowDefTextBlank2->Height = GridLength(25);
	hTextSearchGrid->RowDefinitions->Add(hRowDefTextBlank2);

	// add button
	RowDefinition^ hRowDefText3 = gcnew RowDefinition();
	hRowDefText3->Height = GridLength(25);
	ColumnDefinition^ hColDefText5 = gcnew ColumnDefinition();
	//hColDefConn5->Width = GridLength(150);
	hTextSearchGrid->RowDefinitions->Add(hRowDefText3);
	hTextSearchGrid->SetRow(hTextSearchButton, 5);
	hTextSearchGrid->ColumnDefinitions->Add(hColDefText5);
	hTextSearchButton->Content = "Search";
	hTextSearchButton->Height = 25;
	hTextSearchButton->Width = 75;
	hTextSearchButton->BorderThickness = Thickness(2);
	hTextSearchButton->BorderBrush = Brushes::Black;
	hTextSearchGrid->SetColumn(hTextSearchButton, 1);
	hTextSearchGrid->Children->Add(hTextSearchButton);
}

//----< event handler that searches for text in file pattern >----------------
void WPFCppCliDemo::textSearch(Object^ obj, RoutedEventArgs^ args)
{
	std::string filePattern = toStdString(hPatternTextbox->Text);
	std::string searchString = toStdString(hSearchstringTextbox->Text);
	if (filePattern.empty() || searchString.empty())
	{
		hStatus->Text = "Some fields are empty!";
	}
	else
	{		
		pSendr_->textSearch(filePattern, searchString);
		hStatus->Text = "Query sent";
	}
}

//----< set up the File Search tab >------------------------------------------
void WPFCppCliDemo::setUpFileSearchView()
{

}

//----< event handler that runs when the window loads >-----------------------
void WPFCppCliDemo::OnLoaded(Object^ sender, RoutedEventArgs^ args)
{
  Console::Write("\n  Window loaded");
}

//----< event handler that runs when the window closes >----------------------
void WPFCppCliDemo::Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args)
{
  Console::Write("\n  Window closing");
}

[STAThread]
//int _stdcall WinMain()
int main(array<System::String^>^ args)
{
  Console::WriteLine(L"\n Starting WPFCppCliDemo");

  Application^ app = gcnew Application();
  app->Run(gcnew WPFCppCliDemo());
  Console::WriteLine(L"\n\n");
}