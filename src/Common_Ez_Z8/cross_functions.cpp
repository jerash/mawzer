#include <stdio.h>
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "cross_functions.h"

//---------------------------------------------------------------------------
// COMMON MAWZER CONFIGURATION SOFTWARE CROSS-PLATFORM FUNCTIONS
//---------------------------------------------------------------------------
// jerome dumas copyright Wave Idea 2004
// jdumas@waveidea.com
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Fill_Sysex_Header : fulfill firsts common bytes of sysex header
//---------------------------------------------------------------------------
// Create a sysex for control assign lcd string
//---------------------------------------------------------------------------
void Fill_Sysex_Header(uchar *sysex, uchar SYSEX_ID0, uchar SYSEX_ID1,
                                     uchar SYSEX_ID2, uchar SYSEX_ID3)
{
        sysex[0] = MIDI_SOX;
        sysex[1] = JLABS_SYX_ID0;
        sysex[2] = JLABS_SYX_ID1;
        sysex[3] = JLABS_SYX_ID2;
        sysex[4] = JLABS_SYX_ID3;
        sysex[5] = JLABS_SYX_ID4;
        sysex[6] = SYSEX_ID0;
        sysex[7] = SYSEX_ID1;
        sysex[8] = SYSEX_ID2;
        sysex[9] = SYSEX_ID3;
}

//---------------------------------------------------------------------------
// Check_invalid_request :
//---------------------------------------------------------------------------
// check if the required action is authorized
//---------------------------------------------------------------------------
uchar Check_invalid_request(uchar SYSEX_ID0, uchar SYSEX_ID2, uchar SYSEX_ID3)
{
    uchar result = 0;

    ////////////////////////////////////////////////////////////////
    // Invalid request list for MIDI AND LCD CONTROL ASSIGN UPL/DOWN
    ////////////////////////////////////////////////////////////////
    if(  ((SYSEX_ID0 & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_LCD_CTRL_ASSIGN  )    ||
         ((SYSEX_ID0 & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_MIDI_CTRL_ASSIGN ) )
    {
        // 1) Invalid request if group is diffferent than 0 for transport indexes
//MAWZER        if( (SYSEX_ID2 <= FORWARD_CTRL_INDEX) && (SYSEX_ID3) )
//MAWZER             result = INVALID_REQUEST;

        // 2) Invalid request if index number is out of range
//MAWZER        if( SYSEX_ID2 >= CONTROLS_NB)
//MAWZER             result = INVALID_REQUEST;

        // 3) Invalid request if group number is out of range
        if( SYSEX_ID3 >= GROUP_NB)
             result = INVALID_REQUEST;
    }
    else
    ////////////////////////////////////////////////////////////////
    // Invalid request list for COMMON PARAMETERS UPL/DOWN
    ////////////////////////////////////////////////////////////////
    if( (SYSEX_ID0 & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_COMMON_PARAM)
    {
        // 1) Invalid request if program request on the following common params
        if( (!(SYSEX_ID0 & prog_upload_sysxid0))&&(SYSEX_ID2 == IDENTITY_REQUEST_INDEX))
             result = INVALID_REQUEST;

        // 2) Invalid request if common params address is out of range
//MAWZER        if( SYSEX_ID2 > LAST_EXTERNAL_PARAM_INDEX)
//MAWZER			// only identity address (0x7F) can be out of range
//MAWZER			if( SYSEX_ID2 != IDENTITY_REQUEST_INDEX)
//MAWZER				result = INVALID_REQUEST;

        // 3) Invalid request if group number is out of range
        if( SYSEX_ID3 >= GROUP_NB)
             result = INVALID_REQUEST;

    }

    return(result);
}

//---------------------------------------------------------------------------
// Single_Byte_To_Nibbles : nibble a single byte
//---------------------------------------------------------------------------
// destination buffer is filled with nibbles
// return checksum on all 2 nibbles
//---------------------------------------------------------------------------
uchar Single_Byte_To_Nibbles(uchar data, uchar *destination)
{
   uchar checksum = 0;

   // create MSB nibble
   destination[0]    = (uchar)(data >> 4);
   // calculate checksum modulo 128
   checksum = (uchar)((checksum + destination[0]) & 0x7F);
   // create LSB nibble
   destination[1]  = (uchar)(data & 0x0F);
   // calculate checksum modulo 128
   checksum = (uchar)((checksum + destination[1]) & 0x7F);
   return(checksum);
}

//---------------------------------------------------------------------------
// Nibbles_To_Single_Byte :
//---------------------------------------------------------------------------
// data is filled with nibbles
// return checksum on all 2 nibbles
//---------------------------------------------------------------------------
uchar Nibbles_To_Single_Byte(uchar *data, uchar *nibbles_src)
{
   uchar checksum = 0;

   if(nibbles_src[0] & 0xF0) return(INVALID_SYSEX);
   if(nibbles_src[1] & 0xF0) return(INVALID_SYSEX);

   // create data from MSB nibble
   *data = (uchar)(((uchar)(nibbles_src[0] << 4)) & 0xF0);
   checksum = (uchar)((checksum + nibbles_src[0]) & 0x7F);

   // create data from LSB nibble
   *data |= (uchar)(nibbles_src[1] & 0x0F);
   checksum = (uchar)((checksum + nibbles_src[1]) & 0x7F);

   return(checksum);
}

//---------------------------------------------------------------------------
// Check_Valid_Sysex_Header :
//---------------------------------------------------------------------------
// check sysex header dedicated to Wave Idea
// return sysex data length (not sysex lenght) or INVALID_SYSEX if the sysex is not a valid wave idea sysex
//---------------------------------------------------------------------------
uchar Check_Valid_Sysex_Header(uchar *sysex, uchar index, uchar group)
{
        // Check is the sysex buffer contains a Wave Idea sysex
        if(sysex[0] != MIDI_SOX)        return(INVALID_SYSEX);
        if(sysex[1] != JLABS_SYX_ID0)     return(INVALID_SYSEX);
        if(sysex[2] != JLABS_SYX_ID1)     return(INVALID_SYSEX);
        if(sysex[3] != JLABS_SYX_ID2)     return(INVALID_SYSEX);
        if(sysex[4] != JLABS_SYX_ID3)     return(INVALID_SYSEX);
        if(sysex[5] != JLABS_SYX_ID4)     return(INVALID_SYSEX);
        // Check it is an incoming sysex
        if(!(sysex[6] & sysex_direction_sysxid0)) return(INVALID_SYSEX);
        // Here we have an incoming wave idea sysex
        // check that group and index are the ones expected
        if(sysex[8] != index)           return(INVALID_SYSEX);
        if(sysex[9] != group)           return(INVALID_SYSEX);
        // return sysex data lenght
        return(sysex[7]);
}

//---------------------------------------------------------------------------
// SYSEX TO COMMON PARAM
//---------------------------------------------------------------------------
// extract the common param values from sysex
// address    = address of expected commmon param
//              if common param is group name (address 0x0C) values is a pointer on group
//              name string of 14 bytes (terminated by \0)
// values     = pointer on common param values to be filled (may also be a single value)
// sysex      = pointer on already filled sysex buffer
// group      = group to upload/download (only used for group names)
// returns    = INVALID_SYSEX if not a valid sysex, sysex lenght if OK
//---------------------------------------------------------------------------
uchar Bs3x_Sysex_To_Common_Param(uchar address, uchar *values, uchar *sysex, uchar group)
{
        //uchar SYSEX_ID0, SYSEX_ID1, SYSEX_ID2, SYSEX_ID3;
        uchar result, i, j, k; //, checksum;

        // Check the sysex contains a common param
        if((sysex[6] & FRAME_TO_Z8_TYPE_MSK) != FRAME_IS_COMMON_PARAM)
           return(INVALID_SYSEX);

        // Check if the requested action is coherent
        if(Check_invalid_request(sysex[6], address, group) == INVALID_REQUEST)
          return (INVALID_REQUEST);

        // First check the sysex is a valid incoming wave idea sysex
        result = Check_Valid_Sysex_Header(sysex, address, group);

        // Here the received sysex is valid, result contains sysex data lenght in nibbles
        if(result != Get_Common_Param_Data_Lenght(address)) return(INVALID_SYSEX);

        // group name (address 0x0C) is treated separately since pointer on string is passed
        // un-nibble group names and add terminated string
//MAWZER        if(address == GROUP_NAME_INDEX)
//MAWZER        {
//MAWZER            // Fill group name from sysex content
//MAWZER            j=0;
//MAWZER            for(i=0; i < (result >> 1); i++)
//MAWZER            {
//MAWZER               k = Nibbles_To_Single_Byte(&values[i], &sysex[SYSEX_DATA_START+j]);
//MAWZER               if(k == INVALID_SYSEX) return(INVALID_SYSEX);
//MAWZER               j += 2;
//MAWZER            }
//MAWZER
//MAWZER            // Add null terminated string
//MAWZER            values[i] = '\0';
//MAWZER        }
//MAWZER        else
//MAWZER        if(address == INT_TEMPO_INDEX)
//MAWZER        {
//MAWZER            // Fill tempo from sysex content
//MAWZER            k = Nibbles_To_Single_Byte(&values[0], &sysex[SYSEX_DATA_START]);
//MAWZER			if(k == INVALID_SYSEX) return(INVALID_SYSEX);
//MAWZER		}
//MAWZER		else
		{
            // Fill common param from sysex content
            for(i=0; i < result; i++)
                values[i] = sysex[SYSEX_DATA_START + i];
        }

        // return the whole sysex lenght
        return((uchar)(SYSEX_DATA_START + result + 1 + 1));
}

//---------------------------------------------------------------------------
// Get_Common_Param_Data_Lenght :
//---------------------------------------------------------------------------
// return common param data length (not sysex lenght)
//---------------------------------------------------------------------------
uchar Get_Common_Param_Data_Lenght(uchar address)
{
   uchar data_lenght;

   switch(address)
   {
//MAWZER           case USER_CURVE_0_LOW_INDEX    :
//MAWZER           case USER_CURVE_0_HIGH_INDEX   :
//MAWZER           case USER_CURVE_1_LOW_INDEX    :
//MAWZER           case USER_CURVE_1_HIGH_INDEX   :     data_lenght = CURVE_TABLES_LENGHT >> 1;
//MAWZER                                                break;
//MAWZER           case GROUP_NAME_INDEX          :     data_lenght = GROUP_NAME_LENGHT << 1;
//MAWZER                                                break;
           case IDENTITY_REQUEST_INDEX    :     data_lenght = IDENTITY_LENGHT;
                                                break;
//MAWZER           case INT_TEMPO_INDEX			  :     data_lenght = 2;
//MAWZER                                                break;
           default                        :     data_lenght = 1;
                                                break;
   }
   return(data_lenght);
}


//---------------------------------------------------------------------------
// FIRMWARE TO SYSEX
//---------------------------------------------------------------------------
// Create a sysex for firmware
// action     = START_FW_UPGRADE, DOWNLOAD_EZ, DOWNLOAD_Z8, PREPARE_Z8_UPGRADE, FINISH_FW_UPGRADE
// address    base address of firmware to download (not used if action is INITIATE)
// values     = if action = DOWNLOAD : pointer on firmware int datas
//            = if action = INITIATE_FW_UPGRADE : ez firmware size, z8 firmware size
// lenght     = datas nb to download, must be <= 16 ; if action is INITIATE : must be 6
//              if action = FINISH_FW_UPGRADE length must be 0
// sysex      = pointer on sysex buffer to be filled
// returns created sysex lenght, INVALID_REQUEST
// address are 16bits splitted into : 2 msb ; 0 + 7middes ; 0 +7 lsb to be sent on 7bits sysexs
// values are 16bits containing 0000 , high nibble, 0000, low nibble
//---------------------------------------------------------------------------
uchar Bs3x_Firmware_To_Sysex(uchar action, uint address, uchar lenght, uint *values, uchar *sysex)
{
        uchar SYSEX_ID0, SYSEX_ID1; //, SYSEX_ID2, SYSEX_ID3;
        uchar sysex_lenght, i, k, m, checksum;
		
        SYSEX_ID0 = FRAME_IS_FIRMWARE_OR_PURE_MIDI | acknoledge_sysxid0;

		sysex_lenght = INVALID_REQUEST;

		for(m=0; m<SYSEX_BUFFER_MAX; m++) 
			sysex[m] = 0;


        // ACTION IS A FIRMWARE DOWNLOAD
        if((action == DOWNLOAD_EZ)||(action == DOWNLOAD_Z8))
        {
           // request a checksum check if downloading firmware datas
           SYSEX_ID0 |= checksum_check_sysxid0;
           // when upgrading fw datas, lenght must not be 0
           if(lenght == 0) return (INVALID_REQUEST);

           // firmware data lenght checks
           if(action == DOWNLOAD_EZ)
		   {
			    //if(lenght > EE_24C64_PAGE_LENGHT) return (INVALID_REQUEST);
				if(lenght > FIRMWARE_DATA_MAX_EZ) return (INVALID_REQUEST);
		   }
		   else
           if(action == DOWNLOAD_Z8)
		   {
				if(lenght > FIRMWARE_DATA_MAX_Z8) return (INVALID_REQUEST);
		   }

           // Get lenght of firmware datas in bytes in sysex (2 bytes per int)
           SYSEX_ID1  = (uchar)(lenght * 2);

           // whole sysex lenght = f0 00 20 4f 00 01 UU LL AA AA AA - 2 x data lenght + CS + F7
           sysex_lenght = (uchar)(SYSEX_HEADER_SIZE + 1 + 3 + SYSEX_ID1 + 1 + 1);

		   // check the sysex lenght is < 128
		   if(sysex_lenght > SYSEX_BUFFER_MAX) return (INVALID_REQUEST);

           if(action == DOWNLOAD_Z8)
              SYSEX_ID0 |= prog_ez_z8_sysxid0;

           // Fill sysex header
           Fill_Sysex_Header(sysex, SYSEX_ID0, SYSEX_ID1, 0, 0);

           // assign adress into sysex
           sysex[8]  = (uchar) ((address >> 14) & 0x03);
           sysex[9]  = (uchar) ((address >> 7 ) & 0x7F);
           sysex[10] = (uchar) ((address      ) & 0x7F);

           // assign firmware datas into sysex
           k=0;
           checksum=0;
           for(i=0; i<SYSEX_ID1; i=(uchar)(i+2))
           {
              if((values[k] & 0x5050)!=0x5050) return(INVALID_REQUEST);
              sysex[i+11] = (uchar) ((values[k] >> 8) & 0x00FF);
              checksum = (uchar)((checksum + sysex[i+11]) & 0x7F);
              sysex[i+12] = (uchar) ((values[k] & 0x00FF));
              checksum = (uchar)((checksum + sysex[i+12]) & 0x7F);
              k=(uchar)(k+1);
           }

           // insert calculated checksum into sysex
           sysex[sysex_lenght-2] = checksum;
        }
        else
        if(action == START_FW_UPGRADE)
        {
          // here action is START_FW_UPGRADE
          // simply send frame is firmware with lenght = 127
          if(lenght != DATA_LENGHT_START_BOOTLOADER) return (INVALID_REQUEST);

		  SYSEX_ID0 |= checksum_check_sysxid0;

          // Fill sysex header
          Fill_Sysex_Header(sysex, SYSEX_ID0, lenght, 0, 0);

          // whole sysex lenght = f0 00 20 4f 00 01 UU LL F7
          sysex_lenght = SYSEX_HEADER_SIZE + 1 + 1;

        }
        else
        if( (action == FINISH_FW_UPGRADE) || (action == PREPARE_Z8_UPGRADE))
        {
          // here action is FINISH_FW_UPGRADE
          // lenght must be 0
          if(lenght != 0) return (INVALID_REQUEST);

          // signal to finish upgrade
          SYSEX_ID0 |= fw_znext_finish_sysxid0;

		  // prepare z8 is done setting finish + prog z8 bits
		  if(action == PREPARE_Z8_UPGRADE)
			 SYSEX_ID0 |= prog_ez_z8_sysxid0;

          // Fill sysex header
          Fill_Sysex_Header(sysex, SYSEX_ID0, lenght, 0, 0);

          // whole sysex lenght = f0 00 20 4f 00 01 UU F7
          sysex_lenght = SYSEX_HEADER_SIZE + 1 + 1;

        }
         // insert EOX
        sysex[sysex_lenght-1] = MIDI_EOX;

        return(sysex_lenght);
}

//---------------------------------------------------------------------------
// BFW FILE TO FIRMWARES
//---------------------------------------------------------------------------
// ez_fw_size  = pointer on ez firmware size to get
// z8_fw_size  = pointer on z8 firmware size to get
// ez_firmware = pointer on firmware int datas to fill
// z8_firmware = pointer on firmware int datas to fill
// returns 0 if no error, INVALID_BFW
// both firmwares are read encoded, then stored and sent to midi encoded as well
//---------------------------------------------------------------------------
uchar Bs3x_BfwFile_To_Firmwares(char *filename, uchar *identity, uint *ez_fw_size, uint *z8_fw_size, uint *ez_firmware, uint *z8_firmware)
{
  uint i, checksum, checksum_file;
  uint tmp_read;

  FILE * fic;
  fic = fopen(filename, "rt");
  if (fic == NULL)
    return(INVALID_BFW);
  // place pointer at the begining of file
  fseek(fic, 0, 0);

  // get identity
  for(i=0; i< IDENTITY_LENGHT ; i++)
  {
	  fscanf(fic, "%2x", &tmp_read);
	  identity[i] = (uchar)(tmp_read); 	
  }

  // now should get a ;
  if(fgetc(fic) != ';') return(INVALID_BFW);
  // read fw sizes
  fscanf(fic, "%4x;%4x;", (uint *) ez_fw_size, (uint *) z8_fw_size);
  // get ez firmware datas
  checksum = 0;
  for(i=0; i< *ez_fw_size ; i++)
  {
      fscanf(fic, "%4x", (uint *) &tmp_read);
	  ez_firmware[i] = tmp_read;	
	  // firmware datas MSB & LSB nibbles must be 0
      if((ez_firmware[i] & 0x5050)!=0x5050) return(INVALID_BFW);
      // calculate checksum
      checksum = (checksum + ez_firmware[i]) & 0x3FFF;
  }
  // compare checksums
  fscanf(fic, "%4x", (uint *) &checksum_file);
  if(checksum != checksum_file) return(INVALID_BFW);
  // now should get a ;
  if(fgetc(fic) != ';') return(INVALID_BFW);

  // get z8 firmware datas
  checksum = 0;
  for(i=0; i< *z8_fw_size ; i++)
  {
      fscanf(fic, "%4x", (uint *) &tmp_read);
	  z8_firmware[i] = tmp_read;
      // firmware datas MSB & LSB nibbles must be 0
      if((z8_firmware[i] & 0x5050)!=0x5050) return(INVALID_BFW);
      // calculate checksum
      checksum = (checksum + z8_firmware[i]) & 0x3FFF;
  }
  // compare checksums
  fscanf(fic, "%4x", (uint *) &checksum_file);
  if(checksum != checksum_file) return(INVALID_BFW);
  // now should get a ;
  if(fgetc(fic) != ';') return(INVALID_BFW);

  return (0);
}

//---------------------------------------------------------------------------
// SYSEX FIRMWARE ACKNOLEDGE RESULT
//---------------------------------------------------------------------------
// check acknoledge result after firmware upgrade request
// action     = CHECK_FW_ACKNOLEDGE_RESULT, WAIT_FIRMWARE_ACK
// address    = base address of firmware dowloaded, 0 if action is WAIT_FIRMWARE_ACK
// lenght     = datas nb downloaded, must be < 16
// sysex      = pointer on sysex buffer received
// returns INVALID_SYSEX if not a valid sysex, or :
//         ACK_ANSWER_NO_ERROR, ACK_ANSWER_ERROR if action = CHECK_FW_ACKNOLEDGE_RESULT
//         0 if action is WAIT_FIRMWARE_ACK
// address is 16bits : it must be unsplitted from : 2 msb ; 0 + 7middes ; 0 +7 lsb
//---------------------------------------------------------------------------
uchar Bs3x_Sysex_To_Firmware(uchar action, uchar *sysex)
{
        // Check the sysex contains firmware result
        if((sysex[6] & FRAME_TO_Z8_TYPE_MSK) != FRAME_IS_FIRMWARE_OR_PURE_MIDI)
           return(INVALID_SYSEX);

        // Check is the sysex buffer contains a Wave Idea sysex
        if(sysex[0] != MIDI_SOX)        return(INVALID_SYSEX);
        if(sysex[1] != JLABS_SYX_ID0)     return(INVALID_SYSEX);
        if(sysex[2] != JLABS_SYX_ID1)     return(INVALID_SYSEX);
        if(sysex[3] != JLABS_SYX_ID2)     return(INVALID_SYSEX);
        if(sysex[4] != JLABS_SYX_ID3)     return(INVALID_SYSEX);
        if(sysex[5] != JLABS_SYX_ID4)     return(INVALID_SYSEX);
        // Check it is an incoming sysex
        if(!(sysex[6] & sysex_direction_sysxid0)) return(INVALID_SYSEX);

        if(action == WAIT_FIRMWARE_ACK)
        {
          if(sysex[7] != MIDI_EOX)  return(INVALID_SYSEX);
          if(sysex[6] & acknoledge_sysxid0)
             return (ACK_ANSWER_NO_ERROR);
          return(ACK_ANSWER_ERROR);
        }
/*
        if(action == CHECK_FW_ACKNOLEDGE_RESULT)
        {
           // Check if the lenght matches the expected lenght
           if(sysex[7] != (lenght*2))  return (INVALID_SYSEX);

           // Check if the address matches the expected address
           if(sysex[8]  != (uchar) ((address >> 14) & 0x03))  return(INVALID_SYSEX);
           if(sysex[9]  != (uchar) ((address >> 7 ) & 0x7F))  return(INVALID_SYSEX);
           if(sysex[10] != (uchar) ((address      ) & 0x7F))  return(INVALID_SYSEX);

           if(sysex[11] != MIDI_EOX)  return(INVALID_SYSEX);

           // a valid sysex is found : check acknoledge result
           if(sysex[6] & acknoledge_sysxid0) return(ACK_ANSWER_NO_ERROR);
           return(ACK_ANSWER_ERROR);
        }
*/
    return(INVALID_SYSEX);
}

