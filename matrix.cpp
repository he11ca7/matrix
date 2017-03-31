#include "matrix.h"

#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <assert.h>

// DEBUG
#include <iostream>
#include <iomanip>
using namespace std;

Matrix::Matrix(uint32 rowCount, uint32 colCount, bool storeRows)
{
  _NaN = NAN;

  _storeRows = storeRows;
  _indexer = storeRows ?
        (MatrixIndexer) indexerRow
      :
        (MatrixIndexer) indexerCol;

  // Если одна из размерностей нулевая
  if(
     ((rowCount == 0) && (colCount == 0)) ||
     ((rowCount == 0) && (colCount != 0)) ||
     ((rowCount != 0) && (colCount == 0))
     )
    clear();
  else
    {
      _rowCount = rowCount;
      _colCount = colCount;
      _size = _colCount * _rowCount * sizeof(TT);
      _data = (TT *) malloc(_size);

      assert(_data);
      memset((void *) _data, 0, _size);
    }
}

void Matrix::clear()
{
  if(_data) free(_data);

  _data = NULL;
  _rowCount = 0;
  _colCount = 0;
  _size = 0;
}

uint32 Matrix::indexerRow(uint32 row, uint32 col, uint32, uint32 colCount)
{
  return row * colCount + col;
}

uint32 Matrix::indexerCol(uint32 row, uint32 col, uint32 rowCount, uint32)
{
  return col * rowCount + row;
}

Matrix::~Matrix()
{
  if (!_data) return;
  free((void *) _data);
}

TT &Matrix::v(uint32 row, uint32 col)
{
  return (isEmpty() || (row >= _rowCount) || (col >= _colCount)) ?
        _NaN
      :
        _data[_indexer(row, col, _rowCount, _colCount)];
}

void Matrix::setStoreMode(bool storeRows)
{
  if(_storeRows == storeRows) return;

  MatrixIndexer indexer = _indexer;

  _storeRows = storeRows;
  _indexer = storeRows ?
        (MatrixIndexer) indexerRow
      :
        (MatrixIndexer) indexerCol;

  if(isEmpty()) return;

  // Временно копировать старые данные
  TT *data = (TT *) malloc(_size);
  assert(_data);
  memcpy(
        (void *) data,
        (void *) _data,
        _size
        );

  // Переместить данные
  for(uint32 i = 0; i < _rowCount; ++i)
    for(uint32 j = 0; j < _colCount; ++j)
      _data[_indexer(i, j, _rowCount, _colCount)] =
          data[indexer(i, j, _rowCount, _colCount)];

  free(data);
}

void Matrix::deleteRow(uint32 row, uint32 count)
{
  if(row + count > _rowCount) return;

  // Если удаляются все строки
  if(count == _rowCount)
    {
      clear();
      return;
    }

  _size = _colCount * (_rowCount - count) * sizeof(TT);

  if(_storeRows)
    {
      // Если удаляется не вплоть до последней строки,
      // "сдвинуть" остальную память на место удаляемого блока
      if(row + count < _rowCount)
        {
          // Позиция "сдвигаемой" части памяти
          uint32 pos = _indexer(row + count, 0, _rowCount, _colCount);
          memmove(
                (void *) &_data[_indexer(row, 0, _rowCount, _colCount)],
              (void *) &_data[pos],

              (_rowCount * _colCount - pos) * sizeof(TT)
              );
        }
      _data = (TT *) realloc((void *) _data, _size);
      assert(_data);
    }
  else
    {
      TT *data = _data;

      _data = (TT *) malloc(_size);
      assert(_data);

      for(uint32 i = 0; i < _rowCount - count; ++i)
        for(uint32 j = 0; j < _colCount; ++j)
          _data[_indexer(i, j, _rowCount - count, _colCount)] =
              data[_indexer(i + ((i >= row) ? count : 0),
                            j,
                            _rowCount,
                            _colCount)
              ];
      free(data);
    }

  _rowCount -= count;
}

// Аналогично deleteRow
void Matrix::deleteCol(uint32 col, uint32 count)
{
  if(col + count > _colCount) return;

  // Если удаляются все столбцы
  if(count == _colCount)
    {
      clear();
      return;
    }

  _size = (_colCount - count) * _rowCount * sizeof(TT);

  if(!_storeRows)
    {
      // Если удаляется не вплоть до последней строки,
      // "сдвинуть" остальную память на место удаляемого блока
      if(col + count < _colCount)
        {
          // Позиция "сдвигаемой" части памяти
          uint32 pos = _indexer(0, col + count, _rowCount, _colCount);
          memmove(
                (void *) &_data[_indexer(0, col, _rowCount, _colCount)],
              (void *) &_data[pos],

              (_rowCount * _colCount - pos) * sizeof(TT)
              );
        }
      _data = (TT *) realloc((void *) _data, _size);
      assert(_data);
    }
  else
    {
      TT *data = _data;

      _data = (TT *) malloc(_size);
      assert(_data);

      for(uint32 i = 0; i < _rowCount; ++i)
        for(uint32 j = 0; j < _colCount - count; ++j)
          _data[_indexer(i, j, _rowCount, _colCount - count)] =
              data[_indexer(i,
                            j + ((j >= col) ? count : 0),
                            _rowCount,
                            _colCount)
              ];
      free(data);
    }

  _colCount -= count;
}

void Matrix::resize(uint32 rowCount, uint32 colCount)
{
  if(rowCount == _rowCount && colCount == _colCount) return;

  // Если одна из размерностей нулевая
  if(
     ((rowCount == 0) && (colCount == 0)) ||
     ((rowCount == 0) && (colCount != 0)) ||
     ((rowCount != 0) && (colCount == 0))
     )
    {
      clear();
      return;
    }

  _size = rowCount * colCount * sizeof(TT);

  // Изменение числа строк при хранении строками
  // или
  // Изменение числа столбцов при хранении столбцами
  if(
     (colCount == _colCount && _storeRows)
     ||
     (rowCount == _rowCount && !_storeRows)
     )
    {
      _data = (TT *) realloc((void *) _data, _size);
      assert(_data);

      if(_rowCount < rowCount)
        memset(
              (void *) &_data[_indexer(_rowCount - 1, _colCount - 1, _rowCount, _colCount) + 1],
            0,
            (rowCount * colCount - _rowCount * _colCount) * sizeof(TT)
            );
    }
  // ... остальные случаи
  else
    {
      TT *temp = _data;
      _data = (TT *) malloc(_size);
      assert(_data);

      memset((void *) _data, 0, _size);

      for(uint32 i = 0; i < rowCount; ++i)
        for(uint32 j = 0; j < colCount; ++j)
          if(j < _colCount && i < _rowCount)
            _data[_indexer(i, j, rowCount, colCount)] =
                temp[_indexer(i, j, _rowCount, _colCount)];

      free(temp);
    }

  _colCount = colCount;
  _rowCount = rowCount;
}

TT **Matrix::toPP()
{
  if(isEmpty()) return NULL;

  uint32 dimension1 = _storeRows ? _rowCount : _colCount;
  uint32 dimension2 = _storeRows ? _colCount : _rowCount;

  // Память
  TT **result = (TT **) malloc(dimension1 * sizeof(TT *));
  assert(result);
  for(uint32 i = 0; i < dimension1; ++i)
    {
      result[i] = (TT *) malloc(dimension2 * sizeof(TT));
      assert(result[i]);
    }

  // Содержимое
  for(uint32 i = 0; i < _rowCount; ++i)
    for(uint32 j = 0; j < _colCount; ++j)
      result[i][j] = _data[_indexer(i, j, _rowCount, _colCount)];

  return result;
}

void Matrix::printMatrix(Matrix *m, int width)
{
  cout
      << "Matrix "
      << m->rowCount() << "x" << m->colCount()
      << " (" << m->size() << "B)"
      << endl;
  for(uint32 j = 0; j < m->rowCount(); ++j)
    {
      for(uint32 i = 0; i < m->colCount(); ++i)
        {
          TT value = m->v(j, i);
          cout << setw(width) << value;
        }
      cout << endl;
    }
  cout << endl << endl;
}

void Matrix::printMatrix(TT **m, TT rows, TT cols, int width)
{
  cout << "Matrix" << endl;
  for(uint32 i = 0; i < rows; ++i)
    {
      for(uint32 j = 0; j < cols; ++j)
        cout
            << setw(width)
            << m[i][j];
      cout << endl;
    }
  cout << endl << endl;
}