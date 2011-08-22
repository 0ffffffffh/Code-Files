#include <windows.h>
#include <stdio.h>

/*
Skype SoftICE Work Patch.

Oguz Kartal 2009
Tested with Skype  4.1.0.179
*/

#define byte unsigned char

FILE *fskype=NULL;

struct _instruction
{
	byte *instructions;
	long address;
}*instruction;


struct _instruction inst_blocks[2] = 
{
	{"\x74\x1C\x6A\x00\xFF\x35\x70\x15\xF8\x00",0x11AE30},
	{"\x74\x1A\x6A\x00\x68\xE0\x76\xF7\x00\x68",0xB760A1}
};


void read_block(byte *buf,long address)
{
	fseek(fskype,address,SEEK_SET);
	fread(buf,1,0xA,fskype);
}


int main(int argc,char **argv)
{
	byte context[0xa] = {0,};
	int ret=1,i;
	
	if (argc <= 1)
	{
		printf("Patch'in calismasi icin dosya yolunu girmen gerek sanirim.");
		goto cleanup;
	}

	printf("%s aciliyooooor...\n",argv[1]);
	
	if (!SetFileAttributesA(argv[1],FILE_ATTRIBUTE_NORMAL))
	{
		printf("Dosya nitelik ayarlamasi basarisiz! ");

		switch (GetLastError())
		{
		case ERROR_ACCESS_DENIED:
			printf("Erisim engellendi. Yonetici haklari ile calistirin.\n");
			break;
		case ERROR_FILE_NOT_FOUND:
			printf("Dosya yok.\n");
			break;
		default:
			printf("Hata kodu = %x\n",GetLastError());
			break;
		}

		goto cleanup;
	}

	if (!(fskype = fopen(argv[1],"rb+")))
	{
		printf("hayret yonetici haklari ve dosya mevcut. acilmasi gerekirdi. eh napalim bu patch'e layik degilsiniz :)");
		goto cleanup;
	}


	ret=0;

	for (i=0;i<2;i++)
	{
		printf("%d. blok patch...",i+1);

		read_block(context,inst_blocks[i].address);

		if (!memcmp(context,inst_blocks[i].instructions,0xA))
		{
			fseek(fskype,inst_blocks[i].address,SEEK_SET);
			fputc(0xEB,fskype);
			printf("tamam\n");
		}
		else
		{
			ret++;
			printf("hata\n");
		}

	}


	if (ret==2)
		printf("patch basarisiz. sanirim farkli bir skype versiyonu kullaniyorsunuz");
	else if(ret==1)
		printf("patch kismen basarili. normalde 2 kontrol blogu patch edilmis olmasi lazimdi. ama birine denk geldik :/");
	else
		printf("gozunuz aydin. patch islemi basarili.");

cleanup:
	if (fskype)
		fclose(fskype);
	fflush(stdin);
	getchar();
	return ret;
}