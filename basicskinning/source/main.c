/*---------------------------------------------------------------------------------

	basic skinning example by lily
	built off nehe lesson 6 port to GX by shagkur

---------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

#define DEFAULT_FIFO_SIZE	(256*1024)

static GXRModeObj *rmode = NULL;
static void *frameBuffer[2] = { NULL, NULL};

int main(int argc,char **argv) {
	f32 yscale,zt = 0;
	u32 xfbHeight;
	u32 fb = 0;
	f32 rquad = 0.0f;
	u32 first_frame = 1;
	Mtx view; // view and perspective matrices
	Mtx model, modelview;
	Mtx44 perspective;
	void *gpfifo = NULL;
	GXColor background = {32, 32, 128, 0xff};
	guVector cam = {0.0F, 0.0F, 0.0F},
			up = {0.0F, 1.0F, 0.0F},
		  look = {0.0F, 0.0F, -1.0F};

	VIDEO_Init();
	WPAD_Init();

	// allocate the fifo buffer
	gpfifo = memalign(32,DEFAULT_FIFO_SIZE);
	memset(gpfifo,0,DEFAULT_FIFO_SIZE);

	rmode = VIDEO_GetPreferredMode(NULL);

	// allocate 2 framebuffers for double buffering
	frameBuffer[0] = SYS_AllocateFramebuffer(rmode);
	frameBuffer[1] = SYS_AllocateFramebuffer(rmode);

	// configure video
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	fb ^= 1;

	// init the flipper
	GX_Init(gpfifo,DEFAULT_FIFO_SIZE);

	// clears the bg to color and clears the z buffer
	GX_SetCopyClear(background, 0x00ffffff);

	// other gx setup
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
	xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering, ((rmode->viHeight==2*rmode->xfbHeight) ? GX_ENABLE : GX_DISABLE) );

	if (rmode->aa) {
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
	} else {
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	}

	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(frameBuffer[fb],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	// setup the vertex attribute table
	// describes the data
	// args: vat location 0-7, type of data, data format, size, scale
	// so for ex. in the second call we are sending position data with
	// 3 values X,Y,Z of size F32. scale sets the number of fractional
	// bits for non float data.
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_PTNMTXIDX, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);

	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_PTNMTXIDX, GX_INDEX8, GX_U8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);

	// setup our camera at the origin
	// looking down the -z axis with y up
	guLookAt(view, &cam, &up, &look);


	float aspect = 16.f/9.f;

	// setup our projection matrix
	// this creates a perspective matrix with a view angle of 90
	guPerspective(perspective, 45, aspect, 0.1F, 300.0F);
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);

	guVector cubeAxis = {0,1,0};

	while(1) {

		WPAD_ScanPads();
		if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);
		else if (WPAD_ButtonsHeld(0)&WPAD_BUTTON_UP) zt -= 0.25f;
		else if (WPAD_ButtonsHeld(0)&WPAD_BUTTON_DOWN) zt += 0.25f;

		// set number of rasterized color channels
		GX_SetNumChans(1);

		//set number of textures to generate
		GX_SetNumTexGens(1);

		// setup texture coordinate generation
		// args: texcoord slot 0-7, matrix type, source to generate texture coordinates from, matrix to use
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
		GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

		guMtxIdentity(model);
		guMtxTransApply(model, model, 0.0f,0.0f,zt-7.0f);
		guMtxConcat(view,model,modelview);
		GX_LoadPosMtxImm(modelview, GX_PNMTX0);
		//mtx 0 shan't rotate

		guMtxIdentity(model);
		guMtxRotAxisDeg(model, &cubeAxis, rquad);
		guMtxTransApply(model, model, 0.0f,0.0f,zt-7.0f);
		guMtxConcat(view,model,modelview);
		// load the modelview matrix into matrix memory
		GX_LoadPosMtxImm(modelview, GX_PNMTX1);
		

		//The 3 types of matricies are stored in GP matrix memory interleaved so
		//GX_PNMTX0 is index 0 in matrix memory,
		//GX_PNMTX1 is index 3 in matrix memory, and
		//GX_PNMTX2 is index 6 in matrix memory.

		GX_Begin(GX_QUADS, GX_VTXFMT0, 24);		// Draw a Cube
	
			GX_MatrixIndex1x8(3);
			GX_Position3f32(-1.0f, 1.0f, -1.0f);    // Top Left of the quad (top)
			GX_MatrixIndex1x8(3);
			GX_Position3f32(-1.0f, 1.0f, 1.0f);     // Top Right of the quad (top)
			GX_MatrixIndex1x8(0);
			GX_Position3f32(-1.0f, -1.0f, 1.0f);    // Bottom Right of the quad (top)
			GX_MatrixIndex1x8(0);
			GX_Position3f32(- 1.0f, -1.0f, -1.0f);  // Bottom Left of the quad (top)

			GX_MatrixIndex1x8(3);
			GX_Position3f32( 1.0f,1.0f, -1.0f);     // Top Left of the quad (bottom)
			GX_MatrixIndex1x8(0);
			GX_Position3f32(1.0f,-1.0f, -1.0f);     // Top Right of the quad (bottom)
			GX_MatrixIndex1x8(0);
			GX_Position3f32(1.0f,-1.0f,1.0f);       // Bottom Right of the quad (bottom)
			GX_MatrixIndex1x8(3);
			GX_Position3f32( 1.0f,1.0f,1.0f);       // Bottom Left of the quad (bottom)

			GX_MatrixIndex1x8(0);
			GX_Position3f32( -1.0f, -1.0f, 1.0f);   // Top Right Of The Quad (Front)
			GX_MatrixIndex1x8(0);
			GX_Position3f32(1.0f, -1.0f, 1.0f);     // Top Left Of The Quad (Front)
			GX_MatrixIndex1x8(0);
			GX_Position3f32(1.0f,-1.0f, -1.0f);     // Bottom Left Of The Quad (Front)
			GX_MatrixIndex1x8(0);
			GX_Position3f32( -1.0f,-1.0f, -1.0f);   // Bottom Right Of The Quad (Front)

			GX_MatrixIndex1x8(3);
			GX_Position3f32( -1.0f,1.0f,1.0f);      // Bottom Left Of The Quad (Back)
			GX_MatrixIndex1x8(3);
			GX_Position3f32(-1.0f,1.0f,-1.0f);      // Bottom Right Of The Quad (Back)
			GX_MatrixIndex1x8(3);
			GX_Position3f32(1.0f, 1.0f,-1.0f);      // Top Right Of The Quad (Back)
			GX_MatrixIndex1x8(3);
			GX_Position3f32( 1.0f, 1.0f,1.0f);      // Top Left Of The Quad (Back)

			GX_MatrixIndex1x8(0);
			GX_Position3f32(1.0f, -1.0f, -1.0f);    // Top Right Of The Quad (Left)
			GX_MatrixIndex1x8(3);
			GX_Position3f32(1.0f, 1.0f,-1.0f);      // Top Left Of The Quad (Left)
			GX_MatrixIndex1x8(3);
			GX_Position3f32(-1.0f,1.0f,-1.0f);      // Bottom Left Of The Quad (Left)
			GX_MatrixIndex1x8(0);
			GX_Position3f32(-1.0f,-1.0f, -1.0f);    // Bottom Right Of The Quad (Left)

			GX_MatrixIndex1x8(0);
			GX_Position3f32( 1.0f, -1.0f,1.0f);     // Top Right Of The Quad (Right)
			GX_MatrixIndex1x8(0);
			GX_Position3f32( -1.0f, -1.0f, 1.0f);   // Top Left Of The Quad (Right)
			GX_MatrixIndex1x8(3);
			GX_Position3f32( -1.0f,1.0f, 1.0f);     // Bottom Left Of The Quad (Right)
			GX_MatrixIndex1x8(3);
			GX_Position3f32( 1.0f,1.0f,1.0f);       // Bottom Right Of The Quad (Right)

		//GP knows we're done when we send enough data

		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(frameBuffer[fb],GX_TRUE);

		GX_DrawDone();

		VIDEO_SetNextFramebuffer(frameBuffer[fb]);
		if(first_frame) {
			first_frame = 0;
			VIDEO_SetBlack(false);
		}
		VIDEO_Flush();
		VIDEO_WaitVSync();
		fb ^= 1;

		rquad -= 0.25f;	
	}
}
