/*
 *  SDLBackend.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Oct 24 2004.
 *  Copyright (c) 2004-2006 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#ifndef _SDLBACKEND_H
#define _SDLBACKEND_H

#include "common/backend/Backend.h"

#include <SDL.h>
#include <array>
#include <memory>
#include <string>

class SDLBackend;

class SDLRenderTarget : public RenderTarget {
  public:
	SDLBackend* parent;
	SDL_Texture* texture = nullptr;
	int drawablewidth = 0;
	int drawableheight = 0;
	float scale = 1.0;
	int viewport_offset_top = 0;
	int viewport_offset_bottom = 0;

	SDLRenderTarget(SDLBackend* parent);
	SDLRenderTarget(const SDLRenderTarget&) = delete;
	SDLRenderTarget(SDLRenderTarget&&) = delete;
	SDLRenderTarget& operator=(const SDLRenderTarget&) = delete;
	SDLRenderTarget& operator=(SDLRenderTarget&&) = delete;
	~SDLRenderTarget();
	void renderTexture(const Texture& tex, Rect src, RectF dest, RenderOptions options);
	void renderCreaturesImage(creaturesImage& tex, unsigned int frame, int x, int y, RenderOptions options);
	void renderLine(float x1, float y1, float x2, float y2, unsigned int color);
	void blitRenderTarget(RenderTarget* src, int x, int y, int w, int h);
	unsigned int getWidth() const;
	unsigned int getHeight() const;
	void renderClear();
	void setViewportOffsetTop(int offset_top);
	void setViewportOffsetBottom(int offset_bottom);
};

class SDLBackend : public Backend {
	friend class SDLRenderTarget;

  protected:
	int windowwidth, windowheight;
	SDL_Renderer* renderer = nullptr;
	std::shared_ptr<SDLRenderTarget> mainrendertarget;
	Uint32 last_frame_end = 0;

	SDLBackend();
	SDLBackend(const SDLBackend&) = delete;
	SDLBackend(SDLBackend&&) = delete;
	SDLBackend& operator=(const SDLBackend&) = delete;
	SDLBackend& operator=(SDLBackend&&) = delete;

	void resizeNotify(int _w, int _h);
	SDL_Surface* getMainSDLSurface() { return SDL_GetWindowSurface(window); }

  public:
	SDL_Window* window = nullptr;

	static Backend* get_instance();
	void init(const std::string& name, int width, int height);
	void waitForNextDraw();
	void drawDone();
	void shutdown();

	void resize(int w, int h) { resizeNotify(w, h); }

	bool pollEvent(BackendEvent& e);

	unsigned int ticks() { return SDL_GetTicks(); }

	Texture createTexture(const Image& image);
	Texture createTextureWithTransparentColor(const Image& image, Color transparent);

	std::shared_ptr<RenderTarget> getMainRenderTarget();
	std::shared_ptr<RenderTarget> newRenderTarget(unsigned int width, unsigned int height);

	bool keyDown(int key);
	int translateScancode(int key);
};

#endif