#ifndef EXCEPTION_H
#define EXCEPTION_H

typedef struct Exception Exception;
typedef Exception* ExceptionPtr;
struct Exception {
	char *msg;
	int	 errorCode;
	void *data;
	int  freeDataOnFreeingException;
};

ExceptionPtr createException(char *msg, int errorCode);
void freeException(Exception *e);
void dumpException(Exception *e);

void throwException(int errorCode, void *data, int freeDataOnFreeingException, char *message, ...);

#endif // EXCEPTION_H
