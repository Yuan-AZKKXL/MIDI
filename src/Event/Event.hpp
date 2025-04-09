#ifndef EVENT_HPP
#define EVENT_HPP


enum EventType
{
    None,
    APressed,
    BPressed,
    CPressed,
    DPressed,
    AReleased,
    BReleased,
    CReleased,
    DReleased,
    BtnReleased,
    ALongPressed,
    BLongPressed,
    CLongPressed,
    DLongPressed,
};



class Event
{
public:
    Event(EventType type = None)
        : _type(type)
    {

    }

    Event(EventType type, unsigned long timestamp)
        : _type(type)
        , _timestamp(timestamp)
    {

    }

    virtual ~Event()
    {

    }

    EventType getType() const
    {
        return _type;
    }

    void setType(EventType type)
    {
        _type = type;
    }

    bool isInUse() const {
        return _inUse;
    }

    void setInUse(bool inUse) {
        _inUse = inUse;
    }

    unsigned long getTimestamp() const
    {
        return _timestamp;
    }

private:
    EventType _type;
    unsigned long _timestamp;
    bool _inUse;
};

#endif // EVENT_HPP