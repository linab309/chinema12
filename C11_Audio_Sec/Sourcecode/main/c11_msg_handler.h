#ifndef C11_MSG_HANDLER_H
#define C11_MSG_HANDLER_H

extern unsigned char SetDspPower(unsigned char mode);
extern unsigned char SetDspDelay(unsigned char channel,unsigned short delay_time);
extern unsigned char SetDspCorner(unsigned char channel,unsigned char corner_value);
extern unsigned char SetDspSpkMode(unsigned char channel,unsigned char mode);
extern unsigned char SetDspIrc(unsigned char irc_mode,unsigned char spk_channel);
extern unsigned char SetDspSignal(unsigned char signal_type, unsigned char channel, unsigned char freq);
#endif

