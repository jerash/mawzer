#ifndef IIC
#define IIC

///////////////////////////////////////////////////////////
// Local Definition
///////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////

//uchar ReadByteI2c_24C16(void);
//uchar WriteByteI2c_24C16(uchar data);
void  InitiateI2cWrite_24C16(void);
void  WriteMultiByteI2c_24C16(far uchar *data_ptr, uchar data_lenght);
void  ReadMultiByteI2c_24C16(far uchar *data_ptr, uchar data_lenght);

#endif