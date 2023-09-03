/*
 * Copyright (C) 2018 Yaroslav Pronin <proninyaroslav@mail.ru>
 *
 * This file is part of libnavigation.
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LibreTorrent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LibreTorrent.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nodequeue.h"

NodeQueue::NodeQueue(comparator cmp)
{
	this->cmp = cmp;
}

void NodeQueue::push(AbstractState *state)
{
	if (!state)
		return;

	map<GraphId::id_t, size_t>::iterator it = queue_idx_cache.find(state->node.id);
	if (it != queue_idx_cache.end()) {
		update(it->second);

		return;
	}

	states.push_back(state);
	size_t pos = states.size() - 1;
	queue_idx_cache[state->node.id] = pos;
	up(pos);
}

AbstractState *NodeQueue::pop()
{
	if (states.empty())
		return NULL;

	AbstractState *top = states.at(0);
	size_t size = states.size() - 1;

	if (size > 0) {
		states.at(0) = states.at(size);
		queue_idx_cache[states.at(0)->node.id] = 0;
		down(0);
	}
	states.pop_back();
	queue_idx_cache.erase(top->node.id);

	return top;
}

AbstractState *NodeQueue::peek()
{
	return (states.empty() ? NULL : states.at(0));
}

void NodeQueue::clear()
{
	states.clear();
	queue_idx_cache.clear();
}

void NodeQueue::update(size_t pos)
{
	down(pos);
	up(pos);
}

int NodeQueue::size()
{
	return states.size();
}

void NodeQueue::down(size_t pos)
{
	size_t size = states.size();
	size_t half_length = size / 2;
	AbstractState *state = states.at(pos);

	while (pos < half_length) {
		size_t left = pos * 2 + 1;
		size_t right = left + 1;
		AbstractState *best_state = states.at(left);
		if (right < size && cmp(states.at(right), best_state) < 0) {
			left = right;
			best_state = states.at(right);
		}
		if (cmp(best_state, state) >= 0)
			break;

		states.at(pos) = best_state;
		queue_idx_cache[best_state->node.id] = pos;
		pos = left;
	}

	states.at(pos) = state;
	queue_idx_cache[state->node.id] = pos;
}

void NodeQueue::up(size_t pos)
{
	AbstractState *state = states.at(pos);

	while (pos > 0) {
		size_t parent = (pos - 1) / 2;
		AbstractState *current_state = states.at(parent);
		if (cmp(state, current_state) >= 0)
			break;
		states.at(pos) = current_state;
		queue_idx_cache[current_state->node.id] = pos;
		pos = parent;
	}

	states.at(pos) = state;
	queue_idx_cache[state->node.id] = pos;
}
