/*
 * ---------------- www.spacesimulator.net --------------
 *   ---- Space simulators and 3d engine tutorials ----
 *
 * Author: Damiano Vitulli <info@spacesimulator.net>
 *
 * ALL RIGHTS RESERVED
 *
 *
 * Tutorial 4: 3d engine - 3ds models loader
 * 
 * File header: texture.h
 *  
 */

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <stdio.h>
#include <windows.h>
#include <GL/glut.h>



/**********************************************************
 *
 * VARIABLES DECLARATION
 *
 *********************************************************/

// Counter to keep track of the last loaded texture
extern int num_texture;



/**********************************************************
 *
 * FUNCTION LoadBitmap(char *)
 *
 * This function loads a bitmap file and return the OpenGL reference ID to use that texture
 *
 *********************************************************/

extern int LoadBitmap(char *filename);


#endif //__TEXTURE_H__