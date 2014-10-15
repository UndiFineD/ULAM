#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include "UlamTypeUnsigned.h"
#include "UlamValue.h"
#include "CompilerState.h"

namespace MFM {

  UlamTypeUnsigned::UlamTypeUnsigned(const UlamKeyTypeSignature key, const UTI uti) : UlamType(key, uti)
  {
    m_wordLength = calcWordSize(getTotalBitSize());
  }


   ULAMTYPE UlamTypeUnsigned::getUlamTypeEnum()
   {
     return Unsigned;
   }


  const std::string UlamTypeUnsigned::getUlamTypeVDAsStringForC()
  {
    return "VD::U32";
  }


  const std::string UlamTypeUnsigned::getUlamTypeImmediateMangledName(CompilerState * state)
  {
    if(needsImmediateType())
      return UlamType::getUlamTypeImmediateMangledName(state);

    return "u32";
  }


  bool UlamTypeUnsigned::needsImmediateType()
  {
    return ((getBitSize() == ANYBITSIZECONSTANT || getBitSize() == MAXBITSPERINT) ? false : true);
  }


  const char * UlamTypeUnsigned::getUlamTypeAsSingleLowercaseLetter()
  {
    return "u";
  }


  bool UlamTypeUnsigned::cast(UlamValue & val, CompilerState& state)
  {
    bool brtn = true;
    UTI typidx = getUlamTypeIndex();
    UTI valtypidx = val.getUlamValueTypeIdx();    
    s32 arraysize = getArraySize();
    if(arraysize != state.getArraySize(valtypidx))
      {
	std::ostringstream msg;
	msg << "Casting different Array sizes; " << arraysize << ", Value Type and size was: " << valtypidx << "," << state.getArraySize(valtypidx);
	state.m_err.buildMessage("", msg.str().c_str(),__FILE__, __func__, __LINE__, MSG_ERR);
	return false;
      }
    
    //change the size first of tobe, if necessary
    s32 bitsize = getBitSize();
    s32 valbitsize = state.getBitSize(valtypidx);

    //base types e.g. Int, Bool, Unary, Foo, Bar..
    ULAMTYPE typEnum = getUlamTypeEnum();
    ULAMTYPE valtypEnum = state.getUlamTypeByIndex(valtypidx)->getUlamTypeEnum();

    if((bitsize != valbitsize) && (typEnum != valtypEnum))
      {
	//change to val's size, within the TOBE current type; 
	//get string index for TOBE enum type string
	u32 enumStrIdx = state.m_pool.getIndexForDataString(UlamType::getUlamTypeEnumAsString(typEnum));
	UlamKeyTypeSignature vkey1(enumStrIdx, valbitsize, arraysize);
	UTI vtype1 = state.makeUlamType(vkey1, typEnum); //may not exist yet, create  
	
	if(!(state.getUlamTypeByIndex(vtype1)->cast(val,state))) //val changes!!!
	  {
	    //error! 
	    return false;
	  }
	
	valtypidx = val.getUlamValueTypeIdx();  //reload
	valtypEnum = state.getUlamTypeByIndex(valtypidx)->getUlamTypeEnum();
      }
	
    if(state.isConstant(typidx))
      {
	// avoid using out-of-band value as bitsize
	bitsize = state.getDefaultBitSize(typidx);
      }

    u32 data = val.getImmediateData(state);
    switch(valtypEnum)
      {
      case Int:
      case Unsigned:
      case Bits:
	// casting Int to Unsigned to change type
	// casting UnsignedInt to UnsignedInt to change bits size
	// casting to Bits Unsigned to change type
	val = UlamValue::makeImmediate(typidx, data, state); //overwrite val
	break;
      case Bool:
	{
	  if(state.isConstant(valtypidx))  // bitsize is misleading
	    {
	      if(data != 0) //signed or unsigned
		val = UlamValue::makeImmediate(typidx, 1, state); //overwrite val
	      else
		val = UlamValue::makeImmediate(typidx, 0, state); //overwrite val
	    }
	  else
	    {
	      s32 count1s = PopCount(data);
	      if(count1s > (s32) (valbitsize - count1s))
		val = UlamValue::makeImmediate(typidx, 1, state); //overwrite val
	      else
		val = UlamValue::makeImmediate(typidx, 0, state); //overwrite val
	    }
	}
	break;
      case Unary:
	{
	  u32 count1s = PopCount(data);
	  val = UlamValue::makeImmediate(typidx, count1s, state); //overwrite val
	}
	break;
      case Void:
      default:
	//std::cerr << "UlamTypeInt (cast) error! Value Type was: " << valtypidx << std::endl;
	brtn = false;
      };

    return brtn;
  } //end cast


  void UlamTypeUnsigned::getDataAsString(const u32 data, char * valstr, char prefix, CompilerState& state)
  {
    if(prefix == 'z')
      sprintf(valstr,"%u", data);
    else
      sprintf(valstr,"%c%u", prefix, data);
  }
  
} //end MFM
