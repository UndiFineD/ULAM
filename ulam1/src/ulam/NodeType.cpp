#include <stdlib.h>
#include "NodeType.h"
#include "CompilerState.h"


namespace MFM {

  NodeType::NodeType(Token typetoken, UTI auti, CompilerState & state) : Node(state),  m_typeTok(typetoken), m_ready(false), m_unknownBitsizeSubtree(NULL), m_unknownArraysizeSubtree(NULL){ }

  NodeType::NodeType(const NodeType& ref) : Node(ref), m_typeTok(ref.m_typeTok), m_ready(false), m_unknownBitsizeSubtree(NULL), m_unknownArraysizeSubtree(NULL)
  {
    if(ref.m_unknownBitsizeSubtree)
      m_unknownBitsizeSubtree = new NodeTypeBitsize(*ref.m_unknownBitsizeSubtree); //mapped UTI???

    if(ref.m_unknownArraysizeSubtree)
      m_unknownArraysizeSubtree = new NodeSquareBracket(*ref.m_unknownArraysizeSubtree); //mappedUTI???
  }

  NodeType::~NodeType()
  {
    delete m_unknownBitsizeSubtree;
    m_unknownBitsizeSubtree = NULL;

    delete m_unknownArraysizeSubtree;
    m_unknownArraysizeSubtree = NULL;
  } //destructor

  Node * NodeType::instantiate()
  {
    return new NodeType(*this);
  }

  void NodeType::updateLineage(NNO pno)
  {
    setYourParentNo(pno);
  }//updateLineage

  bool NodeType::findNodeNo(NNO n, Node *& foundNode)
  {
    return Node::findNodeNo(n, foundNode);
  } //findNodeNo

  void NodeType::printPostfix(File * fp)
  {
    fp->write(getName());
  }

  const char * NodeType::getName()
  {
    return m_state.getTokenDataAsString(&m_typeTok).c_str();
  } //getName

  const std::string NodeType::prettyNodeName()
  {
    return nodeName(__PRETTY_FUNCTION__);
  }

  void NodeType::linkConstantExpressionBitsize(NodeTypeBitsize * ceForBitSize)
  {
    assert(!m_unknownBitsizeSubtree);
    m_unknownBitsizeSubtree = ceForBitSize;
  } //linkConstantExpressionBitsize

  void NodeType::linkConstantExpressionArraysize(NodeSquareBracket * ceForArraySize)
  {
    //tfr owner, or deletes if dup or anothertd ???
    assert(!m_unknownArraysizeSubtree);
    m_unknownArraysizeSubtree = ceForArraySize;
  } //linkConstantExpressionArraysize

  bool NodeType::isReadyType()
  {
    return m_ready;
  }

  UTI NodeType::checkAndLabelType()
  {
    if(isReadyType())
      return getNodeType();

    UTI it = Nav;
    if(resolveType(it))
      {
	m_ready = true; // set here
      }

    setNodeType(it);
    return getNodeType();
  } //checkAndLabelType

  bool NodeType::resolveType(UTI& rtnuti)
  {
    bool rtnb = false;
    if(isReadyType())
      {
	rtnuti = getNodeType();
	return true;
      }

    // not node select, we are the leaf Type: a typedef, class or primitive scalar.
    UTI nuti = getNodeType();
    UTI mappedUTI = nuti;
    UTI cuti = m_state.getCompileThisIdx();

    // the symbol associated with this type, was mapped during instantiation
    // since we're call AFTER that (not during), we can look up our
    // new UTI and pass that on up the line of NodeType Selects, if any.
    if(m_state.mappedIncompleteUTI(cuti, nuti, mappedUTI))
      nuti = mappedUTI;

    UlamType * nut = m_state.getUlamTypeByIndex(nuti);
    ULAMTYPE etype = nut->getUlamTypeEnum();
    if(etype == Class)
      {
	if(nut->isComplete())
	  {
	    rtnuti = nuti;
	    rtnb = true;
	  } //else we're not ready!!
      }
    else
      {
	//primitive with possible unknown bit size
	rtnb = resolveTypeBitsize(nuti);
	rtnuti = nuti;
      }
    return rtnb;
  } //resolveType

  bool NodeType::resolveTypeBitsize(UTI auti)
  {
    bool rtnb = true;
    if(m_unknownBitsizeSubtree)
      {
	s32 bs = UNKNOWNSIZE;
	UlamType * ut = m_state.getUlamTypeByIndex(auti);
	ULAMTYPE etype = ut->getUlamTypeEnum();

	//primitive with possible unknown bit sizes.
	rtnb = m_unknownBitsizeSubtree->getTypeBitSizeInParen(bs, etype); //eval
	if(rtnb)
	  {
	    delete m_unknownBitsizeSubtree;
	    m_unknownBitsizeSubtree = NULL;
	    m_state.setUTISizes(auti, bs, ut->getArraySize()); //update UlamType
	  }
      }
    return rtnb;
  } //resolveTypeBitsize

  bool NodeType::resolveTypeArraysize(UTI auti)
  {
    bool rtnb = true;
    if(m_unknownArraysizeSubtree)
      {
	s32 as = UNKNOWNSIZE;
	//array of primitives or classes
	rtnb = m_unknownArraysizeSubtree->getArraysizeInBracket(as); //eval
	if(rtnb && as != UNKNOWNSIZE)
	  {
	    delete m_unknownArraysizeSubtree;
	    m_unknownArraysizeSubtree = NULL;
	    m_state.setUTISizes(auti, m_state.getBitSize(auti), as); //update UlamType
	  }
      }
    return rtnb;
  } //resolveTypeArraysize

  void NodeType::countNavNodes(u32& cnt)
  {
    Node::countNavNodes(cnt);
  }

  bool NodeType::assignClassArgValueInStubCopy()
  {
    return true;
  }

  EvalStatus NodeType::eval()
  {
    assert(0);  //not in parse tree; part of Node's type
    return NORMAL;
  }

} //end MFM