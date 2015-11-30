
#define DEBUG

//Authors:
//Shchitnikov Alexander
//Pustoshilov Alexander
//LDFLAGS+=-pthread -lncurses -lm -lrt  -ldl -lliquid
//

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
//#include <complex.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <pthread.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/select.h>
#include <ncurses.h>
    
#include <liquid/liquid.h>

#include <termios.h>

#define INITGUID

#include "include/stubs.h"
#include "include/ioctl.h"
#include "include/ifc_ldev.h"

using namespace std;
#include <complex>

WINDOW *term,*stat,*wdebug;


//Load parametrs
float paramF1 = 53.0;
float paramF2 = 60.0;
float paramBaudRate = 4.0;
float paramFreqADC = 20000.0;
int paramKu = 160;
float paramPorog = 0.6;

//E-154
PLATA_DESCR_U2 pd;
SLOT_PAR sl;
ADC_PAR adcPar;
ULONG size;

HANDLE hVxd;
void *handle;
char *error;
pthread_t thread1;
IDaqLDevice* pI;

typedef IDaqLDevice* (*CREATEFUNCPTR)(ULONG Slot);
CREATEFUNCPTR CreateInstance;
short *pData;

unsigned long *pSync;
long IrqStep=128;  
long  pages=64;
int complete;
int thread_work;
int savewaveform =1;
int detect;
int paramset=0;	//flag setting 
short *bufferADC;
int bufferADCSize = IrqStep*pages;
char boud[]={'.','E',0x0D,'A',' ','S','I','U',0x0A,'D','R','J','N','F','C','K','T','Z','L','W','H','Y','P','Q','O','B','G',' ','M','X','V',';'};
int charcode;
int bitnum;

//-----------------------------------------
int InitE154(void)
{
//open lib for E154
  handle = dlopen("liblcomp.so",RTLD_LAZY);
   if(!handle)
   {
   wprintw(wdebug,"error open dll!! =  %s \n", (dlerror()));wrefresh(wdebug);
   return 1;
   }
   CreateInstance =(CREATEFUNCPTR) dlsym(handle,"CreateInstance");wrefresh(wdebug);
   if((error = dlerror())!=NULL)
   {
      wprintw(wdebug,"%s \n", error);
   }

   LUnknown* pIUnknown = CreateInstance(0); 
   if(pIUnknown == NULL) 
    { 
       wprintw(wdebug,"CallCreateInstance failed \n"); wrefresh(wdebug);
       return 1; 
    }
   wprintw(wdebug,"Get IDaqLDevice interface\n");wrefresh(wdebug);
   HRESULT hr = pIUnknown->QueryInterface(IID_ILDEV,(void**)&pI);
   if(hr!=S_OK)
    {
     wprintw(wdebug,"Get IDaqLDevice failed\n");wrefresh(wdebug);
     return 1;
    }
   wprintw(wdebug,"IDaqLDevice get success \n");wrefresh(wdebug);
   pIUnknown->Release();
   wprintw(wdebug,"Free IUnknown\n");wrefresh(wdebug);
   wprintw(wdebug,"OpenLDevice Handle \n");wrefresh(wdebug);
   hVxd=pI->OpenLDevice();
   pI->GetSlotParam(&sl);
   pI->ReadPlataDescr(&pd); // fill up properties
   size=10241;
   pI->RequestBufferStream(&size);
   wprintw(wdebug," alloc size %i\n", size);wrefresh(wdebug);
         adcPar.t1.s_Type = L_ADC_PARAM;
         adcPar.t1.AutoInit = 1;
         adcPar.t1.dRate = paramFreqADC/1000;
         adcPar.t1.dKadr = 0;
         adcPar.t1.dScale = 0;
         adcPar.t1.SynchroType = 3;
         adcPar.t1.SynchroType = 0;
         adcPar.t1.SynchroSensitivity = 0;
         adcPar.t1.SynchroMode = 0;
         adcPar.t1.AdChannel = 0;
         adcPar.t1.AdPorog = 0;
         adcPar.t1.NCh = 1;
	 switch (paramKu){
	   case 160: adcPar.t1.Chn[0] = 0xC0;break;
	   case 500: adcPar.t1.Chn[0] = 0x80;break;
	   case 1600: adcPar.t1.Chn[0] = 0x40;break;
	   case 5000: adcPar.t1.Chn[0] = 0x00;break;
	   default: adcPar.t1.Chn[0] = 0x40;
	 }
         adcPar.t1.FIFO = IrqStep;
         adcPar.t1.IrqStep = IrqStep;
         adcPar.t1.Pages = pages;
         adcPar.t1.IrqEna = 1;
         adcPar.t1.AdcEna = 1;
         pI->FillDAQparameters(&adcPar.t1);
         pI->SetParametersStream(&adcPar.t1, &size, (void **)&pData, (void **)&pSync, L_STREAM_ADC);
         IrqStep = adcPar.t1.IrqStep;
         pages = adcPar.t1.Pages;
   complete=0;
   pI->EnableCorrection();
   pI->InitStartLDevice();
   wprintw(wdebug,"ADC Started... \n");
   
//    print adc param
   wmove(wdebug,0,55);
   wprintw(wdebug,"paramF1		%5.0f\n",paramF1);
   wmove(wdebug,1,55);
   wprintw(wdebug,"paramF2:		%5.0f\n",paramF2);
   wmove(wdebug,2,55);
   wprintw(wdebug,"paramBaudRate:	%5.0f\n",paramBaudRate);
   wmove(wdebug,3,55);
   wprintw(wdebug,"paramFreqADC:	%5.0f\n",paramFreqADC);
   wmove(wdebug,4,55);
   wprintw(wdebug,"paramKu:		%5.0f\n",paramKu);
   wmove(wdebug,5,55);
   wprintw(wdebug,"paramPorog:	%5.0f\n",paramPorog);wrefresh(wdebug);
   return 0;
}
//-----------------------------------------

void *thread_func(void *arg)
{
thread_work = 1;  
unsigned long halfbuffer;
unsigned long fl2, fl1;
short *tmp1;
int key;

   halfbuffer = IrqStep * pages / 2;
   fl1 = fl2 = (*pSync <= halfbuffer) ? 0:1;
   while(1)
   {
     key=getch();
      if(key=='q'){break;}
      if(key=='c'){werase(term);wrefresh(term);}
      if(key=='s'){paramset=1;}
      
  
      while (fl2 == fl1)
       {
         fl2 = (*pSync <= halfbuffer) ? 0:1;
       }
      tmp1 = pData + (halfbuffer * fl1);    
      wmove(stat,0,79);
      wprintw(stat,"%i", fl1);
      wrefresh(stat);
      memcpy(bufferADC, tmp1,  halfbuffer*sizeof(short));   
      complete = 1;
      fl1 = (*pSync <= halfbuffer)? 0:1;  
   }  
    pI->StopLDevice();
    thread_work = 0;
    return 0;
}

void init_windows(void){
  initscr();
  start_color();
  init_pair(1,COLOR_WHITE,COLOR_BLUE);
  init_pair(2,COLOR_YELLOW,COLOR_BLACK);
  bkgd(COLOR_PAIR(1));
  noecho();
  halfdelay(1);
  term=subwin(stdscr,15,80,1,0);
  stat=subwin(stdscr,3,80,16,0);
  wdebug=subwin(stdscr,20,80,19,0);
  wbkgd(term,COLOR_PAIR(2));
  wbkgd(stat,COLOR_PAIR(1));
  scrollok(term,true);
  scrollok(wdebug,true);  
  move(0,0);
  printw("SSDR_Terminal 1.1");
  refresh();
}

void param_setting(void)
{
    //pI->StopLDevice();
    werase(term);wrefresh(term);
    wmove(term,0,0);
    wprintw(term,"Parameters setting mode");wrefresh(term);
    while(1)
    {
      wmove(term,1,0);
      wprintw(term,"paramF1");wrefresh(term);
      wmove(term,2,0);
      echo();
      wscanw(term,"%f\r",&paramF1);wrefresh(term);
      noecho();
      break;
    }
    //    print adc param
   wmove(wdebug,0,55);
   wprintw(wdebug,"paramF1		%5.0f\n",paramF1);
   wmove(wdebug,1,55);
   wprintw(wdebug,"paramF2:		%5.0f\n",paramF2);
   wmove(wdebug,2,55);
   wprintw(wdebug,"paramBaudRate:	%5.0f\n",paramBaudRate);
   wmove(wdebug,5,55);
   wprintw(wdebug,"paramPorog:	%5.0f\n",paramPorog);wrefresh(wdebug);
}


int main(int argc, char*argv[])
{
#define OUTPUT_FILENAME "test.m"
FILE * fid = fopen(OUTPUT_FILENAME,"w");    
 
    init_windows();
//filter init
    // options
    unsigned long i=0;
    unsigned long j=0;
    unsigned int order=6;   // filter order
    float f01=paramF1/paramFreqADC;//f1/ADCRATE;          // center frequency
    float f02=paramF2/paramFreqADC;//f2/ADCRATE;          // center frequency
    float fc1=f01+paramBaudRate*0.5/paramFreqADC;          // cutoff frequency
    float fc2=f02+paramBaudRate*0.5/paramFreqADC;          // cutoff frequency
    float Ap=1.0f;          // pass-band ripple
    float As=60.0f;         // stop-band attenuation
    float samplenum=paramFreqADC/paramBaudRate;
    liquid_iirdes_filtertype ftype  = LIQUID_IIRDES_BESSEL;//CHapEBY2;
    liquid_iirdes_bandtype   btype  = LIQUID_IIRDES_BANDPASS;
    liquid_iirdes_format     format = LIQUID_IIRDES_SOS;   

    std::complex<float> x, y, y1, y2,y0;   
    std::complex<float> signallevel =0;

    // design filter from prototype
    iirfilt_cccf q1 = iirfilt_cccf_create_prototype(ftype, btype, format, order, fc1, f01, Ap, As); 
    iirfilt_cccf q2 = iirfilt_cccf_create_prototype(ftype, btype, format, order, fc2, f02, Ap, As);
    iirfilt_cccf q3 = iirfilt_cccf_create_prototype(ftype, LIQUID_IIRDES_LOWPASS, format, order, paramBaudRate/paramFreqADC, 0, Ap, As);
 
////////////////////////
InitE154();
bufferADC  = new short[IrqStep*pages+1];
pI->StartLDevice(); 
//start ADDC e-154 
pthread_create(&thread1, NULL, thread_func, pI);	
usleep(100);	// обязательная пауза для инициализации
detect=0;
charcode=0;
bitnum=1;

long long ind = 0;

wprintw(stat,"samplenum: %5.0f\n",samplenum);wrefresh(stat);
while(thread_work)
{
       if (paramset==1) {param_setting();paramset=0;}	//paramsetting function call
    if(complete==1)
   {
     for (i=0; i<=4096; i++) 
     {		
//      input signal
     	x = std::complex<float>(*(bufferADC+i), 0);
        iirfilt_cccf_execute(q1, reinterpret_cast<liquid_float_complex&>(x), reinterpret_cast<liquid_float_complex*>(&y1));
        iirfilt_cccf_execute(q2, reinterpret_cast<liquid_float_complex&>(x), reinterpret_cast<liquid_float_complex*>(&y2));
	y = pow(y1.real(),2)-pow(y2.real(),2)+(pow(y1.imag(),2)-pow(y2.imag(),2));
	iirfilt_cccf_execute(q3, reinterpret_cast<liquid_float_complex&>(y), reinterpret_cast<liquid_float_complex*>(&y0));
// 	write to file for debug

// 	SAVE WAVEFORMS TO FILE
	if (savewaveform ==1)
	{
	  if (ind<=400000)
	  {
	    fprintf(fid,"%12.4e %12.4e %12.4e %12.4e %12.4e\n",y1.real(),y2.real(),y.real(),y0.real(), x.real());
	    ind++;
	    if(ind>400000)
	    {
	      fclose(fid);
	    } 
	  }
	}
//	noise level estimation
	signallevel += y0.real()+y0.imag();
// 	выделение огибающей
	if(!detect){if ((y0.real()>=paramPorog)||(y0.real()<=-paramPorog)){j=0;detect=1;wmove(stat,0,20);wprintw(stat,"SIGNAL"); wrefresh(stat);}}else{
	if(j==samplenum/2){  	  
	  if(y0.real()>=paramPorog){
	    charcode+=bitnum;
	    bitnum=bitnum<<1;
	  }
	  if(y0.real()<-paramPorog){
	    bitnum=bitnum<<1;
	  }
	  if((y0.real()<paramPorog)&&(y0.real()>-paramPorog)){detect=0;j=0;bitnum=1;charcode=0;wmove(stat,0,20);wprintw(stat,"      ");wprintw(term,"\n"); wrefresh(stat); wrefresh(term);}
 	  wmove(stat,0,45);wprintw(stat,"NUMBIT:    ",bitnum);wmove(stat,0,45);wprintw(stat,"NUMBIT: %i",bitnum); wrefresh(stat);
	}
	if(j==samplenum){j=0;}
	if(bitnum==32){	
	  wprintw(term,"%c",boud[charcode]);
	  wrefresh(term);
	  bitnum=1;
	  charcode=0;
	}
 	wmove(stat,0,30);wprintw(stat,"NUM: %i",j); wrefresh(stat);
	}
	j++;
      }
//standart deviation
      wmove(stat,16,0);
      wprintw(stat,"signal level: %f", pow(abs(signallevel.real())/4096,0.5));wrefresh(stat);	//sigma
      signallevel = 0;
    complete=0;
   }
 }
   // destroy q1 and q2 and q3 filter object
    iirfilt_cccf_destroy(q1);
    iirfilt_cccf_destroy(q2);
    iirfilt_cccf_destroy(q3);

delwin(term);
delwin(stat);
delwin(wdebug);
endwin();

if(ind<400000)
	  {
	    fclose(fid);
	  } 
    return 0;
}