//---------------------------------------------------------------------------
//  This module contain a classes for processing .cfg files
//
//
//    Copyright (c) 1999 Ivan Uskov (ivus@usa.net, 2:5055/101.3@fidonet)
//---------------------------------------------------------------------------
#include <vcl.h>

#pragma hdrstop

#include "Cfg.h"
#include "Strutils.hpp"
#include "FECFG146.H"
#include "log.h"
#include <inifiles.hpp>
#pragma package(smart_init)

//FILE * TLog::fLog=NULL;

bool __fastcall TAreaInfo::isPosting(TIniFile *inifile, TUserInfo &UserInfo, AnsiString TagName)
{
    AnsiString access;
    AnsiString deny;

    access=inifile->ReadString(TagName,"access","all");
    deny=inifile->ReadString(TagName,"deny","");
    UserInfo.Posting=false; //��-��������� ������� ��������

    if(access!="") //���� ����������
    {
        if(access.LowerCase().Pos("all")>0) //���� ���������� ��� �������� ����
            UserInfo.Posting=true;
        if(access.LowerCase().Pos(UserInfo.NAME.LowerCase())>0) //���� ���������� �� ������� ������� ������
            UserInfo.Posting=true;
    }

    if(deny!="") //���� ����������
    {
        if(deny.LowerCase().Pos("all")>0) //���� ���������� ��� �������� ����
            UserInfo.Posting=false;
        if(deny.LowerCase().Pos(UserInfo.NAME.LowerCase())>0) //���� ���������� �� ������� ������� ������
            UserInfo.Posting=false;
    }


    return UserInfo.Posting;
}

AnsiString __fastcall TAreaInfo::getInternalName(AnsiString postingName, AnsiString GroupName, TUserInfo &UserInfo)
{
    AnsiString OutName;
    AnsiString asTemp;
    int pos;

    UserInfo.Posting=true; //��-��������� ������� ��������

    if(this->MaskFileName=="")
        return postingName;

    OutName=postingName;
    UserInfo.Posting=true;

    TIniFile *maskfile=new TIniFile(this->MaskFileName);
    TStringList *sections=new TStringList();
    maskfile->ReadSections(sections);

    if(sections->Count>0)
    {
        for(int i=0;i<sections->Count;i++)
        {
            asTemp=this->getExternalName(sections->Strings[i],GroupName).UpperCase();
            if(asTemp==postingName.UpperCase())
            {
                OutName=sections->Strings[i];
                this->isPosting(maskfile,UserInfo,OutName);
                break;
            }
        }

    }

    delete maskfile;
    delete sections;
    return OutName.UpperCase();
}

__fastcall TAreaInfo::TAreaInfo()
{
    Format=afUnknown;
    Group=IsPassthru=MaxCount=MaxAge=0;
}

AnsiString __fastcall TAreaInfo::getExternalName(AnsiString tagName,AnsiString GroupName)
{
    AnsiString OutName;
    AnsiString tempstr;

    int FindPos;

    if (this->MaskFileName=="")
        return tagName;
    TIniFile *maskfile=new TIniFile(this->MaskFileName);

    TStringList *slSection=new TStringList();

    if(GroupName!="") //���� �������� ������
    {
        tempstr=maskfile->ReadString("Group "+GroupName,"distrib","");
        if(tempstr!="") tagName=tempstr+"."+tagName;
    }


    maskfile->ReadSection(tagName,slSection);
    OutName=tagName;
    if (slSection->Count>0) //���� �������� �����������
    {

        tempstr=maskfile->ReadString(tagName,"distrib","");
        if(tempstr!="") OutName=tempstr+"."+OutName;


        tempstr=maskfile->ReadString(tagName,"areatag","");
        if(tempstr!="") OutName=tempstr;

     }

    delete maskfile;
    delete slSection;

    return OutName;
}


//---------------------------------------------------------------------------
namespace Cfg
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[] = {__classid(TSquishCfg),
                                  __classid(TFastechoCfg),
                                  __classid(TTextCfg),
                                  __classid(TScriptCfg),
                                  __classid(TCustomUsersCfg)
                                  };
     RegisterComponents("FIDO Tech", classes, 4);
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TCustomCfg::TCustomCfg(TComponent* Owner)
  : TComponent(Owner)
{
TraceS(__FUNC__);
  FCfg=NULL;
  FFileMode="r";
}
//---------------------------------------------------------------------------
__fastcall TCustomCfg::~TCustomCfg()
{
}
//---------------------------------------------------------------------------
void __fastcall TCustomCfg::SetPath(AnsiString Value)
{
TraceS(__FUNC__);
  if(Value!=FPath)
  {
  char *Path;
  TraceS(__FUNC__);
    FDiskLetter="";
    FPath=Trim(Value);
    Path=FPath.c_str();
    if(Path[0] && Path[1])
    {
    TraceS(__FUNC__);
      if(Path[1]==':')
        FDiskLetter=AnsiString(Path,1);
      else if(!((Path[0]=='\\') && (Path[1]=='\\')))
      {
      TraceS(__FUNC__);
        FDiskLetter=GetCurrentDir();
        if(FDiskLetter.c_str()[1]==':')
          FDiskLetter.SetLength(1);
        else
          FDiskLetter="";
      }
      TraceS(__FUNC__);
    }
    iLastWriteTime=0;
  }
  Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TCustomCfg::OpenFile(void)
{
TraceS(__FUNC__);
  FCfg=fopen(FPath.c_str(),FFileMode.c_str());
  if(!FCfg)
    throw Exception("Unable open file '"+FPath+"'");
    int iFileAge=FileAge(FPath);
    if(iFileAge==-1)
      throw Exception("Unable get age of file: "+FPath);
    FScanNeed=iLastWriteTime!=iFileAge;
    if(FScanNeed)
    {
//      FScanNeed=true;
      iLastWriteTime=iFileAge;
      TraceS(__FUNC__);
    }
//  else
//    FScanNeed=false;
  TraceS(__FUNC__);
}
//---------------------------------------------------------------------------
void __fastcall TCustomCfg::CloseFile(void)
{
TraceS(__FUNC__);
  if(FCfg)
  {
  TraceS(__FUNC__);
    fclose(FCfg);
    FCfg=NULL;
  }
TraceS(__FUNC__);
}
//---------------------------------------------------------------------------
bool __fastcall TCustomCfg::TrimComments(char* &Line)
{
    for(register int i=0;Line[i];i++)
      if(Line[i]==';')
      {
        Line[i]=0;
        break;
      }
    if(*Line)
      for(;*Line;Line++)
        if(*Line>32)//���������� ��������� ���������� �������/���������
          break;
  return (bool)(*Line);
}
//---------------------------------------------------------------------------
void __fastcall TCustomCfg::Scan(FILE * stream)
{
TraceS(__FUNC__);
char cBuf[MAX_LINE_SIZE+1], *pcCfgLine;
  while(fgets(cBuf,MAX_LINE_SIZE,stream))
  {
    pcCfgLine=cBuf;
    if(TrimComments(pcCfgLine))
      ImportCfgLine(pcCfgLine);
  }
TraceS(__FUNC__);
}
//---------------------------------------------------------------------------
void __fastcall TCustomCfg::Refresh(void)
{
TraceS(__FUNC__);
    OpenFile();
    try
    {
      if(FScanNeed)
      {
        Clear();
        Scan(FCfg);
      }
      if(FOnRefresh)
        FOnRefresh(this);
      ;
    }
    __finally
    {
      CloseFile();
    }
TraceS(__FUNC__);
}
//---------------------------------------------------------------------------
AnsiString __fastcall GetToSpace(char *Line)
{
TraceS(__FUNC__);
char cBuf[(MAX_LINE_SIZE+1)];
int i;
  for(i=0;*Line>32;i++,Line++)
    cBuf[i]=*Line;
  cBuf[i]=0;
TraceS(__FUNC__);
  return AnsiString(cBuf);
}
//---------------------------------------------------------------------------
AnsiString __fastcall GetToSpaceOrQuoted(char *Line,char sym)
{
TraceS(__FUNC__);
  if(Line[0]!=sym)
    return GetToSpace(Line);
  else
  {
  char cBuf[(MAX_LINE_SIZE+1)];
  int i;
    Line++;
    for(i=0;(*Line!=sym)&&(*Line);i++,Line++)
      cBuf[i]=*Line;
  cBuf[i]=0;
TraceS(__FUNC__);
  return AnsiString(cBuf);
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TTextCfg::TTextCfg(TComponent* Owner):TCustomCfg(Owner)
{
  FLines=new TStringList();
  FParamLists=new TStringList();
  FParamLists->Sorted=true;
  FParamLists->Duplicates=dupIgnore;
}
//---------------------------------------------------------------------------
__fastcall TTextCfg::~TTextCfg()
{
  FLines->Free();
  ClearParamLists();
  FParamLists->Free();
}
//---------------------------------------------------------------------------
void __fastcall TTextCfg::ClearParamLists()
{
  for(int i=0;i<FParamLists->Count;i++)
    ((TStringList *)FParamLists->Objects[i])->Free();
  FParamLists->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TTextCfg::Clear()
{
  FLines->Clear();
  ClearParamLists();
}
//---------------------------------------------------------------------------
void __fastcall TTextCfg::ImportCfgLine(char *Line)
{
int j;
  for(j=0;Line[j];j++)
    if(Line[j]<=32)
      break;
  CharUpperBuff(Line,j);
  FLines->Add(AnsiString(Line).TrimRight());
}
//---------------------------------------------------------------------------
int __fastcall TTextCfg::ExistParam(AnsiString ParamName)
{
int iResult=0;
  for(int i=0;i<FLines->Count;i++)
    if(FLines->Strings[i].Pos(ParamName.UpperCase())==1)
    {
      iResult=i+1;
      break;
    }
  return iResult;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TTextCfg::ParamAsString(AnsiString ParamName)
{
int index;
AnsiString Result="";
  index=ExistParam(ParamName);
  if(index)
  {
  char *cPtr=FLines->Strings[index-1].c_str();
    for(;*cPtr;cPtr++)
      if(*cPtr<=32)
        break;
    Result=AnsiString(cPtr).TrimLeft();
  }
  return Result;
}
//---------------------------------------------------------------------------
int __fastcall TTextCfg::ParamAsInt(AnsiString ParamName)
{
  return ParamAsString(ParamName).ToIntDef(0);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TStringList* __fastcall TTextCfg::ParamsList(AnsiString ListName)
{
int Index;
TStringList *ParamsList=NULL;
  ListName=ListName.UpperCase();
  if(FParamLists->Find(ListName,Index))
    ParamsList=(TStringList *)FParamLists->Objects[Index];
  else
  {
    Index=ExistParam(ListName);
    if(Index)
    {
      ParamsList=new TStringList;
      ParamsList->Add(AnsiString(FLines->Strings[Index].c_str()+ListName.Length()+1).TrimLeft());
      for(int i=0;i<FLines->Count;i++)
        if(FLines->Strings[i].Pos(ListName)==1)
          ParamsList->Add(AnsiString(FLines->Strings[i].c_str()+ListName.Length()+1).TrimLeft());
      FParamLists->AddObject(ListName,ParamsList);
      return ParamsList;
    }
  }
  return ParamsList;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TCustomUsersCfg::TCustomUsersCfg(TComponent* Owner):TCustomCfg(Owner)
{
  FUsers=new TListOfX<TUserInfo>;
}
//---------------------------------------------------------------------------
__fastcall TCustomUsersCfg::~TCustomUsersCfg()
{
  FUsers->Free();
}
//---------------------------------------------------------------------------
void __fastcall TCustomUsersCfg::Clear()
{
  FUsers->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TCustomUsersCfg::ImportCfgLine(char *Line)
{
//char cBuf[MAX_LINE_SIZE+1];
char *TagBuf="link";
int i;
  if(Line[4]>32)
    return;
  for(i=0;i<4;i++)
    if(TagBuf[i]!=(Line[i]|0x20))
      return;

  TUserInfo* UserInfo= new TUserInfo;
  UserInfo->Address.AsString=GetToSpace(Line+5);
  for(i=6;Line[i];i++)//���������!
  {
    if(Line[i]<=32)
      continue;
//    if(Line[i]<'0')
//    {
      switch(Line[i++])
      {
        case '~':
          UserInfo->Name=GetToSpaceOrQuoted(Line+i,'"');
          break;
        case '#':
          UserInfo->Password=GetToSpace(Line+i);
          break;
        case '$':
          if(Line[i]=='-')
          {
            i++;
            UserInfo->ROGroups=GetToSpace(Line+i);
          }
          else
          {
            UserInfo->Groups=GetToSpace(Line+i);
          }
          break;
        case '&':
          UserInfo->CharsetName=GetToSpace(Line+i);
          break;
      }
//    }
  }

  if(UserInfo->Name!="")
    FUsers->Add(UserInfo);
  else
    delete UserInfo;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TCustomAreasCfg::TCustomAreasCfg(TComponent* Owner)
  : TCustomCfg(Owner)
{
TraceS(__FUNC__);
  FBadArea=new TAreaInfo;
  FDupeArea=new TAreaInfo;

  FNetAreas= new TListOfX<TAreaInfo>;
  FLocalAreas= new TListOfX<TAreaInfo>;
  FEchoAreas= new TListOfX<TAreaInfo>;
TraceS(__FUNC__);
}
//---------------------------------------------------------------------------
__fastcall TCustomAreasCfg::~TCustomAreasCfg()
{
  FNetAreas->Free();
  FLocalAreas->Free();
  FEchoAreas->Free();
  FBadArea->Free();
  FDupeArea->Free();
}
//---------------------------------------------------------------------------
void __fastcall TCustomAreasCfg::Clear(void)
{
TraceS(__FUNC__);
  FNetAreas->Clear();
  FLocalAreas->Clear();
  FEchoAreas->Clear();
  FBadArea->Tag=FBadArea->Path="";
  FDupeArea->Tag=FDupeArea->Path="";
TraceS(__FUNC__);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TSquishCfg::TSquishCfg(TComponent* Owner)
  : TCustomAreasCfg(Owner)
{
TraceS(__FUNC__);
}
//---------------------------------------------------------------------------
__fastcall TSquishCfg::~TSquishCfg()
{
TraceS(__FUNC__);
}
//---------------------------------------------------------------------------
void __fastcall TSquishCfg::ImportCfgLine(char *Line)
{
TraceS(__FUNC__);
        CharUpperBuff(Line,9);
        if(!strncmp(Line,"ECHOAREA",8))
          AddArea(EchoAreas,Line);
        else
          if(!strncmp(Line,"NETAREA",7))
            AddArea(NetAreas,Line);
          else
            if(!strncmp(Line,"LOCALAREA",9))
              AddArea(LocalAreas,Line);
            else
              if(!strncmp(Line,"BADAREA",7))
                SetArea(BadArea,Line);
              else
                if(!strncmp(Line,"DUPEAREA",8))
                  SetArea(DupeArea,Line);
TraceS(__FUNC__);
}
//---------------------------------------------------------------------------
/*
void __fastcall TSquishCfg::Scan(FILE * stream)
{
  int i;
  char cBuf[MAX_LINE_SIZE+1], *pcCfgLine;
    while(fgets(cBuf,MAX_LINE_SIZE,stream))
    {
      for(i=0;i<AREAS_SCAN_LEN;i++)
        if((cBuf[i]==';') || !cBuf[i])
          break;// ��� �������� ��������� �������� ������ � �����������
      if(i==AREAS_SCAN_LEN)
      {
        for(pcCfgLine=cBuf;*pcCfgLine;pcCfgLine++)
          if(*pcCfgLine>32)//���������� ��������� ���������� �������/���������
            break;
        CharUpperBuff(pcCfgLine,9);
        if(!strncmp(pcCfgLine,"ECHOAREA",8))
          AddArea(EchoAreas,pcCfgLine);
        else
          if(!strncmp(pcCfgLine,"NETAREA",7))
            AddArea(NetAreas,pcCfgLine);
          else
            if(!strncmp(pcCfgLine,"LOCALAREA",9))
              AddArea(LocalAreas,pcCfgLine);
            else
              if(!strncmp(pcCfgLine,"BADAREA",7))
                SetArea(BadArea,pcCfgLine);
              else
                if(!strncmp(pcCfgLine,"DUPEAREA",8))
                  SetArea(DupeArea,pcCfgLine);
      }
    }
}
*/
//---------------------------------------------------------------------------
void __fastcall TSquishCfg::AddArea(TListOfX<TAreaInfo> *AreaList, char *pcLine)
{
  TAreaInfo *Area=new TAreaInfo;
  SetArea(Area,pcLine);
  AreaList->Add(Area);
}
//---------------------------------------------------------------------------
char * __fastcall GetNextWord(char* &pcLine)
{
char *pcResult=pcLine;
  while(*pcLine>32) pcLine++; //���������� ������� �����...
  *(pcLine++)='\0';           //...� ������ � ����� ���� '0'
  while(*pcLine<33)  //���������� ��������� �������, ������������ �����
    if(!*(++pcLine)) //������� ��������� �� ������ ���������� �����
      break;
  return pcResult;
}
//---------------------------------------------------------------------------
char * __fastcall GetNextWord2(char* &pcLine)
{
char *pcResult=pcLine;
  while(*pcLine>34 ) pcLine++; //���������� ������� �����...
  if(*pcLine=='"')            //���� ���������� �� �����, ���� ������
  {
    pcLine++;
    while(*pcLine && (*pcLine!='"')) pcLine++;
//    if(*pcLine=='"')pcLine++;
  }
  *(pcLine++)='\0';           //...� ������ � ����� ���� '0'
  while(*pcLine<33)  //���������� ��������� �������, ������������ �����
    if(!*(++pcLine)) //������� ��������� �� ������ ���������� �����
      break;
  return pcResult;
}
//---------------------------------------------------------------------------
void __fastcall TSquishCfg::SetArea(TAreaInfo *Area, char *pcLine)
{
char *pcCurrent;
  GetNextWord(pcLine);
  Area->Tag=AnsiString(GetNextWord(pcLine));
  if (FTagLowerCase)
    Area->Tag=Area->Tag.LowerCase();
  AnsiString Path=AnsiString(GetNextWord(pcLine));
  if(Path.AnsiCompareIC("Passthrough"))
  {
    Area->Path=Path;
    pcCurrent=Area->Path.c_str();
    if((pcCurrent[1]!=':') && (pcCurrent[1]!='\\'))
      Area->Path=FDiskLetter+":"+Area->Path;
  }
  else
  {
    Area->Path="";
    Area->IsPassthru=true;
  }
  while(*(pcCurrent=GetNextWord2(pcLine)))
  {
    if(!strcmp(pcCurrent,"-$") )
      Area->Format=afSquish;
    if(!strcmp(pcCurrent,"-j") )
      Area->Format=afJAM;
    if(!strcmp(pcCurrent,"-0") )
      Area->IsPassthru=true;

    if(!strcmp(pcCurrent,"-g"))
    {
      pcCurrent=GetNextWord(pcLine);
      Area->Group=pcCurrent[0];
      continue;//HPT
    }
    if(!strcmp(pcCurrent,"-b"))
    {
      if(!GetToSpace(GetNextWord(pcLine)).AnsiCompareIC("Squish"))
        Area->Format=afSquish;
      continue;//HPT
    }
    if(!strnicmp(pcCurrent,"-$g",3))
    {
      if(pcCurrent[3]=='#')//��������� IMail 31.05.2000
      {
        char Buf[4];
        *((int *) Buf)=0;
        for(int i=0, j=4;i<3;i++,j++)
        {
          if(isdigit(pcCurrent[j]))
            Buf[i]=pcCurrent[j];
          else
            break;
        }
        Area->Group=atoi(Buf);
      }
      else
        Area->Group=pcCurrent[3];
    }

    if(!strncmp(pcCurrent,"-$n",3))
    {
      Area->Description=AnsiString(pcCurrent+(pcCurrent[3]=='"' ? 4:3));
    }
  }
  if(Area->Description.IsEmpty())
    Area->Description=Area->Tag;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TFastechoCfg::TFastechoCfg(TComponent* Owner)
  : TCustomAreasCfg(Owner)
{
  FFileMode="rb";
}
//---------------------------------------------------------------------------
__fastcall TFastechoCfg::~TFastechoCfg()
{
}
//---------------------------------------------------------------------------
/*
void __fastcall TFastechoCfg::ImportCfgLine(char *Line)
{
        CharUpperBuff(Line,9);
        if(!strncmp(Line,"ECHOAREA",8))
          AddArea(EchoAreas,Line);
        else
          if(!strncmp(Line,"NETAREA",7))
            AddArea(NetAreas,Line);
          else
            if(!strncmp(Line,"LOCALAREA",9))
              AddArea(LocalAreas,Line);
            else
              if(!strncmp(Line,"BADAREA",7))
                SetArea(BadArea,Line);
              else
                if(!strncmp(Line,"DUPEAREA",8))
                  SetArea(DupeArea,Line);

}
*/
//---------------------------------------------------------------------------
void __fastcall TFastechoCfg::Scan(FILE * stream)
{
CONFIGURATION feConfiguration;
//Node feNode;
static Area feArea;
static size_t size;
  size=fread(&feConfiguration, sizeof(CONFIGURATION),1, stream);
  if(!size)
    throw Exception("Scan file error. Filename: "+Path);
  if(fseek(stream,feConfiguration.offset+feConfiguration.NodeRecSize*feConfiguration.NodeCnt,SEEK_CUR))
    throw Exception("Scan file error. Filename: "+Path);
  for(int i=0;i<feConfiguration.AreaCnt;i++)
  {
    size=fread(&feArea, sizeof(Area), 1, stream);
    if(!size)
      throw Exception("Scan file error. Filename: "+Path);
    if(feArea.board==NO_BOARD)
    {
    static TAreaInfo *Area;
        Area=new TAreaInfo;
        Area->Tag=AnsiString(feArea.name);
        Area->Path=AnsiString(feArea.path);
        Area->Description=AnsiString(feArea.desc);
        Area->Format=(TAreaFormat)feArea.flags.storage;
        Area->Group=feArea.info.group+'A';
        Area->MaxCount=feArea.messages;
        Area->MaxAge=feArea.days;
        Area->IsPassthru=feArea.flags.storage==PASSTHRU;
        if(feArea.flags.atype==AREA_ECHOMAIL)
          EchoAreas->Add(Area);
        else if(feArea.flags.atype==AREA_LOCAL)
          LocalAreas->Add(Area);
        else if(feArea.flags.atype==AREA_NETMAIL)
          NetAreas->Add(Area);
    }
  }

/*
  char cBuf[MAX_LINE_SIZE+1], *pcCfgLine;
    while(fgets(cBuf,MAX_LINE_SIZE,stream))
    {
      for(i=0;i<AREAS_SCAN_LEN;i++)
        if((cBuf[i]==';') || !cBuf[i])
          break;// ��� �������� ��������� �������� ������ � �����������
      if(i==AREAS_SCAN_LEN)
      {
        for(pcCfgLine=cBuf;*pcCfgLine;pcCfgLine++)
          if(*pcCfgLine>32)//���������� ��������� ���������� �������/���������
            break;
        CharUpperBuff(pcCfgLine,9);
        if(!strncmp(pcCfgLine,"ECHOAREA",8))
          AddArea(EchoAreas,pcCfgLine);
        else
          if(!strncmp(pcCfgLine,"NETAREA",7))
            AddArea(NetAreas,pcCfgLine);
          else
            if(!strncmp(pcCfgLine,"LOCALAREA",9))
              AddArea(LocalAreas,pcCfgLine);
            else
              if(!strncmp(pcCfgLine,"BADAREA",7))
                SetArea(BadArea,pcCfgLine);
              else
                if(!strncmp(pcCfgLine,"DUPEAREA",8))
                  SetArea(DupeArea,pcCfgLine);
      }
    }
    */
}
/*
//---------------------------------------------------------------------------
void __fastcall TFastechoCfg::AddArea(TListOfX<TAreaInfo> *AreaList, char *pcLine)
{
  TAreaInfo *Area=new TAreaInfo;
  SetArea(Area,pcLine);
  AreaList->Add(Area);
}
//---------------------------------------------------------------------------

void __fastcall TFastechoCfg::SetArea(TAreaInfo *Area, char *pcLine)
{
char *pcCurrent;
  GetNextWord(pcLine);
  Area->Tag=AnsiString(GetNextWord(pcLine));
  if (FTagLowerCase)
    Area->Tag=Area->Tag.LowerCase();
  Area->Path=AnsiString(GetNextWord(pcLine));
  while(*(pcCurrent=GetNextWord(pcLine)))
  {
    if(!strcmp(pcCurrent,"-$") )
      Area->Format=afSquish;
    if(!strcmp(pcCurrent,"-j") )
      Area->Format=afJAM;
    if(!strcmp(pcCurrent,"-0"))
      Area->IsPassthru=true;
    if(!strnicmp(pcCurrent,"-$g",3))
      Area->Group=pcCurrent[3];

//    if(!strncmp(pcCurrent,"-$n",3))
//      Area->Description=AnsiString
  }
  if(Area->Description.IsEmpty())
    Area->Description=Area->Tag;
}
*/
//---------------------------------------------------------------------------

__fastcall TScriptCfg::TScriptCfg(TComponent* Owner):TCustomCfg(Owner)
{
  FInstructionList=new TListOfX<TScriptInstruction>;
}
//---------------------------------------------------------------------------
__fastcall TScriptCfg::~TScriptCfg()
{
  delete FInstructionList;
}
//---------------------------------------------------------------------------
void __fastcall TScriptCfg::Clear()
{
  FInstructionList->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TScriptCfg::ImportCfgLine(char *Line)
{
static const char cTagBuf[]="WSHFUNCTION";
//int i;
  CharUpperBuff(Line,11);
  if(strstr(Line,cTagBuf)!=Line)
    return;
//����������� sizeof...
//  for(i=sizeof(cTagBuf);(Line[i]==32)||(Line[i]==9);i++)
   // ;//������ �� ������, ������ ���������� �������
   AnsiString TargetLine=AnsiString(Line+12);

  FInstructionList->Add(new TScriptInstruction(Trim(AnsiString(Line+12))));
}
//---------------------------------------------------------------------------
TScriptInstruction* __fastcall TScriptCfg::InstructionByName(AnsiString InstructionName)
{
  for(int i=0;i<FInstructionList->Count;i++)
  {
    if(!FInstructionList->Items[i]->InstructionName.AnsiCompareIC(InstructionName))
    {
      return FInstructionList->Items[i];
    }
  }
  return NULL;
}
//---------------------------------------------------------------------------

