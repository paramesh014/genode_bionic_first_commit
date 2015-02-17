#
# Non-physical true random number generator based on timing jitter.
#
# Copyright Lou Losee <llosee@gmail.com>, 2013
#
# License
# =======
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, and the entire permission notice in its entirety,
#    including the disclaimer of warranties.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote
#    products derived from this software without specific prior
#    written permission.
#
# ALTERNATIVELY, this product may be distributed under the terms of
# the GNU General Public License, in which case the provisions of the GPL are
# required INSTEAD OF the above restrictions.  (This clause is
# necessary due to a potential bad interaction between the GPL and
# the restrictions contained in a BSD-style copyright.)
#
# THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ALL OF
# WHICH ARE HEREBY DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF NOT ADVISED OF THE POSSIBILITY OF SUCH
# DAMAGE.
#

GETTIME  CSECT
         EDCPRLG BASEREG=R10,USRDSAL=WORKLEN
         USING WORKAREA,R13
         L     R3,0(,R1)       get addr of parm
         STCKE TGT             get clock extended
         MVC   0(8,R3),VALUE  stcke result to parm
         EDCEPIL
R0               EQU 0
R1               EQU 1
R2               EQU 2
R3               EQU 3
R4               EQU 4
R5               EQU 5
R6               EQU 6
R7               EQU 7
R8               EQU 8
R9               EQU 9
R10              EQU 10
R11              EQU 11
R12              EQU 12
R13              EQU 13
R14              EQU 14
R15              EQU 15
WORKLEN  EQU   TGTEND-TGT
WORKAREA DSECT
         DS    0D
JUNK     DS    16F
TGT      DS    0D
         DS    CL6
VALUE    DS    CL8
         DS    H
TGTEND   DS    0H
*
MYDSA          EDCDSAD
         END   GETTIME
