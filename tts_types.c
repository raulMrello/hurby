/*************************************************************************************
		This file includes TTS common type declarations. 
 *************************************************************************************/

#include "tts_types.h"


/** 
 * TTS_1 : "A_ahhh jeje tasustado"
 * seconds = 4.3/11
 */
static const float tts_1_percent[] = {0.2, 1.0, 0.4, 0.7, 0.9, 1.0, 0.6, 1.0, 0.6, 0.9, 0.0};
static const float tts_1_period[]  = {0.8, 0.4, 0.7, 0.3, 0.2, 0.3, 0.3, 0.3, 0.2, 0.3, 0.4};

/** 
 * TTS_2 : "A_Hola mira q tengo!" 
 * seconds = 3.9/14
 */
static const float tts_2_percent[] = {0.2, 1.0, 0.7, 1.0, 0.7, 0.9, 0.3, 0.8, 0.9, 0.4, 1.0, 0.6, 1.0, 0.0};
static const float tts_2_period[]  = {0.5, 0.3, 0.2, 0.2, 0.3, 0.1, 0.2, 0.2, 0.2, 0.3, 0.2, 0.3, 0.2, 0.4};

/** 
 * TTS_3 : "A_Hola pongo voz sara!" 
 * seconds = 7.0/26
 */
static const float tts_3_percent[] = {0.2, 1.0, 0.2, 0.7, 0.5, 0.8, 0.3, 0.9, 0.2, 0.0, 0.7, 1.0, 0.2, 0.0, 1.0, 1.0, 0.0, 0.0, 0.8, 0.2, 0.6, 0.3, 0.7, 0.4, 0.8, 0.0};
static const float tts_3_period[]  = {0.6, 0.4, 0.3, 0.2, 0.3, 0.2, 0.2, 0.3, 0.3, 0.8, 0.3, 0.3, 0.5, 0.3, 0.2, 0.3, 0.3, 0.1, 0.2, 0.2, 0.1, 0.2, 0.1, 0.1, 0.1, 0.4};

/** 
 * TTS_4 : "A_intruso llamando policia" 
 * seconds = 7.0/25
 */
static const float tts_4_percent[] = {0.2, 1.0, 0.0, 0.0, 0.4, 0.2, 0.7, 0.4, 0.2, 0.8, 0.4, 0.1, 0.8, 0.4, 0.8, 0.5, 0.8, 0.1, 0.0, 0.5, 0.3, 0.5, 0.3, 0.5, 0.0};
static const float tts_4_period[]  = {0.6, 0.2, 0.2, 0.3, 0.2, 0.1, 0.3, 0.3, 0.1, 0.2, 0.2, 0.2, 0.3, 0.1, 0.1, 0.3, 0.2, 0.2, 0.4, 0.3, 0.4, 0.6, 0.3, 0.3, 0.3};

/** 
 * TTS_5 : "A_mira culo prrrr" 
 * seconds = 7.8/27
 */
static const float tts_5_percent[] = {0.2, 0.5, 0.3, 0.9, 0.2, 0.8, 0.3, 0.6, 0.3, 0.7, 0.0, 0.2, 0.8, 0.2, 0.2, 0.9, 0.1, 0.1, 1.0, 0.2, 0.2, 0.7, 0.2, 0.6, 0.3, 0.6, 0.0};
static const float tts_5_period[]  = {0.7, 0.2, 0.3, 0.2, 0.3, 0.2, 0.4, 0.3, 0.3, 0.4, 0.3, 0.5, 0.1, 0.5, 0.3, 0.2, 0.3, 0.9, 0.2, 0.2, 0.4, 0.2, 0.2, 0.2, 0.1, 0.1, 0.2};

/** 
 * TTS_6 : "A_nenas venir paca jugar" 
 * seconds = 5.0/18
 */
static const float tts_6_percent[] = {0.2, 0.9, 0.5, 1.0, 0.8, 0.6, 0.2, 0.5, 0.1, 0.5, 0.9, 0.8, 0.3, 0.6, 0.3, 1.0, 1.0, 0.0};
static const float tts_6_period[]  = {0.5, 0.4, 0.1, 0.2, 0.2, 0.2, 0.2, 0.2, 0.3, 0.2, 0.2, 0.3, 0.3, 0.2, 0.3, 0.3, 0.2, 0.6};

/** 
 * TTS_7 : "A_pano panojete" 
 * seconds = 7.0/11
 */
static const float tts_7_percent[] = {0.0, 0.4, 1.0, 0.7, 0.5, 0.8, 0.1, 0.9, 0.0, 0.3, 0.0};
static const float tts_7_period[]  = {0.6, 0.4, 0.5, 0.5, 0.5, 0.5, 0.8, 0.9, 1.0, 0.4, 0.5};

/** 
 * TTS_8 : "A_si subes arriba oído voces" 
 * seconds = 4.2/10
 */
static const float tts_8_percent[] = {0.3, 0.8, 1.0, 0.4, 0.6, 0.3, 0.7, 0.4, 0.7, 0.0};
static const float tts_8_period[]  = {0.8, 0.4, 0.3, 0.4, 0.3, 0.3, 0.5, 0.3, 0.3, 0.4};

/** 
 * TTS_9 : "A_uh te asustao" 
 * seconds = 3.9/13
 */
static const float tts_9_percent[] = {0.0, 1.0, 0.0, 0.0, 1.0, 0.5, 1.0, 0.1, 0.7, 0.3, 0.7, 0.5, 0.0};
static const float tts_9_period[]  = {0.7, 0.3, 0.3, 0.4, 0.3, 0.2, 0.3, 0.2, 0.3, 0.2, 0.1, 0.2, 0.3};

/** 
 * TTS_10 : "A_ven paca" 
 * seconds = 4.7/12
 */
static const float tts_10_percent[] = {0.0, 0.5, 0.2, 0.6, 1.0, 0.8, 0.8, 0.3, 0.7, 0.5, 0.1, 0.0};
static const float tts_10_period[]  = {0.7, 0.3, 0.4, 0.3, 0.2, 0.3, 0.2, 0.2 ,0.2, 0.4, 0.3, 0.1};

/** 
 * TTS_11 : "B_chiste cabeza pies" 
 * seconds = 7.0/11
 */
static const float tts_11_percent[] = {0.0, 0.4, 1.0, 0.7, 0.5, 0.8, 0.1, 0.9, 0.0, 0.3, 0.0};
static const float tts_11_period[]  = {0.6, 0.4, 0.5, 0.5, 0.5, 0.5, 0.8, 0.9, 1.0, 0.4, 0.5};

/** 
 * TTS_12 : "B_de las orejas nistocurra" 
 * seconds = 5.0/12
 */
static const float tts_12_percent[] = {0.0, 1.0, 0.6, 0.2, 0.4, 0.1, 0.5, 0.7, 0.1, 0.1, 0.7, 0.0};
static const float tts_12_period[]  = {0.5, 0.9, 0.4, 0.4, 0.4, 0.5, 0.4, 0.5, 0.4, 0.2, 0.3, 0.6};

/** 
 * TTS_13 : "B_jeje cosquillas" 
 * seconds = 5.0/12 (a partir de tts12)
 */
static const float tts_13_percent[] = {0.0, 1.0, 0.6, 0.2, 0.4, 0.1, 0.5, 0.7, 0.1, 0.1, 0.7, 0.0};
static const float tts_13_period[]  = {0.5, 0.9, 0.4, 0.4, 0.4, 0.5, 0.4, 0.5, 0.4, 0.2, 0.3, 0.6};

/** 
 * TTS_14 : "B_si sigues acariciando tosobado" 
 * seconds = 4.2/10 (a partir de tts8)
 */
static const float tts_14_percent[] = {0.3, 0.8, 1.0, 0.4, 0.6, 0.3, 0.7, 0.4, 0.7, 0.0};
static const float tts_14_period[]  = {0.8, 0.4, 0.3, 0.4, 0.3, 0.3, 0.5, 0.3, 0.3, 0.4};

/** 
 * TTS_15 : "B_toca cabecita qesta suave" 
 * seconds = 4.3/11 (a partir de tts1)
 */
static const float tts_15_percent[] = {0.2, 1.0, 0.4, 0.7, 0.9, 1.0, 0.6, 1.0, 0.6, 0.9, 0.0};
static const float tts_15_period[]  = {0.8, 0.4, 0.7, 0.3, 0.2, 0.3, 0.3, 0.3, 0.2, 0.3, 0.4};

/** 
 * TTS_16 : "C_suelta suelta" 
 * seconds = 5.0/12 (a partir de tts12)
 */
static const float tts_16_percent[] = {0.0, 1.0, 0.6, 0.2, 0.4, 0.1, 0.5, 0.7, 0.1, 0.1, 0.7, 0.0};
static const float tts_16_period[]  = {0.5, 0.9, 0.4, 0.4, 0.4, 0.5, 0.4, 0.5, 0.4, 0.2, 0.3, 0.6};

/** 
 * TTS_17 : "C_sueltahora mismo" 
 * seconds = 4.3/11 (a partir de tts1)
 */
static const float tts_17_percent[] = {0.2, 1.0, 0.4, 0.7, 0.9, 1.0, 0.6, 1.0, 0.6, 0.9, 0.0};
static const float tts_17_period[]  = {0.8, 0.4, 0.7, 0.3, 0.2, 0.3, 0.3, 0.3, 0.2, 0.3, 0.4};

/** 
 * TTS_18 : "C_tu loca segura" 
 * seconds = 3.7/13 (a partir de tts2)
 */
static const float tts_18_percent[] = {0.2, 1.0, 0.7, 1.0, 0.7, 0.9, 0.3, 0.8, 0.9, 0.4, 1.0, 0.6, 0.0};
static const float tts_18_period[]  = {0.5, 0.3, 0.2, 0.2, 0.3, 0.1, 0.2, 0.2, 0.2, 0.3, 0.2, 0.3, 0.4};




// static voice/sound entries. Each has an associated id and duration. According with
// their id, all can be reproduced by the VR module.
static const TTS_FLASH tts_0 = {0, 0, 0, 0, 0};
static const TTS_FLASH tts_1 = {1, 4.3, (float*)tts_1_percent, (float*)tts_1_period, 11};
static const TTS_FLASH tts_2 = {2, 3.9, (float*)tts_2_percent, (float*)tts_2_period, 14};
static const TTS_FLASH tts_3 = {3, 7.0, (float*)tts_3_percent, (float*)tts_3_period, 26};
static const TTS_FLASH tts_4 = {4, 7.0, (float*)tts_4_percent, (float*)tts_4_period, 25};
static const TTS_FLASH tts_5 = {5, 7.8, (float*)tts_5_percent, (float*)tts_5_period, 27};
static const TTS_FLASH tts_6 = {6, 5.0, (float*)tts_6_percent, (float*)tts_6_period, 18};
static const TTS_FLASH tts_7 = {7, 7.0, (float*)tts_7_percent, (float*)tts_7_period, 11};
static const TTS_FLASH tts_8 = {8, 4.2, (float*)tts_8_percent, (float*)tts_8_period, 10};
static const TTS_FLASH tts_9 = {9, 3.9, (float*)tts_9_percent, (float*)tts_9_period, 13};
static const TTS_FLASH tts_10 = {10, 4.7, (float*)tts_10_percent, (float*)tts_10_period, 12};
static const TTS_FLASH tts_11 = {11, 7.0, (float*)tts_11_percent, (float*)tts_11_period, 11};
static const TTS_FLASH tts_12 = {12, 5.0, (float*)tts_12_percent, (float*)tts_12_period, 12};
static const TTS_FLASH tts_13 = {13, 5.0, (float*)tts_13_percent, (float*)tts_13_period, 12};
static const TTS_FLASH tts_14 = {14, 4.2, (float*)tts_14_percent, (float*)tts_14_period, 10};
static const TTS_FLASH tts_15 = {15, 4.3, (float*)tts_15_percent, (float*)tts_15_period, 11};
static const TTS_FLASH tts_16 = {16, 5.0, (float*)tts_16_percent, (float*)tts_16_period, 12};
static const TTS_FLASH tts_17 = {17, 4.3, (float*)tts_17_percent, (float*)tts_17_period, 11};
static const TTS_FLASH tts_18 = {18, 3.7, (float*)tts_18_percent, (float*)tts_18_period, 13};

// TTS array incluiding all compiled phrases.
volatile const TTS_FLASH *tts_list[19] = {&tts_0, &tts_1, &tts_2, &tts_3, &tts_4, &tts_5, &tts_6, &tts_7, &tts_8, &tts_9,&tts_10,&tts_11,&tts_12,&tts_13,&tts_14,&tts_15,&tts_16,&tts_17,&tts_18};

/** Random generator *****************************************************************/
static unsigned int m_z=12434,m_w=33254;
double rnd(float min, float max) {
	unsigned int result;
	double res;
    m_z = 36969 * (m_z & 65535) + (m_z >>16);
    m_w = 18000 * (m_w & 65535) + (m_w >>16);
	result = ((m_z <<16) + m_w)%1000;
	res = (double)((((double)(max-min)*result)/1000) + ((double)min));
    return (res);
}

