//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "isInit.h"
//#include "CRC32.h"
#include "log.h"
#include "fConfig.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void __fastcall ReadNNTPServerConfiguration(TiuIssNNTPServer *IssNNTP,TTextCfg *TextCfg)
{
  AnsiString delimiter;
  TraceS("");
//  TextCfg->Refresh();
//    IssNNTP->JamReplyLink=TextCfg->ParamAsString("JAMReplyLink");
  IssNNTP->LogName=TextCfg->ParamAsString("NNTPLogFileName");
  IssNNTP->LogLevel=TextCfg->ParamAsInt("NNTPLogLevel");
  IssNNTP->Port=TextCfg->ParamAsInt("NNTPPort");
  IssNNTP->TimeOut=TextCfg->ParamAsInt("ThreadTimeout")*1000;
  IssNNTP->ThreadCacheSize=TextCfg->ParamAsInt("ThreadCacheSize");
  IssNNTP->FTNAddress=TextCfg->ParamAsString("Address");
  IssNNTP->PktOrigAddress=TextCfg->ParamAsString("PktOrigAddress");
  IssNNTP->DefaultGroups=TextCfg->ParamAsString("DefaultGroups");
  IssNNTP->DefaultROGroups=TextCfg->ParamAsString("DefaultROGroups");
  IssNNTP->PktDir=TextCfg->ParamAsString("PktDir");
  IssNNTP->PostFlag=TextCfg->ParamAsString("PostFlag");
  IssNNTP->TearLine=TextCfg->ParamAsString("TearLine");
  IssNNTP->TryFoundTo=TextCfg->ExistParam("NNTPTryFoundTo");
  IssNNTP->TryFoundTo=TextCfg->ExistParam("NNTPTryFoundTo");
  IssNNTP->NeedSeenBy=TextCfg->ExistParam("NeedSeenBy");
  IssNNTP->DescriptionsInAnsi=TextCfg->ExistParam("NNTPDescriptionsInAnsi");
  IssNNTP->HeadersTypeCode=TextCfg->ExistParam("PlainTextInHeaders") ? htcPlain:htcQuotedPrintable;
  IssNNTP->Origin=TextCfg->ParamAsString("Origin");
  IssNNTP->DefaultInternetGate=TextCfg->ParamAsString("DefaultInternetGate");
  IssNNTP->NumSameConnections=TextCfg->ParamAsInt("NNTPNumSameConnections");
  IssNNTP->AreasMasquerade=TextCfg->ParamAsString("AreasMasquerade");

  delimiter=TextCfg->ParamAsString("NameDelimiter");
  if(delimiter!="")
  {
        IssNNTP->NameDelimiter=delimiter[1];
  }
/*
  if(TextCfg->ExistParam("NNTPMaxMessageSize"))
  {
    IssNNTP->MaxMsgSize=TextCfg->ParamAsInt("NNTPMaxMessageSize");
  }
*/
  IssNNTP->ScriptingEnable=TextCfg->ExistParam("EnableWshSupport");
  IssNNTP->WshTimeout=TextCfg->ParamAsInt("WshTimeout");

  AnsiString asAreasCfg=TextCfg->ParamAsString("AreasCfg");
  if(asAreasCfg=="")
    asAreasCfg=TextCfg->ParamAsString("SquishCfg");
  if(asAreasCfg=="")
  {
    asAreasCfg=TextCfg->ParamAsString("FastechoCfg");
    IssNNTP->AreasConfigType=acFastEcho;
  }
  else
  {
    IssNNTP->AreasConfigType=acSquish;
  }
  IssNNTP->SquishCfgName=asAreasCfg;
  IssNNTP->UsersCfgName=TextCfg->ParamAsString("UsersCfg");
  IssNNTP->PlainFido=TextCfg->ExistParam("PlainFido");
  TraceS("");
}
//---------------------------------------------------------------------------
void __fastcall ReadSMTPServerConfiguration(TiuIssSMTPServer *IssSMTP,TTextCfg *TextCfg)
{
  TraceS("");
//  IssSMTP->u
  IssSMTP->RemoteReadRequestFlags=TextCfg->ParamAsString("RemoteReadRequestFlags");
  IssSMTP->RemoteRequest=TextCfg->ParamAsString("RemoteRequest");
  IssSMTP->UsersCfgName=TextCfg->ParamAsString("UsersCfg");
  IssSMTP->LogName=TextCfg->ParamAsString("SMTPLogFileName");
  IssSMTP->LogLevel=TextCfg->ParamAsInt("SMTPLogLevel");
  IssSMTP->Port=TextCfg->ParamAsInt("SMTPPort");
  IssSMTP->TimeOut=TextCfg->ParamAsInt("ThreadTimeout")*1000;
  IssSMTP->ThreadCacheSize=TextCfg->ParamAsInt("ThreadCacheSize");
  IssSMTP->FTNAddress=TextCfg->ParamAsString("Address");
  IssSMTP->PktOrigAddress=TextCfg->ParamAsString("PktOrigAddress");
  IssSMTP->PktDir=TextCfg->ParamAsString("PktDir");
  IssSMTP->PostFlag=TextCfg->ParamAsString("PostFlag");
  IssSMTP->TearLine=TextCfg->ParamAsString("TearLine");
  IssSMTP->Origin=TextCfg->ParamAsString("Origin");
  IssSMTP->DefaultInternetGate=TextCfg->ParamAsString("DefaultInternetGate");
  IssSMTP->NumSameConnections=TextCfg->ParamAsInt("SMTPNumSameConnections");
  IssSMTP->ForceINTL=TextCfg->ExistParam("ForceINTL");
  IssSMTP->AuthRequired=TextCfg->ExistParam("SMTPAuthRequired");
  IssSMTP->ScriptingEnable=TextCfg->ExistParam("EnableWshSupport");
  if(TextCfg->ExistParam("SMTPMaxMessageSize"))
  {
    IssSMTP->MaxMsgSize=TextCfg->ParamAsInt("SMTPMaxMessageSize");
  }
  IssSMTP->WshTimeout=TextCfg->ParamAsInt("WshTimeout");
  IssSMTP->PlainFido=TextCfg->ExistParam("PlainFido");
  TraceS("");
}
//---------------------------------------------------------------------------
void __fastcall ReadPOP3ServerConfiguration(TiuIssPOP3Server *IssPOP3,TTextCfg *TextCfg)
{
  AnsiString delimiter;

  TraceS("");
  IssPOP3->UsersCfgName=TextCfg->ParamAsString("UsersCfg");
  IssPOP3->FTNAddress=TextCfg->ParamAsString("Address");
  IssPOP3->LogName=TextCfg->ParamAsString("POP3LogFileName");
  IssPOP3->LogLevel=TextCfg->ParamAsInt("POP3LogLevel");
  IssPOP3->Port=TextCfg->ParamAsInt("POP3Port");
  IssPOP3->TimeOut=TextCfg->ParamAsInt("ThreadTimeout")*1000;
  IssPOP3->ThreadCacheSize=TextCfg->ParamAsInt("ThreadCacheSize");
  IssPOP3->BoxesPath=TextCfg->ParamAsString("POP3MailBox");
  IssPOP3->TrashPath=TextCfg->ParamAsString("POP3TrashFolder");
  IssPOP3->HeadersTypeCode=TextCfg->ExistParam("PlainTextInHeaders") ? htcPlain:htcQuotedPrintable;
  IssPOP3->BBSMode=TextCfg->ExistParam("POP3BBSMode");
  IssPOP3->NumSameConnections=TextCfg->ParamAsInt("POP3NumSameConnections");
  IssPOP3->ScriptingEnable=TextCfg->ExistParam("EnableWshSupport");
  IssPOP3->WshTimeout=TextCfg->ParamAsInt("WshTimeout");
  IssPOP3->HoldFlagsMask=TextCfg->ParamAsInt("POP3HoldFlagsMask");
  IssPOP3->NameDelimiter=TextCfg->ParamAsString("NameDelimiter")[1];

  delimiter=TextCfg->ParamAsString("NameDelimiter");
  if(delimiter!="")
  {
        IssPOP3->NameDelimiter=delimiter[1];
  }
  TraceS("");
}
//----------------------- ----------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TisMain::TisMain(void)
{


}
//---------------------------------------------------------------------------
__fastcall TisMain::~TisMain(void)
{
  SquishCfg->Free();
  TextCfg->Free();
  IssSMTP->Free();
  IssNNTP->Free();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
