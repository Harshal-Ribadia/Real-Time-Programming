// ----------------------------------------------------------------------------
/// \file		 dia_comm4.c
/// \brief		 realizes diagnostic ASCII communication (M4 part)
/// \author		 Wolfgang Schulter 
/// \license	 for educational purposes only, no warranty, see license.txt
/// \date		 27.07.2014 ws:  M4 version
/// \date		 17.05.2012 ws:  initial version
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// FreeRTOS includes
// ----------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "mcommon.h"  		// commons for all modules
#include "supervision.h" 	// supervision module (stack, exec time)

// #include "i2s_audio.h"		// HQ I2C-Audio module

#include "dia_comm4.h"		// ASCII dia comm module (M4)

#include "ws_strfun.h"		// small string functions (atoi, htoi, ...)
#include "app.h"			// the M4 app module

// #include "data_monitor.h"	// high bandwidth data monitor
// #include "pwm.h"			// PWM with multiple lines (optional: buzzer for audible output)

// #include "fgen.h"			// FGen module
// #include "filter.h"			// filter module
// #include "fft_filter.h"		// FFT-filter module
// #include "ffti.h"			// xfft[] buffer

#include <stdio.h>			// sprintf
#include <stdlib.h>			// atoi
// #include <string.h>			// strtok

xSemaphoreHandle semDIAC;		// semaphore dia_comm task activation

char _rxbuf[NRXBUF];

// ws: preferred addresses for filter coeff's
#define NUM_PREF_VAR	1

/*enum comm_obj_staes {
 COMM_OBJ_IDLE = 0,
 COMM_OBJ_STARTED_FGEN = 1,
 COMM_OBJ_STARTED_FILT = 2,
 COMM_OBJ_STARTED_DMON = 4,
 COMM_OBJ_STARTED_RMEM = 8,
 COMM_OBJ_STARTED_WMEM = 16
 };*/

#define 	LF		0x0A	// linefeed

char delim[] = " \t";
short comm_obj_state = 0; //COMM_OBJ_IDLE;
unsigned short _nrxbuf = 0;
unsigned short comm_timeout = 0, comm_analyze_command_working = 0;

static int vi[4];

// ----------------------------------------------------------------------------
unsigned int get_addr(char *p)
{
	short i;
	char *pref;

	if (p == 0) return 0;

	if ((p[0] == '0') && (p[1] == 'x'))
		return htoi(p);

	/*for (i=0; i< NUM_PREF_VAR; i++) {
		pref = diag_pref_var[i];
		//_DBG_(pref);
		//_DBG_(p);
		if (!ws_strncmp(p, pref, 16 ))
			return diag_pref_addr[i];
	}*/
	return 0;
}

// ----------------------------------------------------------------------------
/*void set_comm_idle() {
 _rxbuf[_nrxbuf] = 0;
 comm_obj_state = COMM_OBJ_IDLE; // finished
 comm_timeout = 0;
 _nrxbuf = 0;
 }*/

// ----------------------------------------------------------------------------
// ws: comm tick function, called with some periodic time
// ----------------------------------------------------------------------------
/*void comm_tic() {
 if (comm_timeout) {
 comm_timeout--;
 if (comm_timeout == 0) {
 // set_comm_idle(); // force idle state
 _DBG_("comm: timeout, idle");
 }
 }
 }*/

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
char *get_next_item(char *p) {
	if (*p == 0)
		return p;
	p++;
	while ((*p == ' ') || (*p = '\t'))
		p++;
	return p;
}

// ----------------------------------------------------------------------------
// ws: comm analyzer for a command line in rxbuf
// ----------------------------------------------------------------------------
char txt[NTXT];
void comm4_analyze_command(char *buf, int nbuf) {
	// char *prxbuf = rxbuf;
	char sub_cmd;
	int val = 0;
	static double v[8];
	int i, nval = 0;

	comm_analyze_command_working = 1;
	//_DBD(nbuf);

	if ((buf[0] == 'S') || (buf[0] == 's')) {

		// ------------------------------------------------------
		// I2S Audio Sound command	S <value>LF
		// ------------------------------------------------------
		/*if (ha_enable) {
			disable_i2s_audio();  // HQ Audio
		} else {
			enable_i2s_audio();  // HQ Audio
		}
		ha_enable = !ha_enable;
		PRINTF("comm: HQ I2S-Audio: %d, '%c'\n",ha_enable,buf[0]);
		*/
	} else 	if ((buf[0] == 'G') || (buf[0] == 'g')) {
#ifdef _USE_FGEN
		// ------------------------------------------------------
		// FGen command	G M|F|A <value>LF
		// ------------------------------------------------------

		sub_cmd = buf[1];

		if (nbuf > 2) {
			v[0] = atof(buf + 2);
			val = atoi(buf + 2); 		//
			nval = 1;
		}

		switch (sub_cmd) {

		/*case 'F': // set frequency
		case 'f':
			{
				if (fgen_swp_mode) 		// since 1.03. release sweep mode, if set
					fgen_swp_mode = 0;

				// fgen_par.f0 = (short) val;
				if (nval == 1)  {
					fgen_par[0].f0 = fgen_par[1].f0 = fgen_par[2].f0 = (int)(v[0] * PARAM_NF);		// since v.10
					ws_ftoa3(v[0],txt);
				} else if (nval == 2)  {
					fgen_par[0].f0 = (int)(v[0] * PARAM_NF);		// since v.10
					fgen_par[1].f0 = (int)(v[1] * PARAM_NF);		// since v.13
					ws_ftoa3(v[0],txt);
				} else if (nval == 3)  {
					fgen_par[0].f0 = (int)(v[0] * PARAM_NF);		// since v.10
					fgen_par[1].f0 = (int)(v[1] * PARAM_NF);		// since v.13
					fgen_par[2].f0 = (int)(v[2] * PARAM_NF);		// since v.13
					ws_ftoa3(v[0],txt);
				} else
					ws_ftoa3((float)fgen_par[0].f0/PARAM_NF,txt);
				PRINTF("comm: fGen f=%s Hz\n", txt);
				//set_comm_idle();
				// do_menu(0, 0, 1);
				// menu_update_req(MENU_FGEN_F0);
			}
			break;*/

		case 'S': // set sweep frequencies, sweep delay
		case 's':
			// ------------------------------------------------------
			// fGen command	G S f1 f2 mode delay [pow_stop] LF
			// ------------------------------------------------------
			{  // if ((val > 0) && (val < (configTICK_RATE_HZ / 2))) {
				float f1 = 10, f2 = 1000;
				short delay = 100;
				int pow_limit = -1, mode = 0;
				char *p = ws_strtok(buf + 2, delim), *pmem_c;
				unsigned short ind, len;

				if ((p = ws_strtok(0, delim))) {
					f1 = atof(p);
				} else
					goto sweep_mode_off;

				if ((p = ws_strtok(0, delim))) {
					f2 = atof(p);
					// f2 = MAX(f1,f2);
				} else
					goto sweep_mode_off;

				fgen_swp_f1 = (f1 * PARAM_NF);	// since v.10
				fgen_swp_f2 = (f2 * PARAM_NF);

				if ((p = ws_strtok(0, delim))) {
					mode = atoi(p);
					if ((mode >= 0) && (mode < SWEEP_MODE_NUM))
						fgen_swp_mode = mode;
					else
						goto sweep_mode_off;
				}

				if ((p = ws_strtok(0, delim))) {
					delay = atoi(p);
					if (delay > 0)
						fgen_swp_delay = delay;
					else
						goto sweep_mode_off;
				}

				if ((p = ws_strtok(0, delim))) {
					pow_limit = atoi(p);
					if (pow_limit > 0)
						fgen_swp_pow_stop = pow_limit;
					else
						goto sweep_mode_off;
				}
				ws_ftoa3(f1, txt);
				ws_ftoa3(f2, txt+32);
				PRINTF("comm: fGen sweep f1=%s f2=%s sweep_mode=%d delay=%d pow_lim=%d\n", txt, txt+32,
					fgen_swp_mode,fgen_swp_delay,fgen_swp_pow_stop);
				//set_comm_idle();
				// do_menu(0, 0, 1);
				// menu_update_req(MENU_FGEN_F0);
				break;

			sweep_mode_off:
				fgen_swp_mode = 0;
				PRINTF("comm: fGen sweep mode off\n");
				break;
			}

		case 'D': // set duty cycle
		case 'd':
			if ((val >= 0) && (val <= 1000)) {
				fgen_par[0].dty = (short) val;
				PRINTF("comm: fGen duty cycle=%d (1000 is 100%%)\n", fgen_par[0].dty);
				//set_comm_idle();
				// do_menu(0, 0, 1);
				// menu_update_req(MENU_FGEN_DUTY);
			}
			break;

		case 'O': // set Offset
		case 'o':
			{
				int i;
				char *p = ws_strtok(buf + 2, delim); //, *pmem;
				for (i = 0; i < 3; i++) {
					if (!p) break;
					if ((p = ws_strtok(0, delim))) {
						val = atoi(p);
						if ((val >= -32768) && (val < 32767))
							fgen_par[i].off = (short) val;
					}
				}
				PRINTF("comm: fGen offset[] = %d %d %d\n",
						fgen_par[0].off, fgen_par[1].off, fgen_par[2].off);
			}
			break;

		case 'A': // set amplitude (3x)
		case 'a':
			{
				int i;
				char *p = ws_strtok(buf + 2, delim); //, *pmem;
				for (i = 0; i < 3; i++) {
					if (!p) break;
					if ((p = ws_strtok(0, delim))) {
						val = atoi(p);
						if ((val >= -32768) && (val < 32767))
							fgen_par[i].amp = (short) val;
					}
				}
				PRINTF("comm: fGen amp[] = %d %d %d\n",
						fgen_par[0].amp, fgen_par[1].amp, fgen_par[2].amp);
			}
			break;

		case 'M': // set mode
		case 'm':
			{
				int i, n=0;
				char *p = ws_strtok(buf + 2, delim); //, *pmem;
				for (i = 0; i < 3; i++) {
					if (!p) break;
					if ((p = ws_strtok(0, delim))) {
						val = atoi(p);
						n++;
						if ((val >= 0) && (val < FGEN_NUM_MODES))
							fgen_par[i].mode = (short) val;
					}
				}
				if (n==1)
					fgen_par[1].mode = fgen_par[2].mode = fgen_par[0].mode;

				PRINTF("comm: fGen mode[] = %d:%s %d:%s %d:%s\n",
						fgen_par[0].mode, fgen_mode_str[fgen_par[0].mode],
						fgen_par[1].mode, fgen_mode_str[fgen_par[1].mode],
						fgen_par[2].mode, fgen_mode_str[fgen_par[2].mode]);
			}
			/*if ((val >= 0) && (val < FGEN_NUM_MODES)) {
				fgen_par[0].mode = (short) val;
				PRINTF("comm: fGen mode=%d: %s\n", val, fgen_mode_str[val]);
				// menu_update_req(MENU_FGEN_MODE);
			}*/
			break;

		case 'F': // set frequency (3x since v.13)
		case 'f':
			{
				int i, n=0;
				char *p = ws_strtok(buf + 2, delim); //, *pmem;
				for (i = 0; i < 3; i++) {
					if (!p) break;
					if ((p = ws_strtok(0, delim))) {
						v[i] = atof(p);
						n++;
						fgen_par[i].f0 = (int)(v[i] * PARAM_NF);		// since v.13
					}
				}
				if (n == 1) {
					fgen_par[2].f0 = fgen_par[1].f0 = fgen_par[0].f0;
				}
				txt[0] = 0;
				sprintf(_db,"comm: fGen f0[] = ");
				for (i = 0; i < 3; i++) {
					ws_ftoa3((float)fgen_par[i].f0*(1.0/PARAM_NF), txt);
					strcat(_db, txt);
					strcat(_db, " ");
				}
				strcat(_db,"Hz\n");
				DB;
			}
			break;

		case 'P': // set phases (in deg), since 4.0.5
		case 'p':
			{
				int i;
				char *p = ws_strtok(buf + 2, delim); //, *pmem;
				for (i = 0; i < 3; i++) {
					if (!p) break;
					if ((p = ws_strtok(0, delim))) {
						val = atoi(p);
						if ((val >= -32768) && (val < 32767))
							fgen_par[i].phi = (short) val;
					}
				}
				PRINTF("comm: fGen phi[] = %d %d %d\n",
						fgen_par[0].phi, fgen_par[1].phi, fgen_par[2].phi);
			}
			break;

		case 'R': // since 0.97: set amplitude of added random noise
		case 'r':
			if ((val >= -32768) && (val < 32767)) {
				extern short fgen_add_rndn_ampl;
				fgen_add_rndn_ampl = (short) val;
				PRINTF("comm: fGen add_rndn_ampl=%d\n", fgen_add_rndn_ampl);
				//menu_update_req(MENU_FGEN_AMP);
			}
			break;

		default:
			;
		}
#endif  // #ifdef _USE_FGEN

	} else if ((buf[0] == 'F') || (buf[0] == 'f')) {

#ifdef _USE_FILTER
		// ------------------------------------------------------
		// Filter command	F M <value>LF
		// ------------------------------------------------------
		sub_cmd = buf[1];

		if (nbuf > 2)
			val = atoi(buf + 2); //
		// sprintf(_db, "comm: Filter mode %d\n", filter_mode); DB;

		if ((sub_cmd == 'M') || (sub_cmd == 'm')) { // set mode
			// int val = 0;

			if (nbuf > 2) val = atoi(buf + 2); //

			if ((val >= 0) && (val < FILTER_NUM_MODES)) {
				filt_par.mode = (short) val;
				PRINTF("comm: Filter mode %d:%s\n", filt_par.mode, filter_mode_str[val]);
				// do_menu(0, 0, 1);
				// menu_update_req(MENU_FILT_MODE);
			}

		} else if ((sub_cmd == 'S') || (sub_cmd == 's')) { // since 4.0.6: set complementary filter output

			filt_swap_out = ~filt_swap_out;
			PRINTF("comm: swap Filter outputs = %d\n", filt_swap_out);

		} else if ((sub_cmd == 'P') || (sub_cmd == 'p')) { // since 1.03: set filter parameter fp

			// ------------------------------------------------------
			// Filter command	F P <values>LF  (Parameters filter_mode specific)
			// ------------------------------------------------------
			// int i, nval = 0;  // up to 8 values
			int *pi;
			double *pd;
			char *p = ws_strtok(buf + 2, delim); //, *pmem;

			for (i = 0; i < 8; i++) {
				if (!p) break;
				if ((p = ws_strtok(0, delim))) {
					v[i] = atof(p); // M addr
					nval++;
				}
			}
			switch (filt_par.mode) {

			case FILTER_MODE_MULF:
				if (nval > 0) filter_mulf_b0 = v[0]; // set parameter 0
				sprintf(_db, "comm: fm%d, b0 = ", filt_par.mode);
				ws_ftoa(filter_mulf_b0, txt);
				strcat(_db, txt);
				strcat(_db, "\n");
				DB;
				break;

			case FILTER_MODE_IIR1F:
				if (nval > 0) ciir1f_skript_231.b0 = v[0]; // set parameter 0
				if (nval > 1) ciir1f_skript_231.b1 = v[1]; // set parameter 1
				if (nval > 2) ciir1f_skript_231.a1 = v[2]; // set parameter 2

				if (nval) clear_iir1f_buf();  // re-stabilize, if necessary

				sprintf(_db, "comm: fm%d, b0,b1,a1 = ", filt_par.mode);

				ws_dtoa(ciir1f_skript_231.b0, txt);
				strcat(_db, txt);
				strcat(_db, " ");
				ws_dtoa(ciir1f_skript_231.b1, txt);
				strcat(_db, txt);
				strcat(_db, " ");
				ws_dtoa(ciir1f_skript_231.a1, txt);
				strcat(_db, txt);
				strcat(_db, "\n");
				DB;
				break;

			case FILTER_MODE_IIR1I:
				if (nval > 0) ciir1i_skript_231.b0 = v[0]; // set parameter 0
				if (nval > 1) ciir1i_skript_231.b1 = v[1]; // set parameter 1
				if (nval > 2) ciir1i_skript_231.a1 = v[2]; // set parameter 2

				PRINTF("comm: fm%d, b0,b1,a1 = %d %d %d\n",
						filt_par.mode, ciir1i_skript_231.b0, ciir1i_skript_231.b1, ciir1i_skript_231.a1);
				// menu_update_req(MENU_FILT_MODE);
				break;

			case FILTER_MODE_FIR1I:
				for (i = 0; i < 6; i++) {		// length of b_skript232[]
					if (nval > i)
						b_skript232[i] = v[i]; 	// set parameter i
					else
						break;
				}

				PRINTF("comm: fm%d, b0,b1,b2,b3,b4,b5 = %d %d %d %d %d %d\n",
						filt_par.mode, b_skript232[0], b_skript232[1], b_skript232[2],
									 b_skript232[3], b_skript232[4], b_skript232[5]);
				// menu_update_req(MENU_FILT_MODE);
				break;

			case FILTER_MODE_IIR2I_LP1:
				pi = (int*)&iir2i_lp1;
				goto assig_i;

			case FILTER_MODE_IIR2I_BP1:
				pi = (int*)&iir2i_bp1;
				goto assig_i;

			case FILTER_MODE_IIR2I_BS1:
				pi = (int*)&iir2i_bs1;
				// goto assig_i;

assig_i:
				for (i = 0; i < 6; i++) {		// length of ciir2i[]
					if (nval > i)
						pi[i] = v[i]; 	// set parameter i
					else
						break;
				}

				if (nval) clear_iir2i_buf(); 	// re-stabilize, if necessary

				PRINTF("comm: fm%d, b0,b1,b2,a0,a1,a2 = %d %d %d %d %d %d\n",
						filt_par.mode, pi[0], pi[1], pi[2], pi[3], pi[4], pi[5]);
				// menu_update_req(MENU_FILT_MODE);
				break;

			case FILTER_MODE_IIR2D_LP1:
				pd = (double*)&iir2d_lp1;
				goto assig_d;

			case FILTER_MODE_IIR2D_BP1:
				pd = (double*)&iir2d_bp1;
				goto assig_d;

			case FILTER_MODE_IIR2D_BS1:
				pd = (double*)&iir2d_bs1;
				// goto assig_d;

assig_d:
				for (i = 0; i < 6; i++) {		// length of ciir2d[]
					if (nval > i)
						pd[i] = v[i]; 	// set parameter i
					else
						break;
				}

				if (nval) clear_iir2d_buf(); 	// re-stabilize, if necessary

				sprintf(_db, "comm: fm%d, b0,b1,b2,a0,a1,a2 = ", filt_par.mode);
				for (i = 0; i < 6; i++) {		// length of ciir2d[]
					ws_dtoa(pd[i], txt);
					strcat(_db, txt);
					if (i < 5)
						strcat(_db, " ");
					else
						strcat(_db, "\n");
				}
				DB;
				break;

			case FILTER_MODE_IIR2I_5STAGE:
				sprintf(_db, "comm: fm%d, section 0: b=[%d %d %d], a=[%d %d %d]\n", filt_par.mode,
					iir2i_5stage[0].b[0], iir2i_5stage[0].b[1], iir2i_5stage[0].b[2],
					iir2i_5stage[0].a[0],iir2i_5stage[0].a[1],iir2i_5stage[0].a[2]);
				for (i = 1; i < NUM_MSTAGE; i++) {
					sprintf(txt, "            section %d: b=[%d %d %d], a=[%d %d %d]\n",
						i, iir2i_5stage[i].b[0], iir2i_5stage[i].b[1], iir2i_5stage[i].b[2],
						iir2i_5stage[i].a[0],iir2i_5stage[i].a[1],iir2i_5stage[i].a[2]);
					strcat(_db, txt);
				}
				DB;
				break;

			case FILTER_MODE_AL_LP:
			case FILTER_MODE_AL_HP:
				if (nval > 0) {
					if (v[0] == ((int)v[0]))  // integer given ?
						filt_Al_alpha = LIMIT(v[0], 0, PARAM_NALPHA); // set parameter 0
					else					  // float given
						filt_Al_alpha = LIMIT(v[0], 0, 1)*PARAM_NALPHA + 0.5; // set parameter 0
				}

				ws_ftoa((float)filt_Al_alpha/PARAM_NALPHA, txt);
				PRINTF("comm: fm%d, filt_Al_alpha = %d (= %s)\n",
						filt_par.mode, filt_Al_alpha, txt);
				// menu_update_req(MENU_FILT_MODE);
				break;

			case FILTER_MODE_AL_BP:
				if (nval > 0) {
					if (v[0] == ((int)v[0]))  // integer given ?
						filt_Al_alpha2 = LIMIT(v[0], 0, PARAM_NALPHA); // set parameter 0
					else					  // float given
						filt_Al_alpha2 = LIMIT(v[0], 0, 1)*PARAM_NALPHA + 0.5; // set parameter 0
				}
				if (nval > 1) {
					if (v[1] == ((int)v[1]))  // integer given ?
						filt_Al_alpha3 = LIMIT(v[1], 0, PARAM_NALPHA); // set parameter 0
					else					  // float given
						filt_Al_alpha3 = LIMIT(v[1], 0, 1)*PARAM_NALPHA + 0.5; // set parameter 0
				}

				ws_ftoa((float)filt_Al_alpha2/PARAM_NALPHA, txt);
				sprintf(_db,"comm: fm%d, filt_Al_alpha2,3 = %d, %d (= %s, ",
						filt_par.mode, filt_Al_alpha2, filt_Al_alpha3, txt);
				ws_ftoa((float)filt_Al_alpha3/PARAM_NALPHA, txt);
				strcat(_db,txt);
				strcat(_db,")\n");
				DB;
				break;

			case FILTER_MODE_FSH:
				if (nval > 0) filt_fsh_f0 = v[0]; 							// set parameter 0
				if (nval > 1) filt_fsh_B = LIMIT(v[1], 0, fs_Hz >> 1); 		// set parameter 1

				ws_ftoa3(filt_fsh_f0, txt);									// float since v.10
				PRINTF("comm: fm%d, filt_fsh_f0, filt_fsh_B = %s, %d\n",
						filt_par.mode, txt, filt_fsh_B);
				// menu_update_req(MENU_FILT_MODE);
				break;

			case FILTER_IIR_INT:
				sprintf(_db,"comm: fm%d (%s) ",	filt_par.mode, filter_mode_str[filt_par.mode]);
				for (i = 0; i < N_IIR_INT; i++) {
					sprintf(txt,"b%d=%d ", i, b_iir_int[i]);
					strcat(_db, txt);
				}
				strcat(_db,"\n");
				for (i = 0; i < N_IIR_INT; i++)  {
					sprintf(txt,"a%d=%d ", i, a_iir_int[i]);
					strcat(_db, txt);
				}
				strcat(_db,"\n");
				DB;
				break;

			case FILTER_IIR_DBL:
				sprintf(_db, "comm: fm%d (%s) ", filt_par.mode, filter_mode_str[filt_par.mode]);
				for (i = 0; i < N_IIR_DBL; i++) {
					sprintf(txt,"b%d=", i, txt);
					strcat(_db, txt);
					ws_dtoa(b_iir_dbl[i], txt);
					strcat(_db, txt);
					strcat(_db, " ");
				}
				strcat(_db,"\n");
				for (i = 0; i < N_IIR_DBL; i++) {
					sprintf(txt,"a%d=", i, txt);
					strcat(_db, txt);
					ws_dtoa(a_iir_dbl[i], txt);
					strcat(_db, txt);
					strcat(_db, " ");
				}
				strcat(_db,"\n");
				DB;
				break;

			case FILTER_MODE_FIR2I:
				{
					// extern short xfft[];
					int n = xfft[0];
					n = LIMIT(n, 0, NFFTBUF-1);
					sprintf(_db,"comm: fm%d (%s) n=%d\n",	filt_par.mode, filter_mode_str[filt_par.mode], n);
					for (i = 0; i < n; i++) {
						sprintf(txt,"%d ", xfft[i+1]);
						strcat(_db, txt);
						if (i && !(i & 0x0f)) {
							strcat(_db,"\n");
							// DB;
							// vTaskDelay(1);
							//_db[0] = 0;
						}
					}
					strcat(_db,"\n");
					DB;
				}
				break;

			case FILTER_MODE_ALC2_LP:
			case FILTER_MODE_ALC2_HP:
			case FILTER_MODE_ALC2_BP:
			case FILTER_MODE_ALC2_BS:
				if (nval > 0) filt_Alc2LP_fg_f = v[0]; 							// set parameter 0
				// if (nval > 1) filt_fsh_B = LIMIT(v[1], 0, fs_Hz >> 1); 		// set parameter 1

				sprintf(_db,"comm: fm%d, filt_Alc2LP_fg_f = ", filt_par.mode);
				ws_ftoa3(filt_Alc2LP_fg_f, txt);
				strcat(_db, txt);
				strcat(_db, " Hz\n");
				DB;
				break;

			case FILTER_MODE_PID_FLT:
				if (nval > 0) {
					filt_pid_kp = v[0]; 	// set parameter 0
					filt_pid_init = 1;		// re-init PID
				}
				if (nval > 1) filt_pid_tn = v[1]; 		// set parameter 1
				if (nval > 2) filt_pid_tv = v[2]; 	// set parameter 2
				if (nval > 3) filt_pid_cv = v[3]; 	// set parameter 3

				ws_ftoa3(filt_pid_kp, txt);
				sprintf(_db, "comm: fm%d (%s), PID kp = %s", filt_par.mode, filter_mode_str[filt_par.mode], txt);
				sprintf(txt,", tn = ", i, txt);
				strcat(_db, txt);
				ws_ftoa3(filt_pid_tn, txt);
				strcat(_db, txt);
				sprintf(txt,", tv = ", i, txt);
				strcat(_db, txt);
				ws_ftoa3(filt_pid_tv, txt);
				strcat(_db, txt);
				sprintf(txt,", cv = ", i, txt);
				strcat(_db, txt);
				ws_ftoa3(filt_pid_cv, txt);
				strcat(_db, txt);
				strcat(_db,"\n");
				DB;
				break;

			default:
				PRINTF("comm: fm%d, no parameters defined\n",filt_par.mode);

			}
		}
#endif // #ifdef _USE_FILTER
	}

	else if ((buf[0] == 'A') || (buf[0] == 'a')) {
		// ------------------------------------------------------
		// App command	A P <value>LF
		// ------------------------------------------------------
		int val = 0;

		if (nbuf > 2)
			val = atoi(buf + 2); //

		sub_cmd = buf[1];

		// sprintf(_db, "comm: Application mode %d, sub_cmd = %c\n", filter_mode, sub_cmd);DB;
		if ((sub_cmd == 'F') || (sub_cmd == 'f')) { // set sampling frequency directly: new in 4.0
			if ((val > 0) && (val < 65536)) {
				fs_Hz = val;
				Ts_ns = (1UL << 30)/fs_Hz;	// default sampling period in ~ns
				re_init_Tick_Timer(val);
			}
			PRINTF("comm: App sampling fs = %u Hz Ts = %d~ns\n", fs_Hz, Ts_ns);

			// the real settuing is done in M0
			// sprintf(_db,
			//		"comm: App sampling precaler %d, sampling fs = %d Hz Ts = %d~us\n",
			//		fs_div, fs_Hz, Ts_us); DB;

		}  else if ((sub_cmd == 'G') || (sub_cmd == 'g')) {

			int re_init_fg1_Timer(uint16_t _fg1_Hz);
			extern unsigned short fg1_Hz;
			if ((val > 0) && (val < 65536)) {

				fg1_Hz = val;
				re_init_fg1_Timer(val);
			}
			PRINTF("comm: App fg1 = %u Hz\n", fg1_Hz);

			// ------------------------------------------------------
			// App command	A G <value>LF
			// ------------------------------------------------------

			/* handled in M0
		} else if ((sub_cmd == 'T') || (sub_cmd == 't')) { // tasks suspend/resume

			// ------------------------------------------------------
			// App command	A T <value>LF
			// ------------------------------------------------------
			if ((val >= 0) && (val < ST_NUM)) {
				xTaskHandle ht = st[val].hTask;
				char *taskname = pcTaskGetTaskName(ht);
				if ((sub_cmd == 't')) {
					// do_menu(0, 0, 1);
					vTaskSuspend(ht);
					sprintf(_db, "comm: App task '%s' suspended\n", taskname);
					DB;
				} else if ((sub_cmd == 'T')) {
					// do_menu(0, 0, 1);
					vTaskResume(ht);
					sprintf(_db, "comm: App task '%s' resumed\n", taskname);
					DB;
				}
			}

		} else if ((sub_cmd == 'D') || (sub_cmd == 'd')) {

			// ------------------------------------------------------
			// DAC mode: 	D [<value>]LF
			// ------------------------------------------------------
			int val;

			char *p = ws_strtok(buf + 2, delim); //, *pmem;

			// if (nrxbuf > 1) {
			if ((p = ws_strtok(0, delim))) {
				val = htoi(p); // M addr
			} else
				return;

			if (val >= 0)
				DAC_mode = val;
			else
				DAC_mode = 0;

			sprintf(_db, "comm: DAC mode %d\n", DAC_mode);
			DB;
			// set_comm_idle();
			// do_menu(0, 0, 1);
			// menu_update_req(MENU_DMON_MODE);

		} else if ((sub_cmd == 'M') || (sub_cmd == 'm')) {
			// ------------------------------------------------------
			// Data Monitor	M [<value>]LF
			// ------------------------------------------------------
			int val = !dmon_mode;

			char *p = ws_strtok(buf + 2, delim); //, *pmem;

			// if (nrxbuf > 1) {
			if ((p = ws_strtok(0, delim))) {
				val = htoi(p); // M addr
			}

			if (val)
				dmon_mode = 1;
			else
				dmon_mode = 0;

			sprintf(_db, "comm: data Monitor %d\n", dmon_mode);
			DB;
			// set_comm_idle();
			// do_menu(0, 0, 1);
			// menu_update_req(MENU_DMON_MODE);

		} else if ((sub_cmd == 'C') || (sub_cmd == 'c')) {
			// ------------------------------------------------------
			// app cmd	C [<value>]LF
			// ------------------------------------------------------
			extern unsigned char dia_comm_cmd;
			unsigned char val = dia_comm_cmd;

			char *p = ws_strtok(buf + 2, delim); //, *pmem;

			// if (nrxbuf > 1) {
			if ((p = ws_strtok(0, delim))) {
				val = htoi(p); // M addr
			}

			if (val <= 2)
				dia_comm_cmd = val;
			else
				dia_comm_cmd = 0;

			sprintf(_db, "comm: app cmd %d %s\n", dia_comm_cmd, buf);
			DB;
			// set_comm_idle();
			// do_menu(0, 0, 1);
			// menu_update_req(MENU_DMON_MODE);
*/
		}  else if ((sub_cmd == 'Q') || (sub_cmd == 'q')) {

#ifdef USE_QPHASE_DET
			// ------------------------------------------------------
			// print iq -detector output Q [<value>]LF
			// ------------------------------------------------------
			int val = 0;

			// char delim[] = " \t";
			char *p = ws_strtok(buf + 2, delim); //, *pmem;

			// if (nrxbuf > 1) {
			if ((p = ws_strtok(0, delim))) {
				val = htoi(p); // M addr
			}

			if (val == 4) {	// (p[0]=='f' && p[1]=='s' && p[2]=='h') {
				sprintf(_db, "comm: IQ detect 4 (FSH): %d %d\n",
						fsh.yi<<1, fsh.yq<<1); DB;
			} else if (val == 0) {
				sprintf(_db, "comm: IQ detect 0 (fgen): %d %d\n",
						iq_fgen.ycos2, iq_fgen.ysin2); DB;
			} else if (val == 1) {
				sprintf(_db, "comm: IQ detect 1 (filter): %d %d\n",
						iq_filt.ycos2, iq_filt.ysin2); DB;
			} else if (val == 2) {
				sprintf(_db, "comm: IQ detect 2 (adc_in): %d %d\n",
						iq_adc_in.ycos2, iq_adc_in.ysin2); DB;
			} else if (val == 3) {
				sprintf(_db, "comm: IQ detect 3 (adc_in_M): %d %d\n",
						iq_adc_in_M.ycos2, iq_adc_in_M.ysin2); DB;
			}
			// set_comm_idle();
			// do_menu(0, 0, 1);
			// menu_update_req(MENU_DMON_MODE);

		} else if ((sub_cmd == 'S') || (sub_cmd == 's')) {
/* handled in M0
			// ------------------------------------------------------
			// print task stack supervision  S [<value>]LF
			// ------------------------------------------------------
			extern volatile unsigned char do_print_stack;
			extern volatile unsigned char do_print_bwcet;
			int val = 1; // = !do_print_stack;

			char *p = ws_strtok(buf + 2, delim); //, *pmem;

			// if (nrxbuf > 1) {
			if ((p = ws_strtok(0, delim))) {
				val = atoi(p); // M addr
			}

			do_print_stack = do_print_bwcet = val;
			PRINTF("comm: Print stack(%d)/bwcet(%d) supervision\n",
					do_print_stack, do_print_bwcet);

			/*if (val & 1)
				do_print_bwcet = 1;
			if (val & 2) {
				do_print_stack = 1;
				//update_super_stack();
				//print_super_stack();
			}* /
			// set_comm_idle();
			// do_menu(0, 0, 1);
			// menu_update_req(MENU_DMON_MODE);

		} else if ((sub_cmd == 'B') || (sub_cmd == 'b')) {
			// ------------------------------------------------------
			// Buzzer on/off. B [<value>]LF
			// ------------------------------------------------------
			int val = 0;

			char *p = ws_strtok(buf + 2, delim); //, *pmem;

			// if (nrxbuf > 1) {
			if ((p = ws_strtok(0, delim))) {
				val = htoi(p); // M addr
			}
			//set_pwm_buzzer(val);

			sprintf(_db, "comm: set Buzzer %d\n", val); DB;
			// set_comm_idle();
			// do_menu(0, 0, 1);
			// menu_update_req(MENU_DMON_MODE);

		} else if ((sub_cmd == 'O') || (sub_cmd == 'o')) {
			// ------------------------------------------------------
			// set DAC Offset
			// ------------------------------------------------------
			if ((val >= -32768) && (val < 32767)) {
				extern short DAC_out_off;

				DAC_out_off = (short) val;
				//set_comm_idle();
				// do_menu(0, 0, 1);
				// menu_update_req(MENU_FGEN_OFF);
			}
			sprintf(_db, "comm: DAC_out_off=%d\n", DAC_out_off); DB;
*/
#endif // #ifdef USE_QPHASE_DET

		} else if ((sub_cmd == 'I') || (sub_cmd == 'i')) {
			// ------------------------------------------------------
			// Info. I [<value>]LF
			// ------------------------------------------------------
			//int val = 0;
			// extern short nQmax;
			// char *p = ws_strtok(buf + 2, delim); //, *pmem;

			// if (nrxbuf > 1) {
			//if ((p = ws_strtok(0, delim))) {
			//	val = htoi(p); // M addr
			//}
			//set_pwm_buzzer(val);

			// sprintf(_db, "comm: %s (nQmax=%d)\n", IDstr, nQmax); DB;
			PRINTF("comm: %s \n", IDstr);
			// _DBG(ABOUT1 "\n");
			//_DBG(ABOUT2 " " __DATE__ "\n\n");
			// set_comm_idle();
			// do_menu(0, 0, 1);
			// menu_update_req(MENU_DMON_MODE);

		} else if ((sub_cmd == 'P') || (sub_cmd == 'p')) { // PID Control parameters

#ifdef USE_PID_CONTROL
			// ------------------------------------------------------
			// Control Parameter command	A P <values>LF  (Parameters pid_mode specific)
			// ------------------------------------------------------
			// int i, nval = 0;  // up to 8 values
			int *pi;
			double *pd;
			char *p = ws_strtok(buf + 2, delim); //, *pmem;

			for (i = 0; i < 8; i++) {
				if (!p) break;
				if ((p = ws_strtok(0, delim))) {
					v[i] = atof(p); // M addr
					nval++;
				}
			}
			if (nval > 0)
				if ((v[0] >= 0) && (v[0] < NUM_PID_MODES))
					filt_pid_mode = v[0]; 		// set parameter
			if (nval > 1) {
				filt_pid_kp = v[1]; 	// set parameter 0
				filt_pid_init = 1;		// re-init PID
			}
			if (nval > 2) filt_pid_tn = v[2]; 	// set parameters
			if (nval > 3) filt_pid_tv = v[3];
			if (nval > 4) filt_pid_cv = v[4];

			ws_ftoa3(filt_pid_kp, txt);
			sprintf(_db, "comm: PID mode %d(%s), PID kp = %s", filt_pid_mode, pid_mode_str[filt_pid_mode], txt);
			sprintf(txt,", tn = ", i, txt);
			strcat(_db, txt);
			ws_ftoa3(filt_pid_tn, txt);
			strcat(_db, txt);
			sprintf(txt,", tv = ", i, txt);
			strcat(_db, txt);
			ws_ftoa3(filt_pid_tv, txt);
			strcat(_db, txt);
			sprintf(txt,", cv = ", i, txt);
			strcat(_db, txt);
			ws_ftoa3(filt_pid_cv, txt);
			strcat(_db, txt);
			strcat(_db,"\n");
			DB;

		} else if ((sub_cmd == 'L') || (sub_cmd == 'l')) { // since 4.0.9: set LMS stuff

			// ------------------------------------------------------
			// correlator command A L <values>LF  (Parameters lms_mode specific)
			// ------------------------------------------------------
			// int i, nval = 0;  // up to 8 values
			// int *pi;
			// double *pd;
			char *p = ws_strtok(buf + 2, delim); //, *pmem;

			for (i = 0; i < 4; i++) {
				if (!p) break;
				if ((p = ws_strtok(0, delim))) {
					vi[i] = atoi(p); // M addr
					nval++;
				}
			}

			if (nval >= 1) {
				//filt_lms_mode = LIMIT(vi[0], 0, (NUM_CORR_MODES-1));
			}

			//if (nval >= 2) {
			//	corr_delay = LIMIT(vi[1], 0, (NCORRBUF-1));
			//}

			if (nval >= 3) {
				corr_fg = LIMIT(vi[2], 1, (fs_Hz >> 1));
			}

			//sprintf(_db, "comm: [LMS_mode mu_rel] = %d(%s) %d %d -- E{w} = %d\n",
			//		filt_lms_mode, filt_lms_mode_str[filt_lms_mode], filt_lms_mu_rel, filt_lms_mode); DB;


		} else if ((sub_cmd == 'X') || (sub_cmd == 'x')) { // since 1.04: set correlator

			// ------------------------------------------------------
			// correlator command A X <values>LF  (Parameters filter_mode specific)
			// ------------------------------------------------------
			// int i, nval = 0;  // up to 8 values
			// int *pi;
			// double *pd;
			char *p = ws_strtok(buf + 2, delim); //, *pmem;

			for (i = 0; i < 4; i++) {
				if (!p) break;
				if ((p = ws_strtok(0, delim))) {
					vi[i] = atoi(p); // M addr
					nval++;
				}
			}

			if (nval >= 1) {
				corr_mode = LIMIT(vi[0], 0, (NUM_CORR_MODES-1));
			}

			if (nval >= 2) {
				corr_delay = LIMIT(vi[1], 0, (NCORRBUF-1));
			}

			if (nval >= 3) {
				corr_fg = LIMIT(vi[2], 1, (fs_Hz >> 1));
			}
			//sprintf(_db, "comm: corr mode = %d, delay = %d, fg = %d Hz\n",
			//		corr_mode, corr_delay, corr_fg);  DB;
			vTaskDelay(500/portTICK_RATE_MS);		// wait 0.5 s
			sprintf(_db, "comm: [corr_mode delay fg] = %d(%s) %d %d -- E{w} = %d\n",
					corr_mode, corr_mode_str[corr_mode], corr_delay,corr_fg,corr.w2); DB;

#endif // #ifdef USE_PID_CONTROL
		}

	}

	/*else if ((buf[0] == 'C') || (buf[0] == 'c')) {
		// ------------------------------------------------------
		// CWR Msg (remote) ID <data>LF    	send a CAN message (remote)
		// CW  Msg ID <data> LF  			send a CAN message
		// ------------------------------------------------------
		message_object msg = {0,0,0, 0,0,0,0,0,0,0,0};
		int val = 0;
		char subsub_cmd, bremote_flag = 0;
		char *p = ws_strtok(buf + 3, delim);
		void can_send(message_object *pmsg, char remote_flag);

		//if (nbuf > 2)
			//val = htoi(buf + 2); //

		sub_cmd = buf[1];
		subsub_cmd = buf[2];

		if (p) {
			msg.id = htoi(p);	// message ID
		} else
			return;

		for (i=0; i < 8; i++) {
			if (!(p = ws_strtok(0, delim))) break;
			msg.data[i] = htoi(p);
		}
		msg.dlc = i;			// data length
		if ((subsub_cmd == 'R') || (subsub_cmd == 'r'))
			bremote_flag = 1;

		PRINTF("CAN msg(remote=%d) ID=0x%X len=%d: %X %X %X %X %X %X %X %X\n",
			bremote_flag, msg.id, msg.dlc,
			msg.data[0], msg.data[1], msg.data[2], msg.data[3],
			msg.data[4], msg.data[5], msg.data[6], msg.data[7]);
		can_send(&msg, bremote_flag);
	}*/

	else if ((buf[0] == 'D') || (buf[0] == 'd')) {
		// ------------------------------------------------------
		// Data Monitor DR <ind>LF 					   read data monitor pointer an length n
		// Data Monitor DW <addr>  <ind>  <n bytes>LF  write data monitor pointer[ind] -> addr, len[ind] = n
		// ------------------------------------------------------
		unsigned int addr = 0, n = 1, access_mode = 0;

		char *p = ws_strtok(buf + 2, delim), *pmem_c;
		unsigned short ind, len;

		if ((p = ws_strtok(0, delim))) {

			if ((buf[1] == 'R') || (buf[1] == 'r')) {
/*
				ind = atoi(p); // M addr
				ind = MIN(ind, DATAMONITOR_ADDRESS_NUM_K-1);
				// _DBG(p);

				sprintf(_db, "comm: %c dmon_ptr[%d]=0x%X, len=%u\n", 'R', ind, dmon_ptr[ind], dmon_len[ind]);
				DB;
*/
			} else if ((buf[1] == 'W') || (buf[1] == 'w')) {
/*
				//if ((p = ws_strtok(0, delim))) {
					addr = get_addr(p);
					if (addr == 0)	return;
				//}

				if ((p = ws_strtok(0, delim))) {
					ind = atoi(p); // M addr
					ind = MIN(ind, DATAMONITOR_ADDRESS_NUM_K-1);
					// _DBG(p);
				}

				len = dmon_len[ind];
				if ((p = ws_strtok(0, delim))) {
					len = atoi(p);
					len = MIN(len, 32);
				}
				dmon_ptr[ind] = addr;
				dmon_len[ind] = len;
				sprintf(_db, "comm: %c dmon_ptr[%d]=0x%X, len=%u\n", 'W', ind, dmon_ptr[ind], dmon_len[ind]);
				DB;
*/
			}

		} else
			return;
	}
/*
	else if ((buf[0] == 'R') || (buf[0] == 'r')) {
		// ------------------------------------------------------
		// Read Memory	R <addr in hex> [n bytes]LF
		// ------------------------------------------------------
		unsigned int addr = 0, n = 1, access_mode = 0;

		char *p = ws_strtok(buf + 2, delim), *pmem_c;
		short *pmem_s;
		int *pmem_i;
		float *pmem_f;
		double *pmem_d;
		unsigned int *pp;

		if ((p = ws_strtok(0, delim))) {
			// addr = htoi(p); // M addr
			// _DBG(p);
			if (*p == '*') {  	// pointer mode ?
				pp = get_addr(p+1);	// address to the pointer
				if (pp == 0)
					return;
				addr = *pp;    		// the address to read from
			} else
				addr = get_addr(p);
			if (addr == 0)
				return;
		} else
			return;

		if ((buf[1] == 'D') || (buf[1] == 'd')) {
			access_mode = 4; // double mode
			pmem_d = (double *) addr;
		} else if ((buf[1] == 'F') || (buf[1] == 'f')) {
			access_mode = 3; // float mode
			pmem_f = (float *) addr;
		} else if ((buf[1] == 'I') || (buf[1] == 'i')) {
			access_mode = 2; // int mode
			pmem_i = (int *) addr;
		} else if ((buf[1] == 'S') || (buf[1] == 's')) {
			access_mode = 1; // short mode
			pmem_s = (short *) addr;
		} else
			pmem_c = (char *) addr; //	byte mode = default

		if ((p = ws_strtok(0, " \t")))
			n = atoi(p);

		sprintf(_db, "comm: Read(mode %c) 0x%X, n=%u: ", buf[1], addr, n);
		DB;
		while (n-- > 0) {
			if (access_mode == 4) {
				ws_dtoa(*pmem_d++, txt);
				sprintf(_db, "%s ", txt); // sprintf doesnt have %f
			} else if (access_mode == 3) {
				ws_ftoa(*pmem_f++, txt);
				sprintf(_db, "%s ", txt);
			} else if (access_mode == 2)
				sprintf(_db, "%d ", *pmem_i++);
			else if (access_mode == 1)
				sprintf(_db, "%d ", *pmem_s++);
			else
				sprintf(_db, "%02X ", *pmem_c++);
			DB;
		}
		DEBUGSTR("\n"); //_DBC('\n');
	}

	else if ((buf[0] == 'W') || (buf[0] == 'w')) {
		// ------------------------------------------------------
		// Write Memory	W <addr in hex> <data>LF
		// ------------------------------------------------------
		unsigned int addr = 0, n = 0, access_mode = 0;
		// int *pmem;
		char c;
		char *p = ws_strtok(buf + 2, delim), *pmem_c;
		short *pmem_s;
		int *pmem_i;
		float *pmem_f;
		double *pmem_d;
		unsigned int *pp;

		if ((p = ws_strtok(0, delim))) {
			// addr = htoi(p); // M addr
			if (*p == '*') {  	// pointer mode ?
				pp = get_addr(p+1);	// address to the pointer
				if (pp == 0)
					return;
				addr = *pp;    		// the address to write to
			} else
				addr = get_addr(p);
			if (addr == 0)
				return;
			// pmem = (unsigned char *)addr;
		} else
			return;

		if ((buf[1] == 'D') || (buf[1] == 'd')) {
			access_mode = 4; // double mode
			pmem_d = (double *) addr;
		} else if ((buf[1] == 'F') || (buf[1] == 'f')) {
			access_mode = 3; // float mode
			pmem_f = (float *) addr;
		} else if ((buf[1] == 'I') || (buf[1] == 'i')) {
			access_mode = 2; // int mode
			pmem_i = (int *) addr;
		} else if ((buf[1] == 'S') || (buf[1] == 's')) {
			access_mode = 1; // short mode
			pmem_s = (short *) addr;
		} else
			pmem_c = (char *) addr; //	byte mode = default

		while ((p = ws_strtok(0, " \t"))) {

			if (n++ == 0) { // first time
				sprintf(_db, "comm: Write(mode=%c) 0x%X = ", buf[1], addr);
				DB;
			}
			if (access_mode == 4) { // double mode
				*pmem_d++ = (double)atof(p); //strtod(p, 0); 	//
				// ws_dtoa(*pmem_d++, txt);
				//sprintf(_db, "%s ", txt);
			} else if (access_mode == 3) { // float mode
				*pmem_f++ = (float) atof(p); //strtod(p, 0); 	//
				//ftoa(*pmem_f++, txt);
				// sprintf(_db, "%s ", txt);
			} else if (access_mode == 2) { // int mode
				*pmem_i++ = (int) atoi(p);
				;
				// sprintf(_db, "%d ", *pmem_i++);
			} else if (access_mode == 1) { // short mode
				*pmem_s++ = (short) atoi(p);
				;
				//sprintf(_db, "%d ", *pmem_s++);
			} else { // char mode
				c = (char) atoi(p);
				*pmem_c++ = c;
				// sprintf(_db, "%02X ", *pmem_c++);
			}
			//DB;
			DEBUGSTR("\n"); //__DBC('\n');
		}
		sprintf(_db, "... %d items.\n", n); DB;
	}*/
	comm_analyze_command_working = 0;
}

// ----------------------------------------------------------------------------
// ws: comm function, processing a new char
// ----------------------------------------------------------------------------
void comm_newchar(unsigned char c) {
	// -----------------------------------------------
	// Check for new comm object
	// -----------------------------------------------
	/* if (!comm_obj_state) { // we're in idle state

	 nrxbuf = 0; // rxbuf write index
	 comm_obj_state = 1; // receiving now

	 } else */if (c == LF) {

		_rxbuf[_nrxbuf] = 0; // store a 0 terminator byte

		// while (comm_analyze_command_working);	// wait
		xSemaphoreGive(semDIAC);
		// comm_analyze_command(_rxbuf, _nrxbuf);  --> moved to vdia_commTask
		// sprintf(_db, "\ncomm cmd:%s\n", rxbuf); DB;

		/*comm_obj_state = 0;
		_rxbuf[_nrxbuf] = 0; // ready for next line
		_nrxbuf = 0;*/

	} else {
		if (_nrxbuf < (sizeof(_rxbuf) - 3)) {
			_rxbuf[_nrxbuf++] = c; // store message byte
			_rxbuf[_nrxbuf] = 0; // 0 terminator
		} else {
			sprintf(_db, "\ncomm buffer overflow !\n"); //, rxbuf);
			DB;
			_rxbuf[_nrxbuf] = 0; // ready for next line
			_nrxbuf = 0;
		}
	}
}

// ----------------------------------------------------------------------------
void vdia_commTask(void * pvParameters)
{
	uint8_t  tmpc;
	uint32_t rLen;

	// BWCET_INIT(ST_DIA4);				// <---- wcet init
	vSemaphoreCreateBinary(semDIAC);

	while(1){

		xSemaphoreTake(semDIAC, portMAX_DELAY);	// <---- sync/mutex for diac comm processing

		if (pst && !pst[ST_DIA4].bwcet.binit) BWCET_INIT(ST_DIA4);			// <---- wcet init (IPC)

		BWCET_T1(ST_DIA4);					// <---- wcet measure
		if (_rxbuf[0])							// the UART buffer
			comm4_analyze_command(_rxbuf, _nrxbuf);
		BWCET_T2(ST_DIA4);					// ----> wcet measure

		comm_obj_state = 0;
		_rxbuf[_nrxbuf] = 0; // ready for next line
		_nrxbuf = 0;
	}
}

