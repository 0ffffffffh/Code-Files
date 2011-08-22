/*
	Author: Oguz Kartal
	Live Messenger 2009 Multisession Patch / Coklu Oturum Yamasi
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>


#define _CRT_SECURE_NO_WARNING 1

#define OFFSET 0x96c68//0x8ceea 8.5
#define CHECK_OFFSET 0x61668

#define INSTRUCTION_OPERAND_LOCATION 0x8//0x1e 8.5
#define DEFAULT_LOCATION "C:\\Program Files\\Windows Live\Messenger\\msnmsgr.exe"

int check_messenger_pe(const char *filename)
{
	FILE *pe = fopen(filename,"r");
	unsigned char sign[7] = {0,};
	if (!pe)
		return 0;

	fseek(pe,(long)CHECK_OFFSET,SEEK_SET);

	fread(&sign,1,7,pe);
	fclose(pe);

	if (sign[0] == 'M' &&
		sign[1] == 'S' &&
		sign[2] == 'N' &&
		sign[3] == 'M' &&
		sign[4] == 'S' &&
		sign[5] == 'G' &&
		sign[6] == 'R')
	{
		return 1;
	}
	
	return 0;
}

int is_patched(FILE *exe,long offset)
{
	unsigned char c;
	fseek(exe,offset,SEEK_SET);
	fread(&c,1,1,exe);
	return (c == 0xb7) ? 0 : (c == 0xb6) ? 1 : -1;
}

int patch(FILE *exe,long offset,int new_value)
{
	unsigned char patch_value = 0xb6;
	fseek(exe,offset,SEEK_SET);
	fwrite(&patch_value,1,1,exe);
}

char lcase(char c)
{
	_asm
	{
		mov cl, c
		cmp cl, 'A'
		jb Skip
		cmp cl, 'Z'
		ja Skip
		add cl, 32
		mov c, cl
Skip:
	}

	return c;
}

char question(const char *question_str,char pos, char neg)
{
	char answer=0;
	flushall();
	flushall();
	printf(question_str);
	
	answer = getchar();

	return (lcase(answer));
}

void about()
{
	printf("\t\tWindows Live Messenger 2009 Coklu Oturum Yamasi\n");
	printf("\t\t\tCoder: Oguz Kartal\n\n\n");
}

void abort()
{
	flushall();
	getchar();
	exit(1);
}

void find_msnmsgr(char *output)
{
	char path[MAX_PATH];
	char current_pe[MAX_PATH] = {'\0',};

	WIN32_FIND_DATAA find_struct;
	HANDLE handle;
	path[0] = getenv("windir")[0];
	path[1] = '\0';
	strcat(path,":\\Program Files\\Windows Live\\Messenger\\*.*");
	
	handle = FindFirstFileA(path,&find_struct);
	
	if (handle == INVALID_HANDLE_VALUE)
		*output = '\0';

	while (1)
	{
		if (FindNextFileA(handle,&find_struct))
		{
			sprintf(current_pe,"%c%s%s",path[0],":\\Program Files\\Windows Live\\Messenger\\",find_struct.cFileName);
			if (check_messenger_pe(current_pe))
			{
				strcpy(output,current_pe);
				printf("[+] Bulundu. %s\n",output);
				break;
			}
		}
		else
			break;
	}

}

int main()
{
	FILE *msnmsgr = 0;
	char messenger_path[MAX_PATH] = {0,};
	char messenger_backup[MAX_PATH] = {0,};

	about();

	msnmsgr = fopen(DEFAULT_LOCATION,"r+");

	if (!msnmsgr)
	{
		msnmsgr = fopen(DEFAULT_LOCATION,"r");

		if (!msnmsgr)
		{
			printf("[-] msnmsgr.exe Varsayilan dizinde bulunamadi.\n");
			
			if (question("[?] Otomatik aramatmak ister misiniz? (e/h): ",'e','h') == 'e')
			{
				find_msnmsgr(&messenger_path);
				if (messenger_path[0] == '\0')
				{
					printf("[-] msnmsgr.exe arama sonucu bulunamadi.");
					abort();
				}
				else
				{
					msnmsgr = fopen(messenger_path,"r+");
					if (!msnmsgr)
					{
						printf("[-] Yama islemi icin dosya ustunde yazma izni mevcut degil.");
						abort();
					}
				}
			}
			else
			{
				printf("[-] Islem iptal edildi!\n");
				abort();
			}
		}
		else
		{
			fclose(msnmsgr);
			printf("[-] Yama islemi icin dosya ustunde yazma izni mevcut degil.");
			abort();
		}
	}



	switch(is_patched(msnmsgr,(long)OFFSET+INSTRUCTION_OPERAND_LOCATION))
	{
	case -1:
		{
			printf("[x] Gecersiz versiyon!");
			fclose(msnmsgr);
			abort();
		}
		break;
	case 1:
		{
			printf("[-] Kullandiginiz messenger zaten yamanmis!");
			fclose(msnmsgr);
			abort();
		}
		break;
	}

	printf("[+] Yedek aliniyor...\n");
	
	if (messenger_path[0] == '\0')
		strcpy(messenger_path,DEFAULT_LOCATION);

	sprintf(messenger_backup,"%s.bak",messenger_path);

	if (!CopyFileA(messenger_path,messenger_backup,FALSE))
	{
		printf("[!] Yedek alinamadi. Bu islem icin izniniz oldugunu dogrulayin.\n\n");
		
		switch(question("[?] Yine de yama islemine devam etmek istiyor musunuz?",'e','h'))
		{
		case 'h':
		case 'H':
			{
				fclose(msnmsgr);
				exit(0);
			}
			break;
		}
	}

	printf("[+] Yamaniyor...\n");
	
	patch(msnmsgr,(long)OFFSET+INSTRUCTION_OPERAND_LOCATION,0xb6);
	fclose(msnmsgr);

	printf("[+] Yama basariyla uygulandi...\n");	

	flushall();
	getchar();
	return 0;
}

	