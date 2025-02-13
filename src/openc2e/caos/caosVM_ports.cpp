/*
 *  caosVM_vehicles.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on 02/02/2005.
 *  Copyright 2005 Alyssa Milburn. All rights reserved.
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

#include "Agent.h"
#include "Port.h"
#include "caosVM.h"
#include "common/throw_ifnot.h"

#include <memory>

/**
 PRT: BANG (command) strength (integer)
 %status stub
*/
void c_PRT_BANG(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER_UNUSED(strength)
}

/**
 PRT: FRMA (agent) inputport (integer)
 %status maybe

 Returns agent to which the specified input port is connected, NULL if not
 connected or the port doesn't exist.
*/
void v_PRT_FRMA(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(inputport)

	valid_agent(vm->targ);
	if (vm->targ->inports.find(inputport) != vm->targ->inports.end())
		vm->result.setAgent(vm->targ->inports[inputport]->source);
	else
		vm->result.setAgent(0);
}

/**
 PRT: FROM (integer) inputport (integer)
 %status maybe

 Returns the output port id on the source agent connected to the specified
 input port. Returns a negative value if the port is not connected or if the
 source agent does not exist.
*/
void v_PRT_FROM(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(inputport)

	valid_agent(vm->targ);
	if (vm->targ->inports.find(inputport) != vm->targ->inports.end() && vm->targ->inports[inputport]->source)
		vm->result.setInt(vm->targ->inports[inputport]->sourceid);
	else
		vm->result.setInt(-1);
}

/**
 PRT: INEW (command) id (integer) name (string) desc (string) x (integer) y (integer) msgnum (integer)
 %status maybe

 Creates a new input port on targ. The message msgnum will be sent to the agent
 when a signal arrives through the port. _P1_ of that message will be the data
 of the signal.
*/
void c_PRT_INEW(caosVM* vm) {
	VM_VERIFY_SIZE(6)
	VM_PARAM_INTEGER(msgnum)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_STRING(desc)
	VM_PARAM_STRING(name)
	VM_PARAM_INTEGER(id)

	valid_agent(vm->targ);
	THROW_IFNOT(vm->targ->inports.find(id) == vm->targ->inports.end()); // TODO: multiple PRT: INEWs with the same id allowed?
	vm->targ->inports[id] = std::shared_ptr<InputPort>(new InputPort(x, y, name, desc, msgnum));
}

/**
 PRT: ITOT (integer)
 %status maybe

 Returns the total number of input ports.
*/
void v_PRT_ITOT(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);

	// TODO: not strictly correct, I think; the CAOS docs are vague (surprise!)
	vm->result.setInt(vm->targ->inports.size());
}

/**
 PRT: IZAP (command) id (integer)
 %status maybe

 Removes the input port with given id.
*/
void c_PRT_IZAP(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(id)

	valid_agent(vm->targ);
	THROW_IFNOT(vm->targ->inports.find(id) != vm->targ->inports.end());
	AgentRef src = vm->targ->inports[id]->source;
	if (src) {
		PortConnectionList& dests = src->outports[vm->targ->inports[id]->sourceid]->dests;
		PortConnectionList::iterator i = dests.begin();
		while (i != dests.end()) {
			if (id < 0)
				throw caosException("Comparison of signed negative integer to unsigned integer");
			if (i->first == vm->targ && i->second == (unsigned)id) {
				dests.erase(i);
				break;
			}
			i++;
		}
	}
	vm->targ->inports.erase(id);
}

/**
 PRT: JOIN (command) source (agent) outputport (integer) dest (agent) inputport (integer)
 %status maybe

 Joins the output port from source to the input port of dest.
*/
void c_PRT_JOIN(caosVM* vm) {
	VM_VERIFY_SIZE(4)
	VM_PARAM_INTEGER(inputport)
	VM_PARAM_VALIDAGENT(dest)
	VM_PARAM_INTEGER(outputport)
	VM_PARAM_VALIDAGENT(source)

	THROW_IFNOT(source->outports.find(outputport) != source->outports.end());
	THROW_IFNOT(dest->inports.find(inputport) != dest->inports.end());

	source->join(outputport, dest, inputport);
}

/**
 PRT: KRAK (command) agent (agent) is_outport (integer) port (integer)
 %status maybe

 Breaks a connection on agent. If is_outport, kill all connections connected to
 the port. Else, kill the connection to the inport.
*/
void c_PRT_KRAK(caosVM* vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(port)
	VM_PARAM_INTEGER(is_outport)
	VM_PARAM_VALIDAGENT(agent)

	if (is_outport) {
		if (agent->outports.find(port) == agent->outports.end())
			return;
		PortConnectionList::iterator i = agent->outports[port]->dests.begin();
		while (i != agent->outports[port]->dests.end()) {
			PortConnectionList::iterator next = i;
			next++;
			if (i->first && i->first->inports.find(i->second) != i->first->inports.end()) {
				i->first->inports[i->second]->source.clear();
			}
			agent->outports[port]->dests.erase(i);
			i = next;
		}
	} else {
		if (agent->inports.find(port) == agent->inports.end())
			return;
		AgentRef src = agent->inports[port]->source;
		if (src && src->outports.find(agent->inports[port]->sourceid) != src->outports.end()) {
			src->outports[agent->inports[port]->sourceid]->dests.remove(std::pair<AgentRef, unsigned int>(agent, port));
		}
		agent->inports[port]->source.clear();
	}
}

/**
 PRT: NAME (string) agent (agent) is_outport (integer) port (integer)
 %status maybe

 Returns the name of the specified port. Returns "" if the port doesn't exist.
*/
void v_PRT_NAME(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	VM_PARAM_INTEGER(port)
	VM_PARAM_INTEGER(is_outport)
	VM_PARAM_VALIDAGENT(agent)

	vm->result.setString("");
	if (is_outport) {
		if (agent->outports.find(port) != agent->outports.end())
			vm->result.setString(agent->outports[port]->name);
	} else {
		if (agent->inports.find(port) != agent->inports.end())
			vm->result.setString(agent->inports[port]->name);
	}
}

/**
 PRT: ONEW (command) id (integer) name (string) desc (string) x (integer) y (integer)
 %status maybe

 Creates a new output port on targ.
*/
void c_PRT_ONEW(caosVM* vm) {
	VM_VERIFY_SIZE(5)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_STRING(desc)
	VM_PARAM_STRING(name)
	VM_PARAM_INTEGER(id)

	valid_agent(vm->targ);
	THROW_IFNOT(vm->targ->outports.find(id) == vm->targ->outports.end());
	vm->targ->outports[id] = std::shared_ptr<OutputPort>(new OutputPort(x, y, name, desc));
}

/**
 PRT: OTOT (integer)
 %status maybe

 Returns the total number of output ports.
*/
void v_PRT_OTOT(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);
	vm->result.setInt(vm->targ->outports.size());
}

/**
 PRT: OZAP (command) id (integer)
 %status maybe

 Destroys output port with given id.
*/
void c_PRT_OZAP(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(id)

	valid_agent(vm->targ);
	THROW_IFNOT(vm->targ->outports.find(id) != vm->targ->outports.end());
	for (PortConnectionList::iterator i = vm->targ->outports[id]->dests.begin(); i != vm->targ->outports[id]->dests.end(); i++) {
		if (!i->first)
			continue;
		if (i->first->inports.find(i->second) == i->first->inports.end())
			continue;
		i->first->inports[i->second]->source.clear();
	}
	vm->targ->outports.erase(id);
}

/**
 PRT: SEND (command) id (integer) data (anything)
 %status maybe
 
 Sends information over targ's output port.
*/
void c_PRT_SEND(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_VALUE(data)
	VM_PARAM_INTEGER(id)

	valid_agent(vm->targ);

	THROW_IFNOT(vm->targ->outports.find(id) != vm->targ->outports.end());

	PortConnectionList::iterator i = vm->targ->outports[id]->dests.begin();
	while (i != vm->targ->outports[id]->dests.end()) {
		PortConnectionList::iterator next = i;
		next++;
		if (!i->first || i->first->inports.find(i->second) == i->first->inports.end()) {
			vm->targ->outports[id]->dests.erase(i);
			i = next;
			continue;
		}
		i->first->queueScript(i->first->inports[i->second]->messageno, vm->targ, data);
		i = next;
	}
}

/* vim: set noet: */
