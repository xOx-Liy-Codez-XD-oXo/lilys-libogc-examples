#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

#include "globals.h"
#include "rendermgr.h"
#include "init.h"
#include "graphicsutililiys.h"
#include "primitives.h"

#include "data/mdl/primitive/Matcapcone_liym3q.h"
#include "data/mdl/primitive/Teapot_liym3q.h"
#include "matcap.h"

int main(int argc, char **argv) {
	GXColor background = {0, 0, 0, 0xff};
	guVector cam = {0.0F, -5.0F, 0.0F},
			up = {0.0F, 0.0F, 1.0F},
		  look = {0.0F, 0.0F, 0.0F};
	TPLFile crateTPL;

	inits();

	GX_SetCullMode(GX_CULL_NONE);

	liy_VtxDescConfig(LV_VP | LV_VN | LV_VC, 0);

	GX_InvalidateTexAll();

	guPerspective(perspective, 45, 16.0f/9.0f, 0.1F, 300.0F);

	VIDEO_SetBlack(false);

	void * lightcapbuf = memalign(32, 128*128*3);

	GX_SetNumTexGens(1);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0);

	GXTexObj matcap1TexObj;

	while(1) {
		guLookAt(view, &cam, &up, &look);
		guMtxCopy(view, viewreflec);
		viewreflec[0][3] = 0.0f;
		viewreflec[1][3] = 0.0f;
		viewreflec[2][3] = 0.0f;

		GXLightObj lobj;
		guVector lpos = {1000.0f, -100.0f, 1000.0f};
		if((rframe % 240) < 120) {
			lpos.x = sin(((float)rframe * 0.0523f) + 1.57f) * 1000.0f;
			lpos.y = cos(((float)rframe * 0.0523f) + 1.57f) * 1000.0f;
		}
		guVecMultiply(view, &lpos, &lpos);
		GX_InitLightPos(&lobj, lpos.x, lpos.y, lpos.z);
		GX_InitLightColor(&lobj, LC_WHITE);
		GX_LoadLightObj(&lobj, GX_LIGHT0);
		GX_SetNumChans(1);
		GX_SetChanAmbColor(GX_COLOR0A0, LC_DARKGREY);
		GX_SetChanMatColor(GX_COLOR0A0, LC_LIGHTGREY);
		GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
		GX_SetNumTevStages(1);
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_RASC, GX_CC_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	

		lightcap(lightcapbuf);
		resetViewport();
		GX_PixModeSync();
		GX_InitTexObj(&matcap1TexObj, lightcapbuf, 128, 128, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
		GX_InvalidateTexAll();

		GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);

		WPAD_ScanPads();
		PAD_ScanPads();
		if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) shouldquit = 1;
		if(PAD_ButtonsDown(0) & PAD_BUTTON_START) shouldquit = 1;
		if(shouldquit) break;



		Mtx spin;
		if((rframe % 240) > 119) {
			guMtxRotRad(spin, 'z', (float)rframe * 0.0523f);
		} else {
			guMtxIdentity(spin);
		}
		guMtxTransApply(spin, model, -3.0f, 4.0f, -2.0f);
		guMtxConcat(view, model, model);
		GX_LoadPosMtxImm(model, GX_PNMTX0);
		GX_LoadNrmMtxImm(model, GX_PNMTX0);
		GX_SetCurrentMtx(GX_PNMTX0);

		ag_config_Teapot(0);
		ag_draw_Teapot(0);

	
		GX_LoadTexObj(&matcap1TexObj, GX_TEXMAP0);

		GX_SetNumTevStages(1);
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);
		
		guMtxTransApply(spin, model, 3.0f, 4.0f, -2.0f);
		guMtxConcat(view, model, model);
		GX_LoadPosMtxImm(model, GX_PNMTX0);
		GX_LoadNrmMtxImm(model, GX_PNMTX0);
		GX_SetCurrentMtx(GX_PNMTX0);

		//Reflection map needs another matrix to be calculated
		guMtxConcat(viewreflec, spin, model);
		Mtx reflec, s, t;
		guMtxScale(s, 0.5f, -0.5f, 0.0f);
		guMtxTrans(t, 0.5f, 0.5f, 1.0f);
		guMtxConcat(s, model, model);
		guMtxConcat(t, model, model);
		GX_LoadTexMtxImm(model, GX_TEXMTX0, GX_MTX3x4);

		ag_config_Teapot(0);
		ag_draw_Teapot(0);




		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(xfb, GX_TRUE);

		GX_DrawDone();

		VIDEO_SetNextFramebuffer(xfb);

		rframe++;
		VIDEO_Flush();
		VIDEO_WaitVSync();
	}
}
