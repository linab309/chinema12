#ifndef _DEBUG_H
#define _DEBUG_H

#define _DEBUG                       

#define CSG_C11_Bug				0

#ifdef _DEBUG
extern int debug_printf(const char *format,...); 
extern void assert_Function(int expr, char *file_name, unsigned int line_no, char *expression);
#define  assert(expr) assert_Function((expr), __FILE__, __LINE__, #expr)
#else
//#define debug_printf(m) NULL
int debug_printf(const char *format,...);
#define assert(expr) NULL        
#endif    
  
#define dsp_printf debug_printf
extern int rs232_printf(const char *format,...);
extern void DispBuffer(unsigned char *buffer,short count);

#endif

