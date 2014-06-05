#include "macro.h"
#include "debug.h"
#include "task.h"
#include "c11_msg_handler.h"
#include "queue.h"

#include "..\driver\timer.h"
#include "..\cs495300\Dspdrv.h"
//#include "..\cs49403\dsp_protocol.h"

#include <string.h>

#ifdef _USE_DSP_PROTOCOL
unsigned char data_buffer[PROTOCOL_MAX_SIZE];	/*用来作为临时数组，向DSP_TASK发送数据用*/

unsigned char PutMessage(unsigned char *buffer,unsigned char size)
{
	DspProcMsg(buffer);
	return 0;
}

static void StandardMsgHeader(unsigned char *buffer, unsigned char cmd_type, unsigned char size)
{
	buffer[0] = cmd_type;
	buffer[1] = size;
}
#endif

unsigned char SetDspPower(unsigned char mode)
{
#ifdef _USE_DSP_PROTOCOL
	unsigned char i = 2;

	data_buffer[i++] = mode;

	StandardMsgHeader(data_buffer,CMD_POWER,i);

	return PutMessage(data_buffer,i);
#else
	DspProcPower(mode);   
	return TRUE;
#endif
}

unsigned char SetDspDelay(unsigned char channel,unsigned short delay_time)
{
#ifdef _USE_DSP_PROTOCOL
	unsigned char i = 2;

	data_buffer[i++] = channel;
	data_buffer[i++] = (delay_time >> 8) & 0xFF;
	data_buffer[i++] = delay_time & 0xFF;

	StandardMsgHeader(data_buffer,CMD_DELAY,i);

	return PutMessage(data_buffer,i);
#else
	DspProcDelay(channel,delay_time);
	return TRUE;
#endif
}

unsigned char SetDspCorner(unsigned char channel,unsigned char corner_value)
{
#ifdef _USE_DSP_PROTOCOL
	unsigned char i = 2;

	data_buffer[i++] = channel;
	data_buffer[i++] = corner_value;

	StandardMsgHeader(data_buffer,CMD_CORNER,i);

	return PutMessage(data_buffer,i);
#else
	DspProcCorner(channel,corner_value);
	return TRUE;
#endif
}

unsigned char SetDspSpkMode(unsigned char channel,unsigned char mode)
{
#ifdef _USE_DSP_PROTOCOL
	unsigned char i = 2;

	data_buffer[i++] = channel;
	data_buffer[i++] = mode;

	StandardMsgHeader(data_buffer,CMD_SPK_MODE,i);

	return PutMessage(data_buffer,i);
#else
	DspProcSpkMode(channel,mode);
	return TRUE;
#endif
}

unsigned char SetDspSignal(unsigned char signal_type, unsigned char channel, unsigned char freq)
{
#ifdef _USE_DSP_PROTOCOL
	unsigned char i = 2;

	data_buffer[i++] = signal_type;
	data_buffer[i++] = channel;
	data_buffer[i++] = freq;

	StandardMsgHeader(data_buffer,CMD_SIGNAL,i);

	return PutMessage(data_buffer,i);	
#else
	DspProcSignal(signal_type,channel,freq);
	return TRUE;
#endif
}

unsigned char SetDspIrc(unsigned char irc_mode,unsigned char spk_channel)
{
#ifdef _USE_DSP_PROTOCOL
	unsigned char i = 2;

	data_buffer[i++] = irc_mode;
	data_buffer[i++] = spk_channel;

	StandardMsgHeader(data_buffer,CMD_IRC,i);

	return PutMessage(data_buffer,i);
#else
	DspProcIrc( irc_mode, spk_channel);
	return TRUE;
#endif
}

