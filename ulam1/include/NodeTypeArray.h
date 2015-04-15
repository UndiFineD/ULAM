/**                                        -*- mode:C++ -*-
 * NodeTypeArray.h - Basic Node handling of Node Type Arrays for ULAM
 *
 * Copyright (C) 2015 The Regents of the University of New Mexico.
 * Copyright (C) 2015 Ackleyshack LLC.
 *
 * This file is part of the ULAM programming language compilation system.
 *
 * The ULAM programming language compilation system is free software:
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * The ULAM programming language compilation system is distributed in
 * the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the ULAM programming language compilation system
 * software.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 */

/**
  \file NodeTypeArray.h - Basic Node handling of Node Type Arrays for ULAM
  \author Elenas S. Ackley.
  \author David H. Ackley.
  \date (C) 2015 All rights reserved.
  \gpl
*/


#ifndef NODETYPEARRAY_H
#define NODETYPEARRAY_H

#include "NodeType.h"
#include "NodeSquareBracket.h"

namespace MFM{

  class NodeTypeArray : public NodeType
  {
  public:

    NodeTypeArray(Token typetoken, UTI auti, NodeType * scalarnode, CompilerState & state);
    NodeTypeArray(const NodeTypeArray& ref);
    virtual ~NodeTypeArray();

    virtual Node * instantiate();

    virtual void updateLineage(NNO pno);

    virtual bool findNodeNo(NNO n, Node *& foundNode);

    virtual void printPostfix(File * f);

    virtual const char * getName();

    virtual const std::string prettyNodeName();

    void linkConstantExpressionArraysize(NodeSquareBracket * ceForArraySize);

    virtual UTI checkAndLabelType();

    virtual bool resolveType(UTI& rtnuti);

    bool resolveTypeArraysize(UTI auti);

    virtual void countNavNodes(u32& cnt);

    virtual bool assignClassArgValueInStubCopy();

    virtual EvalStatus eval();

  private:
    NodeType * m_nodeScalar;
    NodeSquareBracket * m_unknownArraysizeSubtree;

  };

} //MFM

#endif //NODETYPEARRAY_H