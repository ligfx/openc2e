#pragma once

// Speed up build times by not having everything depend on Engine.h and World.h
// If you need a specific component from the Engine or active World, call getService<T>()
// WARNING: if you edit this file, many other files will have to rebuild !

template <typename T>
T* getService();
