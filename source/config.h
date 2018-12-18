/*
    config: joel16/NX-Shell

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Modifications by CodeMooseUS
*/

#ifndef _CONFIG_H
#define _CONFIG_H

#include <switch.h>

typedef struct {
	char server_address[256];
	int overclock_index;
} Configuration;

extern Configuration config;

Result Config_Save(Configuration config);
Result Config_Load(void);

#endif
