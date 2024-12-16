/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2017, Vimicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
    main

Description:
    Rom Bootloader

Revision History:
    Date                  Author                   Description
    2022.05.30            cxt                       9650
------------------------------------------------------------------------------*/
/*********************************************************************
*                   (c) SEGGER Microcontroller GmbH                  *
*                        The Embedded Experts                        *
*                           www.segger.com                           *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------
Purpose : Implementation of low-level functions for I/O with the
          SEGGER Runtime Library
          using a UART (SEGGER's BSP UART module)
*/

/*********************************************************************
*
*       #include section
*
**********************************************************************
*/

#include "__SEGGER_RTL_Int.h"
#include "stdio.h"
#include "vstdio.h"
#include "vstdlib.h"


/*********************************************************************
*
*       Local types
*
**********************************************************************
*/

struct __SEGGER_RTL_FILE_impl {         // NOTE: Provides implementation for FILE
  int stub; // only needed so impl has size != 0.
};

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static FILE __SEGGER_RTL_stdin_file  = { 0 };  // stdin reads from UART
static FILE __SEGGER_RTL_stdout_file = { 0 };  // stdout writes to UART
static FILE __SEGGER_RTL_stderr_file = { 0 };  // stderr writes to UART

static int          _stdin_ungot  = EOF;

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/

FILE *stdin  = &__SEGGER_RTL_stdin_file;  // NOTE: Provide implementation of stdin for RTL.
FILE *stdout = &__SEGGER_RTL_stdout_file; // NOTE: Provide implementation of stdout for RTL.
FILE *stderr = &__SEGGER_RTL_stderr_file; // NOTE: Provide implementation of stderr for RTL.

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

/*********************************************************************
*
*       _stdin_getc()
*
*  Function description
*    Get character from standard input.
*
*  Return value
*    Character received.
*
*  Additional information
*    This function never fails to deliver a character.
*/
static char _stdin_getc(void) {
  unsigned char c;

  if (_stdin_ungot != EOF) {
    c = _stdin_ungot;
    _stdin_ungot = EOF;
  } else {
    c = vfgetc();
  }
  return c;
}

/*********************************************************************
*
*       __SEGGER_RTL_X_file_stat()
*
*  Function description
*    Get file status.
*
*  Parameters
*    stream - Pointer to file.
*
*  Additional information
*    Low-overhead test to determine if stream is valid.  If stream
*    is a valid pointer and the stream is open, this function must
*    succeed.  If stream is a valid pointer and the stream is closed,
*    this function must fail.
*
*    The implementation may optionally determine whether stream is
*    a valid pointer: this may not always be possible and is not
*    required, but may assist debugging when clients provide wild
*    pointers.
*
*  Return value
*    <  0 - Failure, stream is not a valid file.
*    >= 0 - Success, stream is a valid file.
*/
int __SEGGER_RTL_X_file_stat(FILE *stream) {
  if (stream == stdin || stream == stdout || stream == stderr) {
    return 0;       // NOTE: stdin, stdout, and stderr are assumed to be valid.
  } else {
    return EOF;
  }
}

/*********************************************************************
*
*       __SEGGER_RTL_X_file_bufsize()
*
*  Function description
*    Get stream buffer size.
*
*  Parameters
*    stream - Pointer to file.
*
*  Additional information
*    Returns the number of characters to use for buffered I/O on
*    the file stream.  The I/O buffer is allocated on the stack
*    for the duration of the I/O call, therefore this value should
*    not be set arbitrarily large.
*
*    For unbuffered I/O, return 1.
*
*  Return value
*    Nonzero number of characters to use for buffered I/O; for
*    unbuffered I/O, return 1.
*/
int __SEGGER_RTL_X_file_bufsize(FILE *stream) {
  (void)stream;
  return 1;
}

/*********************************************************************
*
*       __SEGGER_RTL_X_file_read()
*
*  Function description
*    Read data from file.
*
*  Parameters
*    stream - Pointer to file to read from.
*    s      - Pointer to object that receives the input.
*    len    - Number of characters to read from file.
*
*  Return value
*    >= 0 - Success, amount of data read.
*    <  0 - Failure.
*
*  Additional information
*    Reading from any stream other than stdin results in an error.
*/
int __SEGGER_RTL_X_file_read(FILE *stream, char *s, unsigned len) {
  int c;

  if (stream == stdin) {
    c = 0;
    while (len > 0) {
      *s = _stdin_getc();
      ++s;
      ++c;
      --len;
    }
  } else {
    c = EOF;
  }
  return c;
}

/*********************************************************************
*
*       __SEGGER_RTL_X_file_write()
*
*  Function description
*    Write data to file.
*
*  Parameters
*    stream - Pointer to file to write to.
*    s      - Pointer to object to write to file.
*    len    - Number of characters to write to the file.
*
*  Return value 
*    >= 0 - Success.
*    <  0 - Failure.
*
*  Additional information
*    this version is NOT reentrant!
*    stdout and stderr are directed to UART;
*    writing to any stream other than stdout or stderr results in an error
*/
int __SEGGER_RTL_X_file_write(FILE *stream, const char *s, unsigned len) {
  if ((stream == stdout) || (stream == stderr)) {
        while(len--){
                vfputc((UINT8)*s++);
        }
    return len;
  } else {
    return EOF;
  }
}

/*********************************************************************
*
*       __SEGGER_RTL_X_file_unget()
*
*  Function description
*    Push character back to stream.
*
*  Parameters
*    stream - Pointer to file to push back to.
*    c      - Character to push back.
*
*  Return value
*    >= 0 - Success.
*    <  0 - Failure.
*
*  Additional information
*    Push-back is only supported for standard input, and
*    only a single-character pushback buffer is implemented.
*/
int __SEGGER_RTL_X_file_unget(FILE *stream, int c) {
  if (stream == stdin) {
    if (c != EOF && _stdin_ungot == EOF) {
      _stdin_ungot = c;
    } else {
      c = EOF;
    }
  } else {
    c = EOF;
  }
  return c;
}

int __SEGGER_RTL_stdout_putc_std(int c)
{
    vfputc((UINT8)c);
}

int __SEGGER_RTL_stdin_getc_std(void)
{
    int ch = 0;
        ch = vfgetc();
    return (ch);
}



