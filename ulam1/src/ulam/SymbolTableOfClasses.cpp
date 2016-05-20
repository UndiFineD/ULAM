#include <assert.h>
#include <stdio.h>
#include <iostream>
#include "SymbolTableOfClasses.h"
#include "CompilerState.h"
#include "NodeBlockClass.h"

namespace MFM {

  SymbolTableOfClasses::SymbolTableOfClasses(CompilerState& state): SymbolTable(state) { }

  SymbolTableOfClasses::SymbolTableOfClasses(const SymbolTableOfClasses& ref) : SymbolTable(ref)  { }

  SymbolTableOfClasses::~SymbolTableOfClasses()  { }

  void SymbolTableOfClasses::getTargets(TargetMap& classtargets)
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    ((SymbolClassName *) sym)->getTargetDescriptorsForClassInstances(classtargets);
	  }
	it++;
      } //while
  } //getTargets

  void SymbolTableOfClasses::getClassMembers(ClassMemberMap& classmembers)
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    ((SymbolClassName *) sym)->getClassMemberDescriptionsForClassInstances(classmembers);
	  }
	it++;
      } //while
  } //getClassMembers

  void SymbolTableOfClasses::testForTableOfClasses(File * fp)
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();

    m_state.m_err.clearCounts();

    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    ((SymbolClassName *) sym)->testForClassInstances(fp);
	  }
	it++;
      } //while

    //output informational warning and error counts
    u32 warns = m_state.m_err.getWarningCount();
    if(warns > 0)
      {
	std::ostringstream msg;
	msg << warns << " warning" << (warns > 1 ? "s " : " ") << "during eval";
	MSG("", msg.str().c_str(), INFO);
      }

    u32 errs = m_state.m_err.getErrorCount();
    if(errs > 0)
      {
	std::ostringstream msg;
	msg << errs << " TOO MANY EVAL ERRORS";
	MSG("", msg.str().c_str(), INFO);
      }
  } //testForTableOfClasses

  void SymbolTableOfClasses::buildDefaultValuesFromTableOfClasses()
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    ((SymbolClassName *) sym)->buildDefaultValueForClassInstances(); //builds when not ready; must be qk
	  }
	it++;
      } //while
  } //buildDefaultValuesFromTableOfClasses

  void SymbolTableOfClasses::printPostfixForTableOfClasses(File * fp)
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    NodeBlockClass * classNode = ((SymbolClass *) sym)->getClassBlockNode();
	    assert(classNode);
	    m_state.pushClassContext(cuti, classNode, classNode, false, NULL);

	    classNode->printPostfix(fp);
	    m_state.popClassContext(); //restore
	  }
	it++;
      } //while
  } //printPostfixForTableOfClasses

  void SymbolTableOfClasses::printForDebugForTableOfClasses(File * fp)
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());
	NodeBlockClass * classNode = ((SymbolClass *) sym)->getClassBlockNode();
	assert(classNode);
	m_state.pushClassContext(sym->getUlamTypeIdx(), classNode, classNode, false, NULL);

	classNode->print(fp);
	m_state.popClassContext(); //restore
	it++;
      } //while
  } //printForDebugForTableOfClasses

  // adds unknown type names as incomplete classes if still "hzy" after parsing done
  bool SymbolTableOfClasses::checkForUnknownTypeNamesInTableOfClasses()
  {
    bool aok = true;
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());
	UTI suti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(suti))
	  {
	    aok &= ((SymbolClassName *) sym)->statusUnknownTypeNamesInClassInstances();
	  }
	it++;
      } //while
    return aok;
  } //checkForUnknownTypeNamesInTableOfClasses

  bool SymbolTableOfClasses::statusNonreadyClassArgumentsInTableOfClasses()
  {
    bool aok = true;
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());
	if(((SymbolClassName *) sym)->isClassTemplate())
	  aok &= ((SymbolClassNameTemplate *) sym)->statusNonreadyClassArgumentsInStubClassInstances();
	it++;
      } //while
    return aok;
  } //statusNonreadyClassArgumentsInTableOfClasses

  bool SymbolTableOfClasses::fullyInstantiateTableOfClasses()
  {
    bool aok = true;
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());
	if(((SymbolClassName *) sym)->isClassTemplate())
	  aok &= ((SymbolClassNameTemplate *) sym)->fullyInstantiate();
	it++;
      } //while
    return aok;
  } //fullyInstantiateTableOfClasses

  //done after cloning and before checkandlabel;
  //blocks without prevblocks set, are linked to prev block;
  //used for searching for missing symbols in STs during c&l.
  void SymbolTableOfClasses::updateLineageForTableOfClasses()
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    ((SymbolClassName *) sym)->updateLineageOfClass();
	    //only regular and templates immediately after updating lineages
	    // not just for efficiency; helps resolves types
	    ((SymbolClassName *) sym)->checkAndLabelClassFirst();
	  }
	it++;
      } //while
  } //updateLineageForTableOfClasses

  void SymbolTableOfClasses::checkCustomArraysForTableOfClasses()
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());

	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    ((SymbolClassName *) sym)->checkCustomArraysOfClassInstances();
	  }
	it++;
      }
  } //checkCustomArraysForTableOfClasses()

  void SymbolTableOfClasses::checkDuplicateFunctionsForTableOfClasses()
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    ((SymbolClassName *) sym)->checkDuplicateFunctionsForClassInstances();
	  }
	it++;
      }
  } //checkDuplicateFunctionsForTableOfClasses

  void SymbolTableOfClasses::calcMaxDepthOfFunctionsForTableOfClasses()
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());

	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    ((SymbolClassName *) sym)->calcMaxDepthOfFunctionsForClassInstances();
	  }
	it++;
      }
  } //calcMaxDepthOfFunctionsForTableOfClasses

  bool SymbolTableOfClasses::calcMaxIndexOfVirtualFunctionsForTableOfClasses()
  {
    bool aok = true;
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());

	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    aok &= ((SymbolClassName *) sym)->calcMaxIndexOfVirtualFunctionsForClassInstances();
	  }
	it++;
      }
    return aok;
  } //calcMaxIndexOfVirtualFunctionsForTableOfClasses

  void SymbolTableOfClasses::checkAbstractInstanceErrorsForTableOfClasses()
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    ((SymbolClassName *) sym)->checkAbstractInstanceErrorsForClassInstances();
	  }
	it++;
      }
  } //checkAbstractInstanceErrorsForTableOfClasses

  bool SymbolTableOfClasses::labelTableOfClasses()
  {
    m_state.clearGoAgain();

    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();

    while(it != m_idToSymbolPtr.end())
      {
	SymbolClassName * cnsym = (SymbolClassName *) (it->second);
	assert(cnsym->isClass());
	UTI cuti = cnsym->getUlamTypeIdx();
	if( ((SymbolClass *) cnsym)->getUlamClass() == UC_UNSEEN)
	  {
	    //skip anonymous classes
	    if(!m_state.isAnonymousClass(cuti))
	      {
		std::ostringstream msg;
		msg << "Unresolved type <";
		msg << m_state.getUlamTypeNameBriefByIndex(cuti).c_str();
		msg << "> was never defined; Fails labeling";
		MSG(cnsym->getTokPtr(), msg.str().c_str(), DEBUG); //was ERR but typedef junk; was WARN, but too many msgs when ERR with variable name suffices (error/t3370, t3492)
		cnsym->getClassBlockNode()->setNodeType(Nav); //for compiler counter
		//assert(0); wasn't a class at all, e.g. out-of-scope typedef/variable
		break;
	      }
	  }
	else
	  cnsym->checkAndLabelClassInstances();

	it++;
      }
    return (!m_state.goAgain() && (m_state.m_err.getErrorCount() + m_state.m_err.getWarningCount() == 0));
  } //labelTableOfClasses

  void SymbolTableOfClasses::countNavNodesAcrossTableOfClasses(u32& navcount, u32& hzycount, u32& unsetcount)
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();

    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    ((SymbolClassName *) sym)->countNavNodesInClassInstances(navcount, hzycount, unsetcount);
	  }
	it++;
      }
    return;
  } //countNavNodesAcrossTableOfClasses

  u32 SymbolTableOfClasses::reportUnknownTypeNamesAcrossTableOfClasses()
  {
    u32 totalcnt = 0;
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();

    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    totalcnt += ((SymbolClassName *) sym)->reportUnknownTypeNamesInClassInstances();
	  }
	it++;
      }
    return totalcnt;
  } //reportUnknownTypeNamesAcrossTableOfClasses

  //separate pass...after labeling all classes is completed;
  //purpose is to set the size of all the classes, by totalling the size
  //of their data members; returns true if all class sizes complete.
  bool SymbolTableOfClasses::setBitSizeOfTableOfClasses()
  {
    std::vector<u32> lostClassesIds;
    bool aok = true;
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	bool anonymousClass = m_state.isAnonymousClass(cuti);
	ULAMCLASSTYPE classtype = ((SymbolClass *) sym)->getUlamClass();
	if((classtype == UC_UNSEEN) || anonymousClass)
	  {
	    std::ostringstream msg;
	    msg << "Unresolved type <";
	    msg << m_state.getUlamTypeNameBriefByIndex(cuti).c_str();
	    msg << "> was never defined; Fails sizing";
	    if(anonymousClass)
	      MSG(sym->getTokPtr(), msg.str().c_str(), DEBUG);
	    else
	      MSG(sym->getTokPtr(), msg.str().c_str(), DEBUG); //was ERR but typedef junk; also catch at use!
	    //m_state.completeIncompleteClassSymbol(sym->getUlamTypeIdx()); //too late
	    aok = false; //moved here;
	  }
	else
	  {
	    aok = ((SymbolClassName *) sym)->setBitSizeOfClassInstances();
	  }

	//track classes that fail to be sized.
	if(!aok)
	  lostClassesIds.push_back(sym->getId());

	aok = true; //reset for next class
	it++;
      } //next class

    aok = lostClassesIds.empty();
    if(!aok)
      {
	std::ostringstream msg;
	msg << lostClassesIds.size() << " Classes without sizes";
	while(!lostClassesIds.empty())
	  {
	    u32 cid = lostClassesIds.back();
	    msg << ", " << m_state.m_pool.getDataAsString(cid).c_str();
	    lostClassesIds.pop_back();
	  }
	MSG(m_state.getFullLocationAsString(m_state.m_locOfNextLineText).c_str(), msg.str().c_str(),DEBUG);
      }
    lostClassesIds.clear();
    return aok;
  } //setBitSizeOfTableOfClasses

  //separate pass...after labeling all classes is completed;
  void SymbolTableOfClasses::printBitSizeOfTableOfClasses()
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    ((SymbolClassName *) sym)->printBitSizeOfClassInstances();
	  }
	it++;
      }
  } //printBitSizeOfTableOfClasses

  void SymbolTableOfClasses::packBitsForTableOfClasses()
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());

	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    //quark union keep default pos = 0 for each data member, hence skip packing bits.
	    if(!(m_state.isClassAQuarkUnion(cuti)))
	      {
		((SymbolClassName *) sym)->packBitsForClassInstances();
	      }
	  }
	it++;
      }
  } //packBitsForTableOfClasses

  void SymbolTableOfClasses::printUnresolvedVariablesForTableOfClasses()
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym && sym->isClass());

	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    ((SymbolClassName *) sym)->printUnresolvedVariablesForClassInstances();
	  }
	it++;
      }
  } //printUnresolvedVariablesForTableOfClasses

  //bypasses THIS class being compiled
  void SymbolTableOfClasses::generateIncludesForTableOfClasses(File * fp)
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    ((SymbolClassName *) sym)->generateIncludesForClassInstances(fp);
	  }
	it++;
      }
  } //generateIncludesForTableOfClasses

  //bypasses THIS class being compiled
  void SymbolTableOfClasses::generateForwardDefsForTableOfClasses(File * fp)
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    ((SymbolClassName *) sym)->generateForwardDefsForClassInstances(fp);
	  }
	it++;
      }
  } //generateForwardDefsForTableOfClasses

  enum { NORUNTEST = 0, RUNTEST = 1  };

  //test for the current compileThisIdx, with test method
  void SymbolTableOfClasses::generateTestInstancesForTableOfClasses(File * fp)
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    //first output all the element typedefs, skipping quarks
	    //if(((SymbolClass * ) sym)->getUlamClass() != UC_QUARK)
	    if(((SymbolClass * ) sym)->getUlamClass() == UC_ELEMENT)
	      ((SymbolClassName *) sym)->generateTestInstanceForClassInstances(fp, NORUNTEST);
	  }
	it++;
      } //while for typedefs only

    fp->write("\n");

    it = m_idToSymbolPtr.begin();
    s32 idcounter = 1;
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym->isClass());
	//next output all the element typedefs that are m_compileThisId; skipping quarks
	//if(sym->getId() == m_state.getCompileThisId() && ((SymbolClass * ) sym)->getUlamClass() != UC_QUARK)
	if(sym->getId() == m_state.getCompileThisId() && ((SymbolClass * ) sym)->getUlamClass() == UC_ELEMENT)
	  ((SymbolClassName *) sym)->generateTestInstanceForClassInstances(fp, RUNTEST);
	it++;
	idcounter++;
      } //while to run this test

    fp->write("\n");
    m_state.indent(fp);
    fp->write("return 0;\n");
  } //generateTestInstancesForTableOfClasses

  void SymbolTableOfClasses::genCodeForTableOfClasses(FileManager * fm)
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    //output header/body for this class next
	    ((SymbolClassName *) sym)->generateCodeForClassInstances(fm);
	  }
	it++;
      } //while
  } //genCodeForTableOfClasses

  UTI SymbolTableOfClasses::findClassNodeNoForTableOfClasses(NNO n)
  {
    std::map<u32, Symbol *>::iterator it = m_idToSymbolPtr.begin();
    while(it != m_idToSymbolPtr.end())
      {
	Symbol * sym = it->second;
	assert(sym->isClass());
	UTI cuti = sym->getUlamTypeIdx();
	//skip anonymous classes
	if(!m_state.isAnonymousClass(cuti))
	  {
	    NodeBlockClass * classblock = ((SymbolClassName *) sym)->getClassBlockNode();
	    if(classblock->getNodeNo() == n)
	      return sym->getUlamTypeIdx(); //found it!!
	  }
	it++;
      } //while
    return Nav;
  } //findClassNodeNoForTableOfClasses

} //end MFM