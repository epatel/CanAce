/* Emulation of the Z80 CPU with hooks into the other parts of xace.
 * Copyright (C) 1994 Ian Collier.
 * Z81 changes (C) 1995 Russell Marks.
 * xace changes (C) 1997 Edward Patel.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <stdio.h>
#include <string.h>

#include "z80.h"

#define parity(a) (partable[a])

unsigned char partable[256] = {
  4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
  0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
  0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
  4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
  0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
  4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
  4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
  0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
  0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
  4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
  4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
  0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
  4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
  0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
  0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
  4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4
};

static int reset_flag = 0;

void
reset_ace(void)
{
  reset_flag = 1;
  memset(mem+8192, 0xff, 57344);
}

void
mainloop(void) {
  unsigned char a, f, b, c, d, e, h, l;
  unsigned char r, a1, f1, b1, c1, d1, e1, h1, l1, i, iff1, iff2, im;
  unsigned short pc;
  unsigned short ix, iy, sp;
  extern unsigned long tstates,tsmax;
  unsigned int radjust;
  unsigned char ixoriy, new_ixoriy;
  unsigned char intsample;
  unsigned char op;
  
  a=f=b=c=d=e=h=l=a1=f1=b1=c1=d1=e1=h1=l1=i=r=iff1=iff2=im=0;
  ixoriy=new_ixoriy=0;
  ix=iy=sp=pc=0;
  tstates=radjust=0;
  while(1) {
    ixoriy=new_ixoriy;
    new_ixoriy=0;
    intsample=1;
    op=fetch(pc);
    pc++;
    radjust++;

    switch(op) {
      #include "z80ops.c"
    }

    if(tstates>tsmax)
      fix_tstates();

    if(interrupted == 1 && intsample && iff1) {
      do_interrupt();
      push2(pc);
      pc=0x38;
      interrupted=0;
    }

    if (reset_flag) {
      /* actually a kludge to let us do a reset */
      a=f=b=c=d=e=h=l=a1=f1=b1=c1=d1=e1=h1=l1=i=r=iff1=iff2=im=0;
      ixoriy=new_ixoriy=0;
      ix=iy=sp=pc=0;
      tstates=radjust=0;
      reset_flag = 0;
    }
  }
}


