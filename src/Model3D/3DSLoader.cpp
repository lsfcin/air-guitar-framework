#include "3DSLoader.h"

static int num_tex = -1;

void LoadTexture(char* tex_filename, int& texID) 
{
    int i, j=0;
    FILE *l_file;
    unsigned char *l_texture;

    BITMAPFILEHEADER fileheader; 
    BITMAPINFOHEADER infoheader;
    RGBTRIPLE rgb;

    num_tex++;

    if( (l_file = fopen(tex_filename, "rb"))==NULL) {
      printf("Texture loading problem occurred!\n");
      exit(0); 
    }
    
    fread(&fileheader, sizeof(fileheader), 1, l_file);
    
    fseek(l_file, sizeof(fileheader), SEEK_SET);
    fread(&infoheader, sizeof(infoheader), 1, l_file);


    l_texture = (byte *) malloc(infoheader.biWidth * infoheader.biHeight * 4);
    memset(l_texture, 0, infoheader.biWidth * infoheader.biHeight * 4);

    for (i=0; i < infoheader.biWidth*infoheader.biHeight; i++)
    {            
            fread(&rgb, sizeof(rgb), 1, l_file); 

            l_texture[j+0] = rgb.rgbtRed; 
            l_texture[j+1] = rgb.rgbtGreen;
            l_texture[j+2] = rgb.rgbtBlue;
            l_texture[j+3] = 255;
            j += 4;
    }

    fclose(l_file);
     
    glBindTexture(GL_TEXTURE_2D, num_tex);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // We don't combine the color with the original surface color, use only the texture map.

    // Finally we define the 2d texture
    glTexImage2D(GL_TEXTURE_2D, 0, 4, infoheader.biWidth, infoheader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);

    // And create 2d mipmaps for the minifying function
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, infoheader.biWidth, infoheader.biHeight, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);

    free(l_texture); // Free the memory we used to load the texture
}

void Load3DS (char *p_filename, obj_type_ptr p_object) {
	int i;
	
	FILE *l_file;
	
	unsigned short l_chunk_id;
	unsigned int l_chunk_lenght;

	unsigned char l_char;
	unsigned short l_qty;

	unsigned short l_face_flags;

  if ((l_file=fopen (p_filename, "rb"))== NULL) {
    printf("3D model loading problem occurred!\n");
    exit(0);
  }

	while (ftell (l_file) < filelength (fileno (l_file))) {
		fread (&l_chunk_id, 2, 1, l_file);
		fread (&l_chunk_lenght, 4, 1, l_file);

		switch (l_chunk_id) {
			
			case 0x4d4d: 
			break;    

			case 0x3d3d:
			break;
			
			case 0x4000: 
				i=0;
				do {
					fread (&l_char, 1, 1, l_file);
                    p_object->name[i] = l_char;
					i++;
                } while (l_char != '\0' && i < 20);
			break;

			case 0x4100:
			break;
			
			case 0x4110: 
				fread (&l_qty, sizeof (unsigned short), 1, l_file);
                p_object->vertices_qty = l_qty;
                for (i = 0; i < l_qty; i++) {
					fread (&p_object->vertex[i].x, sizeof(float), 1, l_file);
 					fread (&p_object->vertex[i].y, sizeof(float), 1, l_file);
					fread (&p_object->vertex[i].z, sizeof(float), 1, l_file);
				}
				break;

			case 0x4120:
				fread (&l_qty, sizeof (unsigned short), 1, l_file);
                p_object->polygons_qty = l_qty;

                for (i = 0; i < l_qty; i++) {
					p_object->polygon[i].a = 0;
					p_object->polygon[i].b = 0;
					p_object->polygon[i].c = 0;
					fread (&p_object->polygon[i].a, sizeof (unsigned short), 1, l_file);
					fread (&p_object->polygon[i].b, sizeof (unsigned short), 1, l_file);
					fread (&p_object->polygon[i].c, sizeof (unsigned short), 1, l_file);
					fread (&l_face_flags, sizeof (unsigned short), 1, l_file);
				}
                break;

			case 0x4140:
				fread (&l_qty, sizeof (unsigned short), 1, l_file);
				for (i=0; i<l_qty; i++)
				{
					fread (&p_object->mapcoord[i].u, sizeof (float), 1, l_file);
                    fread (&p_object->mapcoord[i].v, sizeof (float), 1, l_file);
				}
                break;

			default:
				 fseek(l_file, l_chunk_lenght-6, SEEK_CUR);
        } 
	}
	fclose (l_file);
}

void RenderObject(obj_type object) {
	glBindTexture(GL_TEXTURE_2D, object.id_texture);

	glColor3f(0.3f, 0.3f, 0.3f);
	glLineWidth(2.f);
	glBegin(GL_TRIANGLES);
    for (int l_index = 0; l_index < object.polygons_qty; l_index++) {

      // a
		  glTexCoord2f( object.mapcoord[ object.polygon[l_index].a ].u,
                    object.mapcoord[ object.polygon[l_index].a ].v);

      glVertex3f( object.vertex[ object.polygon[l_index].a ].x,
                  object.vertex[ object.polygon[l_index].a ].y,
                  object.vertex[ object.polygon[l_index].a ].z);

		  // b
      glTexCoord2f( object.mapcoord[ object.polygon[l_index].b ].u,
                    object.mapcoord[ object.polygon[l_index].b ].v);

      glVertex3f( object.vertex[ object.polygon[l_index].b ].x,
                  object.vertex[ object.polygon[l_index].b ].y,
                  object.vertex[ object.polygon[l_index].b ].z);
          
		  // c
      glTexCoord2f( object.mapcoord[ object.polygon[l_index].c ].u,
                    object.mapcoord[ object.polygon[l_index].c ].v);

      glVertex3f( object.vertex[ object.polygon[l_index].c ].x,
                  object.vertex[ object.polygon[l_index].c ].y,
                  object.vertex[ object.polygon[l_index].c ].z);
    }
  glEnd();
}
