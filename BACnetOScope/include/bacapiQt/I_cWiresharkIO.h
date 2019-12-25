#pragma once
#ifdef BACAPI_BUILD_WIN32
// lookout sere bez ovog ovdje a device sere ako je definiran
//#include <stdafx.h>
//#include <Windows.h>
#include <Windows.h>
#endif
#include "mstpdef.h"
#include <qfile.h>


class I_cWiresharkIO
{
 public:
        I_cWiresharkIO();
       ~I_cWiresharkIO();

			 
        int Init(string pipename);
        int WriteFrame(BACNET_MSTP_FRAME *frame);
        int WriteBuffer(uint8_t *buffer, uint16_t len);


        void open_file(string filepath);
        void close_file();
        int WritePDUBufferIP(uint8_t *buffer, uint8_t *src, uint8_t *dest,uint16_t len);
                //int WritePDUBufferIP(HANDLE file, uint8_t *buffer, uint8_t *src, uint8_t *dest, uint16_t len);
				int WritePDUBufferEth(uint8_t *buffer, uint8_t *src, uint8_t *dest, uint16_t len);


				void write_test(void);
 private:
         int Write(void *buffer, size_t size, size_t nitems);
         int WriteHeader(void);

 public:
         BACNET_MSTP_FRAME *test_frame_write();
         uint16_t MSTP_Create_Frame(uint8_t * buffer,uint16_t buffer_len,uint8_t frame_type, uint8_t destination,uint8_t source,uint8_t * data, uint16_t data_len);

 private:
         string out_filename_;
         string pipename_;
#ifdef BACAPI_BUILD_WIN32
         //int Writef(HANDLE file, void *buffer, size_t size, size_t nitems);
                 //int Writef(void *buffer, size_t size, size_t nitems);
				 HANDLE hpipe_;
         HANDLE hFile;
				 //FILE *fFile;
#endif
                 //int Writef(void *buffer, size_t size, size_t nitems);
         int Writef(void *buffer, size_t size, size_t nitems);

                 FILE *fFile;

};


extern I_cWiresharkIO *wireshark_;
