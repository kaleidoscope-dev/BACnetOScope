#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <stdint.h>


using namespace std;


#define FILE_MODE 0x01
#define CONSOLE_MODE 0x02
#define SILENT_MODE 0x03

class I_cBACLog
{
 public:
        I_cBACLog();
       ~I_cBACLog();
       
    
    bool FileExists(string path);   
	virtual string AppendLog(char *format, ...);
	void StartAppend(int indent, string title);
	void StopAppend(int indent,string title);
	void ResetLog(void);
    //int GetMessageParam(int *param);
	void SetDebugFile(string path);
    void IncInd();
    void IncInd(int i);
    void DecInd();
    void DecInd(int i);
    void SetDebugMode(unsigned char mode);
    void AppendBuffer(uint8_t *buffer, int len);


    string currentDirectory;
 private:           
	string path_;


	private:
	FILE *logfile;
	unsigned long LogCounter;

	int indent_;              //  NI
	string indent_string_;    //  NI
    unsigned char debug_mode;           
    
    


};

extern I_cBACLog *gLog;


