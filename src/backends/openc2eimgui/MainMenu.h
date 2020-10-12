#pragma once

#include "AgentInjector.h"
#include "audiobackend/AudioBackend.h"
#include "BrainViewer.h"
#include "CreatureGrapher.h"
#include "creatures/oldCreature.h"
#include "creatures/SkeletalCreature.h"
#include "Engine.h"
#include "Hatchery.h"
#include "historyManager.h"
#include "PointerAgent.h"
#include "World.h"

static void NewDebugNorn() {
  if (engine.version > 2) return; // TODO: fixme

	std::string genomefile = "test";
	shared_ptr<genomeFile> genome;
	try {
		genome = world.loadGenome(genomefile);
	} catch (creaturesException &e) {
    printf("Couldn't load genome file: %s\n", e.prettyPrint().c_str());
		return;
	}

	if (!genome) {
		//
		return;
	}

	SkeletalCreature *a = new SkeletalCreature(4);

	int sex = 1 + (int) (2.0 * (rand() / (RAND_MAX + 1.0)));
	oldCreature *c;
	try {
		if (engine.version == 1) c = new c1Creature(genome, (sex == 2), 0, a);
		else c = new c2Creature(genome, (sex == 2), 0, a);
	} catch (creaturesException &e) {
		delete a;
    printf("Couldn't create creature: %s\n", e.prettyPrint().c_str());
		return;
	}
	
	a->setCreature(c);
	a->finishInit();

	// if you make this work for c2e, you should probably set sane attributes here?

  a->setSlot(0, genome);
	world.newMoniker(genome, genomefile, a);
	world.history->getMoniker(world.history->findMoniker(genome)).moveToCreature(a);

	// TODO: set it dreaming
	
	c->born();

	world.hand()->addCarried(a);
}

static void NewRandomEgg() {
  std::string eggscript;
	/* create the egg obj */
	eggscript = fmt::sprintf("new: simp eggs 8 %d 2000 0\n", ((rand() % 6) * 8));
	/* set the pose */
	eggscript += "pose 3\n";	
	/* set the correct class/attributes */
	if (engine.version == 1)
		eggscript += "setv clas 33882624\nsetv attr 67\n";
	else if (engine.version == 2)
		eggscript += "setv cls2 2 5 2\nsetv attr 195\n";
	/* create the genome */
	if (engine.version == 1)
		eggscript += fmt::sprintf("new: gene tokn dad%d tokn mum%d obv0\n", (1 + rand() % 6), (1 + rand() % 6));
	else if (engine.version == 2)
		eggscript += "new: gene tokn norn tokn norn obv0\n";
	/* set the gender */
	eggscript += "setv obv1 0\n";
	/* start the clock */
	eggscript += "tick 2400\n";
	
	/* move it into place */
	/* TODO: good positions? */
	if (engine.version == 1)
		eggscript += fmt::sprintf("mvto %d 870\n", (2600 + rand() % 200));
	else if (engine.version == 2)
		eggscript += fmt::sprintf("mvto %d 750\n", (4900 + rand() % 350));

	/* c2: enable gravity */
	if (engine.version == 2)
		eggscript += "setv grav 1\n";

	std::string err = engine.executeNetwork(eggscript);
	if (err.size()) {
    printf("Couldn't create egg: %s\n", err.c_str());
  }
}


void DrawMainMenu() {
  if (ImGui::BeginMenu("File")) {
    if (ImGui::MenuItem("Quit")) {
      world.quitting = true;
    }
    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("View")) {
    ImGui::MenuItem("Show Scrollbars", nullptr, false, false);
    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("Control")) {
    if (ImGui::MenuItem("Pause", nullptr, world.paused)) {
      world.paused = !world.paused;
    }
    if (ImGui::MenuItem("Mute", nullptr, engine.audio->isMuted())) {
      engine.audio->setMute(!engine.audio->isMuted());
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Fast speed", nullptr, engine.fastticks)) {
      engine.fastticks = !engine.fastticks;
    };
    ImGui::MenuItem("Slow display updates", nullptr, false, false);
    if (ImGui::MenuItem("Autokill", nullptr, world.autokill)) {
      world.autokill = !world.autokill;
    }
    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("Debug")) {
    if (ImGui::MenuItem("Show Map", nullptr, world.showrooms)) {
      world.showrooms = !world.showrooms;
    }
    if (ImGui::MenuItem("Create a new (debug) Norn", nullptr, false, engine.version <= 2)) {
      NewDebugNorn();
    }
    if (ImGui::MenuItem("Create a random egg", nullptr, false, engine.version <= 2)) {
      NewRandomEgg();
    };
    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("Tools")) {
    // if (ImGui::MenuItem("Hatchery", nullptr, false, IsHatcheryEnabled())) {
    //   s_hatchery_open = true;
    // }
    if (ImGui::MenuItem("Agent Injector", nullptr, false, IsAgentInjectorEnabled())) {
      SetAgentInjectorOpen(true);
    }
    if (ImGui::MenuItem("Brain Viewer")) {
      s_brain_viewer_open = true;
    }
    if (ImGui::MenuItem("Creature Grapher")) {
      s_creature_grapher_open = true;
    };
    ImGui::EndMenu();
  }
}