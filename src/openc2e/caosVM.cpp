/*
 *  caosVM.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
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

#include "caosVM.h"

#include "World.h"
#include "bytecode.h"
#include "caosScript.h"
#include "common/demangle.h"
#include "common/throw_ifnot.h"

#include <cassert>
#include <climits>
#include <fmt/core.h>
#include <iostream>
#include <memory>

void dumpStack(caosVM* vm) {
	fmt::print(stderr, "\tvalueStack: ");
	int i, c = 0;
	for (i = vm->valueStack.size() - 1; i >= 0 && c++ < 5; i--)
		fmt::print(stderr, "{} | ", vm->valueStack[i].dump());
	if (i >= 0)
		fmt::print(stderr, "...");
	else
		fmt::print(stderr, "END");
	fmt::print(stderr, "\n");
}

caosVM::caosVM(const AgentRef& o)
	: vm(this) {
	owner = o;
	currentscript.reset();
	std::fill(var.begin(), var.end(), 0);
	cip = nip = 0;
	blocking = NULL;
	inputstream = 0;
	outputstream = 0;
	resetCore();
	trace = false;
}

caosVM::~caosVM() {
	resetCore(); // delete blocking, close streams
}

bool caosVM::isBlocking() {
	if (!blocking)
		return false;
	bool bl = (*blocking)();
	if (!bl) {
		delete blocking;
		blocking = NULL;
	}
	return bl;
}

void caosVM::startBlocking(blockCond* whileWhat) {
	if (!owner)
		// TODO: should this just fail to block?
		throw Exception("trying to block in a non-blockable script");
	inst = false;
	if (blocking)
		throw Exception("trying to block with a block condition in-place");
	blocking = whileWhat;
}

inline void caosVM::safeJMP(int dest) {
	//	fmt::print(stderr, "jmp from {} to {} (old nip = {}) in script of length {}\n", cip, dest, nip, currentscript->scriptLength());
	if (dest < 0) {
		fmt::print(stderr, "{}", currentscript->dump());
		throw caosException(fmt::format("Internal error: Unrelocated jump at {:08x}", cip));
	}
	if (dest >= currentscript->scriptLength()) {
		fmt::print(stderr, "{}", currentscript->dump());
		throw Exception(fmt::format("Internal error: Jump out of bounds at {:08x}", cip));
	}
	nip = dest;
}

inline void caosVM::invoke_cmd(script* s, bool is_saver, int opidx) {
	const cmdinfo* ci = s->dialect->getcmd(opidx);
	// We subtract two here to account for a) the missing return, and b)
	// consuming the new value.
	int stackdelta = ci->stackdelta - (is_saver ? 2 : 0);
	unsigned int stackstart = valueStack.size();
	assert(result.isNull());
	if (is_saver) {
		if (ci->savehandler)
			(ci->savehandler)(this);
	} else {
		if (ci->handler)
			(ci->handler)(this);
	}
	if (!is_saver && !result.isNull()) {
		valueStack.push_back(result);
		result.reset();
	} else {
		assert(result.isNull());
	}
	if (stackdelta < INT_MAX - 1) {
		if ((int)stackstart + stackdelta != (int)valueStack.size()) {
			dumpStack(this);
			throw caosException(fmt::format(
				"Internal error: Stack imbalance detected: expected to be {} after start of {}, but stack size is now {}",
				stackdelta, (int)stackstart, (int)valueStack.size()));
		}
	}
}

inline void caosVM::runOpCore(script* s, caosOp op) {
	switch (op.opcode) {
		case CAOS_NOP: break;
		case CAOS_DIE: {
			int idx = op.argument;
			std::string err = "aborted";
			caosValue constVal = s->getConstant(idx);
			if (constVal.hasString())
				err = constVal.getString();
			throw Exception(err);
		}
		case CAOS_STOP: {
			stop();
			break;
		}
		case CAOS_SAVE_CMD: {
			invoke_cmd(s, true, op.argument);
			break;
		}
		case CAOS_CMD: {
			invoke_cmd(s, false, op.argument);
			break;
		}
		case CAOS_YIELD: {
#ifndef NDEBUG
			// This condition can arise as a result of bad save data,
			// so an assert is not appropriate... or is it?
			//
			// In any case, it is mostly harmless but should never occur,
			// as it indicates a bug in the CAOS compiler.
			THROW_IFNOT(auxStack.size() == 0);
#endif
			if (!inst)
				timeslice -= 1;
			break;
		}
		case CAOS_COND: {
			VM_PARAM_VALUE(v2);
			VM_PARAM_VALUE(v1);
			VM_PARAM_INTEGER(condaccum);
			assert(!v1.isEmpty());
			assert(!v2.isEmpty());
			int condition = op.argument;
			if (condition & CAND)
				condition -= CAND;
			if (condition & COR)
				condition -= COR;
			int result = 0;
			if (condition == CEQ)
				result = (v1 == v2);
			if (condition == CNE)
				result = !(v1 == v2);

			if (condition == CLT)
				result = (v1 < v2);
			if (condition == CGE)
				result = !(v1 < v2);
			if (condition == CGT)
				result = (v1 > v2);
			if (condition == CLE)
				result = !(v1 > v2);

			if (condition == CBT) {
				THROW_IFNOT(v1.hasInt() && v2.hasInt());
				result = (v2.getInt() == (v1.getInt() & v2.getInt()));
			}
			if (condition == CBF) {
				THROW_IFNOT(v1.hasInt() && v2.hasInt());
				result = (0 == (v1.getInt() & v2.getInt()));
			}
			if (op.argument & CAND)
				result = (condaccum && result);
			else
				result = (condaccum || result);
			valueStack.push_back(caosValue(result));
			break;
		}
		case CAOS_CONST: {
			valueStack.push_back(s->getConstant(op.argument));
			break;
		}
		case CAOS_CONSTINT: {
			valueStack.push_back(caosValue(op.argument));
			break;
		}
		case CAOS_PUSH_AUX: {
			THROW_IFNOT(op.argument >= 0);
			THROW_IFNOT(op.argument < (int)valueStack.size());
			auxStack.push_back(valueStack[valueStack.size() - op.argument - 1]);
			break;
		}
		case CAOS_RESTORE_AUX: {
			THROW_IFNOT(op.argument >= 0);
			THROW_IFNOT(op.argument <= (int)auxStack.size());
			for (int i = 0; i < op.argument; i++) {
				valueStack.push_back(auxStack.back());
				auxStack.pop_back();
			}
			break;
		}
		case CAOS_STACK_ROT: {
			THROW_IFNOT(op.argument >= 0);
			THROW_IFNOT(op.argument < (int)valueStack.size());
			for (int i = 0; i < op.argument; i++) {
				int top = valueStack.size() - 1;
				std::swap(valueStack[top - i], valueStack[top - i - 1]);
			}
			break;
		}
		case CAOS_CJMP: {
			VM_PARAM_VALUE(v);
			if (v.getInt() != 0)
				safeJMP(op.argument);
			break;
		}
		case CAOS_JMP: {
			safeJMP(op.argument);
			break;
		}
		case CAOS_DECJNZ: {
			VM_PARAM_INTEGER(counter);
			counter--;
			if (counter) {
				safeJMP(op.argument);
				valueStack.push_back(caosValue(counter));
			}
			break;
		}
		case CAOS_GSUB: {
			callStack.push_back(callStackItem());
			callStack.back().nip = nip;
			callStack.back().valueStack.swap(valueStack);
			safeJMP(op.argument);
			break;
		}
		case CAOS_ENUMPOP: {
			VM_PARAM_VALUE(v);
			if (v.isEmpty())
				break;
			if (!v.hasAgent()) {
				dumpStack(this);
				throw caosException(std::string("Stack item type mismatch: ") + v.dump());
			}
			targ = v.getAgentRef();
			safeJMP(op.argument);
			break;
		}
		default:
			throw Exception(fmt::format("Illegal opcode {}", (int)op.opcode));
	}
}

inline void caosVM::runOp() {
	cip = nip;
	nip++;

	runops++;
	if (runops > 1000000)
		throw Exception("script exceeded 1m ops");

	std::shared_ptr<script> scr = currentscript;
	caosOp op = currentscript->getOp(cip);

	try {
		if (trace) {
			fmt::print(
				stderr,
				"optrace({}): INST={} TS={} {} @{:08d} top={} depth={} {}\n",
				scr->filename, (int)inst, (int)timeslice,
				(void*)this, cip,
				(valueStack.empty() ? std::string("(empty)") : valueStack.back().dump()),
				valueStack.size(),
				dumpOp(currentscript->dialect, op));
			if (trace >= 2) {
				dumpStack(this);
			}
		}
		runOpCore(scr.get(), op);
	} catch (caosException& e) {
		e.trace(currentscript, op.traceindex);
		stop();
		throw;
	} catch (Exception& e) {
		caosException c(fmt::format("{}: {}", demangle(typeid(e).name()), +e.what()));
		c.trace(currentscript, op.traceindex);
		stop();
		throw c;
	}
}

void caosVM::stop() {
	lock = false;
	currentscript.reset();
	std::fill(var.begin(), var.end(), 0);
}

void caosVM::runEntirely(std::shared_ptr<script> s) {
	// caller is responsible for resetting/setting *all* state!
	cip = nip = runops = 0;
	currentscript = s;

	while (true) {
		runOp();
		if (!currentscript)
			break;
		if (blocking) {
			delete blocking;
			blocking = NULL;
			throw Exception("blocking in an installation script");
		}
	}
}

bool caosVM::fireScript(std::shared_ptr<script> s, bool nointerrupt, Agent* frm) {
	assert(owner);
	assert(s);
	if (lock)
		return false; // can't interrupt scripts which called LOCK
	if (currentscript && nointerrupt)
		return false; // don't interrupt scripts with a timer script

	resetScriptState();
	currentscript = s;
	targ = owner;
	from.setAgent(frm);
	timeslice = 1;
	return true;
}

void caosVM::resetScriptState() {
	stop();
	resetCore();
}

void caosVM::resetCore() {
	if (blocking)
		delete blocking;
	blocking = NULL;
	result.reset();

	valueStack.clear();
	auxStack.clear();
	callStack.clear();

	inst = lock = 0;
	timeslice = 0;

	if (inputstream) {
		delete inputstream;
		inputstream = nullptr;
	}
	if (outputstream) {
		delete outputstream;
		outputstream = nullptr;
	}

	_it_ = NULL;
	from.setAgent(NULL);
	setTarg(owner);
	part = 0;

	_p_[0].reset();
	_p_[0].setInt(0);
	_p_[1].reset();
	_p_[1].setInt(0);
	std::fill(var.begin(), var.end(), 0);

	camera.reset();

	trace = 0;
	cip = nip = runops = 0;
}

void caosVM::tick() {
	stop_loop = false;
	runops = 0;
	while (currentscript && !stop_loop && (timeslice > 0 || inst)) {
		if (isBlocking())
			return;
		runOp();
	}
}

/* vim: set noet: */
