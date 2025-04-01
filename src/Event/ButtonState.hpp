//
// Created by Administrator on 25-4-1.
//

#ifndef BUTTONSTATE_H
#define BUTTONSTATE_H

#include "State.hpp"
#include "StateManager.h"
#include "Event.hpp"
#include "StateMachine.h"
#include "SAM2695/SAM2695Synth.h"

SAM2695Synth seq = SAM2695Synth();;

//模式1状态
class ButtonState :public State
{
public:
    enum{ID = 2};

    ButtonState()
    {
        seq.begin();
    }

    virtual void onEnter()
    {
        
    }

    virtual void onExit()
    {

    }

    virtual bool handleEvent(StateMachine* machine,Event* event)
    {
        if(machine == nullptr || event == nullptr)
        {
            return false;
        }

        switch(event->getType())
        {
            case EventType::BtnAPressed:
            {
                    seq.setNoteOn(0, 60, 127);
                    return true;
            }
            case EventType::BtnAReleased:
            {
                    seq.setNoteOff(0, 60, 0);
                    return true;
            }
            case EventType::BtnCPressed:
            {

            }
            case EventType::BtnDPressed:
            {

            }
            default:
                break;
        }
    }

    virtual int getID() const {return ID;}
    virtual const char* getName() const {return "ButtonState";};
};

//错误状态
class ErrorState :public State
{
public:
    enum{ID = 100};

    ErrorState():_errorCode(0),_errorMsg("unknown error"){}
    void setError(int code,const char* msg)
    {
        _errorCode = code;
        _errorMsg = msg;
    }

    virtual void onEnter()
    {

    }

    virtual void onExit()
    {

    }
    virtual bool handleEvent(StateMachine* machine,Event* event)
    {
        if(machine == nullptr || event == nullptr)
        {
            return false;
        }
        return true;
    }
    virtual int getID() const {return ID;}
    virtual const char* getName() const {return "Error";};
private:
    int _errorCode;
    const char* _errorMsg;
};



#endif //BUTTONSTATE_H
