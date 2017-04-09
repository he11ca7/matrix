#include "matrix.h"

#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <assert.h>

// DEBUG
#include <iostream>
#include <iomanip>
using namespace std;

/*!
 * \brief Конструктор
 *
 * В случае, если одна из размерностей нулевая, вторая тоже становится нулевой.
 * \param rowCount Число строк
 * \param colCount Число столбцов
 * \param storeRows Признак построчного внутреннего хранения.
 * При построчном хранении данные строк следуют в памяти друг за другом.
 */
Matrix::Matrix(
    uint32 rowCount,
    uint32 colCount,
    bool storeRows)
{
  _defaultValue = -1; // Провоцирует NaN
  _NaN = NAN;
  _data = NULL;

  _storeRows = storeRows;
  _indexer = storeRows ?
        (MatrixIndexer) indexerRow
      :
        (MatrixIndexer) indexerCol;

  // Если одна из размерностей нулевая
  if((rowCount == 0) || (colCount == 0))
    clear();
  else
    {
      _rowCount = rowCount;
      _colCount = colCount;
      _size = _colCount * _rowCount * sizeof(TT);
      _data = (TT *) malloc(_size);

      assert(_data);
      memset(_data, _defaultValue, _size);
    }
}

/*!
 * \brief Очистить матрицу
 *
 * Удалить содержимое и установить размерность 0x0.
 */
void Matrix::clear()
{
  if(_data) free(_data);

  _data = NULL;
  _rowCount = 0;
  _colCount = 0;
  _size = 0;
}

uint32 Matrix::indexerRow(
    uint32 row,
    uint32 col,
    uint32,
    uint32 colCount)
{
  return row * colCount + col;
}

uint32 Matrix::indexerCol(
    uint32 row,
    uint32 col,
    uint32 rowCount,
    uint32)
{
  return col * rowCount + row;
}

/*!
 * \brief Деструктор
 */
Matrix::~Matrix()
{
  if (!_data) return;
  free(_data);
}

/*!
 * \brief Доступ к данным
 *
 * В случае некорректного входа возвращает NaN.
 * \param row Номер строки
 * \param col Номер столбца
 * \return Данные
 */
TT &Matrix::v(
    uint32 row,
    uint32 col)
{
  return (isEmpty() || (row >= _rowCount) || (col >= _colCount)) ?
        _NaN
      :
        _data[_indexer(row, col, _rowCount, _colCount)];
}


/*!
 * \brief Установить способ внутреннего хранения
 *
 * По умолчанию данные в матрице хранятся последовательно строками.
 * Однако, в некоторых случаях полезно хранить данные последовательно столбцами.
 * \param storeRows Признак построчного хранения
 */
void Matrix::setStoreMode(
    bool storeRows)
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
  memcpy(data, _data, _size);

  // Переместить данные
  for(uint32 i = 0; i < _rowCount; ++i)
    for(uint32 j = 0; j < _colCount; ++j)
      _data[_indexer(i, j, _rowCount, _colCount)] =
          data[indexer(i, j, _rowCount, _colCount)];

  free(data);
}

/*!
 * \brief Удалить строку
 * \param row Номер строки
 * \param count Количество удаляемых строк
 */
void Matrix::deleteRow(
    uint32 row,
    uint32 count)
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
                _data + _indexer(row, 0, _rowCount, _colCount),
                _data + pos,
                (_rowCount * _colCount - pos) * sizeof(TT)
                );
        }
      _data = (TT *) realloc(_data, _size);
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

/*!
 * \brief Удалить столбец
 * \param col Номер столбца
 * \param count Количество удаляемых столбцов
 */
void Matrix::deleteCol(
    uint32 col,
    uint32 count)
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
                _data + _indexer(0, col, _rowCount, _colCount),
                _data + pos,
                (_rowCount * _colCount - pos) * sizeof(TT)
                );
        }
      _data = (TT *) realloc(_data, _size);
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

/*!
 * \brief Изменить размер матрицы
 *
 * Если одна из размерностей устанавливается равной нулю, матрица очищается.
 * \param rowCount Количество строк
 * \param colCount Количество столбцов
 */
void Matrix::resize(
    uint32 rowCount,
    uint32 colCount)
{
  if(rowCount == _rowCount && colCount == _colCount) return;

  // Если одна из размерностей нулевая
  if((rowCount == 0) || (colCount == 0))
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
      _data = (TT *) realloc(_data, _size);
      assert(_data);

      if(_rowCount < rowCount)
        memset(
              _data + _indexer(_rowCount - 1, _colCount - 1, _rowCount, _colCount) + 1,
              _defaultValue,
              (rowCount * colCount - _rowCount * _colCount) * sizeof(TT)
              );
    }
  // ... остальные случаи
  else
    {
      TT *temp = _data;
      _data = (TT *) malloc(_size);
      assert(_data);

      memset(_data, _defaultValue, _size);

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

/*!
 * \brief Преобразовать матрицу в вид двумерного динамического массива
 * \return Указатель на указатель
 */
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

/*!
 * \brief Преобразовать матрицу в вид одномерного динамического массива
 * \return Указатель
 */
TT *Matrix::toP()
{
  if(isEmpty()) return NULL;

  // Память
  TT *result = (TT *) malloc(_size);
  assert(result);

  // Содержимое
  memcpy( result, _data, _size);

  return result;
}

void Matrix::printMatrix(
    Matrix *m,
    int width)
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

void Matrix::printMatrix(
    TT **m,
    uint32 rows,
    uint32 cols,
    int width)
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
