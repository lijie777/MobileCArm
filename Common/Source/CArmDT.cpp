#include "CArmDT.h"



CArmDrawTool::CArmDrawTool(int type)
    : IDrawingTool(type)
{
}


CArmDrawTool::~CArmDrawTool()
{
}

void CArmDrawToolPtr::leftPress(glm::vec3 point, QMouseEvent * ev, void * callbackData, DrawToolCallback callback)
{
    if (_Object != nullptr)
    {
        _Object->leftPress(point, ev, callbackData, callback);
    }
}

void CArmDrawToolPtr::rightPress(glm::vec3 point, QMouseEvent * ev, void * callbackData, DrawToolCallback callback)
{
    if (_Object != nullptr)
    {
        _Object->rightPress(point, ev, callbackData, callback);
    }
}

void CArmDrawToolPtr::leftMove(glm::vec3 point, QMouseEvent * ev, void * callbackData, DrawToolCallback callback)
{
    if (_Object != nullptr)
    {
        _Object->leftMove(point, ev, callbackData, callback);
    }
}

void CArmDrawToolPtr::rightMove(glm::vec3 point, QMouseEvent * ev, void * callbackData, DrawToolCallback callback)
{
    if (_Object != nullptr)
    {
        _Object->rightMove(point, ev, callbackData, callback);
    }
}

void CArmDrawToolPtr::move(glm::vec3 point, QMouseEvent * ev, void * callbackData, DrawToolCallback callback)
{
    if (_Object != nullptr)
    {
        _Object->move(point, ev, callbackData, callback);
    }
}

void CArmDrawToolPtr::deletePress()
{
    if (_Object != nullptr)
    {
        _Object->deletePress();
    }
}

int CArmDrawToolPtr::getStatusCode()
{
    if (_Object != nullptr)
    {
       return _Object->getStatusCode();
    }
    return -1;
}

void CArmDrawToolPtr::setStatus(int statusCode)
{
    if (_Object != nullptr)
    {
        _Object->setStatus(statusCode);
    }
}

CArmDrawToolPtr::CArmDrawToolPtr(CArmDrawTool * obj)
    : _Object(obj)
{
    if (obj != nullptr)
    {
        obj->_refCount++;
    }
    _Object = obj;
}

CArmDrawToolPtr::CArmDrawToolPtr(const CArmDrawToolPtr & ptr)
{
    if (ptr._Object != nullptr)
    {
        ptr._Object->_refCount++;
    }
    _Object = ptr._Object;
}

CArmDrawToolPtr::~CArmDrawToolPtr()
{
    if (_Object != nullptr)
    {
        if (--_Object->_refCount == 0)
        {
            delete _Object;
            _Object = nullptr;
        }
    }
}


CArmDrawToolPtr & CArmDrawToolPtr::operator=(const CArmDrawToolPtr & ptr)
{
    //TODO: 在此处插入 return 语句
    if (ptr._Object != nullptr)
    {
        ptr._Object->_refCount++;
    }
    if (_Object != nullptr)
    {
        if (--_Object->_refCount == 0)
        {
            delete _Object;
            _Object = nullptr;
        }
    }
    this->_Object = ptr._Object;
    return *this;
}

bool CArmDrawToolPtr::operator!=(const CArmDrawToolPtr& ptr)
{
    // TODO: 在此处插入 return 语句
    return (this->_Object != ptr._Object);
}

bool CArmDrawToolPtr::operator==(const CArmDrawToolPtr& ptr)
{
    return (this->_Object == ptr._Object);
}

