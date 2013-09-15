#include "tokendef.h"
#include "window.h"
#include "group.h"
#include "reduce.h"
#include "join.h"


/**
 * clone all pipelines within a EPN (PipelineGroup), 
 * setup the connectivity between pipelines 
 * (a) fan-out case via ConnectPointOperator
 * (b) fan-in case via JoinInputProxyOperator
 *
 * @return the cloned PipelineGroup
 */
ThreadedCommandsExecutor::PipelineGroupPtr ThreadedCommandsExecutor::clonePipelines() {
    PipelineGroupPtr theClonedGroup = new vector<OperatorPipeline*>();
    for (vector<OperatorPipeline*>::const_iterator iter = pPipelineList_->begin(); iter < pPipelineList_->end(); iter++) {
        theClonedGroup->push_back(cloneOnePipeline(*iter));
    }

    // establish connectivity for cloned ConnectOperators
    int i =0;
    for (vector<OperatorPipeline*>::const_iterator iter = pPipelineList_->begin(); iter < pPipelineList_->end(); iter++, i++) {
        OperatorPipeline* thePipeline = *iter;
        BaseOperator* theOp = thePipeline->back();

        // establish connectivity for ConnectPointOperators
        ConnectPointOperator* theConnectOp = dynamic_cast<ConnectPointOperator*>(theOp);
        if (theConnectOp != NULL) {
            // since the corresponding cloned pipleline have same operators as the original pipleline.
            // its last operator must be a ConnectPoint operator too.
            ConnectPointOperator* theClonedConnectOp = dynamic_cast<ConnectPointOperator*>((*theClonedGroup)[i]->back());
            theClonedConnectOp->fromPipeline_ = (*theClonedGroup)[i];

            for (vector<OperatorPipeline*>::const_iterator iter2 = theConnectOp->toPipelines_.begin(); iter2 < theConnectOp->toPipelines_.end(); iter2++) {
                int id = (*iter2)->front()->getOperatorId();
                for (vector<OperatorPipeline*>::const_iterator iter3 = theClonedGroup->begin(); iter3 < theClonedGroup->end(); iter3++) {
                    if ((*iter3)->front()->getOperatorId() == id) {
                        theClonedConnectOp->addTargetPipeline(*iter3);
                        continue;
                    }
                }
            }
        }

        // establish connectivity for JoinInputProxyOperator
        JoinInputProxyOperator* theProxyOp = dynamic_cast<JoinInputProxyOperator*>(theOp);
        if (theProxyOp != NULL) {
            JoinInputProxyOperator* theClonedProxyOp = dynamic_cast<JoinInputProxyOperator*>((*theClonedGroup)[i]->back());
            int id = theProxyOp->getJoinOperator()->getOperatorId();
            for (vector<OperatorPipeline*>::const_iterator iter3 = theClonedGroup->begin(); iter3 < theClonedGroup->end(); iter3++) {
                if ((*iter3)->front()->getOperatorId() == id) {
                    JoinOperator* theClonedJoinOp = dynamic_cast<JoinOperator*>((*iter3)->front());
                    theClonedProxyOp->setJoinOperator(theClonedJoinOp);
                    break;  // found it
                }
            }
        }

    }
    return theClonedGroup;
}
