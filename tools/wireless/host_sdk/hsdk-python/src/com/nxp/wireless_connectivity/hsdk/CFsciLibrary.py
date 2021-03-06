'''
* Copyright 2014-2016 Freescale Semiconductor, Inc.
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

from ctypes import c_int, c_uint8, c_uint32, c_longlong, Structure, POINTER


class FsciFrame(Structure):

    '''
    ctypes Structure that maps over the FsciFrame C structure.
    '''

    _fields_ = [
        ('sync', c_uint8),
        ('opGroup', c_uint8),
        ('opCode', c_uint8),
        ('length', c_uint32),
        ('data', POINTER(c_uint8)),
        ('crc', c_uint32),
        ('timestamp', c_longlong),
        ('index', c_uint32),
        ('endian', c_int),
        ('virtualInterface', c_uint8),
        ('direction', c_int)
    ]


class Endianess(object):
    Big = 0
    Little = 1


class DataDirection(object):
    HostToDevice = 0
    DeviceToHost = 1
