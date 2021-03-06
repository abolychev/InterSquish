//---------------------------------------------------------------------------
#include "isSMTP.h"
#include "isInit.h"
//#include "CRC32.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

#pragma link "Cfg"
#pragma link "iuInterSquishServer"
#pragma link "iuServer"
#pragma link "iuSMTPServer"
#pragma link "iuIPAddr"
#pragma resource "*.dfm"

TISsSMTP *ISsSMTP;
//---------------------------------------------------------------------------
__fastcall TISsSMTP::TISsSMTP(TComponent* Owner)
    : TService(Owner)
{
    this->IssSMTP=new TiuIssSMTPServer(Owner);
    this->TextCfg=new TTextCfg(Owner);
    this->ScriptCfg=new TScriptCfg(Owner);
    this->TwitIpCfg=new TTwitIpCfg(Owner);
    this->UsersCfg=new TCustomUsersCfg(Owner);
}

__fastcall TISsSMTP::~TISsSMTP()
{
/*
    delete this->IssSMTP;
    delete this->TextCfg;
    delete this->ScriptCfg;
    delete this->TwitIpCfg;
    delete this->UsersCfg;
*/
}
//---------------------------------------------------------------------------
TServiceController __fastcall TISsSMTP::GetServiceController(void)
{
    return (TServiceController) SMTPServiceController;
}

void __stdcall SMTPServiceController(unsigned CtrlCode)
{
    ISsSMTP->Controller(CtrlCode);
}
//---------------------------------------------------------------------------

void __fastcall TISsSMTP::ISsSMTPStart(TService *Sender, bool &Started)
{
  TextCfg->Path=ParamStr(0).Delete(ParamStr(0).LastDelimiter("."),4)+".cfg";
  TwitIpCfg->Path=TextCfg->Path;
  ScriptCfg->Path=TextCfg->Path;
  AnsiString asUsersCfgName=TextCfg->ParamAsString("UsersCfg");
  //if(asUsersCfgName!="")
  //  UsersCfg->Path=asUsersCfgName;

  //  UsersCfg->Path=TextCfg->ParamAsString("UsersCfg");
  ReadSMTPServerConfiguration(IssSMTP, TextCfg);
  /*
  ISS->LogName=TextCfg->ParamAsString("LogFileName");
  ISS->LogLevel=TextCfg->ParamAsInt("LogLevel");
  ISS->Port=TextCfg->ParamAsInt("NNTPPort");
  ISS->TimeOut=TextCfg->ParamAsInt("ThreadTimeout")*1000;
  ISS->ThreadCacheSize=TextCfg->ParamAsInt("ThreadCacheSize");
*/
  IssSMTP->Open();


  Started=true;

}
//---------------------------------------------------------------------------

void __fastcall TISsSMTP::ISsSMTPStop(TService *Sender, bool &Stopped)
{
  try
  {
    IssSMTP->Close();
  }
  __finally
  {
    Stopped=true;
  }

}
//---------------------------------------------------------------------------
