/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
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
 */

#ifndef _PCB_H_
#define _PCB_H_

/*******************************************************************************
* Definitions
******************************************************************************/
/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

#define __PCB__

#define PCBFree(pcb) ((pcb)->FREE(pcb))

/*--------------------------------------------------------------------------*/
/*
 *                          DATA STRUCTURES
 */

/*
 * The PCB (Packet Control Block)
 *
 * One PCB contains exactly one packet, possibly split over several areas of
 * memory.
 *
 * The PCB structure consists of two pointers (FREE and PRIVATE) followed by a
 * variable-length array of PCB_FRAGMENT structures.  The array is terminated
 * by an entry with LENGTH=0 and FRAGMENT=NULL.
 *
 * The PCB's owner (i.e. the module that allocated it) must initialize the
 * FREE field to point to a function to free the PCB.  PRIVATE can be used by
 * the PCB's owner to store any information it wishes.
 */

struct pcb;

/*! \cond DOXYGEN_IGNORE */
typedef void (*PCB_FREE_F)(struct pcb *);
/*! \endcond */

/*!
 * \brief Structure that contains fragment of PCB.
 */
typedef struct pcbFragment
{
    /*! \brief Length of PCB fragment. */
    uint32_t LENGTH;
    /*! \brief Pointer to fragment. */
    unsigned char *FRAGMENT;
} PCB_FRAGMENT, *PCB_FRAGMENT_PTR;

/*!
 * \brief PCB structure.
 *
 * The array is terminated by an entry with LENGTH=0 and FRAGMENT=NULL.
 */
typedef struct pcb
{
    /*! \brief Function that frees PCB. */
    PCB_FREE_F FREE;
    /*! \brief Private PCB information. */
    void *PRIVATE;
    /*! \brief Pointer to PCB fragment. */
    PCB_FRAGMENT FRAG[1];
} PCB, *PCB_PTR;

// The minimum size of a PCB must contain two fragments, so the following represents
// the smallest PCB
/*!
 * \brief PCB structure contains two fragments.
 *
 * The array is terminated by an entry with LENGTH=0 and FRAGMENT=NULL.
 */
typedef struct pcb2
{
    /*! \brief Function that frees PCB. */
    PCB_FREE_F FREE;
    /*! \brief Private PCB information. */
    void *PRIVATE;
    /*! \brief Pointers to two PCB fragments. */
    PCB_FRAGMENT FRAG[2];
} PCB2, *PCB2_PTR;

#define PCB_MINIMUM_SIZE (sizeof(PCB2))

/*--------------------------------------------------------------------------*/
/*                           EXTERNAL DECLARATIONS                          */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* _PCB_H_ */
