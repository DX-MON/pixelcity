/*-----------------------------------------------------------------------------

  Sky.cpp

  2009 Shamus Young

-------------------------------------------------------------------------------

  Did this need to be written as a class? It did not. There will never be
  more than one sky in play, so the whole class structure here is superflous,
  but harmless.

-----------------------------------------------------------------------------*/

#define SKYPOINTS      24

#ifdef _WINDOWS
	#include <windows.h>
#endif
#include <math.h>
#if defined(__MACOSX__)
	#include <OpenGL/gl.h>
#elif defined(__MACOS__)
	#include <GL/gl.h>
#else
	#include <GL/gl.h>
#include <GTK++/GTK++.h>
#endif

#include "Camera.h"
#include "Macro.h"
#include "Math.h"
#include "Random.h"
#include "Render.h"
#include "Sky.h"
#include "Texture.h"
#include "glTypes.h"
#include "World.h"
#include "OS.h"

static CSky* sky;

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void SkyRender()
{

	if (sky && !RenderFlat())
		sky->Render();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void SkyClear()
{

	if (sky)
		delete sky;
	sky = NULL;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CSky::Render()
{
	GLvector angle, position;

	if (!TextureReady())
		return;
	glDepthMask(false);
	glPushAttrib(GL_POLYGON_BIT | GL_FOG_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_FOG);
	glPushMatrix();
	glLoadIdentity();
	angle = CameraAngle();
	position = CameraPosition();
	glRotatef(angle.x, 1.0f, 0.0f, 0.0f);
	glRotatef(angle.y, 0.0f, 1.0f, 0.0f);
	glRotatef(angle.z, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, -position.y / 100.0f, 0.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TextureId(TEXTURE_SKY));
	glCallList(m_list);
	glPopMatrix();
	glPopAttrib();
	glDepthMask(true);
	glEnable(GL_COLOR_MATERIAL);
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

CSky::CSky()
{

	GLvertex circle[SKYPOINTS];
	GLvector pos;
	float angle;
	int i;
	float size;
	float rad;
	float lum;

	hwnd->glBegin();
	size = 10.0f;
	for (i = 0; i < SKYPOINTS; i++)
	{
		angle = (float)i / (float)(SKYPOINTS - 1);
		angle *= 360;
		angle *= DEGREES_TO_RADIANS;
		circle[i].position.x = sinf(angle) * size;
		circle[i].position.y = 0.1f;
		circle[i].position.z = cosf(angle) * size;
		circle[i].uv.x = ((float)i / (float)(SKYPOINTS - 1)) * 5.0f;
		circle[i].uv.y = 0.5f;
		rad = ((float)i / (SKYPOINTS - 1)) * 180.0f * DEGREES_TO_RADIANS;
		lum = sinf(rad);
		lum = (float)pow(lum, 5);
		circle[i].color = glRgba(lum);
	}
	m_list = glGenLists(1);
	glNewList(m_list, GL_COMPILE);
	glColor3f(1, 1, 1);

	glBegin(GL_QUAD_STRIP);
	for (i = 0; i < SKYPOINTS; i++)
	{
		glTexCoord2f(circle[i].uv.x, 0.0f);
		glVertex3fv(&circle[i].position.x);
		pos = circle[i].position;
		pos.y = size / 3.5f;
		glTexCoord2f(circle[i].uv.x, 1.0f);
		glVertex3fv(&pos.x);
	}
	glEnd();
	glEndList();
	sky = this;
	hwnd->glEnd();
}
