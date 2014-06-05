typedef enum{	/*AN246MPM, pdf P3*/
Reserved=0,
EX_Mode,
MUSIC_Mode,
MOVIE_Mode,
}PLIIx_Multi_chan_Decode_Mode;

extern void DSP_PLIIx_Control(unsigned char enable);
extern void DSP_PLIIx_DECODE_MODE_STEREO(unsigned char mode);
extern void DSP_PLIIx_CNOTROL_REGISTER(unsigned char Panorama_Enable);
extern void DSP_PLIIx_CENTER_WIDTH_CONFIGURATION(unsigned char Width);
extern void DSP_PLIIx_DIMENSION_CONFIGURATION(unsigned char Dimension);
extern void DSP_PLIIx_DECODE_MODE_MULTI(unsigned char mode);

