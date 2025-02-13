/*
 *  Engine.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue Nov 28 2006.
 *  Copyright (c) 2006 Alyssa Milburn. All rights reserved.
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

#ifndef _ENGINE_H
#define _ENGINE_H

#include <ghc/filesystem.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

class caosValue;
class Backend;
class AudioBackend;
class MainCamera;
struct BackendEvent;

class Engine {
  protected:
	unsigned int tickdata;
	unsigned int ticktimes[10];
	unsigned int ticktimeptr;
	unsigned int lasttimestamp;

	bool w_down, a_down, s_down, d_down;
	std::map<std::string, std::string> wordlist_translations;

	bool cmdline_enable_sound;
	bool cmdline_norun;
	std::vector<std::string> cmdline_bootstrap;

	std::string gamename;

	std::map<std::string, Backend*> possible_backends;
	std::map<std::string, AudioBackend*> possible_audiobackends;
	std::string preferred_backend, preferred_audiobackend;

	void handleKeyboardScrolling();
	void handleResizedWindow();
	void handleMouseMove(BackendEvent& event);
	void handleMouseButton(BackendEvent& event);
	void handleTextInput(BackendEvent& event);
	void handleRawKeyDown(BackendEvent& event);
	void handleRawKeyUp(BackendEvent& event);

	class peFile* exefile;
	void loadGameData();

  public:
	std::map<std::string, caosValue> eame_variables; // non-serialised

	std::shared_ptr<class NetBackend> net;
	std::unique_ptr<class MusicManager> musicmanager;
	std::string getBackendName() { return preferred_backend; }
	std::string getAudioBackendName() { return preferred_audiobackend; }

	std::unique_ptr<MainCamera> camera;

	std::vector<std::string> wordlist;
	std::string translateWordlistWord(const std::string& s);
	class peFile* getExeFile() {
		return exefile;
	}

	std::string getGameName() { return gamename; }
	uint32_t get_ticks_msec() const;

	bool done;
	bool dorendering, fastticks, refreshdisplay;
	unsigned int version;
	std::string gametype;
	bool bmprenderer;
	std::string language;

	Engine();
	~Engine();
	void setBackend(Backend* b);
	std::string executeNetwork(std::string in);
	unsigned int msUntilTick();
	void drawWorld();
	void update();
	bool tick();
	void processEvents();

	void addPossibleBackend(std::string, Backend*);
	void addPossibleAudioBackend(std::string, AudioBackend*);

	bool parseCommandLine(int argc, char* argv[]);
	bool initialSetup();
	void shutdown();

	bool noRun() { return cmdline_norun; }
};

extern Engine engine;
#endif
/* vim: set noet: */
