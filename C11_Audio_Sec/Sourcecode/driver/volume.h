#ifndef _VOLUME_H
#define _VOLUME_H

extern void VolumeInit(void);
extern void SetZone2Volume(float volume);
extern void Set2ChVolume(float volume);
extern void Set7Ch1MainVolume(short volume);
extern void SetBalanceVolume(unsigned char channel, float volume);
extern void AutoSoundSetVolume(short volume);	/*auto sound测试的时候，必须将平衡全部设置为0，并且将main volume设置为-20 dB,用此函数实现*/
extern void SoftMute(uchar on);
#endif

