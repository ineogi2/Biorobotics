#ifndef Faulcan_h
#define Faulcan_h

#include "Arduino.h"
#include <FlexCAN.h>

class Faulcan
{
	public:
		Faulcan(int id);
		void Faul_init();
		void Faul_iniset();
		void Faul_iniset_vel();
		void Faul_velset(int vel);
		void Faul_velgo(int vel);
		void Faul_abs_pos_w(int pos);
		int Faul_abs_pos_rw(int pos);
		int Faul_abs_pos_r();

	private:
		int _id;
		int _vel;
		int _pos;
		CAN_message_t msg;
		CAN_message_t inmsg;
		String _vel_hex;
		String _des_pos_hex;
		int currentpos;
};

#endif
