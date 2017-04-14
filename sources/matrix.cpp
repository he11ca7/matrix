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
    TI rowCount,
    TI colCount,
    bool storeRows) :
  _data(NULL),
  _defaultValue(-1), // Провоцирует NaN
  _NaN(NAN),
  _storeRows(storeRows)
{
  _indexer = storeRows ?
        (MatrixIndexer) indexerRow
      :
        (MatrixIndexer) indexerCol;

  if((rowCount == 0) || (colCount == 0))
    // Если одна из размерностей нулевая
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
 * \brief Конструктор копирования
 * \param Копируемый объект
 */
Matrix::Matrix(
    const Matrix &copy)
{
  *this = copy;
}

/*!
 * \brief Индексатор по строкам
 *
 * Индексатор для матрицы, представленной в виде указателя на данные,
 * представленные по строкам
 * \param row Индекс строки
 * \param col Индекс столбца
 * \param colCount Число столбцов
 * \return Индекс места в памяти
 */
Matrix::TI Matrix::indexerRow(
    Matrix::TI row,
    Matrix::TI col,
    Matrix::TI,
    Matrix::TI colCount)
{
  return row * colCount + col;
}

/*!
 * \brief Индексатор по столбцам
 *
 * Индексатор для матрицы, представленной в виде указателя на данные,
 * представленные по столбцам
 * \param row Индекс строки
 * \param col Индекс столбца
 * \param rowCount Число строк
 * \return Индекс места в памяти
 */
Matrix::TI Matrix::indexerCol(
    Matrix::TI row,
    Matrix::TI col,
    Matrix::TI rowCount,
    Matrix::TI)
{
  return col * rowCount + row;
}

/*!
 * \brief Очистить матрицу
 *
 * Удалить содержимое и установить размерность 0x0.
 */
void Matrix::clear()
{
  if(_data) free(_data);

  _rowCount = 0;
  _colCount = 0;
  _size = 0;
  _data = NULL;
}

/*!
 * \brief Обрезать матрицу
 * \param rowBeg Индекс строки-начала
 * \param rowEnd Индекс строки-конца
 * \param colBeg Индекс столбца-начала
 * \param colEnd Индекс столбца-конца
 */
void Matrix::part(
    Matrix::TI rowBeg,
    Matrix::TI rowEnd,
    Matrix::TI colBeg,
    Matrix::TI colEnd)
{
  if((rowBeg > rowEnd) || (colBeg > colEnd) ||
     (rowBeg >= _rowCount) || (rowEnd >= _rowCount) ||
     (colBeg >= _colCount) || (colEnd >= _colCount)
     )
    // Некорректный ввод
    return;

  TI
      rowCount = rowEnd - rowBeg + 1,
      colCount = colEnd - colBeg + 1;

  if((rowCount == _rowCount) && (colCount == _colCount)) return;

  _size = rowCount * colCount * sizeof(TT);

  if(
     ((colCount == _colCount) && _storeRows)
     ||
     ((rowCount == _rowCount) && !_storeRows)
     )
    // Обрезка целыми строками при хранении строками
    // или
    // Обрезка целыми столбцами при хранении столбцами
    {
      if((rowBeg == 0) && (colBeg == 0))
        // Обрезка из начала матрицы
        {
          _data = (TT *) realloc(_data, _size);
          assert(_data);
        }
      else
        // Обрезка не из начала матрицы
        {
          TT *data = _data;
          _data = (TT *) malloc(_size);
          assert(_data);

          memcpy(
                _data,
                data + _indexer(rowBeg, colBeg, _rowCount, _colCount),
                _size);
          free(data);
        }
    }

  // TODO
  // Целесообразно ли вводить отдельный случай для обрезки при хранении строками
  // или столбцами с построчным/постолбцовым переносом?..

  else
    // Остальные случаи
    {
      TT *data = _data;
      _data = (TT *) malloc(_size);
      assert(_data);

      for(TI i = 0; i < rowCount; ++i)
        for(TI j = 0; j < colCount; ++j)
            _data[_indexer(i, j, rowCount, colCount)] =
                data[_indexer(i + rowBeg, j + colBeg, _rowCount, _colCount)];

      free(data);
    }

  _rowCount = rowCount;
  _colCount = colCount;
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
 * \brief Перегрузка operator=
 * \param copy Присваиваемый объект
 * \return
 */
Matrix &Matrix::operator=(
    const Matrix &copy)
{
  if (*this == copy) return *this;

  if(_data) free(_data);

  _rowCount = copy._rowCount;
  _colCount = copy._colCount;
  _size = copy._size;
  _data = (TT *) malloc(_size);
  _storeRows = copy._storeRows;
  _indexer = copy._indexer;

  assert(_data);
  memcpy(_data, copy._data, _size);

  return *this;
}

/*!
 * \brief Перегрузка operator==
 * \param m Сравниваемый объект
 * \return
 */
bool Matrix::operator==(
    const Matrix &other)
{
  if(
     (_rowCount != other._rowCount) || (_colCount != other._colCount) ||
     (_size != other._size) ||
     (_storeRows != other._storeRows)
     )
    return false;

  for(Matrix::TI i = 0; i < rowCount(); ++i)
    for(Matrix::TI j = 0; j < colCount(); ++j)
      if(o(i, j) != other.o(i, j))
        return false;

  return true;
}

/*!
 * \brief Доступ к данным
 *
 * В случае некорректного входа возвращает NaN.
 * \param row Номер строки
 * \param col Номер столбца
 * \return Данные
 */
Matrix::TT &Matrix::o(
    TI row,
    TI col)
{
  return (isEmpty() || (row >= _rowCount) || (col >= _colCount)) ?
        _NaN
      :
        _data[_indexer(row, col, _rowCount, _colCount)];
}

Matrix::TT const &Matrix::o(
    TI row,
    TI col) const
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
  // TODO Обязательно ли копировать данные? Почему нельзя скопировать указатель?
  TT *data = (TT *) malloc(_size);
  assert(data);
  memcpy(data, _data, _size);

  // Переместить данные
  for(TI i = 0; i < _rowCount; ++i)
    for(TI j = 0; j < _colCount; ++j)
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
    TI row,
    TI count)
{
  if(row + count > _rowCount) return;

  if(count == _rowCount)
    // Если удаляются все строки
    {
      clear();
      return;
    }

  _size = _colCount * (_rowCount - count) * sizeof(TT);

  if(_storeRows)
    {
      if(row + count < _rowCount)
        // Если удаляется не вплоть до последней строки,
        // "сдвинуть" остальную память на место удаляемого блока
        {
          // Позиция "сдвигаемой" части памяти
          TI pos = _indexer(row + count, 0, _rowCount, _colCount);
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

      for(TI i = 0; i < _rowCount - count; ++i)
        for(TI j = 0; j < _colCount; ++j)
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
    TI col,
    TI count)
{
  if(col + count > _colCount) return;

  if(count == _colCount)
    // Если удаляются все столбцы
    {
      clear();
      return;
    }

  _size = (_colCount - count) * _rowCount * sizeof(TT);

  if(!_storeRows)
    {
      if(col + count < _colCount)
        // Если удаляется не вплоть до последней строки,
        // "сдвинуть" остальную память на место удаляемого блока
        {
          // Позиция "сдвигаемой" части памяти
          TI pos = _indexer(0, col + count, _rowCount, _colCount);
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

      for(TI i = 0; i < _rowCount; ++i)
        for(TI j = 0; j < _colCount - count; ++j)
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
 * Если обе размерности нулевые, матрица очищается.
 * \param rowCount Количество строк
 * \param colCount Количество столбцов
 */
void Matrix::resize(
    TI rowCount,
    TI colCount)
{
  if((rowCount == _rowCount) && (colCount == _colCount)) return;

  if((rowCount == 0) && (colCount == 0))
    // Если обе размерности нулевые
    {
      clear();
      return;
    }
  // Некоректный ввод
  else if((rowCount == 0) || (colCount == 0)) return;

  _size = rowCount * colCount * sizeof(TT);

  if(
     (colCount == _colCount && _storeRows)
     ||
     (rowCount == _rowCount && !_storeRows)
     )
    // Изменение числа строк при хранении строками
    // или
    // Изменение числа столбцов при хранении столбцами
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
  else
    // ... остальные случаи
    {
      TT *temp = _data;
      _data = (TT *) malloc(_size);
      assert(_data);

      memset(_data, _defaultValue, _size);

      for(TI i = 0; i < rowCount; ++i)
        for(TI j = 0; j < colCount; ++j)
          if(j < _colCount && i < _rowCount)
            _data[_indexer(i, j, rowCount, colCount)] =
                temp[_indexer(i, j, _rowCount, _colCount)];

      free(temp);
    }

  _colCount = colCount;
  _rowCount = rowCount;
}

/*!
 * \brief Изменить число строк
 * \param rowCount Число строк
 */
void Matrix::setRowCount(
    TI rowCount)
{
  resize(rowCount, _colCount);
}

/*!
 * \brief Изменить число столбцов
 * \param colCount Чтсло столбцов
 */
void Matrix::setColCount(
    TI colCount)
{
  resize(_rowCount, colCount);
}

/*!
 * \brief Преобразовать матрицу в вид двумерного динамического массива
 *
 * Признак построчного внутреннего хранения влияет на работу этой функции.
 * Если включено построчное хранение, каждый из указателей *toPP() будет
 * ссылаться на строку.
 * \return Указатель на указатель
 */
Matrix::TT **Matrix::toPP()
{
  if(isEmpty()) return NULL;

  // Память
  TI dimension1 = _storeRows ? _rowCount : _colCount;
  TI dimension2 = _storeRows ? _colCount : _rowCount;
  TT **result = (TT **) malloc(dimension1 * sizeof(TT *));
  assert(result);
  for(TI i = 0; i < dimension1; ++i)
    {
      result[i] = (TT *) malloc(dimension2 * sizeof(TT));
      assert(result[i]);
    }

  // Содержимое
  if(_storeRows)
    for(TI i = 0; i < _rowCount; ++i)
      memcpy(
            *(result + i),
            _data + _indexer(i, 0, _rowCount, _colCount),
            _colCount * sizeof(TT)
            );
  else
    for(TI j = 0; j < _colCount; ++j)
      memcpy(
            *(result + j),
            _data + _indexer(0, j, _rowCount, _colCount),
            _rowCount * sizeof(TT)
            );

  return result;
}

/*!
 * \brief Преобразовать матрицу в вид одномерного динамического массива
 * \return Указатель
 */
Matrix::TT *Matrix::toP()
{
  if(isEmpty()) return NULL;

  // Память
  TT *result = (TT *) malloc(_size);
  assert(result);

  // Содержимое
  memcpy(result, _data, _size);

  return result;
}

/*!
 * \brief Сгенерировать матрицу из указателя на указатель
 *
 * Признак построчного внутреннего хранения влияет на работу этой функции.
 * Если включено построчное хранение, предполагается, что каждый из указателей
 * *PP ссылается на строку.
 * \param PP Указатель на указатель на данные
 * \param rowCount Число строк
 * \param colCount Число столбцов
 * \param storeRows Признак построчного внутреннего хранения
 * \return
 */
Matrix *Matrix::fromPP(
    TT **PP,
    TI rowCount,
    TI colCount,
    bool storeRows)
{
  if(rowCount == 0 || colCount == 0)
    return NULL;

  Matrix *result = new Matrix(rowCount, colCount, storeRows);
  for(TI i = 0; i < result->rowCount(); ++i)
    for(TI j = 0; j < result->colCount(); ++j)
      result->o(i, j) = storeRows ? PP[i][j] : PP[j][i];

  return result;
}

/*!
 * \brief Сгенерировать матрицу из указателя
 *
 * Признак построчного внутреннего хранения влияет на работу этой функции.
 * \param P Указатель на данные
 * \param rowCount Число строк
 * \param colCount Число столбцов
 * \param storeRows Признак построчного внутреннего хранения
 * \return
 */
Matrix *Matrix::fromP(
    TT *P,
    TI rowCount,
    TI colCount,
    bool storeRows)
{
  if(rowCount == 0 || colCount == 0)
    return NULL;

  Matrix *result = new Matrix(rowCount, colCount, storeRows);
  MatrixIndexer indexer = storeRows ?
        (MatrixIndexer) indexerRow
      :
        (MatrixIndexer) indexerCol;
  for(TI i = 0; i < rowCount; ++i)
    for(TI j = 0; j < colCount; ++j)
      result->o(i, j) = P[indexer(i, j, rowCount, colCount)];

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
  for(TI j = 0; j < m->rowCount(); ++j)
    {
      for(TI i = 0; i < m->colCount(); ++i)
        {
          TT value = m->o(j, i);
          cout << setw(width) << value;
        }
      cout << endl;
    }
  cout << endl << endl;
}

void Matrix::printMatrix(
    TT **m,
    TI rows,
    TI cols, bool storeRows,
    int width)
{
  cout << "Matrix" << endl;
  for(TI i = 0; i < rows; ++i)
    {
      for(TI j = 0; j < cols; ++j)
        cout
            << setw(width)
            << (storeRows ? m[i][j] : m[j][i]);
      cout << endl;
    }
  cout << endl << endl;
}