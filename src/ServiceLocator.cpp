#include "ServiceLocator.h"
#include "Engine.h"
#include "World.h"

template <>
class AudioBackend* getService<AudioBackend>() {
    return engine.audio.get();
}

template <>
class Backend* getService<Backend>() {
    return engine.backend.get();
}

template <>
class historyManager* getService<historyManager>() {
    return world.history.get();
}

template <>
class MainCamera* getService<MainCamera>() {
    return engine.camera.get();
}

template <>
class Map* getService<Map>() {
    return world.map.get();
}

template <>
class prayManager* getService<prayManager>() {
    return world.praymanager.get();
}