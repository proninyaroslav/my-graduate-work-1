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

#ifndef NODEQUEUE_H
#define NODEQUEUE_H

#include <map>
#include <vector>
#include <cstddef>
#include "abstractstate.h"

using namespace std;

class NodeQueue
{
public:
	typedef double (*comparator)(AbstractState *, AbstractState *);

	NodeQueue(comparator cmp);
	void push(AbstractState *state);
	AbstractState *pop();
	AbstractState *peek();
	void clear();
	int size();

private:
	vector<AbstractState *> states;
	map<GraphId::id_t, size_t> queue_idx_cache;
	comparator cmp;

	void down(size_t pos);
	void up(size_t pos);
	void update(size_t pos);
};

#endif // NODEQUEUE_H
