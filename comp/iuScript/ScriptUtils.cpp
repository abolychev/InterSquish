//---------------------------------------------------------------------------

#include <vcl.h>
#include <idcoder3to4.hpp>
#pragma hdrstop

#include "ScriptUtils.h"
#include "transliterates.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

AnsiString __fastcall DecodeBase64New(AnsiString value)
{
    AnsiString result;
    AnsiString input;

    TIdBase64Decoder *decoder;
    decoder=new TIdBase64Decoder(NULL); //.Create
    input=value.SubString(12,value.Length()-13);
    if(input.Pos("==")==0) input+="==";
    decoder->CodeString(input);
    result=decoder->CompletedInput();
    KOI2OEM(result.c_str(),result.c_str());

    delete decoder;
    return result.SubString(3,result.Length());
}
