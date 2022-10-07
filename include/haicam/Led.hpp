#ifndef __HAICAM_LED_HPP__
#define __HAICAM_LED_HPP__

namespace haicam
{
    enum LedType
    {
        LED_POWER = 0,
        LED_WIFI,
        LED_ETHERNET
    };

    enum LedStatus
    {
        LED_OFF = 0,
        LED_ON,
        LED_SLOW_BLINK,
        LED_NORMAL_BLINK,
        LED_FAST_BLINK
    };

    class Led
    {
    protected:
        LedType type;
        LedStatus status;

    public:
        Led(LedType type);
        virtual ~Led();

        virtual void on();
        virtual void off();
        virtual void setStatus(LedStatus status);
        virtual LedStatus getStatus();
    };
}
#endif