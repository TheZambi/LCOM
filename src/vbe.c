#include "vbe.h"

extern void *video_mem, *double_buffer;
vbe_mode_info_t vbe;


void* (vg_init)(uint16_t mode)
{
  void* toReturn=NULL;
  reg86_t reg;
  int r = 0;
  struct minix_mem_range mr;
  unsigned int vram_base;
  unsigned int vram_size;

  mr.mr_base = 0;
  mr.mr_limit = 1024 * 1024;

  if (OK != sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))
  {
    panic("FAILED SYS PRIV%d\n", r);
  }

  memset(&reg, 0, sizeof(reg));

  vbe_get_mode_info(mode, &vbe);

  vram_base = vbe.PhysBasePtr;
  vram_size = vbe.XResolution * vbe.YResolution * vbe.BitsPerPixel / 8;

  mr.mr_base = (phys_bytes)vram_base;
  mr.mr_limit = vram_size + mr.mr_base;

  if (OK != sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))
  {
    panic("FAILED SYS PRIV%d\n", r);
  }

  double_buffer = malloc(vram_size);

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if (video_mem == MAP_FAILED)
    panic("FAILED MAP %d\n", r);

  reg.intno = 0x10;
  reg.ah = 0x4F;
  reg.al = 0x02;
  reg.bx = 1 << 14 | mode;

  sys_int86(&reg);

  return toReturn;
}

void (drawPixel)(uint8_t *color,uint8_t *pixelToChange, int row, int col)
{
  uint8_t *colorToBuild32Bits=color;
  uint32_t colorAux=0;
  if(row<0 || row>=vbe.YResolution || col <0 || col >= vbe.XResolution )
    return;
  for(size_t i=0; i<3; i++)
  {
    colorAux |= ((*colorToBuild32Bits)<<(8*i));
    colorToBuild32Bits++;
  }
  int n=0;
  pixelToChange+=(col+row*vbe.XResolution)*((vbe.BitsPerPixel+7)/8);
   for(size_t k=0;k<(vbe.BitsPerPixel+7)/8;k++)
      {
      if(colorAux != (xpm_transparency_color(XPM_8_8_8)))
        *pixelToChange = *color;
      color++;
      pixelToChange++;
      n++;
      }
}

