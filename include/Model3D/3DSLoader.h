/*
 * ---------------- www.spacesimulator.net --------------
 *   ---- Space simulators and 3d engine tutorials ----
 *
 *  Author: Damiano Vitulli <info@spacesimulator.net>
 *
 * ALL RIGHTS RESERVED
 *
 * 
 * File header: 3DSLoader.h
 *  
 */

/**********************************************************
 *
 * TYPES DECLARATION
 *
 *********************************************************/

#ifndef __3DSLOADER_H__
#define __3DSLOADER_H__

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <io.h>

#include <GL\glut.h>
#include <GL\glaux.h>

#define MAX_VERTICES 8000 // Max number of vertices (for each object)
#define MAX_POLYGONS 8000 // Max number of polygons (for each object)

// Our vertex type
typedef struct{
    float x,y,z;
} vertex_type;

// The polygon (triangle), 3 numbers that aim 3 vertices
typedef struct{
    int a,b,c;
} polygon_type;

// The mapcoord type, 2 texture coordinates for each vertex
typedef struct{
    float u,v;
} mapcoord_type;

// The object type
typedef struct {
	char name[20];
    
	int vertices_qty;
    int polygons_qty;

    vertex_type vertex[MAX_VERTICES]; 
    polygon_type polygon[MAX_POLYGONS];
    mapcoord_type mapcoord[MAX_VERTICES];
    int id_texture;
} obj_type, *obj_type_ptr;

/**********************************************************
 *
 * FUNCTION Load3DS (obj_type_ptr, char *)
 *
 * This function loads a mesh from a 3ds file.
 * Please note that we are loading only the vertices, polygons and mapping lists.
 * If you need to load meshes with advanced features as for example: 
 * multi objects, materials, lights and so on, you must insert other chunk parsers.
 *
 *********************************************************/

void LoadTexture(char* tex_filename, int& texID);
void Load3DS (char* p_filename, obj_type_ptr ogg);
void RenderObject(obj_type object);

#endif //__3DSLOADER_H__