/*
    utils: joel16/NX-Shell

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

#ifndef _UTILS_H
#define _UTILS_H

char *Utils_Basename(const char *filename);
void Utils_GetSizeString(char *string, u64 size);
void Utils_SetMax(int *set, int value, int max);
void Utils_SetMin(int *set, int value, int min);
int Utils_Alphasort(const void *p1, const void *p2);
void Utils_AppendArr(char subject[], const char insert[], int pos);
u64 Utils_GetTotalStorage(FsStorageId storage_id);
u64 Utils_GetUsedStorage(FsStorageId storage_id);

#endif
