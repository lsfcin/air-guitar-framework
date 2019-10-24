/*
 *	gsub_lite.h
 *
 *	Graphics Subroutines (Lite) for ARToolKit.
 *
 *	Copyright (c) 2003-2005 Philip Lamb (PRL) phil@eden.net.nz. All rights reserved.
 *	
 *	Rev		Date		Who		Changes
 *  2.7.0   2003-08-13  PRL     Complete rewrite to ARToolKit-2.65 gsub.c API.
 *  2.7.1   2004-03-03  PRL		Avoid defining BOOL if already defined
 *	2.7.1	2004-03-03	PRL		Don't enable lighting if it was not enabled.
 *	2.7.2	2004-04-27	PRL		Added headerdoc markup. See http://developer.apple.com/darwin/projects/headerdoc/
 *	2.7.3	2004-07-02	PRL		Much more object-orientated through use of ARGL_CONTEXT_SETTINGS type.
 *	2.7.4	2004-07-14	PRL		Added gluCheckExtension hack for GLU versions pre-1.3.
 *	2.7.5	2004-07-15	PRL		Added arglDispImageStateful(); removed extraneous glPixelStorei(GL_UNPACK_IMAGE_HEIGHT,...) calls.
 *	2.7.6	2005-02-18	PRL		Go back to using int rather than BOOL, to avoid conflict with Objective-C.
 *	2.7.7	2005-07-26	PRL		Added cleanup routines for texture stuff.
 *	2.7.8	2005-07-29	PRL		Added distortion compensation enabling/disabling.
 *	2.7.9	2005-08-15	PRL		Added complete support for runtime selection of pixel format and rectangle/power-of-2 textures.
 *
 */
/*
 * 
 * This file is part of ARToolKit.
 * 
 * ARToolKit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * ARToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ARToolKit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

/**
	\file gsub_lite.h
	\brief A collection of useful OpenGL routines for ARToolKit.
	\cond GSUB_LITE
	\htmlinclude gsub_lite/index.html
*/

/*!
	@header gsub_lite
	@abstract A collection of useful OpenGL routines for ARToolKit.
	@discussion
		Sample code for example usage of gsub_lite is included with
		ARToolKit, in the directory &lt;AR/examples/simpleLite&gt;.
		
		gsub_lite is the preferred means for drawing camera video 
		images acquired from ARToolKit's video libraries. It includes
		optimized texture handling, and a variety of flexible drawing
		options.
 
		gsub_lite also provides utility functions for setting the
		OpenGL viewing frustum and camera position based on ARToolKit-
		camera parameters and marker positions.
 
		gsub_lite does not depend on GLUT, or indeed, any particular
		window or event handling system. It is therefore well suited
		to use in applications which have their own window and event
		handling code.
 
		gsub_lite v2.7 is intended as a replacement for gsub from
		ARToolKit 2.65, by Mark Billinghurst (MB) & Hirokazu Kato (HK),
		with the following additional functionality:
		<ul>
			<li> Support for true stereo and multiple displays through removal
			of most dependencies on global variables.
			<li> Prepared library for thread-safety by removing global variables.
			<li> Optimised texturing, particularly for Mac OS X platform.
			<li> Added arglCameraFrustum to replace argDraw3dCamera() function.
			<li> Renamed argConvGlpara() to arglCameraView() to more accurately
			represent its functionality.
			<li> Correctly handle textures with non-RGBA handling.
			<li> Library setup and cleanup functions.
			<li> Version numbering.
		</ul>
		It does however lack the following functionality from the original gsub
		library:
		<ul>
			<li> GLUT event handling.
			<li> Sub-window ("MINIWIN") and half-size drawing.
			<li> HMD support for stereo via stencil.
		</ul>
			
		This file is part of ARToolKit.
		
		ARToolKit is free software; you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation; either version 2 of the License, or
		(at your option) any later version.
		
		ARToolKit is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.
		
		You should have received a copy of the GNU General Public License
		along with ARToolKit; if not, write to the Free Software
		Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
	@copyright 2003-2005 Philip Lamb
	@updated 2005-07-29
 */

#ifndef __gsub_lite_h__
#define __gsub_lite_h__

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
//	Public includes.
// ============================================================================

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#else
#  ifdef _WIN32
#    include <windows.h>
#  endif
#  include <GL/gl.h>
#endif
#include <AR/config.h>
#include <AR/ar.h>		// ARUint8
#include <AR/param.h>	// ARParam, arParamDecompMat(), arParamObserv2Ideal()

// ============================================================================
//	Public types and definitions.
// ============================================================================

// Keep code nicely typed.
#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

/*!
    @typedef ARGL_CONTEXT_SETTINGS_REF
    @abstract Opaque type to hold ARGL settings for a given OpenGL context.
    @discussion
		An OpenGL context is an implementation-defined structure which
		keeps track of OpenGL state, including textures and display lists.
		Typically, individual OpenGL windows will have distinct OpenGL
		contexts assigned to them by the host operating system.
 
		As gsub_lite uses textures and display lists, it must be able to
		track which OpenGL context a given texture or display list it is using
		belongs to. This is especially important when gsub_lite is being used to
		draw into more than one window (and therefore more than one context.)
 
		Basically, functions which depend on OpenGL state, will require an
		ARGL_CONTEXT_SETTINGS_REF to be passed to them. An ARGL_CONTEXT_SETTINGS_REF
		is generated by setting the current OpenGL context (e.g. if using GLUT,
		you might call glutSetWindow()) and then calling arglSetupForCurrentContext().
		When you have finished using ARGL in a given context, you should call
		arglCleanup(), passing in an ARGL_CONTEXT_SETTINGS_REF, to free the
		memory used by the settings structure.
	@availability First appeared in ARToolKit 2.68.
 */
typedef struct _ARGL_CONTEXT_SETTINGS *ARGL_CONTEXT_SETTINGS_REF;

/*!
    @typedef ARGL_PIX_FORMAT
    @abstract ARToolKit pixel-format specifiers.
    @discussion
		ARToolKit functions can accept pixel data in a variety of formats.
		This enumerations provides a set of constants you can use to request
		data in a particular pixel format from an ARToolKit function that
		returns data to you, or to specify that data you are providing to an
		ARToolKit function is in a particular pixel format.
    @constant ARGL_PIX_FORMAT_RGBA
		Each pixel is represented by 32 bits. Eight bits per each Red, Green,
		Blue, and Alpha component.
	@constant ARGL_PIX_FORMAT_ABGR
		Each pixel is represented by 32 bits. Eight bits per each Alpha, Blue,
		Green, and Red component. This is the native 32 bit format for the SGI
		platform.
	@constant ARGL_PIX_FORMAT_BGRA
		Each pixel is represented by 32 bits. Eight bits per each Blue, Green,
		Red, and Alpha component. This is the native 32 bit format for the Win32
		platform.
	@constant ARGL_PIX_FORMAT_ARGB
		Each pixel is represented by 32 bits. Eight bits per each Alpha, Red,
		Green, and Blue component. This is the native 32 bit format for the Mac
		platform.
	@constant ARGL_PIX_FORMAT_RGB
		Each pixel is represented by 24 bits. Eight bits per each Red, Green,
		and Blue component. This is the native 24 bit format for the Mac platform.
	@constant ARGL_PIX_FORMAT_BGR
		Each pixel is represented by 24 bits. Eight bits per each Blue, Red, and
		Green component. This is the native 24 bit format for the Win32 platform.
	@constant ARGL_PIX_FORMAT_2vuy
		8-bit 4:2:2 Component Y'CbCr format. Each 16 bit pixel is represented
		by an unsigned eight bit luminance component and two unsigned eight bit
		chroma components. Each pair of pixels shares a common set of chroma
		values. The components are ordered in memory; Cb, Y0, Cr, Y1. The
		luminance components have a range of [16, 235], while the chroma value
		has a range of [16, 240]. This is consistent with the CCIR601 spec.
		This format is fairly prevalent on both Mac and Win32 platforms.
		'2vuy' is the Apple QuickTime four-character code for this pixel format.
		The equivalent Microsoft fourCC is 'UYVY'.
	@constant ARGL_PIX_FORMAT_yuvs
		8-bit 4:2:2 Component Y'CbCr format. Identical to the ARGL_PIX_FORMAT_2vuy except
		each 16 bit word has been byte swapped. This results in a component
		ordering of; Y0, Cb, Y1, Cr.
		This is most prevalent yuv 4:2:2 format on both Mac and Win32 platforms.
		'yuvs' is the Apple QuickTime four-character code for this pixel format.
		The equivalent Microsoft fourCC is 'YUY2'.
*/
typedef enum {
	ARGL_PIX_FORMAT_RGBA = 1,
	ARGL_PIX_FORMAT_ABGR = 2,
	ARGL_PIX_FORMAT_BGRA = 3,
	ARGL_PIX_FORMAT_ARGB = 4,
	ARGL_PIX_FORMAT_RGB = 5,
	ARGL_PIX_FORMAT_BGR = 6,
	ARGL_PIX_FORMAT_2vuy = 7,
	ARGL_PIX_FORMAT_UYVY = ARGL_PIX_FORMAT_2vuy,
	ARGL_PIX_FORMAT_yuvs = 8,
	ARGL_PIX_FORMAT_YUY2 = ARGL_PIX_FORMAT_yuvs,
} ARGL_PIX_FORMAT;

// ============================================================================
//	Public globals.
// ============================================================================

// It'd be nicer if these were wrapped in accessor functions.

/*!
    @var arglDrawMode
	@abstract Determines display method by which arglDispImage() transfers pixels. 
    @discussion
		The value of this variable determines the method by which
		arglDispImage transfers pixels of an image to the display. Setting this
		variable to a value of AR_DRAW_BY_GL_DRAW_PIXELS specifies the use of OpenGL
		DrawPixels to do the transfer. Setting this variable to a value of
		AR_DRAW_BY_TEXTURE_MAPPING specifies the use of OpenGL TexImage2D to do the
		transfer. The DrawPixels method is guaranteed to be available on all
		implementations, but arglDispImage does not correct the image
		for camera lens distortion under this method. In contrast, TexImage2D is only
		available on some implementations, but allows arglDispImage() to apply a correction
		for camera lens distortion, and additionally offers greater potential for
		accelerated drawing on some implementations.
 
		The initial value is defined to the value of the symbolic constant DEFAULT_DRAW_MODE
		(defined in &lt;AR/config.h&gt;).
	@availability First appeared in ARToolKit 2.68.
 */
extern int arglDrawMode;

/*!
    @var arglTexmapMode
	@abstract Determines use of full or half-resolution TexImage2D pixel-transfer in arglDispImage().
	@discussion
		When arglDrawMode is set to AR_DRAW_BY_TEXTURE_MAPPING, the value of this variable
		determines whether full or half-resolution data is transferred to the
		texture. A value of AR_DRAW_TEXTURE_FULL_IMAGE uses all available pixels in the
		source image data. A value of AR_DRAW_TEXTURE_HALF_IMAGE discards every second row
		in the source image data, defining a half-height texture which is then drawn stretched
		vertically to double its height.
 
		The latter method is well-suited to drawing interlaced images, as would be obtained 
		from DV camera sources in interlaced mode or composite video sources.
	@availability First appeared in ARToolKit 2.68.
 */
extern int arglTexmapMode;

/*!
    @var arglTexRectangle
	@abstract Determines use of rectangular TexImage2D pixel-transfer in arglDispImage().
	@discussion
		On implementations which support the OpenGL extension for rectangular textures (of
		non power-of-two size), and when arglDrawMode is set to AR_DRAW_BY_TEXTURE_MAPPING,
		the value of this variable determines whether rectangular textures or ordinary
		(power-of-two) textures are used by arglDispImage(). A value of TRUE specifies the
		use of rectangluar textures. A value of FALSE specifies the use of ordinary textures.
		
		If the OpenGL driver available at runtime does not support for rectangular textures,
		changing the value of this variable to TRUE will result calls to arglDispImage
		performing no drawing.
	@availability First appeared in ARToolKit 2.68.
 */
extern int arglTexRectangle;

#if defined(__APPLE__) && defined(APPLE_TEXTURE_FAST_TRANSFER)
extern int arglAppleClientStorage;
extern int arglAppleTextureRange;
extern GLuint arglAppleTextureRangeStorageHint;
#endif // __APPLE__ && APPLE_TEXTURE_FAST_TRANSFER

// ============================================================================
//	Public functions.
// ============================================================================

/*!
    @function arglSetupForCurrentContext
    @abstract Initialise the gsub_lite library for the current OpenGL context.
    @discussion
		This function performs required setup of the gsub_lite library
		for the current OpenGL context and must be called before any other argl*()
		functions are called for this context.
 
		An OpenGL context holds all of the state of the OpenGL machine, including
		textures and display lists etc. There will usually be one OpenGL context
		for each window displaying OpenGL content.
 
		Other argl*() functions whose operation depends on OpenGL state will
		require an ARGL_CONTEXT_SETTINGS_REF. This is just so that
		they can keep track of per-context variables.
 
		You should call arglCleanup() passing in the ARGL_CONTEXT_SETTINGS_REF
		when you have finished with the library for this context.
    @result An ARGL_CONTEXT_SETTINGS_REF. See the documentation for this type for more info.
	@availability First appeared in ARToolKit 2.68.
*/
ARGL_CONTEXT_SETTINGS_REF arglSetupForCurrentContext(void);

/*!
    @function arglCleanup
    @abstract Free memory used by gsub_lite associated with the specified context.
    @discussion
		Should be called after no more argl* functions are needed, in order
		to prevent memory leaks etc.
 
		The library can be setup again for the context at a later time by calling
		arglSetupForCurrentContext() again.
	@param contextSettings A reference to ARGL's settings for an OpenGL
		context, as returned by arglSetupForCurrentContext().
	@availability First appeared in ARToolKit 2.68.
*/
void arglCleanup(ARGL_CONTEXT_SETTINGS_REF contextSettings);

/*!
    @function arglCameraFrustum
    @abstract Create an OpenGL perspective projection matrix.
    @discussion
		Use this function to create a matrix suitable for passing to OpenGL
		to set the viewing projection.
    @param cparam Pointer to a set of ARToolKit camera parameters for the
		current video source.
	@param focalmax The maximum distance at which geometry will be rendered.
		Any geometry further away from the camera than this distance will be clipped
		and will not be appear in a rendered frame. Thus, this value should be
		set high enough to avoid clipping of any geometry you care about. However,
		the precision of the depth buffer is correlated with the ratio of focalmin
		to focalmax, thus you should not set focalmax any higher than it needs to be.
		This value should be specified in the same units as your OpenGL drawing.
	@param focalmin The minimum distance at which geometry will be rendered.
		Any geometry closer to the camera than this distance will be clipped
		and will not be appear in a rendered frame. Thus, this value should be
		set low enough to avoid clipping of any geometry you care about. However,
		the precision of the depth buffer is correlated with the ratio of focalmin
		to focalmax, thus you should not set focalmin any lower than it needs to be.
		Additionally, geometry viewed in a stereo projections that is too close to
		camera is difficult and tiring to view, so if you are rendering stereo
		perspectives you should set this value no lower than the near-point of
		the eyes. The near point in humans varies, but usually lies between 0.1 m
		0.3 m. This value should be specified in the same units as your OpenGL drawing.
	@param m_projection Pointer to a array of 16 GLdoubles, which will be filled
		out with a projection matrix suitable for passing to OpenGL. The matrix
		is specified in column major order.
	@availability First appeared in ARToolKit 2.68.
*/
void arglCameraFrustum(const ARParam *cparam, const double focalmin, const double focalmax, GLdouble m_projection[16]);

/*!
    @function arglCameraView
    @abstract Create an OpenGL viewing transformation matrix.
	@discussion
		Use this function to create a matrix suitable for passing to OpenGL
		to set the viewing transformation of the virtual camera.
	@param para Pointer to 3x4 matrix array of doubles which specify the
		position of an ARToolKit marker, as returned by arGetTransMat().
	@param m_modelview Pointer to a array of 16 GLdoubles, which will be filled
		out with a modelview matrix suitable for passing to OpenGL. The matrix
		is specified in column major order.
	@param scale Specifies a scaling between ARToolKit's
		units (usually millimeters) and OpenGL's coordinate system units.
	@availability First appeared in ARToolKit 2.68.
*/
void arglCameraView(double para[3][4], GLdouble m_modelview[16], double scale);

/*!
    @function arglDispImage
    @abstract Display an ARVideo image, by drawing it using OpenGL.
    @discussion
		This function draws an image from an ARVideo source to the current
		OpenGL context. This operation is most useful in video see-through
		augmented reality applications for drawing the camera view as a
		background image, but can also be used in other ways.
 
		An undistorted image is drawn with the lower-left corner of the
		bottom-left-most pixel at OpenGL screen coordinates (0,0), and the
		upper-right corner of the top-right-most pixel at OpenGL screen
		coodinates (x * zoom, y * zoom), where x and y are the values of the
		fields cparam->xsize and cparam->ysize (see below) and zoom is the
		value of the parameter zoom (also see below). If cparam->dist_factor
		indicates that an un-warping correction should be applied, the actual
		coordinates will differ from the values specified here. 
 
		OpenGL state: Drawing is performed with depth testing and lighting
		disabled, and thus leaves the the depth buffer (if any) unmodified. If
		pixel transfer is by texturing (see documentation for arglDrawMode),
		the drawing is done in replacement texture environment mode.
		The depth test enable and lighting enable state and the texture
		environment mode are restored before the function returns.
	@param image Pointer to the tightly-packed image data (as returned by
		arVideoGetImage()). The horizontal and vertical dimensions of the image
		data must exactly match the values specified in the fields cparam->xsize
		and cparam->ysize (see below).
 
		The first byte of image data corresponds to the first component of the
		top-left-most pixel in the image. The data continues with the remaining
		pixels of the first row, followed immediately by the pixels of the second
		row, and so on to the last byte of the image data, which corresponds to
		the last component of the bottom-right-most pixel.
	@param cparam Pointer to a set of ARToolKit camera parameters for the
		current video source. The size of the source image is taken from the
		fields xsize and ysize of the ARParam structure pointed to. Also, when
		the draw mode is AR_DRAW_BY_TEXTURE_MAPPING (see the documentation for
		the global variable arglDrawMode) the field dist_factor of the ARParam
		structure pointed to will be taken as the amount to un-warp the supplied
		image.		
	@param zoom The amount to scale the video image up or down. To draw the video
		image double size, use a zoom value of 2.0. To draw the video image
		half size use a zoom value of 0.5.
	@param contextSettings A reference to ARGL's settings for the current OpenGL
		context, as returned by arglSetupForCurrentContext() for this context. It
		is the callers responsibility to make sure that the current context at the
		time arglDisplayImage() is called matches that under which contextSettings
		was created.
	@availability First appeared in ARToolKit 2.68.
*/
void arglDispImage(ARUint8 *image, const ARParam *cparam, const double zoom, ARGL_CONTEXT_SETTINGS_REF contextSettings);

/*!
	@function arglDispImageStateful
    @abstract Display an ARVideo image, by drawing it using OpenGL, using and modifying current OpenGL state.
    @discussion
		This function is identical to arglDispImage except that whereas
		arglDispImage sets an orthographic 2D projection and the OpenGL state
		prior to drawing, this function does not. It also does not restore any
		changes made to OpenGL state.
 
		This allows you to do effects with your image, other than just drawing it
		2D and with the lower-left corner of the bottom-left-most pixel attached
		to the bottom-left (0,0) of the window. For example, you might use a
		perspective projection instead of an orthographic projection with a
		glLoadIdentity() / glTranslate() on the modelview matrix to place the
		lower-left corner of the bottom-left-most pixel somewhere other than 0,0
		and leave depth-testing enabled.
 
		See the documentation for arglDispImage() for more information.
	@availability First appeared in ARToolKit 2.68.2.
 */
void arglDispImageStateful(ARUint8 *image, const ARParam *cparam, const double zoom, ARGL_CONTEXT_SETTINGS_REF contextSettings);

/*!
    @function arglDistortionCompensationSet
    @abstract Set compensation for camera lens distortion in arglDispImage to off or on.
    @discussion
		By default, arglDispImage compensates for the distortion of the camera's
		acquired image caused by the lens when it draws. By calling this function
		with enabled = FALSE, this compensation will be disabled in the specified
		drawing context. It may be re-enabled at any time.
		This function is useful if you need to draw an image, but do not know the
		extent of the camera's lens distortion (such as during distortion calibration).
		While distortion compensation is disabled, the dist_factor[] array in a
		the camera cparam structure passed to arglDispImage is ignored.
	@param contextSettings A reference to ARGL's settings for the current OpenGL
		context, as returned by arglSetupForCurrentContext() for this context. 
	@param enable TRUE to enabled distortion compensation, FALSE to disable it.
		The default state for new contexts is enable = TRUE.
	@result TRUE if the distortion value was set, FALSE if an error occurred.
	@availability First appeared in ARToolKit 2.71.
*/
int arglDistortionCompensationSet(ARGL_CONTEXT_SETTINGS_REF contextSettings, int enable);

/*!
    @function arglDistortionCompensationGet
	@abstract Enquire as to the enable state of camera lens distortion compensation in arglDispImage.
	@discussion
		By default, arglDispImage compensates for the distortion of the camera's
		acquired image caused by the lens when it draws. This function enquires
		as to whether arglDispImage is currently doing compensation or not.
	@param contextSettings A reference to ARGL's settings for the current OpenGL
		context, as returned by arglSetupForCurrentContext() for this context. 
	@param enable Pointer to an integer value which will be set to TRUE if distortion
		compensation is enabled in the specified context, or FALSE if it is disabled.
	@result TRUE if the distortion value was retreived, FALSE if an error occurred.
	@availability First appeared in ARToolKit 2.71.
 */
int arglDistortionCompensationGet(ARGL_CONTEXT_SETTINGS_REF contextSettings, int *enable);

/*!
    @function arglPixelFormatSet
    @abstract Set the format of pixel data which will be passed to arglDispImage*()
    @discussion (description)
		In gsub_lite, the format of the pixels (i.e. the arrangement of components
		within each pixel) can be changed at runtime. Use this function to inform
		gsub_lite the format the pixels being passed to arglDispImage*() functions
		are in. This setting applies only to the context passed in parameter
		contextSettings. The default format is determined by which of the possible
		AR_PIXEL_FORMAT_xxxx symbols was defined at the time the library was built.
		Usually, image data is passed in directly from images generated by ARVideo,
		and so you should ensure that ARVideo is generating pixels of the same format.
	@param contextSettings A reference to ARGL's settings for the current OpenGL
		context, as returned by arglSetupForCurrentContext() for this context. 
    @param format A symbolic constant for the pixel format being set. See
		@link ARGL_PIX_FORMAT ARGL_PIX_FORMAT @/link for a list of all possible formats.
	@result TRUE if the pixel format value was set, FALSE if an error occurred.
	@availability First appeared in ARToolKit 2.71.
*/
int arglPixelFormatSet(ARGL_CONTEXT_SETTINGS_REF contextSettings, ARGL_PIX_FORMAT format);

/*!
    @function arglPixelFormatGet
    @abstract Get the format of pixel data in which arglDispImage*() is expecting data to be passed.
    @discussion This function enquires as to the current format of pixel data being
		expected by the arglDispImage*() functions. The default format is determined by
		which of the possible AR_PIXEL_FORMAT_xxxx symbols was defined at the time the
		library was built.
	@param contextSettings A reference to ARGL's settings for the current OpenGL
		context, as returned by arglSetupForCurrentContext() for this context. 
	@param format A symbolic constant for the pixel format in use. See
		@link ARGL_PIX_FORMAT ARGL_PIX_FORMAT @/link for a list of all possible formats.
	@param size The number of bytes of memory occupied per pixel, for the given format.
	@result TRUE if the pixel format and size values were retreived, FALSE if an error occurred.
	@availability First appeared in ARToolKit 2.71.
*/
int arglPixelFormatGet(ARGL_CONTEXT_SETTINGS_REF contextSettings, ARGL_PIX_FORMAT *format, int *size);

#ifdef __cplusplus
}
#endif

/**
	\endcond
 */

#endif /* !__gsub_lite_h__ */
