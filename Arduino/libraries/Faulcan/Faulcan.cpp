#include "Arduino.h"
#include "Faulcan.h"
#include <FlexCAN.h>

String int2hex_8(int a)
{
  	String s = String(a, HEX);
  	int leng = s.length();
  	if(leng<8)
  	{
    	int resid = 8-leng;
    	for (int i = 0; i<resid;++i)
    	s = '0'+s;
  	}
  	return s;
}

uint16_t hex2uint_16(String s)
{
  uint16_t ret_val;
  if(s[0] >= 'a') ret_val = 16*(s[0]-'a'+10);
  else ret_val = 16*(s[0]-'0');
  if(s[1] >= 'a') ret_val = +ret_val+(s[1]-'a'+10);
  else ret_val = ret_val+(s[1]-'0');
  return ret_val;
}

uint32_t hex2uint_32(String s)
{
  uint32_t ret_val;
  if(s[0] >= 'a') ret_val = 16*16*(s[0]-'a'+10);
  else ret_val = 16*16*(s[0]-'0');
  if(s[1] >= 'a') ret_val = 16*(s[1]-'a'+10);
  else ret_val = 16*(s[1]-'0');
  if(s[2] >= 'a') ret_val = +ret_val+(s[2]-'a'+10);
  else ret_val = ret_val+(s[2]-'0');
  return ret_val;
}

Faulcan::Faulcan(int id)
{
	_id = id;
}

void Faulcan::Faul_init()
{
	msg.ext = 0;
    msg.id = 0x000;
    msg.len = 2;
    msg.buf[0] = 0x01;
    msg.buf[1] = hex2uint_16(_id);
    Can0.write(msg);
    delayMicroseconds(800);
}
void Faulcan::Faul_iniset()
{
    int ini_msgid = 1536+_id;
    msg.ext = 0;
    msg.id = ini_msgid;
    msg.len = 8;
    // shutdown command
    msg.buf[0] = 0x2B;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = 0x06;
    msg.buf[5] = 0x00;
    msg.buf[6] = 0x00;
    msg.buf[7] = 0x00;
    Can0.write(msg);
    //switch on
    delayMicroseconds(800);
    msg.buf[0] = 0x2B;
    msg.buf[1] = 0x40;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = 0x07;
    msg.buf[5] = 0x00;
    msg.buf[6] = 0x00;
    msg.buf[7] = 0x00;
    Can0.write(msg);
    //operation enable
    delayMicroseconds(800);
    msg.buf[0] = 0x2B;
    msg.buf[1] = 0x40;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = 0x0F;
    msg.buf[5] = 0x00;
    msg.buf[6] = 0x00;
    msg.buf[7] = 0x00;
    Can0.write(msg);
    // Operational mode
    delayMicroseconds(800);
    msg.buf[0] = 0x2F;
    msg.buf[1] = 0x60;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = 0x01;
    msg.buf[5] = 0x00;
    msg.buf[6] = 0x00;
    msg.buf[7] = 0x00;
    Can0.write(msg);
    delayMicroseconds(800);
}


void Faulcan::Faul_iniset_vel()
{
    int ini_msgid = 1536+_id;
    msg.ext = 0;
    msg.id = ini_msgid;
    msg.len = 8;
    // shutdown command
    msg.buf[0] = 0x2B;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = 0x06;
    msg.buf[5] = 0x00;
    msg.buf[6] = 0x00;
    msg.buf[7] = 0x00;
    Can0.write(msg);
    //switch on
    delayMicroseconds(800);
    msg.buf[0] = 0x2B;
    msg.buf[1] = 0x40;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = 0x07;
    msg.buf[5] = 0x00;
    msg.buf[6] = 0x00;
    msg.buf[7] = 0x00;
    Can0.write(msg);
    //operation enable
    delayMicroseconds(800);
    msg.buf[0] = 0x2B;
    msg.buf[1] = 0x40;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = 0x0F;
    msg.buf[5] = 0x00;
    msg.buf[6] = 0x00;
    msg.buf[7] = 0x00;
    Can0.write(msg);
    // Operational mode
    delayMicroseconds(800);
    msg.buf[0] = 0x2F;
    msg.buf[1] = 0x60;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = 0x03;
    msg.buf[5] = 0x00;
    msg.buf[6] = 0x00;
    msg.buf[7] = 0x00;
    Can0.write(msg);
    delayMicroseconds(800);
}

void Faulcan::Faul_velset(int vel)
{
    int ini_msgid = 1536+_id;
    msg.ext = 0;
    msg.id = ini_msgid;
    msg.len = 8;
    _vel_hex = int2hex_8(vel);
    ////////
    msg.buf[0] = 0x23;
    msg.buf[1] = 0x81;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = hex2uint_16(_vel_hex.substring(6,8));
    msg.buf[5] = hex2uint_16(_vel_hex.substring(4,6));
    msg.buf[6] = hex2uint_16(_vel_hex.substring(2,4));
    msg.buf[7] = hex2uint_16(_vel_hex.substring(0,2));
    Can0.write(msg);
    //make same
    delayMicroseconds(800);
    msg.buf[1] = 0x7F;
    Can0.write(msg);
    delayMicroseconds(800);
}
void Faulcan::Faul_velgo(int vel)
{
    int ini_msgid = 1536+_id;
    msg.ext = 0;
    msg.id = ini_msgid;
    msg.len = 8;
    _vel_hex = int2hex_8(vel);
    ////////
    msg.buf[0] = 0x23;
    msg.buf[1] = 0xFF;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = hex2uint_16(_vel_hex.substring(6,8));
    msg.buf[5] = hex2uint_16(_vel_hex.substring(4,6));
    msg.buf[6] = hex2uint_16(_vel_hex.substring(2,4));
    msg.buf[7] = hex2uint_16(_vel_hex.substring(0,2));
    Can0.write(msg);
    //make same
    delayMicroseconds(800);
    msg.buf[1] = 0x7F;
    Can0.write(msg);
    delayMicroseconds(800);
}
void Faulcan::Faul_abs_pos_w(int pos)
{
    int ini_msgid = 1536+_id;
    msg.ext = 0;
    msg.id = ini_msgid;
    msg.len = 8;
	_des_pos_hex = int2hex_8(pos);
	//start msg. stt_msg
  	msg.buf[0] = 0x2B;
    msg.buf[1] = 0x40;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = 0x1F;
    msg.buf[5] = 0x00;
    msg.buf[6] = 0x00;
    msg.buf[7] = 0x00;
    Can0.write(msg);
    delayMicroseconds(800);
    msg.buf[0] = 0x23;
    msg.buf[1] = 0x7A;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = hex2uint_16(_des_pos_hex .substring(6,8));
    msg.buf[5] = hex2uint_16(_des_pos_hex .substring(4,6));
    msg.buf[6] = hex2uint_16(_des_pos_hex .substring(2,4));
    msg.buf[7] = hex2uint_16(_des_pos_hex .substring(0,2));
    Can0.write(msg);
    delayMicroseconds(800);
    //end msg. fnl_msg
    msg.buf[0] = 0x2B;
    msg.buf[1] = 0x40;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = 0x0F;
    msg.buf[5] = 0x00;
    msg.buf[6] = 0x00;
    msg.buf[7] = 0x00;
    Can0.write(msg);
    delayMicroseconds(800);
}

int Faulcan::Faul_abs_pos_rw(int pos)
{
    int ini_msgid = 1536+_id;
    msg.ext = 0;
    msg.id = ini_msgid;
    msg.len = 8;
	_des_pos_hex = int2hex_8(pos);
	//start msg. stt_msg
  	msg.buf[0] = 0x22;
    msg.buf[1] = 0x40;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = 0x1F;
    msg.buf[5] = 0x00;
    msg.buf[6] = 0x00;
    msg.buf[7] = 0x00;
    Can0.write(msg);
    delayMicroseconds(800);
    msg.buf[0] = 0x22;
    msg.buf[1] = 0x7A;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = hex2uint_16(_des_pos_hex .substring(6,8));
    msg.buf[5] = hex2uint_16(_des_pos_hex .substring(4,6));
    msg.buf[6] = hex2uint_16(_des_pos_hex .substring(2,4));
    msg.buf[7] = hex2uint_16(_des_pos_hex .substring(0,2));
    Can0.write(msg);
    delayMicroseconds(800);
    //end msg. fnl_msg
    msg.buf[0] = 0x22;
    msg.buf[1] = 0x40;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = 0x0F;
    msg.buf[5] = 0x00;
    msg.buf[6] = 0x00;
    msg.buf[7] = 0x00;
    Can0.write(msg);
    //pos_msg
    msg.buf[0] = 0x40;
    msg.buf[1] = 0x64;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = 0x00;
    msg.buf[5] = 0x00;
    msg.buf[6] = 0x00;
    msg.buf[7] = 0x00;
    Can0.write(msg);
    delayMicroseconds(800);
    while(Can0.available())
    {
	    Can0.read(inmsg);
    	int target_id = 1408+_id;
	    //int target_id = 640+_id;
	    if(inmsg.id == target_id)
	    {
        if(inmsg.buf[1] == 100 && inmsg.buf[2] == 96)
        {
          currentpos = ((int32_t)inmsg.buf[7] << 24) | ((int32_t)inmsg.buf[6] << 16) | ((int16_t)inmsg.buf[5] << 8) | inmsg.buf[4];
          break;
        }

		}
		delayMicroseconds(800);
	}
	return currentpos;
}

int Faulcan::Faul_abs_pos_r()
{
    int ini_msgid = 1536+_id;
    msg.ext = 0;
    msg.id = ini_msgid;
    msg.len = 8;
    msg.buf[0] = 0x40;
    msg.buf[1] = 0x64;
    msg.buf[2] = 0x60;
    msg.buf[3] = 0x00;
    msg.buf[4] = 0x00;
    msg.buf[5] = 0x00;
    msg.buf[6] = 0x00;
    msg.buf[7] = 0x00;
    Can0.write(msg);
    delayMicroseconds(800);
    while(Can0.available())
    {
	    Can0.read(inmsg);
	    int target_id = 1408+_id;
	    if(inmsg.id == target_id)
	    {
			if(inmsg.buf[1] == 100 && inmsg.buf[2] == 96)
			{
				currentpos = ((int32_t)inmsg.buf[7] << 24) | ((int32_t)inmsg.buf[6] << 16) | ((int16_t)inmsg.buf[5] << 8) | inmsg.buf[4];
				break;
			}
		}
		delayMicroseconds(800);
	}

	return currentpos;
}
