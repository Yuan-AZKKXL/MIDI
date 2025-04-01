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
bool _longPressed = true;

//模式1
class ButtonState1 :public State{
public:
	enum {ID = 1};
    ButtonState1(){ seq.begin();}

    virtual void onEnter(){ }

    virtual void onExit(){ }
    //处理事件
    virtual bool handleEvent(StateMachine* machine,Event* event)
    {
        if(machine == nullptr || event == nullptr){
            return false;
        }

        switch(event->getType()){
            //按键A短按
            case EventType::BtnAPressed:{

            }break;
            //按键B短按
            case EventType::BtnBPressed:{

            }break;
            //按键C短按
            case EventType::BtnCPressed:{

            }break;
            //按键D短按
            case EventType::BtnDPressed:{

            }break;
            //按键A长按
            case EventType::BtnALongPressed:{

            }break;
            //按键B长按
            case EventType::BtnBLongPressed:{

            }break;
            //按键C长按
            case EventType::BtnCLongPressed:{

            }break;
            //按键D长按
            case EventType::BtnDLongPressed:{
            	//长按状态没有打开
            	if(_longPressed == false)
            		return false;

				int index = 2;
            	State* nextState = StateManager::getInstance()->getState(index);
            	if(nextState != nullptr){
            		machine->changeState(nextState);
            		//切换成功响一下，表示切换成功
            		seq.setNoteOn(0, 60, 127);
            	}
            	//关闭长按状态
            	_longPressed = false;
            	return true;
            }
        	case EventType::NoEvent:{
				//打开长按状态
            	_longPressed = true;
            }
            default:
                return false;
        }
    }
    virtual int getID() const {return ID;}
    virtual const char* getName() const {return "ButtonStateOne";}
};

//模式2状态
class ButtonState2 :public State{
public:
	enum {ID = 2};
    virtual void onEnter(){}

    virtual void onExit(){}

    //处理事件
    virtual bool handleEvent(StateMachine* machine,Event* event){
	    if(machine == nullptr || event == nullptr){
	        return false;
	    }

	    switch(event->getType()){
	        //按键A短按
			case EventType::BtnAPressed:{

	        }break;
	        //按键B短按
			case EventType::BtnBPressed:{

	        }break;
	        //按键C短按
			case EventType::BtnCPressed:{

	        }break;
	        //按键D短按
			case EventType::BtnDPressed:{

	        }break;
	        //按键A长按
			case EventType::BtnALongPressed:{

	        }break;
	        //按键B长按
			case EventType::BtnBLongPressed:{

	        }break;
	        //按键C长按
			case EventType::BtnCLongPressed:{

	        }break;
	        //按键D长按
			case EventType::BtnDLongPressed:{
				//长按状态没有打开
				if(_longPressed == false)
					return false;
				int index = 3;
				State* nextState = StateManager::getInstance()->getState(index);
				if(nextState != nullptr){
					machine->changeState(nextState);
					//切换成功响一下，表示切换成功
					seq.setNoteOn(0, 70, 127);
					//关闭长按状态
					_longPressed = false;
				}
				return true;
	        }
		    case EventType::NoEvent:{
				//打开长按状态
				_longPressed = true;
		    }break;
			default:
				break;
	    }
	}
    virtual int getID() const {return ID;}
    virtual const char* getName() const {return "ButtonStateTwo";};
};

//模式3状态
class ButtonState3 :public State{
public:
	enum {ID = 3};
    virtual void onEnter(){

    }

    virtual void onExit(){

    }
	//处理事件
	virtual bool handleEvent(StateMachine* machine,Event* event){
    	if(machine == nullptr || event == nullptr){
    		return false;
    	}

    	switch(event->getType()){
    		//按键A短按
    		case EventType::BtnAPressed:{
    			seq.setNoteOn(0, 90, 127);
    			return true;
    		}
    		//按键B短按
    		case EventType::BtnBPressed:{

    		}break;
    		//按键C短按
    		case EventType::BtnCPressed:{

    		}break;
    		//按键D短按
    		case EventType::BtnDPressed:{

    		}break;
    		//按键A长按
    		case EventType::BtnALongPressed:{

    		}break;
    		//按键B长按
    		case EventType::BtnBLongPressed:{

    		}break;
    		//按键C长按
    		case EventType::BtnCLongPressed:{

    		}break;
    		//按键D长按
    		case EventType::BtnDLongPressed:{
    			if(_longPressed == false)
    				return false;
    			int index = 1;
    			State* nextState = StateManager::getInstance()->getState(index);
    			if(nextState != nullptr){
    				machine->changeState(nextState);
    				//切换成功响一下，表示切换成功
    				seq.setNoteOn(0, 80, 127);
    				//关闭长按状态
    				_longPressed = false;
    			}
    			return true;
    		}
    		case EventType::NoEvent:{
    			//打开长按状态
    			_longPressed = true;
    		}break;
    		default:
    			break;
    	}
    }
	virtual int getID() const {return ID;}
	virtual const char* getName() const {return "ButtonStateThree";};
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
