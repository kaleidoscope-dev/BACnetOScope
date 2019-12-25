#include "I_cWiresharkIO.h"
#include "I_cBACdec.h"
//#include <fileapi.h>
#include "I_cTimer.h"
#include <qdatetime.h>
#include "I_cBACLog.h"
/*
#include "I_cTimer.h"
#include "I_cBACLog.h"
#include <time.h>
#include "I_cBACdec.h"
#ifdef BACAPI_BUILD_WIN32
#include <algorithm>

//#include <winsock2.h>  // struct timeval
#endif
*/
#ifdef __ANDROID__
#include <byteswap.h>
#endif

I_cWiresharkIO *wireshark_ = NULL;


I_cWiresharkIO::I_cWiresharkIO()
{
#ifdef BACAPI_BUILD_WIN32
// hpipe_ = NULL;
	fFile = NULL;
#endif
}


I_cWiresharkIO::~I_cWiresharkIO()
{
}




int I_cWiresharkIO::Init(string pipename)
{
#ifdef BACAPI_BUILD_WIN32
 /*
 gLog->AppendLog("Waiting for Wireshark pipe activation...");
 //hpipe_ = CreateNamedPipe(pipename.c_str(), PIPE_ACCESS_OUTBOUND|FILE_FLAG_OVERLAPPED,PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, 65536, 65536, 15000, NULL);

 if (hpipe_ == INVALID_HANDLE_VALUE)
    {
     gLog->AppendLog("MSTPCapture: Unable to create named pipe\n");
     hpipe_ = NULL;
     return -1;
    }



OVERLAPPED ol = {0,0,0,0,NULL};
BOOL ret = 0;

ol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

ret = ConnectNamedPipe(hpipe_, &ol);

if (ret == 0)
   {
    switch( GetLastError() )
          {
		   case ERROR_PIPE_CONNECTED:
			   ret = true;
			   break;

		   case ERROR_IO_PENDING:
			   if( WaitForSingleObject(ol.hEvent, 15000) == WAIT_OBJECT_0 )
			     {
				   DWORD dwIgnore;
				   ret = GetOverlappedResult(hpipe_, &ol, &dwIgnore, FALSE);
			     } else {
				         CancelIo(hpipe_);
				         gLog->AppendLog("MSTPCapture: ConnectNamedPipe() fail!\n");
		                }
    		   break;
    	   default: return ret = -1;
          }
     }

 CloseHandle(ol.hEvent);

 if( ret == 0 )
   {
    return -1;
   }
#endif

 WriteHeader();
 pipename_ = pipename;
 */
 #endif
 return 0;
}



int I_cWiresharkIO::WriteFrame(BACNET_MSTP_FRAME *frame)
{
/*
 uint32_t ts_sec=0;    // timestamp seconds
 uint32_t ts_usec=0;   // timestamp microseconds
 uint32_t incl_len=8;  // number of octets of packet saved in file
 uint32_t orig_len=8;  // actual length of packet


 Write(&ts_sec, sizeof(ts_sec), 1);
 Write(&ts_usec, sizeof(ts_sec), 1);

 Write(&incl_len, sizeof(incl_len), 1);
 Write(&orig_len, sizeof(orig_len), 1);


#ifdef BACAPI_BUILD_WIN32
 DWORD nwritten = 0;
 WriteFile(hpipe_,frame->data, 8, &nwritten, NULL);
#endif

*/
 return 0;
}


int I_cWiresharkIO::WriteBuffer(uint8_t *buffer, uint16_t len)
{
/*
#ifdef BACAPI_BUILD_WIN32
 uint32_t ts_sec=0;    // timestamp seconds
 uint32_t ts_usec=0;   // timestamp microseconds
 uint32_t incl_len=8;  // number of octets of packet saved in file
 uint32_t orig_len=8;  // actual length of packet
 struct timeval tv;


 gTimer.gettimeofday(&tv, NULL);

 ts_sec = tv.tv_sec;
 ts_usec = tv.tv_usec;


 Write(&ts_sec, sizeof(ts_sec), 1);
 Write(&ts_usec, sizeof(ts_sec), 1);

 if (len > 8) // postoji data
    {
     incl_len = orig_len = len;
    }


 Write(&incl_len, sizeof(incl_len), 1);
 Write(&orig_len, sizeof(orig_len), 1);

 DWORD nwritten = 0;
 if (hpipe_)
    {
	  bool ret = WriteFile(hpipe_,buffer, len, &nwritten,NULL);
	  uint32_t nwritten = 0;
	  //printf("write ret = %d, nw = %d\n",ret,nwritten);
	  return nwritten;
    }
#endif
*/
  return 0;
}



int I_cWiresharkIO::Write(void *buffer, size_t size, size_t nitems)
{
/*
#ifdef BACAPI_BUILD_WIN32
 DWORD nwritten = 0;
 if (hpipe_)
    {
     WriteFile(hpipe_,(LPVOID)buffer,size,&nwritten,NULL);
     uint32_t nwritten = 0;
    }
 return nwritten;
#else
 return 0;
#endif
*/
return 0;
}


int I_cWiresharkIO::WriteHeader(void)
{
 static bool pipe_enable = true;
 uint32_t magic_number = 0xa1b2c3d4; // magic number
 uint16_t version_major = 2; // major version number
 uint16_t version_minor = 4; // minor version number
 int32_t thiszone = 0;       // GMT to local correction
 uint32_t sigfigs = 0;       // accuracy of timestamps
 uint32_t snaplen = 65535;   // max length of captured packets, in octets
 uint32_t network = 165;     // data link type - BACNET_MS_TP


 Write(&magic_number, sizeof(magic_number), 1);
 Write(&version_major, sizeof(version_major), 1);
 Write(&version_minor, sizeof(version_minor), 1);
 Write(&thiszone, sizeof(thiszone), 1);
 Write(&sigfigs, sizeof(sigfigs), 1);
 Write(&snaplen, sizeof(snaplen), 1);
 Write(&network, sizeof(network), 1);

 return 0;
}




//#ifdef BACAPI_BUILD_WIN32
//int I_cWiresharkIO::Writef(HANDLE file, void *buffer, size_t size, size_t nitems)
//{
// DWORD nwritten = 0;
// WriteFile(file,(LPVOID)buffer,size,&nwritten,NULL);
// return nwritten;
//}
//#endif

int I_cWiresharkIO::Writef(void *buffer, size_t size, size_t nitems)
{
	int ret = fwrite(buffer, 1, size, fFile);

	return ret;
}



void I_cWiresharkIO::open_file(string filepath)
{


 fFile = fopen(filepath.c_str(),"wb");
 if (fFile == NULL)
    {
      gLog->AppendLog("WireSharkIO::open_file: fFile = NULL (%s)",filepath.c_str());
      return;
    }

 static bool pipe_enable = true;
 uint32_t magic_number = 0xa1b2c3d4; // magic number  4
 uint16_t version_major = 2; // major version number  2
 uint16_t version_minor = 4; // minor version number  2
 uint32_t thiszone = 0;       // GMT to local correction  4
 uint32_t sigfigs = 0;       // accuracy of timestamps 4
 uint32_t snaplen = 65535;   // max length of captured packets, in octets  4
 uint32_t network = 0x01;     // data link type - ethernet - udp - bvlc - bacnet  4




 Writef(&magic_number, sizeof(magic_number), 1);
 Writef(&version_major, sizeof(version_major), 1);
 Writef(&version_minor, sizeof(version_minor), 1);
 Writef(&thiszone, sizeof(thiszone), 1);
 Writef(&sigfigs, sizeof(sigfigs), 1);
 Writef(&snaplen, sizeof(snaplen), 1);
 Writef(&network, sizeof(network), 1);


}






void I_cWiresharkIO::close_file()
{
 if (fFile)
  fclose(fFile);

}



/*
int I_cWiresharkIO::WritePDUBufferIP(HANDLE file, uint8_t *buffer, uint8_t *src, uint8_t *dest, uint16_t len)
{
#ifdef BACAPI_BUILD_WIN32



 uint32_t ts_sec=0;    // timestamp seconds
 uint32_t ts_usec=0;   // timestamp microseconds
 uint32_t incl_len=len+42;  // number of octets of packet saved in file
 uint32_t orig_len=len+42;  // actual length of packet
 struct timeval tv;


 gTimer.gettimeofday(&tv, NULL);

 ts_sec = tv.tv_sec;
 ts_usec = tv.tv_usec;


 Writef(hFile,&ts_sec, sizeof(ts_sec), 1);
 Writef(hFile,&ts_usec, sizeof(ts_sec), 1);


 // len
 Writef(hFile,&incl_len, sizeof(incl_len), 1);
 Writef(hFile,&orig_len, sizeof(orig_len), 1);


 Writef(hFile,dest, 6, 1);  // dest
 Writef(hFile,src, 6, 1);   // src


 uint16_t type = 0x0008;  // type (IP)
 Writef(hFile,&type, sizeof(type), 1);

 uint8_t buff[20] = {0x45,0,0,0x34,0x43,0xf0,0x0,0,0x40,0x11,0x1d,0x68,0x0a,0,0x01,0x63,0x0a,0,0x03,0xff};
 // byte 2 i 3 su total_length u Internet Protocol Version 4 polju
 uint16_t tot_len = orig_len - 14;  // 14 je ethenetII polje
 gDec->encode_unsigned16(&buff[2],tot_len);


 Writef(hFile,buff, 20, 1);

// source/dest port
 uint8_t buff_src_dest_port[4] = {0xba,0xc0,0xba,0xc0};
 Writef(hFile,buff_src_dest_port, 4, 1);

 // len
 uint8_t buff_len[2];
 uint16_t code_len = len+8;
 gDec->encode_unsigned16(&buff_len[0],code_len);
 //buff_len[0] = code_len << 8;
 //buff_len[1] = code_len;

 Writef(hFile,buff_len, 2, 1);

 //  checksum
 uint8_t buff_checksum[2] = {0x38,0xed};

 Writef(hFile,buff_checksum, 2, 1);



 DWORD nwritten = 0;
 Writef(hFile,buffer, len, 1);


 return 0;
#endif
 return 0;
}
*/



int I_cWiresharkIO::WritePDUBufferIP(uint8_t *buffer, uint8_t *src, uint8_t *dest, uint16_t len)
{
    
//#ifdef BACAPI_BUILD_WIN32
  uint8_t bfr[1024];
	int plen = 0;

	uint32_t ts_sec = 0;    // timestamp seconds
	uint32_t ts_usec = 0;   // timestamp microseconds
	uint32_t incl_len = len + 42;  // number of octets of packet saved in file
	uint32_t orig_len = len + 42;  // actual length of packet
	//uint32_t incl_len = len+12;  // number of octets of packet saved in file
	//uint32_t orig_len = len+12+2;  // actual length of packet
	struct timeval tv;



	QDateTime dt;
	ts_sec = dt.currentSecsSinceEpoch();
	ts_usec = dt.currentMSecsSinceEpoch();

#ifdef __ANDROID__
    uint32_t swap_sec = bswap_32(ts_sec);
    uint32_t swap_usec = bswap_32(ts_usec);
		uint32_t swap_incl = bswap_32(incl_len);
		uint32_t swap_orig = bswap_32(orig_len);
#else

	uint32_t swap_sec = _byteswap_ulong(ts_sec);
	uint32_t swap_usec = _byteswap_ulong(ts_usec);
	uint32_t swap_incl = _byteswap_ulong(incl_len);
	uint32_t swap_orig = _byteswap_ulong(orig_len);
#endif


	plen += gDec->encode_unsigned32(&bfr[plen], swap_sec);
	plen += gDec->encode_unsigned32(&bfr[plen], swap_usec);
	plen += gDec->encode_unsigned32(&bfr[plen], swap_incl);
	plen += gDec->encode_unsigned32(&bfr[plen], swap_orig);

	
	for (int c=0;c<6;c++)
	    bfr[plen++] = src[c];

	for (int c = 0; c < 6; c++)
		bfr[plen++] = dest[c];
	
	
	fwrite(bfr, 1, plen, fFile);


	uint16_t type = 0x0008;  // type (IP)
	Writef(&type, sizeof(type), 1);

	// version + header len 45
	// diff service field 00
	// total len 1 00
	// total len 2 34

	uint8_t buff[20];

	buff[0] = 0x45;
	buff[1] = 0x00;
	buff[2] = 0x00;   // len1
	buff[3] = 0x28;   // len2
	buff[4] = 0x43;  // id1
	buff[5] = 0xf0;   // id2
	buff[6] = 0x00;   //flags1
	buff[7] = 0x00;    // flags2
	buff[8] = 0x40;  // ttl
	buff[9] = 0x11;  // protocol UDP 17
	buff[10] = 0x1d;  // header checksum 1
	buff[11] = 0x68;   // header checksum 2

	//uint8_t buff[20] = { 0x45,0,0,0x34,0x43,0xf0,0x0,0,0x40,0x11,0x1d,0x68 }; 
	buff[12] = dest[0];
	buff[13] = dest[1];
	buff[14] = dest[2];
	buff[15] = dest[3];

	buff[16] = src[0];
	buff[17] = src[1];
	buff[18] = src[2];
	buff[19] = src[3];

	// byte 2 i 3 su total_length u Internet Protocol Version 4 polju
	uint16_t tot_len = orig_len - 14;  // 14 je ethenetII polje
	gDec->encode_unsigned16(&buff[2], tot_len);


	Writef(buff, 20, 1);

	// source/dest port
	uint8_t buff_src_dest_port[4] = { 0xba,0xc0,0xba,0xc0 };
	Writef(buff_src_dest_port, 4, 1);

	// len
	uint8_t buff_len[2];
	uint16_t code_len = len + 8;
	gDec->encode_unsigned16(&buff_len[0], code_len);

	Writef(buff_len, 2, 1);

	//  checksum
	uint8_t buff_checksum[2] = { 0x38,0xed };

	Writef(buff_checksum, 2, 1);



    //DWORD nwritten = 0;
	Writef(buffer, len, 1);

	
	return 0;
//#endif

	return 0;
}


/*

int I_cWiresharkIO::WritePDUBufferIP(uint8_t *buffer, uint8_t *src, uint8_t *dest, uint16_t len)
{
#ifdef BACAPI_BUILD_WIN32
	uint8_t bfr[1024];
	int plen = 0;

	uint32_t ts_sec = 0;    // timestamp seconds
	uint32_t ts_usec = 0;   // timestamp microseconds
	uint32_t incl_len = len + 42;  // number of octets of packet saved in file
	uint32_t orig_len = len + 42;  // actual length of packet
	//uint32_t incl_len = len+12;  // number of octets of packet saved in file
	//uint32_t orig_len = len+12+2;  // actual length of packet
	struct timeval tv;



	QDateTime dt;
	ts_sec = dt.currentSecsSinceEpoch();
	ts_usec = dt.currentMSecsSinceEpoch();


	uint32_t swap_sec = _byteswap_ulong(ts_sec);
	uint32_t swap_usec = _byteswap_ulong(ts_usec);

	plen += gDec->encode_unsigned32(&bfr[plen], swap_sec);
	plen += gDec->encode_unsigned32(&bfr[plen], swap_usec);

	uint32_t swap_incl = _byteswap_ulong(incl_len);
	uint32_t swap_orig = _byteswap_ulong(orig_len);

	// 	
	plen += gDec->encode_unsigned32(&bfr[plen], swap_incl);
	plen += gDec->encode_unsigned32(&bfr[plen], swap_orig);


	for (int c = 0; c < 6; c++)
		bfr[plen++] = src[c];

	for (int c = 0; c < 6; c++)
		bfr[plen++] = dest[c];





	fwrite(bfr, 1, plen, fFile);




	uint16_t type = 0x0008;  // type (IP)
	Writef(&type, sizeof(type), 1);

	// version + header len 45
	// diff service field 00
	// total len 1 00
	// total len 2 34
	buff[0] = 0x45;
	buff[1] = 0x00;
	buff[2] = 0x00;   // len1
	//buff[3] = 

	uint8_t buff[20] = { 0x45,0,0,0x34,0x43,0xf0,0x0,0,0x40,0x11,0x1d,0x68 };
	buff[12] = dest[0];
	buff[13] = dest[1];
	buff[14] = dest[2];
	buff[15] = dest[3];

	buff[16] = src[0];
	buff[17] = src[1];
	buff[18] = src[2];
	buff[19] = src[3];

	// byte 2 i 3 su total_length u Internet Protocol Version 4 polju
	uint16_t tot_len = orig_len - 14;  // 14 je ethenetII polje
	gDec->encode_unsigned16(&buff[2], tot_len);


	Writef(buff, 20, 1);

	// source/dest port
	uint8_t buff_src_dest_port[4] = { 0xba,0xc0,0xba,0xc0 };
	Writef(buff_src_dest_port, 4, 1);

	// len
	uint8_t buff_len[2];
	uint16_t code_len = len + 8;
	gDec->encode_unsigned16(&buff_len[0], code_len);

	Writef(buff_len, 2, 1);

	//  checksum
	uint8_t buff_checksum[2] = { 0x38,0xed };

	Writef(buff_checksum, 2, 1);



	DWORD nwritten = 0;
	Writef(buffer, len, 1);


	return 0;
#endif
	return 0;
}

*/



int I_cWiresharkIO::WritePDUBufferEth(uint8_t *buffer, uint8_t *src, uint8_t *dest, uint16_t len)
{

#ifdef BACAPI_BUILD_WIN32

/*
 uint32_t ts_sec=0;    // timestamp seconds
 uint32_t ts_usec=0;   // timestamp microseconds
 uint32_t incl_len=len;  // number of octets of packet saved in file
 uint32_t orig_len=len;  // actual length of packet

if (len <= 46)
   {
    incl_len=60;  // number of octets of packet saved in file
    orig_len=60;  // actual length of packet
   }

 struct timeval tv;


 gTimer.gettimeofday(&tv, NULL);

 ts_sec = tv.tv_sec;
 ts_usec = tv.tv_usec;


 Writef(hFile,&ts_sec, sizeof(ts_sec), 1);
 Writef(hFile,&ts_usec, sizeof(ts_sec), 1);

 // len
 Writef(hFile,&incl_len, sizeof(incl_len), 1);
 Writef(hFile,&orig_len, sizeof(orig_len), 1);



 Writef(hFile,dest, 6, 1);  // dest
 Writef(hFile,src, 6, 1);   // src

 uint8_t buff_len[2];
 gDec->encode_unsigned16(&buff_len[0],len);
 Writef(hFile,buff_len, 2, 1);


 DWORD nwritten = 0;
 Writef(hFile,buffer, len, 1);

 if (len <= 46)
    {
     uint8_t new_len = 46-len;
     uint8_t *tmp_buf = new uint8_t[new_len];
     memset(tmp_buf,0,new_len);
     Writef(hFile,tmp_buf, new_len, 1);
    }

 return 0;
 */
#endif

 return 0;
}



void I_cWiresharkIO::write_test(void)
{
/*
	uint8_t buff[54] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x04, 0x92, 0x26, 0xbe, 0x1a, 0x30, 0x08, 0x20, 0x45, 0x20, 0x20,
											  0x28, 0x01, 0x4d, 0x20, 0x20, 0x80, 0x11, 0xad, 0x1c, 0xc0, 0xa8, 0x05, 0x0c, 0xc0, 0xa8, 0x05, 0xff,
												0xba, 0xc0, 0xba, 0xc0, 0x20, 0x14, 0x8c, 0x81, 0x81, 0x0b, 0x20, 0x0c, 0x01, 0x20, 0xff, 0xff, 0x20, 0xff, 0x10, 0x08};


 fwrite(buff,1,54,fFile);
 



 uint8_t buff[54] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x04, 0x92, 0x26, 0xbe, 0x1a, 0x30, 0x08, 0x20, 0x45, 0x20, 0x20,
										 0x28, 0x01, 0x4d, 0x20, 0x20, 0x80, 0x11, 0xad, 0x1c, 0xc0, 0xa8, 0x05, 0x0c, 0xc0, 0xa8, 0x05, 0xff,
										 0xba, 0xc0, 0xba, 0xc0, 0x20, 0x14, 0x8c, 0x81, 0x81, 0x0b, 0x20, 0x0c, 0x01, 0x20, 0xff, 0xff, 0x20, 0xff, 0x10, 0x08 };


 for (int c = 0; c < 54; c++)
 {
	 bfr[plen++] = buff[c];
 }*/
}

