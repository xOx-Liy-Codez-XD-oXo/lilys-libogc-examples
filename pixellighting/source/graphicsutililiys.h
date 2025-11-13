#ifndef UTILILIYS_H
#define UTILILIYS_H

#include "colors.h"

//liy vert
#define LV_VP 1	        //0000 0000  0000 0001
#define LV_VN 2	        //0000 0000  0000 0010
#define LV_TC 4	        //0000 0000  0000 0100
#define LV_VC 8	        //0000 0000  0000 1000
#define LV_MX 16        //0000 0000  0001 0000
#define LV_VPIDX8 32    //0000 0000  0010 0000
#define LV_VPIDX16 64   //0000 0000  0100 0000
#define LV_VNIDX8 128   //0000 0000  1000 0000
#define LV_VNIDX16 256  //0000 0001  0000 0000
#define LV_TCIDX8  512  //0000 0010  0000 0000
#define LV_TCIDX16 1024 //0000 0100  0000 0000
#define LV_TC1	 2048   //0000 1000  0000 0000

void liy_VtxDescConfig(uint16_t config, int vtxfmt) {
	GX_ClearVtxDesc();
	if(config & LV_MX) { GX_SetVtxDesc(GX_VA_PTNMTXIDX, GX_DIRECT); }
	if(config & LV_VP) { GX_SetVtxDesc(GX_VA_POS, GX_DIRECT); }
	if(config & LV_VPIDX8) { GX_SetVtxDesc(GX_VA_POS, GX_INDEX8); }
	if(config & LV_VPIDX16) { GX_SetVtxDesc(GX_VA_POS, GX_INDEX16); }
	if(config & LV_VN) { GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT); }
	if(config & LV_VNIDX8) { GX_SetVtxDesc(GX_VA_NRM, GX_INDEX8); }
	if(config & LV_VNIDX16) { GX_SetVtxDesc(GX_VA_NRM, GX_INDEX16); }
	if(config & LV_VC) { GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT); }
	if(config & LV_TC) { GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT); }
	if(config & LV_TC1){ GX_SetVtxDesc(GX_VA_TEX1, GX_DIRECT); }
	if(config & LV_TCIDX8) { GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX8); }
	if(config & LV_TCIDX16) { GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16); }
	//attrfmts
	if(config & LV_MX) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_PTNMTXIDX, GX_INDEX8, GX_U8, 0); }
	if(config & LV_VP) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_POS, GX_POS_XYZ, GX_F32, 0); }
	if(config & LV_VPIDX8) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_POS, GX_POS_XYZ, GX_F32, 0); }
	if(config & LV_VPIDX16) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_POS, GX_POS_XYZ, GX_F32, 0); }
	if(config & LV_VN) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0); }
	if(config & LV_VNIDX8) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0); }
	if(config & LV_VNIDX16) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0); }
	if(config & LV_VC) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0); }
	if(config & LV_TC) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TC1){ GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX8) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX16) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0); }
}

#endif