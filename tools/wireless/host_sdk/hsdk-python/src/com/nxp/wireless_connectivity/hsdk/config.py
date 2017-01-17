'''
* Copyright 2016 Freescale Semiconductor, Inc.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* o Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
*
* o Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* o Neither the name of Freescale Semiconductor, Inc. nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'''

# Whether or not the Framer will be put in monitor mode.
# That means every RX event will get printed instead of observed.
MONITOR_MODE = False

# Print summary of sent/received frames. When available, use colorama
# to differentiate commands from events, by enabling COLORED_OUTPUT.
VERBOSE = True

# Print more information, e.g. the actual bytes the host sends or
# the system time of received events.
DEBUG = False

# Use FSCI_TX_ACK to validate ACKs from Python instead of C
# IMPORTANT: Before enabling, set FsciTxAck=0 in /usr/local/etc/hsdk/hsdk.conf
FSCI_TX_ACK = False

# The speed (Hz) used for SPI communication.
MAX_SPEED_HZ = 1000000  # 1Mhz

# Whether or not the same device supports both Thread and BLE commands
# sent to separate virtual interfaces, 0 and 1 respectively
MULTI_MODE_THREAD_BLE = False

# Distinguish commands by events through colors in console
# Uses Python module colorama
COLORED_OUTPUT = True

# Use Python's logging module.
USE_LOGGER = False

# Whether or not parameter endianness is relevant only for integers in BLE tests.
# List-type parameters will not account for endianness in both TX and RX packets.
# Status parameters in events will be integers.
BLE_PTS_TEST = False

# Whether the current package is being bundled in the Thread Harness
THREAD_HARNESS = False
