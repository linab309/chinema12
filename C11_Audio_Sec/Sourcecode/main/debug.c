#include "macro.h"
#include "..\driver\uart.h"

#include "message.h"
#include "debug.h"
#include "task.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>



int debug_printf(const char *format,...)
{
	char temp_buffer[128];
	int length = 0,i = 0;

	va_list arg_ptr;

	va_start (arg_ptr, format);           /* format string */
	vsprintf (temp_buffer, format, arg_ptr);
	length = strlen(temp_buffer);

	for(i=0;i<length;i++)
		{
//		tsk_lock();
		Uart0Send(temp_buffer[i]);
//		tsk_unlock();
		}

	va_end (arg_ptr);

	return length;
}
#ifdef _DEBUG
void assert_Function(int expr, char *file_name, unsigned int line_no, char *expression)
{
	if(expr)
		{
		debug_printf("\nFile: %s,line:%d Error.\n",file_name,line_no);
		debug_printf(expression);
		debug_printf("\n");
		}
}
#endif

int rs232_printf(const char *format,...)
{
	char temp_buffer[128];
	int length = 0,i = 0;

	va_list arg_ptr;

	va_start (arg_ptr, format);           /* format string */
	vsprintf (temp_buffer, format, arg_ptr);
	length = strlen(temp_buffer);

	for(i=0;i<length;i++)
		{
//		tsk_lock();
		Uart0Send(temp_buffer[i]);
//		tsk_unlock();
		}

	va_end (arg_ptr);

	return length;
}


#ifdef _DEBUG
const unsigned char *msg_type[] = 
{
"MSG_NULL",
"MSG_KeyPress",
"MSG_KeyHold",
"MSG_KeyUp",

"MSG_Stream",
"MSG_IrcDelay",
"MSG_IrcTrims",
"MSG_IrcEq",
"MSG_Irc",

"MSG_Show",
"MSG_Flash",
"MSG_Dimer",

"MSG_RadioSearch",
"MSG_RadioPreset",
"MSG_RadioFreq",

"Msg_Volume",

"MSG_Power",
"MSG_InputSource",

"MSG_Mute",
};

void debug_message(MSG message)
{	
#ifdef _DEBUG_DSP_TASK
	debug_printf("\n(%d)%s:",message.t_id,msg_type[message.buffer[0]]);
	DispBuffer(message.buffer,MESSAGE_SIZE);
#endif	
}
#endif

