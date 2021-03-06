//---------------------------------------------------------------------------
#include "isPOP3.h"
#include "isInit.h"
#include "log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Cfg"
#pragma link "iuInterSquishServer"
#pragma link "iuPOP3Server"
#pragma link "iuServer"
#pragma link "iuPOP3Server"
#pragma link "iuIPAddr"
#pragma resource "*.dfm"

TISsPOP3 *ISsPOP3;
//---------------------------------------------------------------------------
__fastcall TISsPOP3::TISsPOP3(TComponent* Owner)
	: TService(Owner)
{
  TraceS("");
  this->IssPOP3Srv=new TiuIssPOP3Server(Owner);
  this->IssPOP3Srv->Active=false;
  this->ScriptCfg=new TScriptCfg(Owner);
  this->TextCfg=new TTextCfg(Owner);
  this->TwitIpCfg=new TTwitIpCfg(Owner);
  this->UsersCfg=new TCustomUsersCfg(Owner);
}

__fastcall TISsPOP3::~TISsPOP3()
{
  //this->IssPOP3Srv->Active=false;
  //if(this->IssPOP3Srv) delete this->IssPOP3Srv;
  /*if(this->ScriptCfg) delete this->ScriptCfg;
  if(this->TextCfg) delete this->TextCfg;
  if(this->TwitIpCfg) delete this->TwitIpCfg;
  if(this->UsersCfg) delete this->UsersCfg;*/

}

TServiceController __fastcall TISsPOP3::GetServiceController(void)
{
	return (TServiceController) POP3ServiceController;
}

void __stdcall POP3ServiceController(unsigned CtrlCode)
{
	ISsPOP3->Controller(CtrlCode);
}
//---------------------------------------------------------------------------
void __fastcall TISsPOP3::ISsPOP3Start(TService *Sender, bool &Started)
{
  TraceS("");
  TextCfg->Path=ParamStr(0).Delete(ParamStr(0).LastDelimiter("."),4)+".cfg";
  TwitIpCfg->Path=TextCfg->Path;
  ScriptCfg->Path=TextCfg->Path;
  AnsiString asUsersCfgName=TextCfg->ParamAsString("UsersCfg");
  if(asUsersCfgName!="")
    UsersCfg->Path=asUsersCfgName;
  ReadPOP3ServerConfiguration(IssPOP3Srv, TextCfg);
  /*
  ISS->LogName=TextCfg->ParamAsString("LogFileName");
  ISS->LogLevel=TextCfg->ParamAsInt("LogLevel");
  ISS->Port=TextCfg->ParamAsInt("NNTPPort");
  ISS->TimeOut=TextCfg->ParamAsInt("ThreadTimeout")*1000;
  ISS->ThreadCacheSize=TextCfg->ParamAsInt("ThreadCacheSize");
*/
  IssPOP3Srv->Open();
  Started=true;
  TraceS("");
}
//---------------------------------------------------------------------------
void __fastcall TISsPOP3::ISsPOP3Stop(TService *Sender, bool &Stopped)
{
  TraceS("");
  IssPOP3Srv->Close();
  Stopped=true;
  TraceS("");
}
//---------------------------------------------------------------------------



