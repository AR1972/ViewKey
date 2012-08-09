// ViewKey.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <comdef.h>


int _tmain(int argc, _TCHAR* argv[])
{
	unsigned char KeyData[0xA4];
	unsigned long Size = sizeof(KeyData);
	unsigned long Status;
	HKEY KeyHandle = NULL;
	char Chars[] = "BCDFGHJKMPQRTVWXY2346789";
	char KeyOutput[25] = {};
	char NewKeyOutput[25] = {};
	const int KeyOffset = 52;
	int i = 24;
	int Last = 0;
	bool isWin8 = FALSE;

	Status = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\"),
		0,
		KEY_READ|KEY_WOW64_64KEY,
		&KeyHandle );

	if (Status != ERROR_SUCCESS)
		return Status;

	Status = RegQueryValueEx(
		KeyHandle,
		TEXT("DigitalProductId"),
		NULL,
		NULL,
		KeyData,
		&Size );

	if (Status != ERROR_SUCCESS)
		return Status;

	RegCloseKey(KeyHandle);

	isWin8 = (KeyData[66] >> 3) & 1;

	KeyData[66] = (KeyData[66] & 0xF7) | ((isWin8 & 2) << 2);
	/* decode the key */
	do {
		int Cur = 0;
		int X = 14;
		do {
			Cur = Cur << 8;
			Cur = KeyData[X + KeyOffset] + Cur;
			KeyData[X + KeyOffset] = (Cur / 24);
			Cur %= 24;
			X--;
		} while (X >= 0);
		i--;
		KeyOutput[i + 1] = Chars[Cur];
		Last = Cur;
	} while (i >= 0);

	if (isWin8) {
		/* copy first part of key into new array */
		memcpy(NewKeyOutput, KeyOutput, Last);
		/* set the 'N' in the new array */
		NewKeyOutput[Last] = 'N';
		/* copy the second part of the key into the new array after 'N' */
		memcpy(&NewKeyOutput[Last + 1], &KeyOutput[Last + 1], sizeof(KeyOutput) - (Last + 1));
		/* copy the finished key back into the original array */
		memcpy(&KeyOutput[0], &NewKeyOutput[0], sizeof(KeyOutput));
	}
	/* display the key */
	for(int i = 0; i < sizeof(KeyOutput); i++) {
		if (i == 5 || i == 10 || i == 15 || i == 20)  {
			printf("%c", '-');
		}
		printf("%c", KeyOutput[i]);	
	}

	printf("\n");
	Sleep(30000);
	return Status;
}

