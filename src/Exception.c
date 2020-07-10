#include "Exception.h"
#include <stdio.h>
#include <malloc.h>				//memory allocation library
#include <stdarg.h>

//char buffer[1024];			//static memory allocation (automatically assign memory space itself)
//char *buffer = malloc(1024);  //dynamic memory allocation (manually assign memory space for the program)

//function which returns ' Exception* ' data type.
ExceptionPtr createException(char *msg, int errorCode) {
	Exception *ex;						//ex is a pointer to 'Exception' struct type
	//can also be written as ExceptionPtr ex;
	ex = malloc(sizeof(Exception)); 	//dynamic memory allocation
	ex->msg = msg;						//is equivalent to (*ex).msg
	ex->errorCode = errorCode;
	ex->data = NULL;					//is equivalent to (*ex).data
	return ex;							//information is now stored in the struct where the ex points to
}

void freeException(Exception *e) {
	if(e->data && e->freeDataOnFreeingException) {
		free(e->data);
	}
	if(e->msg) {
		free(e->msg);
	}
	free(e);
}

void dumpException(Exception *e) {
	printf("%s (error = 0x%x)\n", e->msg, e->errorCode);
}

/*
void throwException(int errorCode, void *data, int freeDataOnFreeingException, char *message, int count, ...) {

	Exception *e = createException(message, errorCode);
	e->data = data;
	e->freeDataOnFreeingException = freeDataOnFreeingException;

	va_list ap;
	va_start(ap, count);

	for(int i = 1; i <= count; i++)
	{
//		int value = va_arg(ap, double);
//		printf("%d: %f\n", i, value);
		int value = va_arg(ap, int);
		printf("%d : %d\n", i, value);
	}
	va_end(ap);

	Throw(e);
}
*/

void throwException(int errorCode, void *data, int freeDataOnFreeingException, char *message, ...) {
	Exception *e;						//e is a pointer to 'Exception' struct type
	//can also be written as ExceptionPtr e;
	size_t len;
	char *buffer;						//dynamic memory allocation is used as we need to throw the exception

	va_list va;
	va_start(va, message);
	len = vsnprintf(NULL, 0, message, va);	//copy 0 bytes from 'message' to 'NULL', vsnprintf just returns the number of bytes needed to print the string 'message' to len
	buffer = malloc(len + 1);				//+ 1 is because we need to include the null character
	vsprintf(buffer, message, va);			//va list is read into message, and message is printed to buffer
	va_end(va);

	e = createException(buffer, errorCode);			//pointer e now has the data stored in the 'Exception' struct, it got them from pointer 'ex'
	e->data = data;
	e->freeDataOnFreeingException = freeDataOnFreeingException;


	Throw(e);										//exception is thrown in 'ExceptionPtr' data type
}
