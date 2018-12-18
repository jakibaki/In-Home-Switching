/*
    osk: joel16/NX-Shell

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

#ifndef _OSK_H
#define _OSK_H

#define MAX_Y 4
#define MAX_X 9

char osk_buffer[256];

void OSK_Display(char *title, char *msg);

#endif
