/*
 *  (C) 2021 MEAN00 fixounet@free.fr
 *  See license file
 */
#include "lnGPIO.h"
#include "lnArduino.h"
#include "lnGPIO_priv.h"

#include "lnPeripheral_priv.h"

static LN_GPIO *gpio[5] = {(LN_GPIO *)LN_GPIOA_ADR, (LN_GPIO *)LN_GPIOB_ADR, (LN_GPIO *)LN_GPIOC_ADR,
                           (LN_GPIO *)LN_GPIOD_ADR, (LN_GPIO *)LN_GPIOE_ADR};

static LN_GPIO *gpioA = (LN_GPIO *)LN_GPIOA_ADR;
static LN_GPIO *gpioB = (LN_GPIO *)LN_GPIOB_ADR;
static LN_GPIO *gpioC = (LN_GPIO *)LN_GPIOC_ADR;

static LN_GPIO *gpios[3] = {gpioA, gpioB, gpioC};

/**
 *
 * @param pin
 * @param mode
 */
void lnPinMode(const lnPin xpin, const GpioMode mode)
{
    LN_GPIO *port = gpio[xpin >> 4];
    int pin = xpin & 0xf;
    volatile uint32_t *CTL;

    uint32_t value;
    switch (mode)
    {
    case lnINPUT_PULLUP:
        port->BOP = 1 << pin;
        value = LNGPIOSET(LN_CTL_MD_INPUT, LN_CTL_INPUT_PULLUPPULLDOWN);
        break;
    case lnINPUT_PULLDOWN:
        port->BOP = 1 << (16 + pin);
        value = LNGPIOSET(LN_CTL_MD_INPUT, LN_CTL_INPUT_PULLUPPULLDOWN);
        break;

    case lnOUTPUT:
        value = LNGPIOSET(LN_CTL_MD_OUTPUT, LN_CTL_OUTPUT_PP);
        break;
    case lnOUTPUT_OPEN_DRAIN:
        value = LNGPIOSET(LN_CTL_MD_OUTPUT, LN_CTL_OUTPUT_OD);
        break;
    case lnALTERNATE_PP:
        value = LNGPIOSET(LN_CTL_MD_OUTPUT, LN_CTL_OUTPUT_ALTERNAT_PP);
        break;
    case lnALTERNATE_OD:
        value = LNGPIOSET(LN_CTL_MD_OUTPUT, LN_CTL_OUTPUT_ALTERNAT_OD);
        break;
    default:
        xAssert(0);
        break;
    }
    if (pin > 7)
    {
        CTL = &(port->CTL1);
        pin &= 7;
    }
    else
    {
        CTL = &(port->CTL0);
    }
    uint32_t ref = *CTL;
    ref &= ~(0Xf << (pin * 4));
    ref |= value << (pin * 4);
    *CTL = ref;
}

/**
 *
 * @param pin
 * @param value
 */
void lnDigitalWrite(const lnPin pin, bool value)
{
    LN_GPIO *port = gpio[pin >> 4];
    int xpin = pin & 0xf;
    uint32_t mask, radix;
    if (value)
    {
        radix = 1;
    }
    else
    {
        radix = (1 << 16);
    }
    port->BOP = radix << xpin;
}
/**
 *
 * @param port
 * @return
 */
volatile uint32_t *lnGetGpioToggleRegister(int port)
{
    return &(gpios[port]->BOP);
}
/**
 *
 * @param port
 * @return
 */
volatile uint32_t *lnGetGpioDirectionRegister(int port)
{
    return &(gpios[port]->CTL0);
}
/**
 *
 * @param port
 * @return
 */
volatile uint32_t *lnGetGpioValueRegister(int port)
{
    return &(gpios[port]->OCTL);
}

/**
 *
 * @param pin
 * @return
 */
bool lnDigitalRead(const lnPin pin)
{
    LN_GPIO *port = gpio[pin >> 4];
    int xpin = pin & 0xf;

    uint32_t v = port->ISTAT;
    return !!(v & (1 << xpin));
}
/**
 *
 * @param port
 * @return
 */
uint32_t lnReadPort(int port)
{
    return (gpios[port]->ISTAT);
}

/**
 *
 * @param p
 */
lnFastIO::lnFastIO(lnPin pin)
{
    lnPinMode(pin, lnOUTPUT);

    LN_GPIO *port = gpio[pin >> 4];
    _onoff = (uint32_t *)&port->BOP;
    int bit = 1 << (pin & 0xf);
    _offbit = bit << 16;
    _onbit = bit;
}

// EOF
