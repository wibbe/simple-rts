
#include "collide.h"
#include "tcl.h"

#include <stdio.h>
#include <memory.h>
#include <cmath>

namespace collide
{
  namespace
  {
    struct Cell
    {
      uint16_t row[16];
      uint32_t sum;
    };

    Cell * _cells = NULL;
    uint32_t _width = 0;
    uint32_t _height = 0;
    uint32_t _cellWidth = 0;
    uint32_t _cellHeight = 0;
  }

  void reset(uint32_t width, uint32_t height)
  {
    delete[] _cells;
    _cells = NULL;

    _width = width;
    _height = height;
    _cellWidth = width / 8;
    _cellHeight = height / 8;

    _cells = new Cell[_cellWidth * _cellHeight];
    memset(_cells, 0, sizeof(Cell) * _cellWidth * _cellHeight);
  }

  void setI(uint32_t x, uint32_t z, bool on)
  {
    const uint32_t cellX = x / 16;
    const uint32_t cellZ = z / 16;
    const uint32_t inX = x % 16;
    const uint32_t inZ = z % 16;
    const uint16_t bit = 1 << inX;

    Cell & cell = _cells[cellZ * _width + cellX];
    uint16_t & row = cell.row[inZ];

    cell.sum -= row;
    row = (row & ~(1 << inX)) | (on << inX);
    cell.sum += row;
  }

  void set(float x, float z, bool on)
  {
    const uint32_t cellX = std::floor(x * 2.0f);
    const uint32_t cellZ = std::floor(z * 2.0f);
    setI(cellX, cellZ, on);
  }

  bool checkI(uint32_t x, uint32_t z)
  {
    const uint32_t cellX = x / 16;
    const uint32_t cellZ = z / 16;
    const uint32_t inX = x % 16;
    const uint32_t inZ = z % 16;

    const Cell & cell = _cells[cellZ * _width + cellX];
    const uint16_t & row = cell.row[inZ];

    return row & (1 << inX);
  }

  bool check(float x, float z)
  {
    const uint32_t cellX = std::floor(x * 2.0f);
    const uint32_t cellZ = std::floor(z * 2.0f);
    return checkI(cellX, cellZ);
  }

  PROC("collide:set", set);
  PROC("collide:check", check);
  PROC("collide:setI", setI);
  PROC("collide:check", checkI);

}