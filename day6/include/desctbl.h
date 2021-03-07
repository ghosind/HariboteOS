#ifndef _DESCTBL_H_
#define _DESCTBL_H_

struct SegmentDescriptor {
  short limit_low, base_low;
  char base_mid, access_right;
  char limit_high, base_high;
};

struct GateDescriptor {
  short offset_low, selector;
  char dw_count, access_right;
  short offset_high;
};

void init_gdtidt(void);
void set_segmdesc(struct SegmentDescriptor *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GateDescriptor *gd, int offset, int selector, int ar);

#endif // _DESCTBL_H_
