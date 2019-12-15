#pragma comment(lib, "Ws2_32.lib")		// ���������� �������
#include <WinSock2.h>					// ����������� �������
#include <iostream>						// ����������� �������
#include <WS2tcpip.h>					// ���������� ��� ������ (�����������, ��������� ���������� �� �������, � �.�.)
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <thread>
#include <ctime>
#include <time.h>

SOCKET Connect;				// �������� ������, ����������� �� ������������ �������������
SOCKET* Connections;		// ��������� �������, ������������� �������������
SOCKET Listen;				// ����� ��� �����������

using namespace std;

int ClientCount = 0;	// ���������� ������������ ��������������
char strTime[80];

void LoadInfoServer();					// �������� Log.txt
void SetTimeForMess(char* strTime);		// ������������� ������� ����
void SeparateStr(const string& str, vector<string>& tokens, const string& delimiters);
void WriteToLog(string mess);
void SendMessageToClient(int id);

int main() {
	setlocale(LC_ALL, "Rus");
	WSAData data;
	WORD version = MAKEWORD(2, 2);						// ��������� ������ ������� 2.2
	int res = WSAStartup(version, &data);				/* WSAStartup(version, &data) ������������� �������
															res �������� �� ���������: ����������� ����� ��� ��� */
	if (res != 0)
		return 0;

	struct addrinfo hints;
	struct addrinfo* result;

	Connections = (SOCKET*)calloc(64, sizeof(SOCKET));	// ������������� � ��������� ������ ��� ��������� �������

	ZeroMemory(&hints, sizeof(hints));					// ������� ��������� hints

	hints.ai_family = AF_INET;							// ����� ��� ������
	hints.ai_flags = AI_PASSIVE;						// ����� ����
	hints.ai_socktype = SOCK_STREAM;					// ����� ��� ������ TCP
	hints.ai_protocol = IPPROTO_TCP;

	getaddrinfo(NULL, "1234", &hints, &result);			// �������� ���������� � �����

	Listen = socket(result->ai_family, result->ai_socktype, result->ai_protocol);	// ��������� ������ Listen
	::bind(Listen, result->ai_addr, result->ai_addrlen);							// ���������� �������
	::listen(Listen, SOMAXCONN);													//	��������� �����������

	freeaddrinfo(result);								// �������� ����������, �.�. ��� ������ ��� ���������

	SetTimeForMess(strTime);
	printf("<%s> Start server ...\n", strTime);


	const char* messToClient = new char[1024];	// ������, ������������ �������
	string strbuff;								// �����
	vector<string> arrStr;
	char buff[250];								// ����� �������������� �������� ������������ �� ����� ������
	char* messFromClient = new char[1024];		// ������, ���������� ��������

	LoadInfoServer();		// �������� Log.txt

	// ���� ��� �������� ����������� ������ ������������
	for (;; Sleep(75)) {

		if (Connect = accept(Listen, NULL, NULL)) {				// ���� ���� ����������� �������
			SetTimeForMess(strTime);
			printf("<%s> Client connect (%u)...\n", strTime, ClientCount);
			Connections[ClientCount] = Connect;					// ��������� ����� ������������� �������
			strbuff = "setYourID;";
			_itoa_s(ClientCount, buff, 5, 10);					// �������������� ClientCount � ������ buff ������ 5, � 10-�� �������
			strbuff = strbuff + buff;
			messToClient = strbuff.c_str();						// ����������� �� string � const char
			SetTimeForMess(strTime);
			printf("<%s> �������: %s \n", strTime, messToClient);

			send(Connections[ClientCount], messToClient, strlen(messToClient), NULL);	// ���������� ��������� �������������� �������
			ClientCount++;										// ���������� ������������ �������� +1
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SendMessageToClient, (LPVOID)(ClientCount - 1), NULL, NULL);
		}
	}
	return 0;
}

void LoadInfoServer() {
	ofstream file;
	file.open("Log.txt");
	if (file.is_open())
	{
		file.close();
	}
	else;
}

void SetTimeForMess(char* strTime) {
	time_t t = time(0);   // ��������� �������� �������
	struct tm now;
	localtime_s(&now, &t);
	const char* format = "%d.%m.%Y %H:%M:%S";
	strftime(strTime, 80, format, &now);
}

// ��������� �� string � vector<string>
void SeparateStr(const string& str, vector<string>& tokens, const string& delimiters)
{
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type pos = str.find_first_of(delimiters, lastPos);
	while (string::npos != pos || string::npos != lastPos)
	{
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}

void WriteToLog(string mess) {
	ofstream fout("Log.txt", std::ios::app);	// ������� ���� ��� ������
	if (!fout.is_open()) {						// ���� ���� �� ������
		SetTimeForMess(strTime);
		cout << "<" + (string)strTime + "> ���� Log.txt �� ����� ���� ������!\n";	// �������� �� ����
	}
	else {
		fout << mess;
	}
	fout.close();
}

void SendMessageToClient(int id) {
	char buffer[1024];
	vector<string> arrStr;
	string strbuff;


	for (;; Sleep(75)) {
		memset(buffer, 0, sizeof(buffer));
		if (recv(Connections[id], buffer, 1024, NULL)) {

			SetTimeForMess(strTime);
			printf("<%s> %s \n", strTime, buffer);
			arrStr.clear();
			strbuff = buffer;
			SeparateStr(strbuff, arrStr, ";");

			if (arrStr.size() > 2) {
				if (arrStr[1] == "sendMess") {
					SetTimeForMess(strTime);

					strbuff = "newMess;<";
					strbuff += strTime;
					strbuff += "> " + arrStr[2] + ": " + arrStr[3] + "\n";
					WriteToLog(strbuff);

					strncpy_s(buffer, strbuff.c_str(), sizeof(buffer) - 1);
					buffer[sizeof(buffer) - 1] = 0;

					for (int i = 0; i < ClientCount; i++) {
						send(Connections[i], buffer, strlen(buffer), NULL);
					}
				}
			}
		}
	}
}