# name of your application
APPLICATION = ribot_coap

# If no BOARD is found in the environment, use this default:
BOARD ?= nucleo-f446

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

ifeq ($(BOARD), nucleo-f446)
## Uncomment to support the XBee module and nomac
USEMODULE += xbee
USEMODULE += gnrc_nomac

## set default UART to use in case none was defined
XBEE_UART ?= "1"

## export UART to params file
CFLAGS += -DXBEE_UART=$(XBEE_UART)

## D4
RIGHT_DIRECTION_PIN = GPIO_PIN\(PORT_B,5\)
## D5
RIGHT_PWM_DEV  = 1
RIGHT_PWM_CHAN = 0

## D7
LEFT_DIRECTION_PIN = GPIO_PIN\(PORT_A,8\)
## D6
LEFT_PWM_DEV   = 0
LEFT_PWM_CHAN  = 2
else ## SAMR21
RIGHT_DIRECTION_PIN = GPIO_PIN\(PA,18\)
LEFT_DIRECTION_PIN = GPIO_PIN\(PA,19\)
## PA6
RIGHT_PWM_DEV  = 0
RIGHT_PWM_CHAN = 0
## PA7
LEFT_PWM_DEV   = 0
LEFT_PWM_CHAN  = 1
endif

CFLAGS += -DRIGHT_DIRECTION_PIN=$(RIGHT_DIRECTION_PIN)
CFLAGS += -DRIGHT_PWM_DEV=$(RIGHT_PWM_DEV)
CFLAGS += -DRIGHT_PWM_CHAN=$(RIGHT_PWM_CHAN)

CFLAGS += -DLEFT_DIRECTION_PIN=$(LEFT_DIRECTION_PIN)
CFLAGS += -DLEFT_PWM_DEV=$(LEFT_PWM_DEV)
CFLAGS += -DLEFT_PWM_CHAN=$(LEFT_PWM_CHAN)

## add current directory to the include path. Putting it in CFLAGS will make
## it go to the beginning, before the standard includes.
## That way xbee_params.h get's included and auto configuration can pick it up.
CFLAGS += -I$(CURDIR)

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_conn_udp

USEPKG += microcoap
CFLAGS += -DMICROCOAP_DEBUG

# include this for printing IP addresses
USEMODULE += shell_commands

FEATURES_REQUIRED += periph_gpio
FEATURES_REQUIRED += periph_pwm

# CoAP broker server information
BROKER_ADDR ?= 2001:660:3207:102::4

CFLAGS += -DBROKER_ADDR=\"$(BROKER_ADDR)\"

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
ifneq (,$(filter cc110x,$(USEMODULE)))          # radio is cc110x sub-GHz
  DEFAULT_CHANNEL ?= 0
  CFLAGS += -DCC110X_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
else
  ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio is IEEE 802.15.4 sub-GHz
    DEFAULT_CHANNEL ?= 5
    FLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(DEFAULT_CHANNEL)
  else                                          # radio is IEEE 802.15.4 2.4 GHz
    DEFAULT_CHANNEL ?= 26
    CFLAGS += -DIEEE802154_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
  endif
endif
