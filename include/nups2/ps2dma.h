#ifndef NUPS2_PS2DMA_H
#define NUPS2_PS2DMA_H

#include "nu_types.h"

u8 *vpDmaTag_CntEx(u8 *cur, u32 vif0, u32 vif1);
u8 *vpDmaTag_RefEx(u8 *cur, u32 addr, u32 vif0, u32 vif1);
u8 *vpDmaTag_Call(u8 *cur, u32 addr);
u8 *vpDmaTag_RetEx(u8 *cur, u32 vif0, u32 vif1);
u8 *vpDmaTag_Close(u8 *cur);
u8 *vpDmaTag_Ret(u8 *cur);

#endif /* NUPS2_PS2DMA_H */
