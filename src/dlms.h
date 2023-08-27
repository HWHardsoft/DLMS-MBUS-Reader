static const uint8_t KEY_LENGTH = 16;

#define DLMS_HEADER1_OFFSET 0 // Start of first DLMS header
#define DLMS_HEADER1_LENGTH 26

#define DLMS_HEADER2_OFFSET 256 // Start of second DLMS header
#define DLMS_HEADER2_LENGTH 9

#define DLMS_SYST_OFFSET 11 // system title
#define DLMS_SYST_LENGTH 8

#define DLMS_IC_OFFSET 22 // invocation counter
#define DLMS_IC_LENGTH 4

#define READ_TIMEOUT 1000
#define RECEIVE_BUFFER_SIZE 1024
