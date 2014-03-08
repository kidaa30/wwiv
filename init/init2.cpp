/**************************************************************************/
/*                                                                        */
/*                 WWIV Initialization Utility Version 5.0                */
/*             Copyright (C)1998-2014, WWIV Software Services             */
/*                                                                        */
/*    Licensed  under the  Apache License, Version  2.0 (the "License");  */
/*    you may not use this  file  except in compliance with the License.  */
/*    You may obtain a copy of the License at                             */
/*                                                                        */
/*                http://www.apache.org/licenses/LICENSE-2.0              */
/*                                                                        */
/*    Unless  required  by  applicable  law  or agreed to  in  writing,   */
/*    software  distributed  under  the  License  is  distributed on an   */
/*    "AS IS"  BASIS, WITHOUT  WARRANTIES  OR  CONDITIONS OF ANY  KIND,   */
/*    either  express  or implied.  See  the  License for  the specific   */
/*    language governing permissions and limitations under the License.   */
/*                                                                        */
/**************************************************************************/
#include "wwivinit.h"
#include "subacc.h"

extern char **mdm_desc;
extern int mdm_count, mdm_cur;

extern void *autosel_info;
extern int num_autosel;

extern resultrec *result_codes;
extern int num_result_codes;

extern net_networks_rec *net_networks;

extern int inst;
/****************************************************************************/

char *ctim(double d)
{
	static char ch[10],cm[10],cs[10];
	
	long h = static_cast<long>( d / 3600.0 );
	d-=(double) (h*3600);
	long m = static_cast<long>( d / 60.0 );
	d-=(double) (m*60);
	long s = static_cast<long>( d );
	
	_ltoa( h, ch, 10 );
	_ltoa( m, cm, 10 );
	if (cm[1]==0) 
	{
		cm[2]=0;
		cm[1]=cm[0];
		cm[0]='0';
	}
	_ltoa(s, cs, 10);
	if (cs[1]==0) 
	{
		cs[2]=0;
		cs[1]=cs[0];
		cs[0]='0';
	}
	strcat(ch,":");
	strcat(ch,cm);
	strcat(ch,":");
	strcat(ch,cs);
	return ch;
}



void trimstr(char *s)
{
	int i = strlen( s );
	while ((i>=0) && (s[i-1]==32))
	{
		--i;
	}
	s[i] = '\0';
}


void trimstrpath(char *s)
{
	trimstr( s );
	
	int i=strlen(s);
	if (i && (s[i-1]!='\\'))
	{
		strcat(s,"\\");
	}
}


/****************************************************************************/


void print_time(unsigned short t, char *s)
{
	sprintf( s, "%02.2d:%02.2d", t / 60, t % 60 );
}


unsigned short get_time(char *s)
{
    if (s[2]!=':')
    {
        return 0xffff;
    }

    unsigned short h = atoi( s );
    unsigned short m = atoi( s + 3 );

    if ( h > 23 || m > 59 )
    {
        return 0xffff;
    }

    unsigned short t = h * 60 + m;

    return t;
}

/****************************************************************************/


/* change newuserpw, systempw, systemname, systemphone, sysopname,
   newusersl, newuserdsl, maxwaiting, closedsystem, systemnumber,
   maxusers, newuser_restrict, sysconfig, sysoplowtime, sysophightime,
   req_ratio, newusergold, sl, autoval
 */
void sysinfo1()
{
    char j0[15],j1[10],j2[20],j3[5],j4[5],j5[20],j6[10],j7[10],j8[10],
        j9[5],j10[10],j11[10],j12[5],j17[10],j18[10],j19[10], rs[20];
    int i,i1,cp;

    char ch1='0';

    read_status();

    if (status.callernum!=65535) 
    {
        status.callernum1=(long)status.callernum;
        status.callernum=65535;
        save_status();
    }


    _ultoa((int)syscfg.newusersl,j3,10);
    _ultoa((int)syscfg.newuserdsl,j4,10);
    print_time(syscfg.sysoplowtime,j6);
    print_time(syscfg.sysophightime,j7);
    print_time(syscfg.netlowtime,j1);
    print_time(syscfg.nethightime,j10);
    _ultoa((int)syscfg.maxwaiting,j9,10);
    _ultoa(syscfg.maxusers,j11,10);
    if (syscfg.closedsystem)
    {
        strcpy( j12, "Y" );
    }
    else
    {
        strcpy( j12, "N" );
    }
    _ltoa( status.callernum1,j17,10);
    _ultoa(status.days,j19,10);
    _ltoa( static_cast<long>( syscfg.newusergold ), j5, 10 );
    sprintf(j8,"%5.3f",syscfg.req_ratio);
    sprintf(j18,"%5.3f",syscfg.post_call_ratio);
    _ltoa(syscfg.wwiv_reg_number,j0,10);

    strcpy(rs,restrict_string);
    for (i=0; i<=15; i++) 
    {
        if (rs[i]==' ')
        {
            rs[i]=ch1++;
        }
        if (syscfg.newuser_restrict & (1 << i))
        {
            j2[i]=rs[i];
        }
        else
        {
            j2[i]=32;
        }
    }
    j2[16]=0;
    app->localIO->LocalCls();
    Printf("System PW        : %s\n",syscfg.systempw);
    Printf("System name      : %s\n",syscfg.systemname);
    Printf("System phone     : %s\n",syscfg.systemphone);
    Printf("Closed system    : %s\n",j12);

    Printf("Newuser PW       : %s\n",syscfg.newuserpw);
    Printf("Newuser restrict : %s\n",j2);
    Printf("Newuser SL       : %-3s\n",j3);
    Printf("Newuser DSL      : %-3s\n",j4);
    Printf("Newuser gold     : %-5s\n",j5);

    Printf("Sysop name       : %s\n",syscfg.sysopname);
    Printf("Sysop low time   : %-5s\n",j6);
    Printf("Sysop high time  : %-5s\n",j7);

    Printf("Net low time     : %-5s\n",j1);
    Printf("Net high time    : %-5s\n",j10);

    Printf("Up/Download ratio: %-5s\n",j8);
    Printf("Post/Call ratio  : %-5s\n",j18);

    Printf("Max waiting      : %-3s\n",j9);
    Printf("Max users        : %-5s\n",j11);
    Printf("Caller number    : %-7s\n",j17);
    Printf("Days active      : %-7s\n",j19);

    textattr(14);
    Puts("\r\n<ESC> when done.");
    textattr(3);

    cp=0;
    bool done=false;
    do 
    {
        app->localIO->LocalGotoXY(19,cp);
        switch(cp) 
        {
        case 0:
            editline(syscfg.systempw,20,UPPER_ONLY,&i1,"");
            trimstr(syscfg.systempw);
            break;
        case 1:
            editline(syscfg.systemname,50,ALL,&i1,"");
            trimstr(syscfg.systemname);
            break;
        case 2:
            editline(syscfg.systemphone,12,UPPER_ONLY,&i1,"");
            break;
        case 3:
            editline(j12,1,UPPER_ONLY,&i1,"");
            if (j12[0]=='Y') 
            {
                syscfg.closedsystem=1;
                strcpy(j12,"Y");
            } 
            else 
            {
                syscfg.closedsystem=0;
                strcpy(j12,"N");
            }
            Printf("%-1s",j12);
            break;
        case 4:
            editline(syscfg.newuserpw,20,UPPER_ONLY,&i1,"");
            trimstr(syscfg.newuserpw);
            break;
        case 5:
            editline(j2,16,SET,&i1,rs);
            syscfg.newuser_restrict=0;
            for (i=0; i<16; i++)
            {
                if (j2[i]!=32)
                {
                    syscfg.newuser_restrict |= (1 << i);
                }
            }
            break;
        case 6:
            editline(j3,3,NUM_ONLY,&i1,"");
            syscfg.newusersl=atoi(j3);
            _ultoa(syscfg.newusersl,j3,10);
            Printf("%-3s",j3);
            break;
        case 7:
            editline(j4,3,NUM_ONLY,&i1,"");
            syscfg.newuserdsl=atoi(j4);
            _ultoa(syscfg.newuserdsl,j4,10);
            Printf("%-3s",j4);
            break;
        case 8:
            editline(j5,5,NUM_ONLY,&i1,"");
            syscfg.newusergold=(float) atoi(j5);
            _ltoa((long)syscfg.newusergold,j5,10);
            Printf("%-5s",j5);
            break;
        case 9:
            editline(syscfg.sysopname,50,ALL,&i1,"");
            trimstr(syscfg.sysopname);
            break;
        case 10:
            editline(j6,5,UPPER_ONLY,&i1,"");
            if (get_time(j6)!=0xffff)
            {
                syscfg.sysoplowtime=get_time(j6);
            }
            print_time(syscfg.sysoplowtime,j6);
            Printf("%-5s",j6);
            break;
        case 11:
            editline(j7,5,UPPER_ONLY,&i1,"");
            if (get_time(j7)!=0xffff)
            {
                syscfg.sysophightime=get_time(j7);
            }
            print_time(syscfg.sysophightime,j7);
            Printf("%-5s",j7);
            break;
        case 12:
            editline(j1,5,UPPER_ONLY,&i1,"");
            if (get_time(j1)!=0xffff)
            {
                syscfg.netlowtime=get_time(j1);
            }
            print_time(syscfg.netlowtime,j1);
            Printf("%-5s",j1);
            break;
        case 13:
            editline(j10,5,UPPER_ONLY,&i1,"");
            if (get_time(j10)!=0xffff)
            {
                syscfg.nethightime=get_time(j10);
            }
            print_time(syscfg.nethightime,j10);
            Printf("%-5s",j10);
            break;
        case 14:
            {
                editline(j8,5,UPPER_ONLY,&i1,"");
                float fRatio = syscfg.req_ratio;
                sscanf(j8,"%f",&fRatio);
                if ((fRatio>9.999) || (fRatio<0.001))
                {
                    fRatio=0.0;
                }
                syscfg.req_ratio=fRatio;
                sprintf(j8,"%5.3f",syscfg.req_ratio);
                Puts(j8);
            }
            break;
        case 15:
            {
                editline(j18,5,UPPER_ONLY,&i1,"");
                float fRatio =syscfg.post_call_ratio;
                sscanf(j18,"%f",&fRatio);
                if ((fRatio>9.999) || (fRatio<0.001))
                {
                    fRatio=0.0;
                }
                syscfg.post_call_ratio=fRatio;
                sprintf(j18,"%5.3f",syscfg.post_call_ratio);
                Puts(j18);
            }
            break;
        case 16:
            editline(j9,3,NUM_ONLY,&i1,"");
            syscfg.maxwaiting=atoi(j9);
            _ultoa(syscfg.maxwaiting,j9,10);
            Printf("%-3s",j9);
            break;
        case 17:
            editline(j11,5,NUM_ONLY,&i1,"");
            syscfg.maxusers=atoi(j11);
            _ultoa(syscfg.maxusers,j11,10);
            Printf("%-5s",j11);
            break;
        case 18:
            editline(j17,7,NUM_ONLY,&i1,"");
            if ((unsigned long) atol(j17)!=status.callernum1) 
            {
                status.callernum1=atol(j17);
                _ltoa(status.callernum1,j17,10);
                Printf("%-7s",j17);
                save_status();
            }
            break;
        case 19:
            editline(j19,7,NUM_ONLY,&i1,"");
            if (atoi(j19) != status.days) 
            {
                status.days = atoi(j19);
                _ultoa((int)status.days,j19,10);
                Printf("%-7s",j19);
                save_status();
            }
            break;
        }
        cp = GetNextSelectionPosition( 0, 19, cp, i1 );
        if ( i1 == DONE )
        {
            done = true;
        }
    } while ( !done && !hangup );
    save_config();
}


/****************************************************************************/


/* change msgsdir, gfilesdir, datadir, dloadsdir, ramdrive, tempdir */
void setpaths()
{
    app->localIO->LocalCls();
    textattr( 3 );
    Printf("Messages Directory : %s\n",syscfg.msgsdir);
    Printf("GFiles Directory   : %s\n",syscfg.gfilesdir);
    Printf("Menu Directory     : %s\n",syscfg.menudir);
    Printf("Data Directory     : %s\n",syscfg.datadir);
    Printf("Downloads Directory: %s\n",syscfg.dloadsdir);
    Printf("Temporary Directory: %s\n",syscfgovr.tempdir);
    Printf("Batch Directory    : %s\n",syscfgovr.batchdir);

    textattr( 14 );
    Puts("\r\n<ESC> when done.\r\n\r\n\r\n");
    textattr( 13 );
    Printf("CAUTION: ONLY EXPERIENCED SYSOPS SHOULD MODIFY THESE SETTINGS.\r\n\n");
    textattr( 11 );
    Printf(" Changing any of these (except Temporary and Batch) requires YOU\r\n");
    Printf(" to MANUALLY move files and / or directory structures.  Consult the\r\n");
    Printf(" documentation prior to changing any of these settings.\r\n");
    textattr( 3 );

    int i1;
    int cp = 0;
    bool done = false;
    do 
    {
        done = false;
        switch( cp ) 
        {
        case 0:
            app->localIO->LocalGotoXY(21,cp);
            editline(syscfg.msgsdir,50,UPPER_ONLY,&i1,"");
            trimstrpath(syscfg.msgsdir);
            Puts(syscfg.msgsdir);
            //          verify_dir("Messages", syscfg.msgsdir);
            break;
        case 1:
            app->localIO->LocalGotoXY(21,cp);
            editline(syscfg.gfilesdir,50,UPPER_ONLY,&i1,"");
            trimstrpath(syscfg.gfilesdir);
            Puts(syscfg.gfilesdir);
            //          verify_dir("Gfiles", syscfg.gfilesdir);
            break;
        case 2:
            app->localIO->LocalGotoXY(21,cp);
            editline(syscfg.menudir,50,UPPER_ONLY,&i1,"");
            trimstrpath(syscfg.menudir);
            Puts(syscfg.menudir);
            //          verify_dir("Menu", syscfg.menudir);
            break;
        case 3:
            app->localIO->LocalGotoXY(21,cp);
            editline(syscfg.datadir,50,UPPER_ONLY,&i1,"");
            trimstrpath(syscfg.datadir);
            Puts(syscfg.datadir);
            //          verify_dir("Data", syscfg.datadir);
            break;
        case 4:
            app->localIO->LocalGotoXY(21,cp);
            editline(syscfg.dloadsdir,50,UPPER_ONLY,&i1,"");
            trimstrpath(syscfg.dloadsdir);
            Puts(syscfg.dloadsdir);
            //          verify_dir("Downloads", syscfg.dloadsdir);
            break;
        case 5:
            do 
            {
                app->localIO->LocalGotoXY(21,cp);
                editline(syscfgovr.tempdir,50,UPPER_ONLY,&i1,"");
                trimstrpath(syscfgovr.tempdir);
                Puts(syscfgovr.tempdir);
                //        } while ((verify_dir("Temporary", syscfgovr.tempdir)) ||
            } while (verify_inst_dirs(&syscfgovr, inst));
            break;
        case 6:
            do 
            {
                app->localIO->LocalGotoXY(21,cp);
                editline(syscfgovr.batchdir,50,UPPER_ONLY,&i1,"");
                trimstrpath(syscfgovr.batchdir);
                Puts(syscfgovr.batchdir);
                //        } while ((verify_dir("Batch", syscfgovr.batchdir)) ||
            } while (verify_inst_dirs(&syscfgovr, inst));
            break;
        }
        cp = GetNextSelectionPosition( 0, 6, cp, i1 );
        if ( i1 == DONE )
        {
            done = true;
        }
    } while ( !done );

    save_config();
}

/****************************************************************************/


void PrintComPortInfo(int comPortNumber)
{
	app->localIO->LocalGotoXY(0,8);
	if (comPortNumber) 
	{
		switch(check_comport(comPortNumber)) 
		{
		case 0: // no com port
			Printf("No com port detected on COM%d:                  \n", syscfgovr.primaryport);
			break;
		case 1: // normal com port
			Printf("Unbuffered UART on COM%d:                       \n", syscfgovr.primaryport);
			break;
		case 2: // buffering UART
			Printf("Buffered UART on COM%d:                         \n", syscfgovr.primaryport);
		case 3: // OS does not support detection
			Printf("OS Supported Communications on COM%d:           \n", syscfgovr.primaryport);
		}
	} 
	else 
	{
		Printf("Modem usage disabled.                               \r\n\n");
	}
}


void setupcom()
{
	int i1;
	char szComport[10];
	
	_ultoa(syscfgovr.primaryport,szComport,10);
	
	//ultoa(syscfg.baudrate[syscfgovr.primaryport],j2,10);
	app->localIO->LocalCls();

    Printf("Com Port      : %s\n\n",szComport);
	textattr(14);
	Puts("\r\n<ESC> when done.");
	textattr(3);
	
	PrintComPortInfo(syscfgovr.primaryport);
	
	int cp=0;
	bool done=false;
	do 
	{
		app->localIO->LocalGotoXY(16,cp);
		switch(cp) 
		{
		case 0:
			editline(szComport,2,NUM_ONLY,&i1,"");
			syscfgovr.primaryport=atoi(szComport);
			_ultoa(syscfgovr.primaryport,szComport,10);
			Printf("%-2s",szComport);
			/*
			if (syscfgovr.primaryport <= MAX_ALLOWED_PORT)
			{
				_ultoa(syscfg.baudrate[syscfgovr.primaryport],j2,10);
			}
			*/
			PrintComPortInfo(syscfgovr.primaryport);
			break;
		}
        cp = GetNextSelectionPosition( 0, 0, cp, i1 );
        if ( i1 == DONE )
        {
            done = true;
        }
	} while ( !done && !hangup );
	save_config();
}


/****************************************************************************/


int xlate_from_char(char ch)
{
	if ((ch<='0') || ((ch>'9') && (ch<'A')) || (ch>'Z'))
	{
		return 34;
	}
	if (ch<='9')
	{
		return( ch - '1' );
	}
	return( ch - 'A' + 9 );
}


/****************************************************************************/

int read_subs()
{
	char szFileName[MAX_PATH];
	
	sprintf(szFileName,"%sSUBS.DAT",syscfg.datadir);
	int i = open(szFileName,O_RDWR | O_BINARY);
	if ( i > 0 ) 
	{
		subboards=(subboardrec *) bbsmalloc(filelength(i)+1);
		if (!subboards) 
		{
			Printf("needed %ld bytes\n",filelength(i));
			textattr(7);
			exit( 2 );
		}
		
		initinfo.num_subs=(read(i,subboards, (filelength(i))))/
			sizeof(subboardrec);
		close( i );
	} 
	else 
	{
		Printf( "%s NOT FOUND.\n", szFileName );
		textattr(7);
		exit( 2 );
	}
	return 0;
}


void write_subs()
{
	char szFileName[MAX_PATH];
	
	if (subboards) 
	{
		sprintf(szFileName,"%sSUBS.DAT",syscfg.datadir);
		int i=open(szFileName,O_RDWR | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
		if (i>0) 
		{
			write(i,(void *)&subboards[0], initinfo.num_subs * sizeof(subboardrec));
			close(i);
		}
		initinfo.num_subs=0;
		BbsFreeMemory(subboards);
		subboards=NULL;
	}
}


#define UINT(u,n)  (*((int  *)(((char *)(u))+(n))))
#define UCHAR(u,n) (*((char *)(((char *)(u))+(n))))


void del_net(int nn)
{
    int i,t,r,nu,i1,i2;
    mailrec m;
    char *u;
    postrec *p;

    read_subs();

    for (i=0; i<initinfo.num_subs; i++) 
    {
        if (subboards[i].age&0x80) 
        {
            if (subboards[i].name[40]==nn) 
            {
                subboards[i].type=0;
                subboards[i].age&=0x7f;
                subboards[i].name[40]=0;
            } 
            else if (subboards[i].name[40]>nn) 
            {
                subboards[i].name[40]--;
            }
        }
        for (i2=0; i2<i; i2++) 
        {
            if (strcmp(subboards[i].filename, subboards[i2].filename)==0)
            {
                break;
            }
        }
        if (i2>=i) 
        {
            iscan1(i,false);
            open_sub(true);
            for (i1=1; i1<=initinfo.nNumMsgsInCurrentSub; i1++) 
            {
                p=get_post(i1);
                if (p->status & status_post_new_net) 
                {
                    if (p->title[80]==nn) 
                    {
                        p->title[80]=-1;
                        write_post(i1, p);
                    } 
                    else if (p->title[80]>nn) 
                    {
                        p->title[80]--;
                        write_post(i1, p);
                    }
                }
            }
            close_sub();
        }
    }

    write_subs();

    char szFileName[ MAX_PATH ];
    sprintf(szFileName,"%sEMAIL.DAT",syscfg.datadir);
    int hFile = open(szFileName,O_BINARY | O_RDWR);
    if (hFile!=-1) 
    {
        t=(int) (filelength(hFile)/sizeof(mailrec));
        for (r=0; r<t; r++) 
        {
            lseek(hFile,(long)(sizeof(mailrec)) * (long)(r),SEEK_SET);
            read(hFile,(void *)&m,sizeof(mailrec));
            if (((m.tosys!=0) || (m.touser!=0)) && m.fromsys) 
            {
                if (m.status & status_source_verified)
                {
                    i=78;
                }
                else
                {
                    i=80;
                }
                if ((int) strlen(m.title)>=i)
                {
                    m.title[i]=m.title[i-1]=0;
                }
                m.status |= status_new_net;
                if (m.title[i]==nn)
                {
                    m.title[i]=-1;
                }
                else if (m.title[i]>nn)
                {
                    m.title[i]--;
                }
                lseek(hFile,(long)(sizeof(mailrec)) * (long)(r),SEEK_SET);
                write(hFile,(void *)&m,sizeof(mailrec));
            }
        }
        close(hFile);
    }


    u=(char *)bbsmalloc(syscfg.userreclen);

    read_user(1,(userrec *)u);
    nu=number_userrecs();
    for (i=1; i<=nu; i++) 
    {
        read_user(i,(userrec *)u);
        if (UINT(u,syscfg.fsoffset)) 
        {
            if (UCHAR(u,syscfg.fnoffset)==nn) 
            {
                UINT(u,syscfg.fsoffset)=UINT(u,syscfg.fuoffset)=UCHAR(u,syscfg.fnoffset)=0;
                write_user(i,(userrec *)u);
            } 
            else if (UCHAR(u,syscfg.fnoffset)>nn) 
            {
                UCHAR(u,syscfg.fnoffset)--;
                write_user(i,(userrec *)u);
            }
        }
    }

    BbsFreeMemory(u);

    for (i=nn;i<initinfo.net_num_max; i++) 
    {
        net_networks[i]=net_networks[i+1];
    }
    initinfo.net_num_max--;

    sprintf( szFileName, "%sNETWORKS.DAT", syscfg.datadir );
    i=open(szFileName,O_RDWR | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
    write(i,(void *)net_networks, initinfo.net_num_max*sizeof(net_networks_rec));
    close(i);																														
}


void insert_net(int nn)
{
    int i,t,r,nu,i1,i2;
    mailrec m;
    char *u;
    postrec *p;

    read_subs();

    for (i=0; i<initinfo.num_subs; i++) 
    {
        if (subboards[i].age&0x80) 
        {
            if (subboards[i].name[40]>=nn)
            {
                subboards[i].name[40]++;
            }
        }
        for (i2=0; i2<i; i2++) 
        {
            if (strcmp(subboards[i].filename, subboards[i2].filename)==0)
            {
                break;
            }
        }
        if (i2>=i) 
        {
            iscan1(i, false);
            open_sub(true);
            for (i1=1; i1<=initinfo.nNumMsgsInCurrentSub; i1++) 
            {
                p=get_post(i1);
                if (p->status & status_post_new_net) 
                {
                    if (p->title[80]>=nn) 
                    {
                        p->title[80]++;
                        write_post(i1, p);
                    }
                }
            }
            close_sub();
        }
    }

    write_subs();

    char szFileName[ MAX_PATH ];
    sprintf(szFileName,"%sEMAIL.DAT",syscfg.datadir);
    int hFile=open(szFileName,O_BINARY | O_RDWR);
    if (hFile!=-1) 
    {
        t=(int) (filelength(hFile)/sizeof(mailrec));
        for (r=0; r<t; r++) 
        {
            lseek(hFile,(long)(sizeof(mailrec)) * (long)(r),SEEK_SET);
            read(hFile,(void *)&m,sizeof(mailrec));
            if (((m.tosys!=0) || (m.touser!=0)) && m.fromsys) 
            {
                i = (m.status & status_source_verified) ? 78 : 80;
                if ((int) strlen(m.title)>=i)
                {
                    m.title[i]=m.title[i-1]=0;
                }
                m.status |= status_new_net;
                if (m.title[i]>=nn)
                {
                    m.title[i]++;
                }
                lseek(hFile,(long)(sizeof(mailrec)) * (long)(r),SEEK_SET);
                write(hFile,(void *)&m,sizeof(mailrec));
            }
        }
        close(hFile);
    }

    u=(char *)bbsmalloc(syscfg.userreclen);

    read_user(1,(userrec *)u);
    nu=number_userrecs();
    for (i=1; i<=nu; i++) 
    {
        read_user(i,(userrec *)u);
        if (UINT(u,syscfg.fsoffset)) 
        {
            if (UCHAR(u,syscfg.fnoffset)>=nn) 
            {
                UCHAR(u,syscfg.fnoffset)++;
                write_user(i,(userrec *)u);
            }
        }
    }


    BbsFreeMemory(u);

    for (i=initinfo.net_num_max; i>nn; i--) 
    {
        net_networks[i]=net_networks[i-1];
    }
    initinfo.net_num_max++;
    memset(&(net_networks[nn]),0,sizeof(net_networks_rec));
    strcpy(net_networks[nn].name,"NewNet");
    strcpy(net_networks[nn].dir,"NEWNET.DIR\\");

    sprintf( szFileName, "%sNETWORKS.DAT", syscfg.datadir );
    i=open(szFileName,O_RDWR | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
    write(i,(void *)net_networks, initinfo.net_num_max*sizeof(net_networks_rec));
    close(i);																														

    edit_net(nn);
}


/****************************************************************************/


#define MAX_SUBS_DIRS 4096


void convert_to(int num_subs, int num_dirs)
{
    int oqf, nqf,nu,i;
    char oqfn[81],nqfn[81];
    unsigned long *nqsc, *nqsc_n, *nqsc_q, *nqsc_p;
    unsigned long *oqsc, *oqsc_n, *oqsc_q, *oqsc_p;
    int l1,l2,l3,nqscn_len;

    if ( num_subs%32)
    {
        num_subs=(num_subs/32+1)*32;
    }
    if (num_dirs%32)
    {
        num_dirs=(num_dirs/32+1)*32;
    }

    if (num_subs<32)
    {
        num_subs=32;
    }
    if (num_dirs<32)
    {
        num_dirs=32;
    }

    if (num_subs>MAX_SUBS_DIRS)
    {
        num_subs=MAX_SUBS_DIRS;
    }
    if (num_dirs>MAX_SUBS_DIRS)
    {
        num_dirs=MAX_SUBS_DIRS;
    }

    nqscn_len=4*(1+num_subs+((num_subs+31)/32)+((num_dirs+31)/32));

    nqsc=(unsigned long *)bbsmalloc(nqscn_len);
    if (!nqsc) 
    {
        Printf("Could not allocate %d bytes for new quickscan rec\r\n",nqscn_len);
        return;
    }
    memset(nqsc, 0, nqscn_len);

    nqsc_n=nqsc+1;
    nqsc_q=nqsc_n+((num_dirs+31)/32);
    nqsc_p=nqsc_q+((num_subs+31)/32);

    memset(nqsc_n, 0xff, ((num_dirs+31)/32)*4);
    memset(nqsc_q, 0xff, ((num_subs+31)/32)*4);

    oqsc=(unsigned long *)bbsmalloc(syscfg.qscn_len);
    if (!oqsc) 
    {
        BbsFreeMemory(nqsc);
        Printf("Could not allocate %d bytes for old quickscan rec\r\n",syscfg.qscn_len);
        return;
    }
    memset(oqsc, 0, syscfg.qscn_len);

    oqsc_n=oqsc+1;
    oqsc_q=oqsc_n+((syscfg.max_dirs+31)/32);
    oqsc_p=oqsc_q+((syscfg.max_subs+31)/32);

    if (num_dirs<syscfg.max_dirs) 
    {
        l1=((num_dirs+31)/32)*4;
    } 
    else 
    {
        l1=((syscfg.max_dirs+31)/32)*4;
    }

    if (num_subs<syscfg.max_subs) 
    {
        l2=((num_subs+31)/32)*4;
        l3=num_subs*4;
    } 
    else 
    {
        l2=((syscfg.max_subs+31)/32)*4;
        l3=syscfg.max_subs*4;
    }


    sprintf(oqfn,"%sUSER.QSC",syscfg.datadir);
    sprintf(nqfn,"%sUSERQSC.NEW",syscfg.datadir);

    oqf=open(oqfn,O_RDWR|O_BINARY);
    if (oqf<0) 
    {
        BbsFreeMemory(nqsc); BbsFreeMemory(oqsc);
        Printf("Could not open user.qsc\r\n");
        return;
    }
    nqf=open(nqfn,O_RDWR|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);
    if (nqf<0) 
    {
        BbsFreeMemory(nqsc); BbsFreeMemory(oqsc);
        close(oqf);
        Printf("Could not open userqsc.new\r\n");
        return;
    }

    nu=filelength(oqf)/syscfg.qscn_len;
    for (i=0; i<nu; i++) 
    {
        if (i%10==0)
        {
            Printf("%u/%u\r",i,nu);
        }
        read(oqf, oqsc, syscfg.qscn_len);

        *nqsc=*oqsc;
        memcpy(nqsc_n, oqsc_n, l1);
        memcpy(nqsc_q, oqsc_q, l2);
        memcpy(nqsc_p, oqsc_p, l3);

        write(nqf, nqsc, nqscn_len);
    }


    close(oqf);
    close(nqf);
    unlink(oqfn);
    rename(nqfn,oqfn);

    syscfg.max_subs=num_subs;
    syscfg.max_dirs=num_dirs;
    syscfg.qscn_len=nqscn_len;
    save_config();

    BbsFreeMemory(nqsc);
    BbsFreeMemory(oqsc);
    Printf("\nDone\n");
}


void up_subs_dirs()
{
    char s[81];
    int num_subs, num_dirs;

    app->localIO->LocalCls();

    textattr(3);
    Printf("Current max # subs: %d\r\n",syscfg.max_subs);
    Printf("Current max # dirs: %d\r\n",syscfg.max_dirs);
    nlx(2);

    textattr(14);
    Puts("Change # subs or # dirs? ");
    if (yn()) 
    {
        nlx();
        textattr(11);
        Printf("Enter the new max subs/dirs you wish.  Just hit <enter> to leave that\r\n");
        Printf("value unchanged.  All values will be rounded up to the next 32.\r\n");
        Printf("Values can range from 32-1024\r\n\n");

        textattr(14);
        Puts("New max subs: ");
        textattr(3);
        input(s,4);
        num_subs=atoi(s);
        if (!num_subs)
        {
            num_subs=syscfg.max_subs;
        }
		nlx(2);
        textattr(14);
        Puts("New max dirs: ");
        textattr(3);
        input(s,4);
        num_dirs=atoi(s);
        if (!num_dirs)
        {
            num_dirs=syscfg.max_dirs;
        }

        if ( num_subs%32)
        {
            num_subs=(num_subs/32+1)*32;
        }
        if (num_dirs%32)
        {
            num_dirs=(num_dirs/32+1)*32;
        }

        if (num_subs<32)
        {
            num_subs=32;
        }
        if (num_dirs<32)
        {
            num_dirs=32;
        }

        if (num_subs>MAX_SUBS_DIRS)
        {
            num_subs=MAX_SUBS_DIRS;
        }
        if (num_dirs>MAX_SUBS_DIRS)
        {
            num_dirs=MAX_SUBS_DIRS;
        }

        if ((num_subs!=syscfg.max_subs) || (num_dirs!=syscfg.max_dirs)) 
        {
            nlx();
            textattr(14);
            Printf("Change to %d subs and", num_subs);
            Printf(" %d dirs? ", num_dirs);
            if (yn()) 
            {
                nlx();
                textattr(11);
                Printf("Please wait...\r\n");
                convert_to(num_subs, num_dirs);
            }
        }
    }
}



void edit_lang(int nn)
{
    int i1;
    languagerec *n;

    app->localIO->LocalCls();
    bool done=false;
    int cp=0;
    n=&(languages[nn]);
    Printf("Language name  : %s\n",n->name);
    Printf("Data Directory : %s\n",n->dir);
    Printf("Menu Directory : %s\n",n->mdir);
    textattr(14);
    Puts("\r\n<ESC> when done.\r\n\r\n");
    textattr(3);
    do 
    {
        app->localIO->LocalGotoXY(17,cp);
        switch(cp) 
        {
        case 0:
            editline(n->name,sizeof(n->name)-1,ALL,&i1,"");
            trimstr(n->name);
#ifdef WHY
            ss=strchr(n->name,' ');
            if (ss)
            {
                *ss=0;
            }
#endif
            Puts(n->name);
            Puts("                  ");
            break;
        case 1:
            editline(n->dir,60,UPPER_ONLY,&i1,"");
            trimstrpath(n->dir);
            Puts(n->dir);
            break;
        case 2:
            editline(n->mdir,60,UPPER_ONLY,&i1,"");
            trimstrpath(n->mdir);
            Puts(n->mdir);
            break;
        }
        cp = GetNextSelectionPosition( 0, 2, cp, i1 );
        if ( i1 == DONE )
        {
            done = true;
        }
    } while ( !done&& !hangup );
}



void up_langs()
{
    char s[181],s1[81];
    int i,i1,i2;

    bool done=false;
    do 
    {
        app->localIO->LocalCls();
        nlx();
        for (i=0; i<initinfo.num_languages; i++) 
        {
            if (i && ((i%23)==0))
            {
                pausescr();
            }
            Printf( "%-2d. %-20s    %-50s\r\n", i+1, languages[i].name, languages[i].dir);
        }
        nlx();
        textattr(14);
        Puts("Languages: M:odify, D:elete, I:nsert, Q:uit : ");
        textattr(3);
        char ch = onek("Q\033MID");
        switch( ch ) 
        {
        case 'Q':
        case '\033':
            done=true;
            break;
        case 'M':
            nlx();
            textattr(14);
            sprintf(s1,"Edit which (1-%d) ? ",initinfo.num_languages);
            Puts(s1);
            textattr(3);
            input(s,2);
            i=atoi(s);
            if ((i>0) && (i<=initinfo.num_languages))
            {
                edit_lang(i-1);
            }
            break;
        case 'D':
            if (initinfo.num_languages>1) 
            {
                nlx();
                sprintf(s1,"Delete which (1-%d) ? ",initinfo.num_languages);
                textattr(14);
                Puts(s1);
                textattr(3);
                input(s,2);
                i=atoi(s);
                if ((i>0) && (i<=initinfo.num_languages)) 
                {
                    nlx();
                    textattr(12);
                    Puts("Are you sure? ");
                    textattr(3);
                    ch=onek("YN\r");
                    if (ch=='Y') 
                    {
                        initinfo.num_languages--;
                        for (i1=i-1; i1<initinfo.num_languages; i1++)
                        {
                            languages[i1]=languages[i1+1];
                        }
                        if (initinfo.num_languages==1)
                        {
                            languages[0].num=0;
                        }
                    }
                }
            } 
            else 
            {
                nlx();
                textattr(11);
                Printf("You must leave at least one language.\r\n");
                textattr(3);
                nlx();
                app->localIO->getchd();
            }
            break;
        case 'I':
            if (initinfo.num_languages>=MAX_LANGUAGES) 
            {
                textattr(12);
                Printf("Too many languages.\r\n");
                textattr(3);
                nlx();
                app->localIO->getchd();
                break;
            }
            nlx();
            textattr(14);
            sprintf(s1,"Insert before which (1-%d) ? ",initinfo.num_languages+1);
            Puts(s1);
            textattr(3);
            input(s,2);
            i=atoi(s);
            if ((i>0) && (i<=initinfo.num_languages+1)) 
            {
                textattr(12);
                Puts("Are you sure? ");
                textattr(3);
                ch=onek("YN\r");
                if (ch=='Y') 
                {
                    --i;
                    for (i1=initinfo.num_languages; i1>i; i1--)
                    {
                        languages[i1]=languages[i1-1];
                    }
                    initinfo.num_languages++;
                    strcpy(languages[i].name, "English");
                    strncpy(languages[i].dir,syscfg.gfilesdir, sizeof(languages[i1].dir)-1);
                    strncpy(languages[i].mdir,syscfg.gfilesdir, sizeof(languages[i1].mdir)-1);
                    i2=0;
                    for (i1=0; i1<initinfo.num_languages; i1++) 
                    {
                        if ((i!=i1) && (languages[i1].num>=i2)) 
                        {
                            i2=languages[i1].num+1;
                        }
                    }
                    if (i2>=250) 
                    {
                        for (i2=0; i2<255; i2++) 
                        {
                            for (i1=0; i1<initinfo.num_languages; i1++) 
                            {
                                if ((i!=i1) && (languages[i1].num==i2))
                                {
                                    break;
                                }
                            }
                            if (i1>=initinfo.num_languages)
                            {
                                break;
                            }
                        }
                    }
                    languages[i].num=i2;
                    edit_lang(i);
                }
            }
            break;
        }
    } while ( !done && !hangup );

    char szFileName[ MAX_PATH ];
    sprintf( szFileName, "%LANGUAGE.DAT", syscfg.datadir );
    unlink(szFileName);
    int hFile = open(szFileName,O_RDWR | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
    write(hFile,(void *)languages, initinfo.num_languages*sizeof(languagerec));
    close( hFile );																														
}


/****************************************************************************/


void edit_editor(int n)
{
    int i1;
    editorrec c;

    app->localIO->LocalCls();
    c=editors[n];
    bool done=false;
    int cp=0;
    Printf("Description     : %s\n",c.description);
    Printf("Filename to run remotely\r\n%s\n",c.filename);
    Printf("Filename to run locally\r\n%s\n",c.filenamecon);
    textattr(14);
    Puts("\r\n<ESC> when done.\r\n\r\n");
    textattr(11);
    Printf("%%1 = filename to edit\r\n");
    Printf("%%2 = chars per line\r\n");
    Printf("%%3 = lines per page\r\n");
    Printf("%%4 = max lines\r\n");
    Printf("%%5 = instance number\r\n");
    textattr(3);

    do 
    {
        switch(cp) 
        {
        case 0: app->localIO->LocalGotoXY(18,0); break;
        case 1: app->localIO->LocalGotoXY(0,2); break;
        case 2: app->localIO->LocalGotoXY(0,4); break;
        }
        switch(cp) 
        {
        case 0:
            editline(c.description,35,ALL,&i1,"");
            trimstr(c.description);
            break;
        case 1:
            editline(c.filename,75,ALL,&i1,"");
            trimstr(c.filename);
            break;
        case 2:
            editline(c.filenamecon,75,ALL,&i1,"");
            trimstr(c.filenamecon);
            break;
        }
        cp = GetNextSelectionPosition( 0, 2, cp, i1 );
        if ( i1 == DONE )
        {
            done = true;
        }
    } while ( !done && !hangup );
    editors[n]=c;
}


void extrn_editors()
{
	char s[81];
	int i,i1;
	
	bool done=false;
	do 
	{
		app->localIO->LocalCls();
		nlx();
		for (i=0; i<initinfo.numeditors; i++) 
		{
			Printf( "%d. %s\r\n", i + 1, editors[i].description);
		}
		nlx();
		textattr(14);
		Puts("Editors: M:odify, D:elete, I:nsert, Q:uit : ");
		textattr(3);
		char ch = onek("Q\033MID");
		switch(ch) 
		{
		case 'Q':
		case '\033':
			done=true;
			break;
		case 'M':
			if (initinfo.numeditors) 
			{
				nlx();
				textattr(12);
				Printf( "Edit which (1-%d) ? ", initinfo.numeditors );
				textattr(3);
				input(s,2);
				i=atoi(s);
				if ((i>0) && (i<=initinfo.numeditors))
				{
					edit_editor(i-1);
				}
			}
			break;
		case 'D':
			if (initinfo.numeditors) 
			{
				nlx();
				textattr(12);
				Printf( "Delete which (1-%d) ? ", initinfo.numeditors);
				textattr(3);
				input(s,2);
				i=atoi(s);
				if ((i>0) && (i<=initinfo.numeditors)) 
				{
					for (i1=i-1; i1<initinfo.numeditors; i1++)
					{
						editors[i1]=editors[i1+1];
					}
					--initinfo.numeditors;
				}
			}
			break;
		case 'I':
			if (initinfo.numeditors>=10) 
			{
				textattr(12);
				Printf("Too many editors.\r\n");
				textattr(3);
				nlx();
				break;
			}
			nlx();
			textattr(14);
			Printf( "Insert before which (1-%d) ? ", initinfo.numeditors + 1 );
			textattr(3);
			input(s,2);
			i=atoi(s);
			if ((i>0) && (i<=initinfo.numeditors+1)) 
			{
				for (i1=initinfo.numeditors; i1>i-1; i1--)
				{
					editors[i1]=editors[i1-1];
				}
				++initinfo.numeditors;
				editors[i-1].description[0]=0;
				editors[i-1].filenamecon[0]=0;
				editors[i-1].filename[0]=0;
				edit_editor(i-1);
			}
			break;
		}
	} while ( !done && !hangup );
    char szFileName[ MAX_PATH ];
    sprintf( szFileName, "%sEDITORS.DAT", syscfg.datadir );
	int hFile = open( szFileName,O_RDWR | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE );
	write(hFile,(void *)editors, initinfo.numeditors*sizeof(editorrec));
	close(hFile);
}

/****************************************************************************/


char *prot_name(int pn)
{
	char *ss=">NONE<";
	
	switch(pn) {
    case 1:
		ss="ASCII";
		break;
    case 2:
		ss="Xmodem";
		break;
    case 3:
		ss="Xmodem-CRC";
		break;
    case 4:
		ss="Ymodem";
		break;
    case 5:
		ss="Batch";
		break;
    default:
		if ((pn>5) || (pn<(initinfo.numexterns+6)))
		{
			ss=externs[pn-6].description;
		}
		break;
	}
	
	return ss;
}


void edit_prot(int n)
{
    char s[81],s1[81];
    newexternalrec c;

    app->localIO->LocalCls();
    if (n>=6)
    {
        c=externs[n-6];
    }
    else 
    {
        c=over_intern[n-2];
        strcpy(c.description, prot_name(n));
        strcpy(c.receivebatchfn,"-- N/A --");
        strcpy(c.bibatchfn,"-- N/A --");
        if (n!=4)
        {
            strcpy(c.sendbatchfn,"-- N/A --");
        }
    }
    bool done=false;
    int cp=0;
    int i1=NEXT;
    _ultoa(c.ok1,s,10);
    if (c.othr & othr_error_correct)
    {
        strcpy(s1,"Y");
    }
    else
    {
        strcpy(s1,"N");
    }
    Printf("Description          : %s\n",c.description);
    Printf("Xfer OK code         : %s\n",s);
    Printf("Require MNP/LAPM     : %s\n",s1);
    Printf("Receive command line:\r\n%s\r\n", c.receivefn );
    Printf("Send command line:\r\n%s\r\n", c.sendfn );
    Printf("Receive batch command line:\r\n%s\r\n", c.receivebatchfn );
    Printf("Send batch command line:\r\n%s\r\n", c.sendbatchfn );
    Printf("Bi-directional transfer command line:\r\n%s\r\n", c.bibatchfn );
    textattr(14);
    Puts("\r\n<ESC> when done.\r\n\r\n");
    textattr(11);
    Printf("%%1 = com port baud rate\r\n");
    Printf("%%2 = port number\r\n");
    Printf("%%3 = filename to send/receive, filename list to send for batch\r\n");
    Printf("%%4 = modem speed\r\n");
    Printf("%%5 = filename list to receive for batch UL and bi-directional batch\r\n");
    nlx();
    textattr(13);
    Printf("NOTE: Batch protocols >MUST< correctly support DSZLOG.\r\n");
    textattr(3);

    do 
    {
        if (cp<3)
        {
            app->localIO->LocalGotoXY(23,cp);
        }
        else
        {
            app->localIO->LocalGotoXY(0, cp*2-2);
        }
        switch(cp) 
        {
        case 0:
            if (n>=6) 
            {
                editline(c.description,50,ALL,&i1,"");
                trimstr(c.description);
            }
            break;
        case 1:
            editline(s,3,NUM_ONLY,&i1,"");
            trimstr(s);
            c.ok1=atoi(s);
            _ultoa(c.ok1,s,10);
            Puts(s);
            break;
        case 2:
            if (n>=6) 
            {
                editline(s1,1,UPPER_ONLY,&i1,"");
                if (s1[0]!='Y')
                {
                    s1[0]='N';
                }
                s1[1]=0;
                if (s1[0]=='Y')
                {
                    c.othr |= othr_error_correct;
                }
                else
                {
                    c.othr &= (~othr_error_correct);
                }
                OutputStringRaw(s1);
            }
            break;
        case 3:
            editline(c.receivefn,78,ALL,&i1,"");
            trimstr(c.receivefn);
            if (c.sendfn[0] == 0)
            {
                strcpy(c.sendfn, c.receivefn);
            }
            if (c.sendbatchfn[0] == 0)
            {
                strcpy(c.sendbatchfn, c.receivefn);
            }
            if (c.receivebatchfn[0] == 0)
            {
                strcpy(c.receivebatchfn, c.receivefn);
            }
            break;
        case 4:
            editline(c.sendfn,78,ALL,&i1,"");
            trimstr(c.sendfn);
            break;
        case 5:
            if (n>=6) 
            {
                editline(c.receivebatchfn,78,ALL,&i1,"");
                trimstr(c.receivebatchfn);
            }
            break;
        case 6:
            if (n>=4) 
            {
                editline(c.sendbatchfn,78,ALL,&i1,"");
                trimstr(c.sendbatchfn);
            }
            break;
        case 7:
            if (n>=6) 
            {
                editline(c.bibatchfn,78,ALL,&i1,"");
                trimstr(c.bibatchfn);
            }
            break;

        }
        cp = GetNextSelectionPosition( 0, 7, cp, i1 );
        if ( i1 == DONE )
        {
            done = true;
        }
    } while ( !done && !hangup );

    if (n>=6)
    {
        externs[n-6]=c;
    }
    else 
    {
        if (c.receivefn[0] || c.sendfn[0] || (c.sendbatchfn[0] && (n==4)))
        {
            c.othr|=othr_override_internal;
        }
        else
        {
            c.othr&= ~othr_override_internal;
        }
        over_intern[n-2]=c;
    }
}


#define BASE_CHAR '!'
#define END_CHAR (BASE_CHAR+10)


void extrn_prots()
{
    char s[81];
    int i,i1;
    int nMaxProtocolNumber = 6;

    bool done=false;
    do 
    {
        app->localIO->LocalCls();
        nlx();
        for (i=2; i<6+initinfo.numexterns; i++) 
        {
            if (i==5)
            {
                continue;
            }
            Printf( "%c. %s\r\n",(i<10)?(i+'0'):(i-10+BASE_CHAR), prot_name( i ) );
            //maxp=s[0];
        }
        nMaxProtocolNumber = initinfo.numexterns + 6;
        nlx();
        textattr(14);
        Puts("Externals: M:odify, D:elete, I:nsert, Q:uit : ");
        textattr(3);
        char ch = onek("Q\033MID");
        switch(ch) 
        {
        case 'Q':
        case '\033':
            done=true;
            break;
        case 'M':
            nlx();
            textattr(14);
            Printf( "Edit which (2-%d) ? ", nMaxProtocolNumber );
            textattr(3);
            input(s,2);
            i=-1;
            if ((s[0]>='2') && (s[0]<='9') && (s[0]!='5'))
            {
                i=s[0]-'0';
            }
            if ((s[0]>=BASE_CHAR) && (s[0]<=END_CHAR))
            {
                i=s[0]-BASE_CHAR+10;
            }
            if ((i>-1) && (i<initinfo.numexterns+6))
            {
                edit_prot(i);
            }
            break;
        case 'D':
            if (initinfo.numexterns) 
            {
                nlx();
                textattr( 14 );
                Printf(  "Delete which (6-%d) ? ", nMaxProtocolNumber );
                textattr(3);
                input(s,2);
                i=-1;
                if ((s[0]>='6') && (s[0]<='9'))
                {
                    i=s[0]-'6';
                }
                if ((s[0]>=BASE_CHAR) && (s[0]<=END_CHAR))
                {
                    i=s[0]-BASE_CHAR+4;
                }
                if ((i>-1) && (i<initinfo.numexterns)) 
                {
                    for (i1=i; i1<initinfo.numexterns; i1++)
                    {
                        externs[i1]=externs[i1+1];
                    }
                    --initinfo.numexterns;
                }
            }
            break;
        case 'I':
            if (initinfo.numexterns>=15) 
            {
                textattr(12);
                Printf("Too many external protocols.\r\n");
                textattr(3);
                nlx();
                break;
            }
            nlx();
            textattr(14);
            Printf( "Insert before which (6-%d) ? ", nMaxProtocolNumber );
            textattr(3);
            input(s,2);
            i=-1;
            if ((s[0]>='6') && (s[0]<='9'))
            {
                i=s[0]-'6';
            }
            if ((s[0]>=BASE_CHAR) && (s[0]<=END_CHAR))
            {
                i=s[0]-BASE_CHAR+4;
            }
            if ((i>-1) && (i<=initinfo.numexterns)) 
            {
                for (i1=initinfo.numexterns; i1>i; i1--)
                {
                    externs[i1]=externs[i1-1];
                }
                ++initinfo.numexterns;
                memset(externs+i,0,sizeof(newexternalrec));
                edit_prot(i+6);
            }
            break;
        }
    } while ( !done && !hangup );
    char szFileName[ MAX_PATH ];
    sprintf( szFileName, "%sNEXTERN.DAT", syscfg.datadir );
    int hFile=open( szFileName,O_RDWR | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE );
    write(hFile,(void *)externs, initinfo.numexterns*sizeof(newexternalrec));
    close(hFile);

    sprintf(szFileName,"%sNINTERN.DAT",syscfg.datadir);
    if ((over_intern[0].othr|over_intern[1].othr|over_intern[2].othr)&othr_override_internal) 
    {
        hFile=open(szFileName,O_RDWR | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
        if (hFile>0) 
        {
            write(hFile,over_intern, 3*sizeof(newexternalrec));
            close(hFile);
        }
    } 
    else
    {
        unlink(szFileName);
    }
}

