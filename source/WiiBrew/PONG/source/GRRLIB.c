/********************************************************************************************
   _____       _____       _     _____                _                    
  / ____|     |  __ \     | |   |  __ \              | |                   
 | |  ____  __| |__) |__ _| |__ | |__) |___ _ __   __| | ___ _ __ ___ _ __ 
 | | |_ \ \/ /|  _  // _` | '_ \|  _  // _ \ '_ \ / _` |/ _ \ '__/ _ \ '__|
 | |__| |>  < | | \ \ (_| | |_) | | \ \  __/ | | | (_| |  __/ | |  __/ |   
  \_____/_/\_\|_|  \_\__, |_.__/|_|  \_\___|_| |_|\__,_|\___|_|  \___|_|   
                      __/ |                                                
                     |___/           
GRRLIB Version : 1.6

Coder : NoNameNo
	uschghost
                 
info : http://wiibrew.org/index.php?title=Homebrew_apps/GRRLIB                   
********************************************************************************************/
#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>

static unsigned Seed1;
static unsigned Seed2;

u16 *GRRLIB_buffer;
u16 GRRLIB_magenta=0xf01e;

void GRRLIB_FillScreen(u16 color){
        int x;
        for(x=0;x<640*480;x++){
                GRRLIB_buffer[x]=color;
        }
}


void GRRLIB_PutPixel(int x,int y, u16 color){
        if(((x>=0) & (x<640)) & ((y>=0) & (y<480))){
                GRRLIB_buffer[y*640+x]=color;
        }
}


void GRRLIB_Line(short int x1, short int y1, short int x2, short int y2, u16 color){
 short int dx, dy, sdx, sdy, x, y, px, py;

 x = 0;
 y = 0;
 dx = x2 - x1;
 dy = y2 - y1;
 sdx = (dx < 0) ? -1 : 1;
 sdy = (dy < 0) ? -1 : 1;
 dx = sdx * dx + 1;
 dy = sdy * dy + 1;
 px = x1;
 py = y1;

 if (dx >= dy)
 {
        for (x = 0; x < dx; x++)
        {
                GRRLIB_PutPixel(px,py,color);
                y += dy;
                if (y >= dx)
                {
                        y -= dx;
                        py += sdy;
                }
                px += sdx;
        }
 }
   else
 {
        for (y = 0; y < dy; y++)
        {
                GRRLIB_PutPixel(px,py,color);
                x += dx;
                if (x >= dy)
                {
                        x -= dy;
                        px += sdx;
                }
                py += sdy;
        }
 }
}

void GRRLIB_DrawImg(int xpos, int ypos, int width, int high, const u16 data[], float rad_ang, float rapport ){
 int ni=0;
 int mx, my, mxdest, mydest;    /* Coordonnées des centres des surfaces */
 int i=0, j;                    /* Coordonnées de travail lors du parcourt de la surface de destination */
 unsigned int bx, by;           /* Coordonnées de travail, correspondent à i et j dans l'image d'origine */
 int x, y;                      /* valeurs de travail (bx et by avant arrondit */
 int ligne_x, ligne_y;          /* valeurs au debut des lignes (retour de i à 0) */
 int dxi, dyi, dxj, dyj;        /* increments des x et y suivant i et j (compteurs des x et y) */
 float tcos, tsin;              /* cos et sin de angle_radian */
 int largeurdest, hauteurdest;  /* Dimentions de la surface de destination */

   if((rad_ang==0)&(rapport==1)){
	 for(y=ypos;y<ypos+high;y++){
                for(x=xpos;x<xpos+width;x++){
						if(data[ni]!=GRRLIB_magenta)
                        GRRLIB_PutPixel(x,y,data[ni]);
                        ni++;
                }
        }	
   }
   else{
        tcos = cos(rad_ang);
        tsin = sin(rad_ang);

        largeurdest = (int) ceil( (width * fabs(tcos) + high * fabs(tsin)) * rapport );
        hauteurdest = (int) ceil( (width * fabs(tsin) + high * fabs(tcos)) * rapport );

        mxdest = (int) largeurdest/2;
        mydest = (int) hauteurdest/2;
        mx = (int) width/2;
        my = (int) high/2;

        /* Initialisation et calculs des incréments */
        ligne_x = mx*65536 + (-tcos * mxdest - tsin * mydest) / rapport * 65536.0;
        ligne_y = my*65536 + ( tsin * mxdest - tcos * mydest) / rapport * 65536.0;
        dxi = tcos / rapport * 65536.0;
        dxj = tsin / rapport * 65536.0;
        dyi = -tsin / rapport * 65536.0;
        dyj = tcos / rapport * 65536.0;

        for(j=0;j<hauteurdest;j++){
                x = ligne_x;
                y = ligne_y;
                for(i=0;i<largeurdest;i++) {
                        bx = ((unsigned int) x) >> 16;
                        by = ((unsigned int) y) >> 16;
                        /* on vérifie que l'on ne sort pas des bords*/
                        if (bx>=0 && bx< width && by>=0 && by< high){
								if(data[by*width+bx]!=GRRLIB_magenta)
                                GRRLIB_PutPixel(i+xpos-mxdest+mx,j+ypos-mydest+my,data[by*width+bx]);
                        }

                                /* On incrémente les coordonnées dans l'image d'origine */
                        x += dxi;
                        y += dyi;
                }
                /* incrément pour le changement de ligne */
                ligne_x += dxj;
                ligne_y += dyj;
        }

   }
}




void GRRLIB_DrawTile(int xpos, int ypos, int width, int high, const u16 data[], int frame, float rad_ang, float rapport ){
 int ni=(width*high*frame);
 int mx, my, mxdest, mydest;    /* Coordonnées des centres des surfaces */
 int i=0, j;                    /* Coordonnées de travail lors du parcourt de la surface de destination */
 unsigned int bx, by;           /* Coordonnées de travail, correspondent à i et j dans l'image d'origine */
 int x, y;                      /* valeurs de travail (bx et by avant arrondit */
 int ligne_x, ligne_y;          /* valeurs au debut des lignes (retour de i à 0) */
 int dxi, dyi, dxj, dyj;        /* increments des x et y suivant i et j (compteurs des x et y) */
 float tcos, tsin;              /* cos et sin de angle_radian */
 int largeurdest, hauteurdest;  /* Dimentions de la surface de destination */

   if((rad_ang==0)&(rapport==1)){
	for(y=ypos;y<ypos+high;y++){
				for(x=xpos;x<xpos+width;x++){
						if(data[ni]!=GRRLIB_magenta)
							GRRLIB_PutPixel(x,y,data[ni]);
						ni++;
                }
        }
   }
   else{
        tcos = cos(rad_ang);
        tsin = sin(rad_ang);

        largeurdest = (int) ceil( (width * fabs(tcos) + high * fabs(tsin)) * rapport );
        hauteurdest = (int) ceil( (width * fabs(tsin) + high * fabs(tcos)) * rapport );

        mxdest = (int) largeurdest/2;
        mydest = (int) hauteurdest/2;
        mx = (int) width/2;
        my = (int) high/2;

        /* Initialisation et calculs des incréments */
        ligne_x = mx*65536 + (-tcos * mxdest - tsin * mydest) / rapport * 65536.0;
        ligne_y = my*65536 + ( tsin * mxdest - tcos * mydest) / rapport * 65536.0;
        dxi = tcos / rapport * 65536.0;
        dxj = tsin / rapport * 65536.0;
        dyi = -tsin / rapport * 65536.0;
        dyj = tcos / rapport * 65536.0;

        for(j=0;j<hauteurdest;j++){
                x = ligne_x;
                y = ligne_y;
                for(i=0;i<largeurdest;i++) {
                        bx = ((unsigned int) x) >> 16;
                        by = ((unsigned int) y) >> 16;
                        /* on vérifie que l'on ne sort pas des bords*/
                        if (bx>=0 && bx< width && by>=0 && by< high){
								if(data[by*width+bx]!=GRRLIB_magenta)
                                GRRLIB_PutPixel(i+xpos-mxdest+mx,j+ypos-mydest+my,data[by*width+bx+(width*high*frame)]);
                        }

                                /* On incrémente les coordonnées dans l'image d'origine */
                        x += dxi;
                        y += dyi;
                }
                /* incrément pour le changement de ligne */
                ligne_x += dxj;
                ligne_y += dyj;
        }

   }
}

void GRRLIB_DrawMap(int xsize, int ysize, int map[],const u16 tdata[] ,int tsize,int scrollx, int scrolly){
	int m,n;

	for(m=0;m<=(xsize-1);m++)
		for(n=0;n<=(ysize-1);n++)
			GRRLIB_DrawTile((m*tsize)+scrollx,(n*tsize)+scrolly,tsize,tsize,tdata,map[m+n*xsize],0,1);

}


void GRRLIB_DrawRectangle(int x, int y, int hight, int widht, u16 color, int filled)
{
	int i;
	if(filled==0)
	{
		GRRLIB_Line(x, y, x + widht, y, color);
		GRRLIB_Line(x, y, x, y + hight, color);
		GRRLIB_Line(x, y+ hight, x + widht, y + hight, color);
		GRRLIB_Line(x + widht, y, x + widht, y + hight, color);
	}
	else
	{
		for(i=0;i<=hight;i++)
			GRRLIB_Line(x, y+i, x + widht, y+i, color);
	}
}

void GRRLIB_DrawTriangle(int x, int y, int xa, int ya, int xb, int yb, u16 color)
{
	GRRLIB_Line(x, y, xa, ya, color);
	GRRLIB_Line(x, y, xb, yb, color);
	GRRLIB_Line(xa, ya, xb, yb, color);
}



int GRRLIB_GetMapTile(int x, int y, int map[],int mapwidth)
{
	return map[x+y*mapwidth];
}










u16 GRRLIB_Conv888_565(u32 colorin){
	u16 colorout;
	u8 r,g,b;

	r = (((colorin >> 16) & 0xFF)*31)/255;
	g = (((colorin >> 8) & 0xFF)*63)/255;
	b = (((colorin) & 0xFF)*31)/255;
	colorout=(((r<<6)|g)<<5)|b;

	return(colorout);	
}

unsigned GRRLIB_Rand(unsigned long MAX)
{
        Seed1 = (Seed1 + 46906481) ^ Seed2;
        Seed2 = Seed1 ^ ( ((Seed2<<3) | (Seed2 >> 29)) + 103995407);
        return (Seed1 - Seed2)%MAX;
}
























//********************************************************************************************
// ALL CODE ABOVE THIS LINE IS THE "GX RGB RENDERER" CORE !!!!!!!!!!!!!!!!!!!! DONT TOUCH !!!!
//********************************************************************************************

#define DEFAULT_FIFO_SIZE (256 * 1024)

#define HASPECT 80
#define VASPECT 60

static u32 whichfb;
static u32 *xfb[2];
static GXRModeObj *vmode;

static u8 *gp_fifo;

static u8 *texturemem;
static u32 texturesize;

GXTexObj texobj;
static Mtx view;
static u16 vwidth, vheight, oldvwidth, oldvheight;

typedef struct tagcamera {
        Vector pos;
        Vector up;
        Vector view;
} camera;

static s16 square[] ATTRIBUTE_ALIGN(32) = {
        -HASPECT, VASPECT, 0,     // 0
        HASPECT, VASPECT, 0,      // 1
        HASPECT, -VASPECT, 0,     // 2
        -HASPECT, -VASPECT, 0,    // 3
};

static camera cam = {
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.5f, 0.0f },
        { 0.0f, 0.0f, -0.5f }
};

void GRRLIB_InitVideo () {
        vmode = VIDEO_GetPreferredMode(NULL);

        VIDEO_Configure (vmode);

        xfb[0] = (u32 *) MEM_K0_TO_K1 (SYS_AllocateFramebuffer (vmode));
        xfb[1] = (u32 *) MEM_K0_TO_K1 (SYS_AllocateFramebuffer (vmode));
        gp_fifo = (u8 *) memalign (32, DEFAULT_FIFO_SIZE);

        VIDEO_ClearFrameBuffer (vmode, xfb[0], COLOR_BLACK);
        VIDEO_ClearFrameBuffer (vmode, xfb[1], COLOR_BLACK);

        whichfb = 0;
        VIDEO_SetNextFramebuffer (xfb[whichfb]);
        VIDEO_SetBlack (FALSE);
        VIDEO_Flush ();
        VIDEO_WaitVSync ();

        if (vmode->viTVMode & VI_NON_INTERLACE)
                VIDEO_WaitVSync ();
}

static void draw_init (void) {
        GX_ClearVtxDesc ();
        GX_SetVtxDesc (GX_VA_POS, GX_INDEX8);
        GX_SetVtxDesc (GX_VA_CLR0, GX_INDEX8);
        GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);

        GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
        GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
        GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

        GX_SetArray (GX_VA_POS, square, 3 * sizeof (s16));

        GX_SetNumTexGens (1);
        GX_SetTexCoordGen (GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

        GX_InvalidateTexAll ();

        GX_InitTexObj (&texobj, texturemem, vwidth, vheight, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
}

static void draw_vert (u8 pos, u8 c, f32 s, f32 t) {
        GX_Position1x8 (pos);
        GX_Color1x8 (c);
        GX_TexCoord2f32 (s, t);
}

static void draw_square (Mtx v) {
        Mtx m;  // model matrix.
        Mtx mv; // modelview matrix.

        guMtxIdentity (m);
        guMtxTransApply (m, m, 0, 0, -100);
        guMtxConcat (v, m, mv);

        GX_LoadPosMtxImm (mv, GX_PNMTX0);
        GX_Begin (GX_QUADS, GX_VTXFMT0, 4);
        draw_vert (0, 0, 0.0, 0.0);
        draw_vert (1, 0, 1.0, 0.0);
        draw_vert (2, 0, 1.0, 1.0);
        draw_vert (3, 0, 0.0, 1.0);
        GX_End ();
}

void GRRLIB_Start(){
	u16 width=640;
	u16 height=480;
	s16 haspect=80;
	s16 vaspect=60;
	
        Mtx p;

        /*** Set new aspect ***/
        square[0] = square[9] = -haspect;
        square[3] = square[6] = haspect;
        square[1] = square[4] = vaspect;
        square[7] = square[10] = -vaspect;

        texturesize = (width * height) * 2;
        texturemem = (u8 *) memalign (32, texturesize);

        GXColor gxbackground = { 0, 0, 0, 0xff };

        memset (gp_fifo, 0, DEFAULT_FIFO_SIZE);

        GX_Init (gp_fifo, DEFAULT_FIFO_SIZE);
        GX_SetCopyClear (gxbackground, 0x00ffffff);

        GX_SetViewport (0, 0, vmode->fbWidth, vmode->efbHeight, 0, 1);
        GX_SetDispCopyYScale ((f32) vmode->xfbHeight / (f32) vmode->efbHeight);
        GX_SetScissor (0, 0, vmode->fbWidth, vmode->efbHeight);
        GX_SetDispCopySrc (0, 0, vmode->fbWidth, vmode->efbHeight);
        GX_SetDispCopyDst (vmode->fbWidth, vmode->xfbHeight);
        GX_SetCopyFilter (vmode->aa, vmode->sample_pattern, GX_TRUE, vmode->vfilter);
        GX_SetFieldMode (vmode->field_rendering, ((vmode->viHeight == 2 * vmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));
        GX_SetPixelFmt (GX_PF_RGB8_Z24, GX_ZC_LINEAR);
        GX_SetCullMode (GX_CULL_NONE);
        GX_CopyDisp (xfb[whichfb ^ 1], GX_TRUE);
        GX_SetDispCopyGamma (GX_GM_1_0);

        guPerspective (p, 60, 1.33f, 10.0f, 1000.0f);
        GX_LoadProjectionMtx (p, GX_PERSPECTIVE);
        memset (texturemem, 0, texturesize);

        vwidth = vheight = -1;
}

void GRRLIB_Render () {
	u16 width=640;
	u16 height=480;
	u8 *buffer=(u8*)GRRLIB_buffer;
	u16 pitch= 640*2;
        u16 h, w;
        u64 *dst = (u64 *) texturemem;
        u64 *src1 = (u64 *) buffer;
        u64 *src2 = (u64 *) (buffer + pitch);
        u64 *src3 = (u64 *) (buffer + (pitch * 2));
        u64 *src4 = (u64 *) (buffer + (pitch * 3));
        u16 rowpitch = (pitch >> 3) * 3 + pitch % 8;

        vwidth = width;
        vheight = height;

        whichfb ^= 1;

        if ((oldvheight != vheight) || (oldvwidth != vwidth)) {
                /** Update scaling **/
                oldvwidth = vwidth;
                oldvheight = vheight;
                draw_init ();
                memset (&view, 0, sizeof(Mtx));
                guLookAt (view, &cam.pos, &cam.up, &cam.view);
                GX_SetViewport (0, 0, vmode->fbWidth, vmode->efbHeight, 0, 1);
        }

        GX_InvVtxCache ();
        GX_InvalidateTexAll ();
        GX_SetTevOp (GX_TEVSTAGE0, GX_DECAL);
        GX_SetTevOrder (GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

        for (h = 0; h < vheight; h += 4) {
                for (w = 0; w < (vwidth >> 2); w++) {
                        *dst++ = *src1++;
                        *dst++ = *src2++;
                        *dst++ = *src3++;
                        *dst++ = *src4++;
                }

                src1 += rowpitch;
                src2 += rowpitch;
                src3 += rowpitch;
                src4 += rowpitch;
        }

        DCFlushRange (texturemem, texturesize);

        GX_SetNumChans (1);
        GX_LoadTexObj (&texobj, GX_TEXMAP0);

        draw_square (view);

        GX_DrawDone ();

        GX_SetZMode (GX_TRUE, GX_LEQUAL, GX_TRUE);
        GX_SetColorUpdate (GX_TRUE);
        GX_CopyDisp (xfb[whichfb], GX_TRUE);
        GX_Flush ();

        VIDEO_SetNextFramebuffer (xfb[whichfb]);
        VIDEO_Flush ();
}


void GRRLIB_Print(int x, int y, int xsize, int ysize,char* text,const u16 tdata[], int fg, int bg){
	int px,py;
	int ni;
	int i;

	for(i=0;i<strlen(text);i++){	
		ni=(xsize*ysize*(text[i]-1));
		for(py=y;py<y+ysize;py++){
			for(px=x;px<x+xsize;px++){
				if(tdata[ni]==0xffff)
                			GRRLIB_PutPixel(px+i*xsize,py,bg);
				else
					GRRLIB_PutPixel(px+i*xsize,py,fg);
                		ni++;
                	}	
         	}
	}
}

void GRRLIB_ImgPrint(int x, int y, int xsize, int ysize,char* text,const u16 tdata[]){
        int i;
        for(i=0;i<strlen(text);i++){
                GRRLIB_DrawTile(x+i*xsize,y,xsize,ysize,tdata,text[i]-32,0,1);
        }
}

/**********************************************************************************
*
* The "hidden" ScreenShoter Routine is here ;)
*
***********************************************************************************/
/*
void GRRLIB_ScrShot(char* file){
    SDCARD_Init();
    sd_file *handle;
    handle = SDCARD_OpenFile(file, "w");
    SDCARD_WriteFile(handle, (u16*)GRRLIB_buffer, 640*480*2);
    SDCARD_CloseFile(handle);
}
*/
