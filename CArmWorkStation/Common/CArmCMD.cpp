#include "CArmCMD.h"



std::list<CArmObject*> CArmObject::m_Lst;


void CArmObject::registerObject(CArmObject* object)
{
    m_Lst.push_back(object); 
}

void CArmObject::executeCmd(const CArmCMD& cmd)
{
    for (auto & i : m_Lst)
    {
        i->doCmd(cmd);
    }
}