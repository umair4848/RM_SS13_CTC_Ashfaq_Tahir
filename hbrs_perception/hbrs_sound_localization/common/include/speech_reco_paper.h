#include <windows.h>
#include <sapi.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <atlbase.h>
#include "sphelper.h"
#include "paper_sr_nr.h"
#include <iostream>
///////////////////////////
#include "Localize_alg.h"
#include "RtAudio.h"

using namespace std;
//////////////////////////////////////////////
class SRjin
{
  public:
   
    Localize_thread *Localize_thread_obj;

    int CommandintVal;
    SRjin();
    string checkCommand(ULONG );
    //void returnCommand(string );
    int   getCommandint();
    void  setCommandint(int _CommandintVal)
    {
      cout<<"in set "<<endl;
      this->CommandintVal=_CommandintVal;
    };
    void thomas_ExecuteCommand(ISpPhrase *);
    inline HRESULT BlockForResult(ISpRecoContext * , ISpRecoResult ** );
    ~SRjin(){};


};
/////////////////////////////////////////////////
string SRjin::checkCommand(ULONG commandID)
{// check command and give display
	string command;
	switch( commandID )
    {
      case VID_Comeval:
			command = "VID_Comeval";
      cout<<"VID_Comeval"<<endl;
      setCommandint(3);
			break;
      case VID_Leaveval:
			command = "VID_Leaveval";
      cout<<"VID_Leaveval"<<endl;
      setCommandint(1);
			break;
     
      case VID_Single:
			command = "VID_Single";
      cout<<"VID_Single"<<endl;
       setCommandint(2);
			break;
      case VID_Yesval:
			command = "VID_Yesval";
      cout<<"VID_Yesval"<<endl;
       setCommandint(0);
			break;
      case VID_Noval:
			command = "VID_Noval";
      cout<<"VID_Noval"<<endl;
       setCommandint(4);
			break;
			case VID_Counter:
			command = "VID_Counter";
      cout<<"VID_Counter"<<endl;
       setCommandint(5);
			break;
      case VID_Stopval:
			command = "VID_Stopval";
      cout<<"VID_Stopval"<<endl;
      setCommandint(6);
			break;
      

     
	  };
	return command;
};
/////////////////////////////
int SRjin::getCommandint()
{
    cout<<"getCommandint value "<<CommandintVal<<endl;
    return CommandintVal;

};


/////////////////////////////////////////////////
void SRjin::thomas_ExecuteCommand(ISpPhrase *pPhrase)
{
    SPPHRASE *pElements;
    stringstream command;
    // Get the phrase elements, one of which is the rule id we specified in
    // the grammar.  Switch on it to figure out which command was recognized.
    if (SUCCEEDED(pPhrase->GetPhrase(&pElements))&&pElements->pProperties !=0)
    {  
       const SPPHRASEPROPERTY *pNextSiblingIterate;//->vValue.ulVal);
        pNextSiblingIterate=pElements->pProperties ;
        while(pNextSiblingIterate!=0)
        {
                    
                 command<< checkCommand(pNextSiblingIterate->vValue.ulVal);
                
                  pNextSiblingIterate=pNextSiblingIterate->pNextSibling ;
                  
        }
     
           
     }
         // returnCommand(command.str());
        // Free the pElements memory which was allocated for us
        ::CoTaskMemFree(pElements);
    

};

//////////////////////////////////////////////////
inline HRESULT SRjin::BlockForResult(ISpRecoContext * pRecoCtxt, ISpRecoResult ** ppResult)
{// check for next words
    HRESULT hr = S_OK;
	  CSpEvent event;

    while (SUCCEEDED(hr) &&
           SUCCEEDED(hr = event.GetFrom(pRecoCtxt)) &&
           hr == S_FALSE)
    {
       hr = pRecoCtxt->SetNotifyWin32Event();//WaitForNotifyEvent(INFINITE);
      //hr = pRecoCtxt->WaitForNotifyEvent(INFINITE);
       
    }
     thomas_ExecuteCommand(event.RecoResult());
     


    return hr;
};


 
SRjin::SRjin()
{  
    HRESULT hr = E_FAIL;
    this->CommandintVal=0;
    double *localize_result=new double[3];
    
    if (SUCCEEDED(hr = ::CoInitialize(NULL)))// initialize all import port
    {
        
            CComPtr<ISpRecoContext> cpRecoCtxt;
            CComPtr<ISpRecoGrammar> cpGrammar;
            CComPtr<ISpVoice> cpVoice;
            hr = cpRecoCtxt.CoCreateInstance(CLSID_SpSharedRecoContext);
            if(SUCCEEDED(hr))
            {
                hr = cpRecoCtxt->GetVoice(&cpVoice);
               // hr=cpVoice->Speak( L"Please say Johnny come",  SPF_DEFAULT, NULL);
            }
            ////////////////////////////////////////
        
           
            if (cpRecoCtxt && cpVoice &&
                SUCCEEDED(hr = cpRecoCtxt->SetNotifyWin32Event()) &&
               // SUCCEEDED(hr = cpRecoCtxt->SetInterest(SPFEI(SPEI_RECOGNITION) |
			         // SPFEI(SPEI_END_SR_STREAM), SPFEI(SPEI_RECOGNITION) |
			         // SPFEI(SPEI_END_SR_STREAM)) &&
                SUCCEEDED(hr = cpRecoCtxt->SetAudioOptions(SPAO_RETAIN_AUDIO, NULL, NULL)) &&
                SUCCEEDED(hr = cpRecoCtxt->CreateGrammar(1, &cpGrammar)) &&
                SUCCEEDED(hr = cpGrammar->LoadCmdFromFile( L"paper_locjin_nr.xml",SPLO_DYNAMIC))&&
            
                SUCCEEDED(hr = cpGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE ))&&
                SUCCEEDED(hr = cpRecoCtxt->SetInterest( SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION) ) ) ) 
            {
                cout<<"after init "<<endl;
               
                CComPtr<ISpRecoResult> cpResult;
                  
                  
               // printf( "I will repeat everything you say.\nSay \"%s\" to exit.\n", W2A(pchStop) );
                cout<<"!!!!!!!!!!!!!begin speech recognition plz speak to me"<<endl;
                while (SUCCEEDED(hr = BlockForResult(cpRecoCtxt, &cpResult)))
                {
                    //cpGrammar->SetDictationState( SPRS_INACTIVE );
                   cout<<"get word "<<endl;
                   cpResult.Release ();
                   if(getCommandint ()==0)
                    {
                        hr=cpVoice->Speak( L"talk",  SPF_DEFAULT, NULL);// give you a sign to do ur experiment 
                         Sleep(1000);// just for give u time for move to the test position can be removed
                      //////////only with localize//////////
                         Localize_thread_obj =new Localize_thread();// call localize thread
                         Localize_thread_obj->Localize_algorithm(); 
                         delete Localize_thread_obj;
                         //////////////////////////////////////
                         //////////with noise reduction and localize


                         ////////////////////////////////////////
                    }
                  
                   else if(getCommandint()==6)// if stop command then stop
                   {
                     
                     cout<<"sorry to disturb i ll leave "<<endl;
                     break;
                   }
                   else 
                     {
                       
                        cout<<"not important command "<<endl;

                    }

                    
                   
                                  
                              
                }//end while
              
              }// end if for case
        }
         
        ::CoUninitialize();
   
};
  ///////////////////////////////
